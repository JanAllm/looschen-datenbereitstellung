#include <gtest/gtest.h>
#include "WebVisu/WebServerDialog.h"
#include "CNC_Daten/preview_renderer.h"
#include "CNC_Daten/cnc_projekt_manager.h"
#include "CNC_Daten/gcode_parser.h"
#include "CNC_Daten/cnc_projekt_manager.h"
#include "CNC_Daten/project_file_reader.h"
#include "interfaces/SPSController.h"
#include "interfaces/DataType.h"
#include "interfaces/VariablenInfo.h"
#include "interfaces/OPCUAServer.h"
#include "load_settings.h"
#include <thread>
#include <chrono>
#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
#include <cstdlib>
#include <array>
#include <fstream>
#include <sstream>
#include <iostream>
#include <filesystem>
#include "path_utils.h"
// ========== HELPER: Python-Pfad finden (VENV-FIRST) ==========
std::string findPythonPath()
{
    std::cerr << "Suche Python (venv-first)..." << std::endl;

    namespace fs = std::filesystem;

    // Liste von Kandidaten - venv hat Priorität!
    std::vector<std::string> candidates;

    // 1. Projekt-venv (relativ zum Build-Verzeichnis)
    // Von build/tests -> zurück zum Projekt-Root
    fs::path buildDir = fs::current_path();
    fs::path venvPath = buildDir.parent_path().parent_path() / ".venv" / "Scripts" / "python.exe";
    candidates.push_back(venvPath.string());

    // 2. Absoluter Pfad zur venv (Fallback)
    candidates.push_back("D:/Arbeit/Looschen_Datenbereitstellung/.venv/Scripts/python.exe");

#ifdef PYTHON_VENV_PATH
    // 3. CMake-konfigurierter Pfad (wenn definiert)
    candidates.push_back(PYTHON_VENV_PATH);
#endif

    // 4. System-Python (letzter Fallback)
    candidates.push_back("python");

    // Teste alle Kandidaten
    for (const auto &candidate : candidates)
    {
        std::cerr << "Prüfe: " << candidate << std::endl;

        // Bei absolutem Pfad: Prüfe Existenz
        fs::path candidatePath(candidate);
        if (candidatePath.is_absolute())
        {
            if (!fs::exists(candidatePath))
            {
                std::cerr << "  → Nicht gefunden" << std::endl;
                continue;
            }
        }

        // Teste Ausführbarkeit
        std::string testCmd = "\"" + candidate + "\" --version >nul 2>&1";
        int result = std::system(testCmd.c_str());

        if (result == 0)
        {
            std::string pythonPath = candidate;

            // Für "python": Hole vollständigen Pfad mit where.exe
            if (candidate == "python")
            {
                std::string tempFile = "python_path_temp.txt";
                std::string whereCmd = "where.exe python > " + tempFile + " 2>&1";

                if (std::system(whereCmd.c_str()) == 0)
                {
                    std::ifstream file(tempFile);
                    if (file.is_open() && std::getline(file, pythonPath))
                    {
                        file.close();
                        pythonPath.erase(0, pythonPath.find_first_not_of(" \t\r\n"));
                        pythonPath.erase(pythonPath.find_last_not_of(" \t\r\n") + 1);
                    }
                    std::remove(tempFile.c_str());
                }
            }

            std::cerr << "✓ PYTHON GEFUNDEN: " << pythonPath << std::endl;

            // Zeige Version
            std::string versionCmd = "\"" + pythonPath + "\" --version";
            [[maybe_unused]] int result = std::system(versionCmd.c_str());

            return pythonPath;
        }
        else
        {
            std::cerr << "  → Nicht ausführbar" << std::endl;
        }
    }

    std::cerr << "✗ PYTHON NICHT GEFUNDEN!" << std::endl;
    return "";
}

// ========== HELPER: Dependencies überprüfen ==========
bool checkPythonDependencies(const std::string &pythonPath)
{
    std::cerr << "\n========== PRÜFE DEPENDENCIES ==========" << std::endl;

    if (pythonPath.empty())
    {
        std::cerr << "FEHLER: Python-Pfad ist leer!" << std::endl;
        return false;
    }

    std::cerr << "Python-Pfad: " << pythonPath << std::endl;

    // Prüfe ob es venv-Python ist
    if (pythonPath.find(".venv") != std::string::npos)
    {
        std::cerr << "✓ Verwende venv-Python" << std::endl;
    }
    else
    {
        std::cerr << "⚠ Verwende System-Python (venv nicht gefunden)" << std::endl;
    }

    // Version anzeigen
    std::string versionCmd = "\"" + pythonPath + "\" --version";
    std::cerr << "Teste Version..." << std::endl;
    int versionResult = std::system(versionCmd.c_str());
    std::cerr << "Version Return Code: " << versionResult << std::endl;

    if (versionResult != 0)
    {
        std::cerr << "FEHLER: Python nicht ausführbar!" << std::endl;
        return false;
    }

    // Dependencies prüfen
    std::array<const char *, 3> deps = {"flask", "PIL", "cv2"};
    bool allOk = true;

    for (const auto *dep : deps)
    {
// Verwende cmd /c um Escaping-Probleme zu vermeiden
#ifdef _WIN32
        std::string checkCmd = "cmd /c \"\"" + pythonPath + "\" -c \"import " + std::string(dep) + "\"\" >nul 2>&1";
#else
        std::string checkCmd = pythonPath + " -c \"import " + std::string(dep) + "\" >/dev/null 2>&1";
#endif

        std::cerr << "Prüfe " << dep << "... ";
        int result = std::system(checkCmd.c_str());

        if (result == 0)
        {
            std::cerr << "✓" << std::endl;
        }
        else
        {
            std::cerr << "✗ FEHLT!" << std::endl;
            allOk = false;
        }
    }

    if (!allOk)
    {
        std::cerr << "\n⚠️  INSTALLIERE DEPENDENCIES:" << std::endl;
        if (pythonPath.find(".venv") != std::string::npos)
        {
            std::cerr << "   In CMD/PowerShell ausführen:" << std::endl;
            std::cerr << "   \"" << pythonPath << "\" -m pip install flask pillow opencv-python" << std::endl;
        }
        else
        {
            std::cerr << "   \"" << pythonPath << "\" -m pip install flask pillow opencv-python" << std::endl;
        }
    }

    std::cerr << "==========================================\n"
              << std::endl;
    return allOk;
}

// ========== TEST: WebServer Start ==========
TEST(Webserver, start_Web_Server)
{
    std::cerr << "\n************ TEST START ************" << std::endl;

    try
    {
        std::string pythonPath = findPythonPath();

        ASSERT_FALSE(pythonPath.empty()) << "Python nicht gefunden!";

        // Dependencies prüfen
        bool depsOk = checkPythonDependencies(pythonPath);

        if (!depsOk)
        {
            std::cerr << "SKIP: Dependencies fehlen!" << std::endl;
            GTEST_SKIP() << "Python-Dependencies fehlen. Test übersprungen.";
        }

        std::string scriptPath = "./webVisu";

        std::cerr << "\nStarte WebServer:" << std::endl;
        std::cerr << "  Python: " << pythonPath << std::endl;
        std::cerr << "  Script: " << scriptPath << std::endl;

        WebServer webServer(pythonPath, scriptPath, 500, 1000);

        int result = webServer.startServer();

        EXPECT_EQ(result, 0) << "Server-Start fehlgeschlagen";

        if (result == 0)
        {
            std::cerr << "✓ WebServer erfolgreich gestartet" << std::endl;
            std::this_thread::sleep_for(std::chrono::seconds(60));
            webServer.stopServer();
            std::cerr << "✓ WebServer gestoppt" << std::endl;
        }
    }
    catch (const std::exception &e)
    {
        std::cerr << "EXCEPTION: " << e.what() << std::endl;
        FAIL() << "Exception: " << e.what();
    }

    std::cerr << "************ TEST ENDE ************\n"
              << std::endl;
}

// ========== TEST: WebServer Bild laden ==========
TEST(Webserver, LoadDatasetAndShowImg)
{
    std::cerr << "\n=== TEST: LoadDatasetAndShowImg ===" << std::endl;
    try
    {
        setupVarInfos setupVars;
        std::string exeDir = getExecutableDirectory();

        std::cerr << "DEBUG: Executable Directory: " << exeDir << std::endl;

#ifdef _WIN32
        std::string configPath = exeDir + "\\config.txt";
#else
        std::string configPath = exeDir + "/config.txt";
#endif

        std::cerr << "DEBUG: Config Path: " << configPath << std::endl;

        // Prüfe ob Datei existiert
        std::ifstream testFile(configPath);
        if (!testFile.good())
        {
            std::cerr << "❌ FEHLER: config.txt existiert nicht!" << std::endl;
            std::cerr << "   Suche in: " << configPath << std::endl;
            std::cerr << "   Dateien im Verzeichnis:" << std::endl;
            std::system(("ls -la " + exeDir).c_str());
        }
        else
        {
            std::cerr << "✓ config.txt gefunden" << std::endl;
        }
        testFile.close();

        int webserVerImgSizeX = 1000;
        int webserVerImgSizeY = 500;
        std::string ProgrammId = "";
        std::string folderPath = "";
        int sizeObjx = 200;
        int sizeObjy = 200;
        int MaximaleAnzahlProjekte = 10;
        int lenDataArray = 1000;
        std::string ipOPC = "opc.tcp://CP-5E1BE0:4840";

        std::cerr << "DEBUG: Lade Settings..." << std::endl;
        loadSettings(configPath, setupVars, ipOPC, ProgrammId, folderPath,
                     sizeObjx, sizeObjy, webserVerImgSizeX, webserVerImgSizeY,
                     MaximaleAnzahlProjekte, lenDataArray);

        std::cerr << "DEBUG: setupVars nach loadSettings:" << std::endl;
        std::cerr << "  iHeartbeat: " << setupVars.iHeartbeat << std::endl;
        std::cerr << "  iError: " << setupVars.iError << std::endl;

        std::cerr << "1. Erstelle OPC UA Mock-Server..." << std::endl;
        OPCUAServer mockServer(4842);

        std::cerr << "2. Füge ALLE Variablen mit ns=" << setupVars.ns << " und numerischen IDs hinzu..." << std::endl;

        // Verwende setupVars.ns (standardmäßig 2)
        ASSERT_TRUE(mockServer.addBoolVariable(setupVars.ns, 120, "Error", false));
        ASSERT_TRUE(mockServer.addStringVariable(setupVars.ns, 122, "Info_Text", ""));
        ASSERT_TRUE(mockServer.addInt16Variable(setupVars.ns, 123, "Heartbeat", 0));
        ASSERT_TRUE(mockServer.addInt16Variable(setupVars.ns, 124, "StatusCode", 0));

        // Arrays
        ASSERT_TRUE(mockServer.addStringArrayVariable(setupVars.ns, 19, "DataArray", {}));
        ASSERT_TRUE(mockServer.addStringArrayVariable(setupVars.ns, 138, "ProjektArray", {}));

        // Dialog Variablen
        ASSERT_TRUE(mockServer.addBoolVariable(setupVars.ns, 125, "UpdateProjektList", false));
        ASSERT_TRUE(mockServer.addBoolVariable(setupVars.ns, 126, "ProjektlistUpdated", false));
        ASSERT_TRUE(mockServer.addBoolVariable(setupVars.ns, 127, "showProjektInfo", false));
        ASSERT_TRUE(mockServer.addStringVariable(setupVars.ns, 128, "ProjektName", ""));
        ASSERT_TRUE(mockServer.addInt16Variable(setupVars.ns, 129, "LenProjekt", 0));
        ASSERT_TRUE(mockServer.addInt16Variable(setupVars.ns, 130, "AnzDatenblöcke", 0));
        ASSERT_TRUE(mockServer.addBoolVariable(setupVars.ns, 131, "ProjektVorhanden", false));
        ASSERT_TRUE(mockServer.addBoolVariable(setupVars.ns, 132, "ProjektInfoWrote", false));
        ASSERT_TRUE(mockServer.addBoolVariable(setupVars.ns, 133, "ReadProjekt", false));
        ASSERT_TRUE(mockServer.addBoolVariable(setupVars.ns, 134, "WriteData", false));
        ASSERT_TRUE(mockServer.addInt16Variable(setupVars.ns, 135, "IndexData", 0));
        ASSERT_TRUE(mockServer.addBoolVariable(setupVars.ns, 136, "ReadData", false));
        ASSERT_TRUE(mockServer.addBoolVariable(setupVars.ns, 137, "ubertragenAbbrechen", false));
        ASSERT_TRUE(mockServer.addBoolVariable(setupVars.ns, 139, "UebertgarungBeendet", false));
        ASSERT_TRUE(mockServer.addInt16Variable(setupVars.ns, 140, "Uebertragungslaege", 100));

        // Farben und Stärken
        ASSERT_TRUE(mockServer.addInt16ArrayVariable(setupVars.ns, 200, "colorG0", {}));
        ASSERT_TRUE(mockServer.addInt16Variable(setupVars.ns, 201, "streghtsG0", 5));
        ASSERT_TRUE(mockServer.addInt16ArrayVariable(setupVars.ns, 202, "colorG1", {}));
        ASSERT_TRUE(mockServer.addInt16Variable(setupVars.ns, 203, "streghtsG1", 5));
        ASSERT_TRUE(mockServer.addInt16ArrayVariable(setupVars.ns, 204, "colorG2", {}));
        ASSERT_TRUE(mockServer.addInt16Variable(setupVars.ns, 205, "streghtsG2", 5));
        ASSERT_TRUE(mockServer.addInt16ArrayVariable(setupVars.ns, 206, "colorG3", {}));
        ASSERT_TRUE(mockServer.addInt16Variable(setupVars.ns, 207, "streghtsG3", 5));
        ASSERT_TRUE(mockServer.addInt16ArrayVariable(setupVars.ns, 210, "FarbeLive", {}));
        ASSERT_TRUE(mockServer.addInt16Variable(setupVars.ns, 211, "StaerkeLive", 5));

        // Livebild
        ASSERT_TRUE(mockServer.addInt16Variable(setupVars.ns, 212, "LiveStand", 0));
        ASSERT_TRUE(mockServer.addInt16Variable(setupVars.ns, 213, "LiveAbbruch", 0));

        // Bogenglättung
        ASSERT_TRUE(mockServer.addFloatVariable(setupVars.ns, 214, "minArcRadius", 0.0f));

        // Canvas
        ASSERT_TRUE(mockServer.addInt16Variable(setupVars.ns, 208, "objektgroesseX", 1000));
        ASSERT_TRUE(mockServer.addInt16Variable(setupVars.ns, 209, "objektgroesseY", 2000));

        std::cerr << "✓ Alle " << 31 << " Variablen erstellt" << std::endl;

        std::cerr << "3. Starte Server..." << std::endl;
        ASSERT_TRUE(mockServer.start());
        std::cerr << "✓ Mock-Server läuft auf: " << mockServer.getEndpointUrl() << std::endl;

        // Kurze Wartezeit für Server-Initialisierung
        std::this_thread::sleep_for(std::chrono::milliseconds(100));

        std::cerr << "4. Verbinde SPSController..." << std::endl;
        SPSController sps("opc", "opc.tcp://localhost:4842");

        bool connected = false;
        for (int i = 0; i < 10 && !connected; i++)
        {
            std::cerr << "   Versuch " << (i + 1) << "/10..." << std::endl;
            connected = sps.connect();
            if (!connected)
            {
                std::this_thread::sleep_for(std::chrono::milliseconds(500));
            }
        }
        ASSERT_TRUE(connected) << "Konnte nicht verbinden nach 10 Versuchen!";
        std::cerr << "✓ SPSController verbunden" << std::endl;

        std::cerr << "5. Rufe setupVariables auf..." << std::endl;
        auto succ = sps.setupVariables(setupVars);
        std::cerr << "DEBUG: setupVariables Ergebnis: " << (succ ? "true" : "false") << std::endl;

        ASSERT_TRUE(succ) << "Konnte Variablen nicht einrichten!";
        std::cerr << "✓ Variablen eingerichtet" << std::endl;

        std::string pythonPath = findPythonPath();

        ASSERT_FALSE(pythonPath.empty()) << "Python nicht gefunden!";

        // Dependencies prüfen
        bool depsOk = checkPythonDependencies(pythonPath);

        if (!depsOk)
        {
            std::cerr << "SKIP: Dependencies fehlen!" << std::endl;
            GTEST_SKIP() << "Python-Dependencies fehlen. Test übersprungen.";
        }

        std::string scriptPath = "./webVisu";

        std::cerr << "\nStarte WebServer:" << std::endl;
        std::cerr << "  Python: " << pythonPath << std::endl;
        std::cerr << "  Script: " << scriptPath << std::endl;

        WebServer webServer(pythonPath, scriptPath, webserVerImgSizeX, webserVerImgSizeY);

        // # Hauptlogic des Tests ja der wiurd vioel zu groß aber passt erstemal so

        int result = webServer.startServer();
        ASSERT_EQ(result, 0) << "Server-Start fehlgeschlagen";
        std::cerr << "✓ WebServer erfolgreich gestartet" << std::endl;
        std::this_thread::sleep_for(std::chrono::seconds(1));

        folderPath = "D:/Arbeit/Looschen_Datenbereitstellung/BeispielDatensatz";
        ProjectManager projectManager(sps, webServer, folderPath);
        auto start = std::chrono::steady_clock::now();
        while (true)
        {
            auto now = std::chrono::steady_clock::now();
            auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(now - start).count();

            if (elapsed >= 10)
            {
                std::cerr << "✓ 10 Sekunden erreicht, beende Schleife" << std::endl;
                break;
            }

            auto [showRequested, showSuccess, showCode] =
                sps.readBool("showProjektInfo");

            if (showSuccess && showRequested)
            {
                sps.writeBool("showProjektInfo", false);
                projectManager.showProjectInfo();
                sps.writeBool("ProjektInfoWrote", true);
            }

            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }

        std::cerr << "6. Cleanup..." << std::endl;
        sps.disconnect();
        mockServer.stop();
        std::cerr << "✅ TEST ERFOLGREICH!\n"
                  << std::endl;
    }
    catch (const std::exception &e)
    {
        std::cerr << "❌ EXCEPTION: " << e.what() << std::endl;
        FAIL() << "Exception: " << e.what();
    }
}