cmake_minimum_required(VERSION 3.18)

# GoogleTest: bevorzugt vom System, sonst automatisch herunterladen
find_package(GTest QUIET)
if(NOT GTest_FOUND)
    message(STATUS "GTest nicht installiert - hole es per FetchContent")
    include(FetchContent)
    FetchContent_Declare(
        googletest
        GIT_REPOSITORY https://github.com/google/googletest.git
        GIT_TAG v1.15.2
    )
    # MSVC: dynamische Runtime (/MD) erzwingen, passend zum restlichen Build
    set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
    FetchContent_MakeAvailable(googletest)
endif()
include(GoogleTest)

# Test-Quellen finden
file(GLOB_RECURSE TEST_SOURCES "${CMAKE_SOURCE_DIR}/tests/*.cpp")
# Legacy test for the removed Python/Flask web server - its header no longer exists
list(FILTER TEST_SOURCES EXCLUDE REGEX ".*/testWebServer\\.cpp$")

# Test-Executable
add_executable(MyCppTests ${TEST_SOURCES})

# ────────────────────────────────
# Test-EXE direkt in build/tests/ erstellen
# ────────────────────────────────
set_target_properties(MyCppTests PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/tests"
)

# UTF-8-Manifest auch für die Tests (Umlaut-Pfade), nur MSVC/Windows.
if(WIN32 AND MSVC)
    target_sources(MyCppTests PRIVATE ${CMAKE_SOURCE_DIR}/app.manifest)
endif()

# Includes
target_include_directories(MyCppTests PRIVATE
    ${CMAKE_SOURCE_DIR}/include
    ${OpenCV_INCLUDE_DIRS}
)

# Libraries
target_link_libraries(MyCppTests
    PRIVATE MyCppLibrary
    PRIVATE gtest
    PRIVATE gtest_main
    PRIVATE ${OpenCV_LIBRARIES}
)

# Automatische Test-Erkennung
include(GoogleTest)
gtest_discover_tests(MyCppTests
    WORKING_DIRECTORY "${CMAKE_BINARY_DIR}/tests"
    DISCOVERY_TIMEOUT 60
    DISCOVERY_MODE PRE_TEST  # ← Tests werden erst beim Ausführen entdeckt
)

# ────────────────────────────────
# Test-Verzeichnis
# ────────────────────────────────
set(TEST_DEST_DIR "${CMAKE_BINARY_DIR}/tests")

# ────────────────────────────────
# Hilfsfunktion zum Kopieren für Tests
# ────────────────────────────────
function(copy_to_tests FILE_PATH)
    if(EXISTS "${FILE_PATH}")
        add_custom_command(TARGET MyCppTests POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
                "${FILE_PATH}"
                "${TEST_DEST_DIR}"
            COMMENT "📦 Kopiere für Tests: ${FILE_PATH}"
        )
    else()
        message(WARNING "⚠ Datei nicht gefunden: ${FILE_PATH}")
    endif()
endfunction()

# ────────────────────────────────
# OpenCV DLLs kopieren
# ────────────────────────────────
file(GLOB OpenCV_DLLS "${OpenCV_DIR}/x64/vc16/bin/*.dll")
foreach(DLL ${OpenCV_DLLS})
    copy_to_tests("${DLL}")
endforeach()

# ────────────────────────────────
# Ressourcen kopieren
# ────────────────────────────────

# config.txt kopieren (falls vorhanden)
if(EXISTS "${CMAKE_SOURCE_DIR}/src/config.txt")
    add_custom_command(TARGET MyCppTests POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "${CMAKE_SOURCE_DIR}/src/config.txt"
            "${TEST_DEST_DIR}"
        COMMENT "📦 Kopiere config.txt"
    )
endif()

# webVisu kopieren
add_custom_command(TARGET MyCppTests POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_SOURCE_DIR}/src/webVisu"
        "${TEST_DEST_DIR}/webVisu"
    COMMENT "📦 Kopiere webVisu"
)

# models kopieren (falls vorhanden)
if(EXISTS "${CMAKE_SOURCE_DIR}/models")
    add_custom_command(TARGET MyCppTests POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
            "${CMAKE_SOURCE_DIR}/models"
            "${TEST_DEST_DIR}/models"
        COMMENT "📦 Kopiere models"
    )
endif()

# config-Verzeichnis kopieren (falls vorhanden)
if(EXISTS "${CMAKE_SOURCE_DIR}/config")
    add_custom_command(TARGET MyCppTests POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
            "${CMAKE_SOURCE_DIR}/config"
            "${TEST_DEST_DIR}/config"
        COMMENT "📦 Kopiere config-Verzeichnis"
    )
endif()