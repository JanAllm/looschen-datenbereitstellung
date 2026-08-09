cmake_minimum_required(VERSION 3.18 FATAL_ERROR)

# ────────────────────────────────
# Projektname und Version
# ────────────────────────────────
# C wird zusätzlich benötigt: die SQLite-Amalgamation (sqlite3.c) ist reines C.
project(MyCppExecutable VERSION 1.0 LANGUAGES C CXX)

# ────────────────────────────────
# Version, Bauzeitpunkt, Git-Stand für die Weboberfläche
# ────────────────────────────────
# Zweck: auf dem Pi soll ohne SSH erkennbar sein, welcher Stand dort läuft.
# Der Bauzeitpunkt entsteht beim Konfigurieren und ist überall verfügbar.
# Der Git-Stand nicht: .dockerignore schließt .git/ aus, im Container gibt es
# also kein Repository. Deshalb kann er per -DAPP_GIT_REV hereingereicht
# werden (der Dockerfile tut das über ARG GIT_REV), und nur wenn das fehlt,
# wird lokal git befragt.
string(TIMESTAMP APP_BUILD_DATE "%Y-%m-%d %H:%M UTC" UTC)

if(NOT DEFINED APP_GIT_REV OR APP_GIT_REV STREQUAL "")
    set(APP_GIT_REV "")
    find_package(Git QUIET)
    if(GIT_FOUND AND EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/.git")
        execute_process(
            COMMAND "${GIT_EXECUTABLE}" rev-parse --short HEAD
            WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
            OUTPUT_VARIABLE APP_GIT_REV
            OUTPUT_STRIP_TRAILING_WHITESPACE
            ERROR_QUIET)
        # Uncommittete Änderungen kennzeichnen, sonst zeigt ein Testbau auf dem
        # Pi einen Commit an, der so nie gebaut wurde.
        execute_process(
            COMMAND "${GIT_EXECUTABLE}" diff --quiet HEAD
            WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
            RESULT_VARIABLE APP_GIT_DIRTY
            ERROR_QUIET)
        if(APP_GIT_REV AND NOT APP_GIT_DIRTY EQUAL 0)
            set(APP_GIT_REV "${APP_GIT_REV}+")
        endif()
    endif()
endif()

if(NOT APP_GIT_REV)
    set(APP_GIT_REV "unbekannt")
endif()

message(STATUS "✓ Version ${PROJECT_VERSION} (${APP_GIT_REV}), gebaut ${APP_BUILD_DATE}")
# Bewusst NICHT add_compile_definitions: APP_BUILD_DATE ändert sich bei jedem
# Konfigurieren, global gesetzt würden dadurch auch open62541 und SQLite jedes
# Mal neu übersetzt. Die Zuweisung an die eigenen Targets steht weiter unten.

# ────────────────────────────────
# C++-Standard festlegen
# ────────────────────────────────
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# UTF-8-Unterstützung für MSVC
if(MSVC)
    add_compile_options(/utf-8)
endif()

# Nur 64-Bit-Builds erlauben
if(NOT CMAKE_SIZEOF_VOID_P EQUAL 8)
    message(FATAL_ERROR "❌ Dieses Projekt unterstützt nur die x64-Architektur!")
endif()

# Debug-Informationen
message(STATUS "📂 Source Directory: ${CMAKE_SOURCE_DIR}")
message(STATUS "📂 Binary Directory: ${CMAKE_BINARY_DIR}")
message(STATUS "🖥️  Platform: ${CMAKE_SYSTEM_NAME}")

# ────────────────────────────────
# Dateien automatisch einlesen
# ────────────────────────────────
file(GLOB_RECURSE SOURCES "${CMAKE_SOURCE_DIR}/src/*.cpp")
file(GLOB_RECURSE HEADERS "${CMAKE_SOURCE_DIR}/include/*.h")

# Ausschließen von Dateien, die TwinCAT/Pylon benötigen (Windows-only Hardware)
list(FILTER SOURCES EXCLUDE REGEX ".*/baslerKameras\\.cpp$")
list(FILTER SOURCES EXCLUDE REGEX ".*/TwincatAds\\.cpp$")
list(FILTER SOURCES EXCLUDE REGEX ".*/TwincatAds_Arrays\\.cpp$")
# Python-abhängige Altlasten: OCR (pybind11, vom Dienst nicht genutzt) und der
# alte Flask-Webserver. Der Webserver ist jetzt nativ (src/web/*).
list(FILTER SOURCES EXCLUDE REGEX ".*/OCR\\.cpp$")
list(FILTER SOURCES EXCLUDE REGEX ".*/WebServerDialog\\.cpp$")

include_directories("${CMAKE_SOURCE_DIR}/include")

message(STATUS "📄 Quellen: ${SOURCES}")

# ────────────────────────────────
# Nativer Webserver: cpp-httplib + SQLite + (optional) OpenSSL
# ────────────────────────────────
# Python/pybind11 werden NICHT mehr benötigt - der Webserver ist nativ C++.
include(FetchContent)

# cpp-httplib (header-only HTTP/HTTPS-Server)
FetchContent_Declare(
    httplib
    GIT_REPOSITORY https://github.com/yhirose/cpp-httplib.git
    GIT_TAG v0.18.3
)
FetchContent_MakeAvailable(httplib)

# SQLite (Amalgamation) - liest die bestehende data/settings.db weiter.
# Einzelne C-Datei, public domain, keine System-Abhängigkeit.
FetchContent_Declare(
    sqlite3_amalg
    URL https://www.sqlite.org/2024/sqlite-amalgamation-3450100.zip
    DOWNLOAD_EXTRACT_TIMESTAMP TRUE
)
FetchContent_MakeAvailable(sqlite3_amalg)
add_library(sqlite3 STATIC ${sqlite3_amalg_SOURCE_DIR}/sqlite3.c)
target_include_directories(sqlite3 PUBLIC ${sqlite3_amalg_SOURCE_DIR})
if(NOT WIN32)
    target_link_libraries(sqlite3 PUBLIC dl pthread)
endif()

# OpenSSL: aktiviert HTTPS (selbstsigniertes Zertifikat). NICHT zwingend -
# ohne OpenSSL läuft der Server als HTTP (praktisch für lokale Windows-Builds).
find_package(OpenSSL)
if(OpenSSL_FOUND)
    message(STATUS "✓ OpenSSL gefunden (${OPENSSL_VERSION}) - HTTPS aktiv")
else()
    message(WARNING "OpenSSL NICHT gefunden - Webserver läuft als HTTP (ohne TLS)")
endif()

# ────────────────────────────────
# OpenCV
# ────────────────────────────────
if(WIN32)
    set(OpenCV_DIR "D:/opencv/build")
    set(OpenCV_RUNTIME vc16)
endif()
# Only link the modules we use: the full OpenCV_LIBS would also pull in
# opencv_viz, which fails to link unless VTK is installed (optional on Arch).
# geometry (OpenCV 5's new home for the contour/shape functions) is OPTIONAL
# because it does not exist on OpenCV 4 (Windows build).
find_package(OpenCV REQUIRED
    COMPONENTS core imgproc imgcodecs highgui
    OPTIONAL_COMPONENTS geometry)
message(STATUS "✓ OpenCV gefunden: ${OpenCV_VERSION} (Module: ${OpenCV_LIBS})")

# ────────────────────────────────
# open62541 (OPC UA)
# ────────────────────────────────
# open62541 wird auf allen Plattformen im Projekt-Build mitgebaut (Windows + Linux).
# Amalgamation erzwingen: erzeugt den zusammengefassten Header open62541.h,
# den unsere Interfaces (interfaces/OPC_UA.h, OPCUAServer.h) einbinden.
set(UA_ENABLE_AMALGAMATION ON CACHE BOOL "" FORCE)
# Nur unter Linux/GCC: MT=0. Der amalgamierte Header nutzt bei MT>=100 sonst
# C11-Atomics (<stdatomic.h>), die g++ im C++-Kontext nicht übersetzen kann
# (atomic_uintptr_t). Unter Windows/MSVC MUSS der Default (MT>=100) bleiben,
# da dort im MT>=100-Zweig <windows.h> eingebunden wird (sonst fehlt z.B.
# SecureZeroMemory beim Linken). MSVC kompiliert den _WIN32-Atomics-Zweig ohnehin.
if(WIN32)
    set(UA_MULTITHREADING 100 CACHE STRING "" FORCE)
else()
    set(UA_MULTITHREADING 0 CACHE STRING "" FORCE)
endif()
add_subdirectory(${CMAKE_SOURCE_DIR}/third_party/open62541)
include_directories(
    ${CMAKE_SOURCE_DIR}/third_party/open62541/include
    ${CMAKE_BINARY_DIR}/third_party/open62541/src_generated/open62541
    # open62541 1.4 legt den amalgamierten open62541.h hier ab:
    ${CMAKE_BINARY_DIR}/third_party/open62541
)
set(OPCUA_LIBRARIES open62541)
message(STATUS "✓ open62541 aktiviert (Amalgamation, ${CMAKE_SYSTEM_NAME})")

# ────────────────────────────────
# nlohmann::json
# ────────────────────────────────
include(FetchContent)
FetchContent_Declare(
    nlohmann_json
    GIT_REPOSITORY https://github.com/nlohmann/json.git
    GIT_TAG v3.11.3
)
FetchContent_MakeAvailable(nlohmann_json)

# ────────────────────────────────
# Bibliothek erstellen
# ────────────────────────────────
add_library(MyCppLibrary ${SOURCES})

target_include_directories(MyCppLibrary PRIVATE
    ${OpenCV_INCLUDE_DIRS}
)

target_link_libraries(MyCppLibrary
    PUBLIC ${OpenCV_LIBRARIES}
    PUBLIC nlohmann_json::nlohmann_json
    PUBLIC ${OPCUA_LIBRARIES}
    PUBLIC httplib::httplib
    PUBLIC sqlite3
)

# HTTPS nur wenn OpenSSL vorhanden ist (sonst HTTP-Fallback).
if(OpenSSL_FOUND)
    target_compile_definitions(MyCppLibrary PUBLIC CPPHTTPLIB_OPENSSL_SUPPORT)
    target_link_libraries(MyCppLibrary PUBLIC OpenSSL::SSL OpenSSL::Crypto)
endif()

# Versionsangaben für /api/version (oben ermittelt). PRIVATE reicht: gelesen
# werden sie nur in src/web/http_server.cpp.
target_compile_definitions(MyCppLibrary PRIVATE
    APP_VERSION="${PROJECT_VERSION}"
    APP_BUILD_DATE="${APP_BUILD_DATE}"
    APP_GIT_REV="${APP_GIT_REV}")

# Der amalgamierte Header open62541.h wird von einem separaten Target erzeugt.
# MyCppLibrary muss darauf warten, sonst schlägt der Include beim Kompilieren fehl.
if(TARGET open62541-amalgamation)
    add_dependencies(MyCppLibrary open62541-amalgamation)
endif()

# ────────────────────────────────
# Executable erstellen (CROSS-PLATFORM)
# ────────────────────────────────
add_executable(MyCppExecutable src/main.cpp)

# UTF-8-Manifest einbetten (Umlaute in Datei-/Ordnerpfaden), nur MSVC/Windows.
if(WIN32 AND MSVC)
    target_sources(MyCppExecutable PRIVATE ${CMAKE_SOURCE_DIR}/app.manifest)
endif()

target_link_libraries(MyCppExecutable
    PRIVATE MyCppLibrary
    PRIVATE ${OPCUA_LIBRARIES}
    PRIVATE nlohmann_json::nlohmann_json
    PRIVATE ${OpenCV_LIBRARIES}
    PRIVATE httplib::httplib
    PRIVATE sqlite3
)

# main.cpp bindet open62541.h transitiv ein -> ebenfalls auf Amalgamation warten.
if(TARGET open62541-amalgamation)
    add_dependencies(MyCppExecutable open62541-amalgamation)
endif()

# Platform-spezifische Libraries
if(WIN32)
    target_link_libraries(MyCppExecutable PRIVATE ws2_32 winmm)
else()
    target_link_libraries(MyCppExecutable PRIVATE pthread)
endif()

message(STATUS "✓ MyCppExecutable wird gebaut (${CMAKE_SYSTEM_NAME})")

# ────────────────────────────────
# Ausgabe-Verzeichnisse
# ────────────────────────────────
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(TEST_DIR "${CMAKE_BINARY_DIR}/tests")

# ────────────────────────────────
# Windows-spezifische DLL-Kopien
# ────────────────────────────────
if(WIN32)
    # Hilfsfunktion zum Kopieren
    function(copy_file FILE_PATH TARGET_NAME EXTRA_DIR)
        if(EXISTS "${FILE_PATH}")
            add_custom_command(TARGET ${TARGET_NAME} POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E copy_if_different "${FILE_PATH}" $<TARGET_FILE_DIR:${TARGET_NAME}>
                COMMAND ${CMAKE_COMMAND} -E copy_if_different "${FILE_PATH}" "${EXTRA_DIR}"
                COMMENT "📦 Kopiere Datei: ${FILE_PATH}"
            )
        else()
            message(WARNING "⚠ Datei nicht gefunden: ${FILE_PATH}")
        endif()
    endfunction()

    # OpenCV DLLs kopieren
    file(GLOB OpenCV_DLLS "${OpenCV_DIR}/x64/vc16/bin/*.dll")
    foreach(DLL ${OpenCV_DLLS})
        copy_file("${DLL}" MyCppExecutable "${TEST_DIR}")
    endforeach()

    # Python DLL kopieren
    get_filename_component(PYTHON_DLL_DIR "${Python3_EXECUTABLE}" DIRECTORY)
    set(PYTHON_DLL_PATH "${PYTHON_DLL_DIR}/python314.dll")
    if(EXISTS "${PYTHON_DLL_PATH}")
        copy_file("${PYTHON_DLL_PATH}" MyCppExecutable "${TEST_DIR}")
    else()
        message(WARNING "⚠ Python DLL nicht gefunden: ${PYTHON_DLL_PATH}")
    endif()

    # Konfigurationsdateien kopieren
    if(EXISTS "${CMAKE_SOURCE_DIR}/config")
        add_custom_command(TARGET MyCppExecutable POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_directory
                "${CMAKE_SOURCE_DIR}/config"
                "$<TARGET_FILE_DIR:MyCppExecutable>/config"
        )
    endif()

    # WebVisu kopieren
    add_custom_command(TARGET MyCppExecutable POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
            "${CMAKE_SOURCE_DIR}/src/webVisu"
            "$<TARGET_FILE_DIR:MyCppExecutable>/webVisu"
    )
endif()

# ────────────────────────────────
# Python venv konfigurieren
# ────────────────────────────────
if(WIN32)
    set(PYTHON_VENV_PATH "${CMAKE_SOURCE_DIR}/.venv/Scripts/python.exe")
else()
    set(PYTHON_VENV_PATH "${CMAKE_SOURCE_DIR}/.venv/bin/python3")
endif()

if(EXISTS ${PYTHON_VENV_PATH})
    message(STATUS "✓ Python venv gefunden: ${PYTHON_VENV_PATH}")
    set(Python3_EXECUTABLE ${PYTHON_VENV_PATH})
    add_compile_definitions(PYTHON_VENV_PATH="${PYTHON_VENV_PATH}")
else()
    message(WARNING "⚠ Python venv nicht gefunden: ${PYTHON_VENV_PATH}")
endif()

# ────────────────────────────────
# Tests (optional - im Docker-Image via -DBUILD_TESTS=OFF deaktivierbar,
# spart den GoogleTest-Download beim Image-Build)
# ────────────────────────────────
option(BUILD_TESTS "Unit-Tests bauen" ON)
if(BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()

message(STATUS "✅ Konfiguration abgeschlossen. Baue mit: cmake --build .")