#pragma once

#include <atomic>
#include <mutex>
#include <optional>
#include <string>
#include <vector>

#include <nlohmann/json.hpp>

/**
 * @file app_state.h
 * @brief Gemeinsamer Zustand zwischen Webserver-Threads und Arbeits-Threads.
 *
 * Ersetzt die frühere Python-Brücke (src/webVisu/preview_bridge.py). Da jetzt
 * alles nativ im selben Prozess läuft, genügt ein mutex-geschützter Zustand -
 * kein GIL, kein JSON-Marshalling über die Sprachgrenze.
 *
 * Schreiber: Vorschau-Worker und OPC-Thread. Leser: HTTP-Handler.
 */

/// Befehl aus der Web-Oberfläche an den Vorschau-Worker.
struct PreviewCommand
{
    std::string action;   ///< "list" | "render" | "testrun"
    std::string project;
    int intervalMs{0};
};

class AppState
{
public:
    // ---- Bild (PNG-Bytes, wie sie /image ausliefert) ----
    void setImage(std::vector<unsigned char> png);
    std::vector<unsigned char> image() const;
    bool hasImage() const;

    // ---- Befehls-Queue (es zählt immer nur der zuletzt gesetzte Befehl) ----
    void enqueue(PreviewCommand cmd);
    std::optional<PreviewCommand> takeCommand();

    /// Abbruch eines laufenden Testdurchlaufs anfordern bzw. abholen.
    void requestStop();
    bool takeStop();

    // ---- Fortschritt/Status ----
    void setStatus(bool running, int step, int total,
                   const std::string& project, const std::string& message = "");
    nlohmann::json status() const;
    /// Neuen Auftrag markieren (Frontend erkennt daran zuverlässig das Ende).
    void bumpGeneration();

    // ---- Projektliste ----
    void setProjects(const std::string& folder, std::vector<std::string> projects);
    nlohmann::json projects() const;

    // ---- SPS-Verbindung ----
    void setSpsConnected(bool connected) { spsConnected_.store(connected); }
    bool spsConnected() const { return spsConnected_.load(); }
    void setSpsMessage(const std::string& msg);
    std::string spsMessage() const;

    // ---- Live-Diagnose der OPC-Knoten (fuer /api/opc_nodes) ----
    void setNodeDiagnostics(nlohmann::json diag);
    nlohmann::json nodeDiagnostics() const;

    // ---- Laufzeit-Steuerung ----
    void requestShutdown() { shutdown_.store(true); }
    bool shutdownRequested() const { return shutdown_.load(); }

private:
    mutable std::mutex mtx_;

    std::vector<unsigned char> image_;
    std::optional<PreviewCommand> pending_;
    bool stop_{false};

    bool running_{false};
    int step_{0};
    int total_{0};
    std::string project_;
    std::string message_;

    std::string folder_;
    std::vector<std::string> projectList_;

    std::string spsMessage_;
    nlohmann::json nodeDiag_ = nlohmann::json::array();

    std::atomic<long long> gen_{0};
    std::atomic<bool> spsConnected_{false};
    std::atomic<bool> shutdown_{false};
};
