#include "WebVisu/WebServerDialog.h"
#include <iostream>
#include <filesystem>
#include <fstream>
#include <algorithm>

#ifdef _WIN32
    #include <Windows.h>
#else
    #include <unistd.h>
    #include <cstdlib>
#endif

#include "ErrorLoggerSingleton.h"
#include "path_utils.h"

WebServer::WebServer(const std::string &pythonPath,
                     const std::string &scriptPath,
                     const int webserVerImgSizeX,
                     const int webserVerImgSizeY)
    : pythonPath_(pythonPath),
      scriptPath_(scriptPath),
      moduleName_("server"),
      startName_("start"),
      stopName_("stop"),
      pModule_(nullptr),
      pServerInstance_(nullptr),
      pStartFn_(nullptr),
      pStopFn_(nullptr),
      pBridgeModule_(nullptr),
      running_(false),
      webserVerImgSizeX_(webserVerImgSizeX),
      webserVerImgSizeY_(webserVerImgSizeY)
{
    // Python-Pfad ermitteln
    if (pythonPath_.empty())
    {
        pythonPath_ = findPythonPath();
    }

    // Script-Pfad ermitteln (Standard: EXE-Verzeichnis)
    if (scriptPath_.empty())
    {
        scriptPath_ = getExecutableDirectory();
        ErrorLoggerSingleton::instance().logError(
            "Script-Pfad automatisch gesetzt: " + scriptPath_);
    }

    if (!std::filesystem::exists(scriptPath_))
    {
        ErrorLoggerSingleton::instance().logError(
            "WARNUNG: Script-Verzeichnis existiert nicht: " + scriptPath_);
    }

    // Prüfe ob server.py existiert
    std::string serverPyPath = (std::filesystem::path(scriptPath_) / "server.py").string();
    if (!std::filesystem::exists(serverPyPath))
    {
        ErrorLoggerSingleton::instance().logError(
            "FEHLER: server.py nicht gefunden: " + serverPyPath);
        throw std::runtime_error("server.py nicht gefunden!");
    }

    ErrorLoggerSingleton::instance().logError("Python-Pfad: " + pythonPath_);
    ErrorLoggerSingleton::instance().logError("Script-Pfad: " + scriptPath_);

    initInterpreter();
}

WebServer::~WebServer()
{
    stopServer();
    finalizeInterpreter();
}

// ========== Python-Pfad finden (Cross-Platform) ==========
std::string WebServer::findPythonPath()
{
    ErrorLoggerSingleton::instance().logError("Suche Python-Installation...");

#ifdef _WIN32
    // Windows: Registry durchsuchen
    std::vector<std::string> registryPaths = {
        "SOFTWARE\\Python\\PythonCore\\3.14\\InstallPath",
        "SOFTWARE\\Python\\PythonCore\\3.13\\InstallPath",
        "SOFTWARE\\Python\\PythonCore\\3.12\\InstallPath",
        "SOFTWARE\\Python\\PythonCore\\3.11\\InstallPath",
        "SOFTWARE\\Python\\PythonCore\\3.10\\InstallPath",
    };

    // Sowohl systemweite (HKLM) als auch Per-User-Installationen (HKCU) prüfen.
    const HKEY hives[] = {HKEY_CURRENT_USER, HKEY_LOCAL_MACHINE};
    for (HKEY hive : hives)
    {
        for (const auto &regPath : registryPaths)
        {
            HKEY hKey;
            LONG result = RegOpenKeyExA(hive, regPath.c_str(), 0, KEY_READ, &hKey);

            if (result == ERROR_SUCCESS)
            {
                char buffer[MAX_PATH];
                DWORD bufferSize = sizeof(buffer);

                result = RegQueryValueExA(hKey, "ExecutablePath", NULL, NULL,
                                          (LPBYTE)buffer, &bufferSize);

                if (result == ERROR_SUCCESS)
                {
                    RegCloseKey(hKey);
                    std::string pythonExe(buffer);
                    ErrorLoggerSingleton::instance().logError("Python gefunden: " + pythonExe);
                    return pythonExe;
                }

                // Fallback: Installationspfad + python.exe
                bufferSize = sizeof(buffer);
                result = RegQueryValueExA(hKey, "", NULL, NULL, (LPBYTE)buffer, &bufferSize);
                if (result == ERROR_SUCCESS)
                {
                    RegCloseKey(hKey);
                    std::string pythonDir(buffer);
                    std::string pythonExe = pythonDir + "\\python.exe";

                    if (std::filesystem::exists(pythonExe))
                    {
                        ErrorLoggerSingleton::instance().logError("Python gefunden: " + pythonExe);
                        return pythonExe;
                    }
                }

                RegCloseKey(hKey);
            }
        }
    }

    // Per-User-Standardinstallation (LocalAppData\Programs\Python\Python3XX).
    if (const char *localAppData = std::getenv("LOCALAPPDATA"))
    {
        for (const char *ver : {"314", "313", "312", "311", "310"})
        {
            std::string p = std::string(localAppData) +
                            "\\Programs\\Python\\Python" + ver + "\\python.exe";
            if (std::filesystem::exists(p))
            {
                ErrorLoggerSingleton::instance().logError("Python gefunden: " + p);
                return p;
            }
        }
    }

    // Windows: Standard-Pfade
    std::vector<std::string> standardPaths = {
        "C:\\Python314\\python.exe",
        "C:\\Python313\\python.exe",
        "C:\\Python312\\python.exe",
        "C:\\Python311\\python.exe",
    };

    for (const auto &path : standardPaths)
    {
        if (std::filesystem::exists(path))
        {
            ErrorLoggerSingleton::instance().logError("Python gefunden: " + path);
            return path;
        }
    }
#else
    // Linux/Unix: which python3
    FILE* pipe = popen("which python3", "r");
    if (pipe)
    {
        char buffer[256];
        if (fgets(buffer, sizeof(buffer), pipe))
        {
            std::string result(buffer);
            result.erase(std::remove(result.begin(), result.end(), '\n'), result.end());
            pclose(pipe);
            
            if (!result.empty() && std::filesystem::exists(result))
            {
                ErrorLoggerSingleton::instance().logError("Python gefunden: " + result);
                return result;
            }
        }
        pclose(pipe);
    }

    // Linux: Standard-Pfade
    std::vector<std::string> standardPaths = {
        "/usr/bin/python3",
        "/usr/local/bin/python3",
    };

    for (const auto &path : standardPaths)
    {
        if (std::filesystem::exists(path))
        {
            ErrorLoggerSingleton::instance().logError("Python gefunden: " + path);
            return path;
        }
    }
#endif

    ErrorLoggerSingleton::instance().logError("WARNUNG: Keine Python-Installation gefunden!");
    return "";
}

// ========== Umgebungsvariable setzen (Cross-Platform) ==========
void setEnvironmentVariable(const std::string& name, const std::string& value)
{
#ifdef _WIN32
    std::wstring wName(name.begin(), name.end());
    std::wstring wValue(value.begin(), value.end());
    _wputenv_s(wName.c_str(), wValue.c_str());
#else
    if (value.empty())
    {
        unsetenv(name.c_str());
    }
    else
    {
        setenv(name.c_str(), value.c_str(), 1);
    }
#endif
}

std::string getEnvironmentVariable(const std::string& name)
{
#ifdef _WIN32
    wchar_t buffer[32767];
    std::wstring wName(name.begin(), name.end());
    DWORD len = GetEnvironmentVariableW(wName.c_str(), buffer, 32767);
    if (len == 0)
    {
        return "";
    }
    std::wstring wValue(buffer, len);
    return std::string(wValue.begin(), wValue.end());
#else
    const char* value = getenv(name.c_str());
    return value ? std::string(value) : "";
#endif
}

// ========== Interpreter initialisieren ==========
void WebServer::initInterpreter()
{
    try
    {
        ErrorLoggerSingleton::instance().logError("=== Python-Interpreter Initialisierung ===");

        // ========== 1. Validierung ==========
        if (pythonPath_.empty())
        {
            throw std::runtime_error("Python-Pfad ist leer!");
        }

        if (!std::filesystem::exists(pythonPath_))
        {
            throw std::runtime_error("Python-Executable existiert nicht: " + pythonPath_);
        }

        if (!std::filesystem::exists(scriptPath_))
        {
            throw std::runtime_error("Script-Verzeichnis existiert nicht: " + scriptPath_);
        }

        std::string serverPyPath = (std::filesystem::path(scriptPath_) / "server.py").string();
        if (!std::filesystem::exists(serverPyPath))
        {
            throw std::runtime_error("server.py nicht gefunden: " + serverPyPath);
        }

        // ========== 2. Python-Verzeichnisse ermitteln ==========
        std::filesystem::path pythonExePath(pythonPath_);
        std::string pythonDir = pythonExePath.parent_path().string();
        bool isVenv = false;
        std::string systemPythonHome;

        // Prüfe ob venv
#ifdef _WIN32
        if (pythonDir.find("Scripts") != std::string::npos)
#else
        if (pythonDir.find("bin") != std::string::npos)
#endif
        {
            pythonDir = pythonExePath.parent_path().parent_path().string();
            isVenv = true;
            ErrorLoggerSingleton::instance().logError("✓ venv erkannt: " + pythonDir);

            // Lese pyvenv.cfg
            std::string pyvenvCfg = (std::filesystem::path(pythonDir) / "pyvenv.cfg").string();
            if (std::filesystem::exists(pyvenvCfg))
            {
                std::ifstream cfgFile(pyvenvCfg);
                std::string line;
                while (std::getline(cfgFile, line))
                {
                    if (line.find("home = ") == 0)
                    {
                        systemPythonHome = line.substr(7);
                        // Trimme Whitespace
                        systemPythonHome.erase(0, systemPythonHome.find_first_not_of(" \t\r\n"));
                        systemPythonHome.erase(systemPythonHome.find_last_not_of(" \t\r\n") + 1);
                        ErrorLoggerSingleton::instance().logError("System-Python: " + systemPythonHome);
                        break;
                    }
                }
            }
        }

        // ========== 3. Umgebungsvariablen setzen ==========
        setEnvironmentVariable("PYTHONHOME", "");
        setEnvironmentVariable("PYTHONPATH", "");

        // PATH erweitern
        std::string currentPath = getEnvironmentVariable("PATH");
        std::string newPath;

        if (isVenv && !systemPythonHome.empty())
        {
#ifdef _WIN32
            std::string venvScripts = (std::filesystem::path(pythonDir) / "Scripts").string();
            std::string systemDLLs = (std::filesystem::path(systemPythonHome) / "DLLs").string();
            newPath = venvScripts + ";" + pythonDir + ";" + systemPythonHome + ";" + systemDLLs + ";" + currentPath;
#else
            std::string venvBin = (std::filesystem::path(pythonDir) / "bin").string();
            newPath = venvBin + ":" + pythonDir + ":" + systemPythonHome + ":" + currentPath;
#endif
        }
        else
        {
#ifdef _WIN32
            newPath = pythonDir + ";" + currentPath;
#else
            newPath = pythonDir + ":" + currentPath;
#endif
        }

        setEnvironmentVariable("PATH", newPath);
        setEnvironmentVariable("PYTHONEXECUTABLE", pythonPath_);

        // ========== 4. Python initialisieren ==========
        std::wstring wProgram(pythonPath_.begin(), pythonPath_.end());
        Py_SetProgramName(const_cast<wchar_t *>(wProgram.c_str()));

        // Bei venv: NICHT Py_SetPythonHome setzen!
        if (!isVenv)
        {
            std::wstring wHome(pythonDir.begin(), pythonDir.end());
            Py_SetPythonHome(const_cast<wchar_t *>(wHome.c_str()));
        }

        Py_Initialize();

        if (!Py_IsInitialized())
        {
            throw std::runtime_error("Python-Initialisierung fehlgeschlagen!");
        }

        ErrorLoggerSingleton::instance().logError("✓ Python-Core initialisiert");

        // ========== 5. GIL holen ==========
        PyGILState_STATE gstate = PyGILState_Ensure();

        // ========== 6. sys.path konfigurieren ==========
        PyRun_SimpleString("import sys, os");

        auto addToSysPath = [&](const std::string &path, int index)
        {
            if (!std::filesystem::exists(path))
            {
                return;
            }

            // Normalisiere Pfad für Python
            std::string normalizedPath = std::filesystem::path(path).string();
            
            std::string cmd = "sys.path.insert(" + std::to_string(index) + ", r'" + normalizedPath + "')";
            int result = PyRun_SimpleString(cmd.c_str());
            
            if (result == 0)
            {
                ErrorLoggerSingleton::instance().logError("sys.path[" + std::to_string(index) + "]: " + normalizedPath);
            }
        };

        addToSysPath(scriptPath_, 0);

        // ========== 7. Modul importieren ==========
        PyObject *pModuleName = PyUnicode_FromString(moduleName_.c_str());
        pModule_ = PyImport_Import(pModuleName);
        Py_DECREF(pModuleName);

        if (!pModule_)
        {
            ErrorLoggerSingleton::instance().logError("FEHLER: Modul nicht importierbar!");
            PyErr_Print();
            PyGILState_Release(gstate);
            throw std::runtime_error("Python-Modul konnte nicht geladen werden");
        }

        // ========== 8. FlaskServer-Klasse ==========
        PyObject *pClass = PyObject_GetAttrString(pModule_, "FlaskServer");
        if (!pClass || !PyCallable_Check(pClass))
        {
            PyErr_Print();
            PyGILState_Release(gstate);
            throw std::runtime_error("FlaskServer-Klasse nicht gefunden");
        }

        // ========== 9. Instanz erstellen ==========
        pServerInstance_ = PyObject_CallObject(pClass, NULL);
        Py_DECREF(pClass);

        if (!pServerInstance_)
        {
            PyErr_Print();
            PyGILState_Release(gstate);
            throw std::runtime_error("FlaskServer-Instanziierung fehlgeschlagen");
        }

        // ========== 10. Methoden abrufen ==========
        pStartFn_ = PyObject_GetAttrString(pServerInstance_, startName_.c_str());
        if (!pStartFn_ || !PyCallable_Check(pStartFn_))
        {
            PyErr_Print();
            PyGILState_Release(gstate);
            throw std::runtime_error("start-Methode nicht gefunden");
        }

        pStopFn_ = PyObject_GetAttrString(pServerInstance_, stopName_.c_str());
        if (!pStopFn_ || !PyCallable_Check(pStopFn_))
        {
            PyErr_Print();
            PyGILState_Release(gstate);
            throw std::runtime_error("stop-Methode nicht gefunden");
        }

        // ========== 10b. preview_bridge importieren (Web<->C++-Vermittler) ==========
        {
            PyObject *pBridgeName = PyUnicode_FromString("preview_bridge");
            pBridgeModule_ = PyImport_Import(pBridgeName);
            Py_XDECREF(pBridgeName);
            if (!pBridgeModule_)
            {
                PyErr_Print();
                PyErr_Clear();
                ErrorLoggerSingleton::instance().logError(
                    "WARNUNG: preview_bridge nicht importierbar - Vorschau/SPS-Status inaktiv");
            }
            else
            {
                ErrorLoggerSingleton::instance().logError("✓ preview_bridge importiert");
            }
        }

        // ========== 11. GIL freigeben für Threading! KRITISCH! ==========
        PyGILState_Release(gstate);
        PyEval_SaveThread();  // Erlaubt anderen Threads die GIL zu holen!

        ErrorLoggerSingleton::instance().logError("=== Python-Interpreter erfolgreich initialisiert! ===");
    }
    catch (const std::exception &e)
    {
        ErrorLoggerSingleton::instance().logError("KRITISCHER FEHLER: " + std::string(e.what()));
        throw;
    }
}

void WebServer::finalizeInterpreter()
{
    if (Py_IsInitialized())
    {
        // GIL wieder holen für Cleanup
        PyGILState_STATE g = PyGILState_Ensure();

        Py_XDECREF(pStartFn_);
        Py_XDECREF(pStopFn_);
        Py_XDECREF(pServerInstance_);
        Py_XDECREF(pModule_);
        Py_XDECREF(pBridgeModule_);

        PyGILState_Release(g);
        Py_Finalize();
    }
}

int WebServer::startServer()
{
    ErrorLoggerSingleton::instance().logError(">>> startServer() aufgerufen");
    
    if (running_.load())
    {
        ErrorLoggerSingleton::instance().logError(">>> Server läuft bereits!");
        return -1;
    }

    running_.store(true);
    ErrorLoggerSingleton::instance().logError(">>> Starte Thread...");
    
    serverThread_ = std::thread(&WebServer::runServerLoop, this);
    
    ErrorLoggerSingleton::instance().logError(">>> Thread erstellt, warte 2 Sekunden...");
    std::this_thread::sleep_for(std::chrono::seconds(2));
    ErrorLoggerSingleton::instance().logError(">>> startServer() erfolgreich beendet");

    return 0;
}

void WebServer::runServerLoop()
{
    ErrorLoggerSingleton::instance().logError(">>> runServerLoop GESTARTET");
    
    try
    {
        ErrorLoggerSingleton::instance().logError(">>> Hole GIL...");
        PyGILState_STATE g = PyGILState_Ensure();
        ErrorLoggerSingleton::instance().logError(">>> GIL erhalten");
        
        if (!pStartFn_)
        {
            ErrorLoggerSingleton::instance().logError("❌ pStartFn_ ist NULL!");
            PyGILState_Release(g);
            return;
        }
        
        ErrorLoggerSingleton::instance().logError(">>> Rufe Python start() auf...");
        PyObject *res = PyObject_CallObject(pStartFn_, nullptr);
        
        if (!res)
        {
            ErrorLoggerSingleton::instance().logError("❌ Python start() returnierte NULL");
            PyObject *ptype, *pvalue, *ptraceback;
            PyErr_Fetch(&ptype, &pvalue, &ptraceback);
            
            if (pvalue)
            {
                PyObject* str = PyObject_Str(pvalue);
                const char* errorMsg = PyUnicode_AsUTF8(str);
                ErrorLoggerSingleton::instance().logError(
                    "PYTHON FEHLER: " + std::string(errorMsg ? errorMsg : "Unknown"));
                Py_XDECREF(str);
            }
            
            PyErr_Restore(ptype, pvalue, ptraceback);
            PyErr_Print();
            
            Py_XDECREF(ptype);
            Py_XDECREF(pvalue);
            Py_XDECREF(ptraceback);
        }
        else
        {
            ErrorLoggerSingleton::instance().logError("✅ Flask start() erfolgreich");
            Py_DECREF(res);
        }

        PyGILState_Release(g);
        ErrorLoggerSingleton::instance().logError(">>> runServerLoop BEENDET");
    }
    catch (const std::exception &e)
    {
        ErrorLoggerSingleton::instance().logError("C++ Exception in runServerLoop: " + std::string(e.what()));
    }
}

void WebServer::stopServer()
{
    if (!running_.load())
    {
        return;
    }

    running_.store(false);

    try
    {
        PyGILState_STATE g = PyGILState_Ensure();
        PyObject *res = PyObject_CallObject(pStopFn_, nullptr);
        Py_XDECREF(res);
        PyGILState_Release(g);
    }
    catch (const std::exception &e)
    {
        ErrorLoggerSingleton::instance().logError("Exception: " + std::string(e.what()));
    }

    if (serverThread_.joinable())
    {
        serverThread_.join();
    }
}

int WebServer::loadImg(cv::Mat &img, bool nativeResolution)
{
    if (!pServerInstance_)
    {
        return -1;
    }

    PyGILState_STATE g = PyGILState_Ensure();

    PyObject *pFunc = PyObject_GetAttrString(pServerInstance_, "load_image");
    if (!pFunc || !PyCallable_Check(pFunc))
    {
        PyErr_Print();
        PyGILState_Release(g);
        return -1;
    }

    std::vector<uchar> buffer;
    if (nativeResolution)
    {
        // Vorschau: verlustfrei (PNG) und in Originalauflösung -> scharfes Zoomen.
        cv::imencode(".png", img, buffer);
    }
    else
    {
        // SPS-Live-Ansicht: auf die konfigurierte Anzeigegröße verkleinern (JPEG).
        cv::Mat resizedImg;
        cv::resize(img, resizedImg, cv::Size(webserVerImgSizeX_, webserVerImgSizeY_));
        cv::imencode(".jpg", resizedImg, buffer);
    }

    PyObject *pBytes = PyBytes_FromStringAndSize(
        reinterpret_cast<const char *>(buffer.data()), buffer.size());
    PyObject *pArgs = PyTuple_Pack(1, pBytes);
    PyObject *pResult = PyObject_CallObject(pFunc, pArgs);

    int retVal = -1;
    if (pResult && PyLong_Check(pResult))
    {
        retVal = static_cast<int>(PyLong_AsLong(pResult));
    }

    Py_XDECREF(pResult);
    Py_DECREF(pArgs);
    Py_DECREF(pBytes);
    Py_DECREF(pFunc);
    PyGILState_Release(g);

    return retVal;
}

// ========== Brücke zum Python-Vermittler (preview_bridge) ==========

std::string WebServer::callBridge(const char *func, const std::string *arg)
{
    if (!pBridgeModule_)
    {
        return "";
    }

    PyGILState_STATE g = PyGILState_Ensure();
    std::string result;

    PyObject *pFunc = PyObject_GetAttrString(pBridgeModule_, func);
    if (pFunc && PyCallable_Check(pFunc))
    {
        PyObject *pResult = nullptr;
        if (arg)
        {
            PyObject *pArg = PyUnicode_FromStringAndSize(arg->data(),
                                                         static_cast<Py_ssize_t>(arg->size()));
            PyObject *pArgs = PyTuple_Pack(1, pArg);
            pResult = PyObject_CallObject(pFunc, pArgs);
            Py_XDECREF(pArgs);
            Py_XDECREF(pArg);
        }
        else
        {
            pResult = PyObject_CallObject(pFunc, nullptr);
        }

        if (pResult)
        {
            if (PyUnicode_Check(pResult))
            {
                const char *s = PyUnicode_AsUTF8(pResult);
                if (s)
                {
                    result = s;
                }
            }
            Py_DECREF(pResult);
        }
        else
        {
            PyErr_Print();
            PyErr_Clear();
        }
    }
    Py_XDECREF(pFunc);

    PyGILState_Release(g);
    return result;
}

std::string WebServer::previewPoll()
{
    return callBridge("poll");
}

bool WebServer::previewTakeStop()
{
    return callBridge("take_stop") == "1";
}

void WebServer::previewSetStatus(const std::string &jsonStatus)
{
    callBridge("set_status", &jsonStatus);
}

void WebServer::previewSetProjects(const std::string &jsonProjects)
{
    callBridge("set_projects", &jsonProjects);
}

void WebServer::reportSpsConnected(bool connected)
{
    std::string arg = connected ? "1" : "0";
    callBridge("set_sps", &arg);
}

std::string WebServer::getRenderParamsJson()
{
    return callBridge("get_render_params");
}