#pragma once

#include <string>
#include <thread>
#include <atomic>
#include <Python.h>
#include <opencv2/opencv.hpp>
#include "ErrorLoggerSingleton.h"


class WebServer {
public:
    WebServer(const std::string& pythonPath = "",  // ← NEU: Python-Pfad als Parameter
            const std::string& scriptPath = "",
              const int webserVerImgSizeX = 500,
              const int webserVerImgSizeY = 1000);
    ~WebServer();

    int startServer();
    void stopServer();
    /// Bild an die Weboberfläche pushen.
    /// nativeResolution=false: auf die SPS-Anzeigegröße verkleinern + JPEG (Live-Ansicht).
    /// nativeResolution=true:  Originalauflösung + PNG (Vorschau, scharfes Zoomen).
    int loadImg(cv::Mat& img, bool nativeResolution = false);

    // ---- Brücke zum Python-Vermittler (preview_bridge) für den C++-Vorschau-Worker ----
    /// Nächsten Web-Befehl abholen (JSON-String, "" = keiner).
    std::string previewPoll();
    /// Stop-Anforderung für einen laufenden Testdurchlauf lesen (+ zurücksetzen).
    bool previewTakeStop();
    /// Fortschritt/Status als JSON melden.
    void previewSetStatus(const std::string& jsonStatus);
    /// Projektliste als JSON melden ({folder, projects[]}).
    void previewSetProjects(const std::string& jsonProjects);
    /// SPS-Verbindungsstatus melden (für die Settings-Seite).
    void reportSpsConnected(bool connected);
    /// Render-Werte (Radius/Farben/Stärken) als JSON aus der Settings-DB holen.
    std::string getRenderParamsJson();

private:
    void initInterpreter();
    void finalizeInterpreter();
    void runServerLoop();
    std::string findPythonPath();  // ← NEU: Sucht Python in Registry

    // Ruft preview_bridge.<func>(optionales str-Argument); gibt einen str oder "" zurück.
    std::string callBridge(const char* func, const std::string* arg = nullptr);

    std::string pythonPath_;
    std::string moduleName_;
    std::string startName_;
    std::string stopName_;
    
    PyObject* pModule_;
    PyObject* pServerInstance_;
    PyObject* pStartFn_;
    PyObject* pStopFn_;
    PyObject* pBridgeModule_;  // preview_bridge-Modul (Web<->C++)
    
    std::atomic<bool> running_;
    std::thread serverThread_;
    
    int webserVerImgSizeX_;
    int webserVerImgSizeY_;
    std::string scriptPath_;
};