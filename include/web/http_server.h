#pragma once

#include <memory>
#include <string>

class AppState;
class SettingsStore;

/**
 * @file http_server.h
 * @brief Nativer HTTP(S)-Server - ersetzt Flask (src/webVisu/flask_app.py).
 *
 * Bietet exakt dieselben Endpunkte wie vorher, damit das bestehende Frontend
 * unverändert weiterläuft:
 *   GET  /                     -> Weiterleitung auf /settings
 *   GET  /live                 -> Bildansicht (auto-refresh, für die SPS-Anzeige)
 *   GET  /settings /preview    -> HTML-Seiten
 *   GET  /image  /img          -> aktuelles PNG
 *   GET  /health /status
 *   POST /shutdown
 *   GET|PUT /api/settings
 *   GET  /api/projects
 *   POST /api/preview/full | /api/preview/testrun | /api/preview/stop
 *   GET  /api/preview/status | /api/sps_status | /api/render_params
 *
 * HTTPS wird genutzt, wenn mit OpenSSL gebaut wurde (CPPHTTPLIB_OPENSSL_SUPPORT);
 * ein selbstsigniertes Zertifikat wird bei Bedarf selbst erzeugt. Ohne OpenSSL
 * läuft der Server als HTTP.
 */
class HttpServer
{
public:
    /**
     * @param port     Listen-Port (z.B. 5000)
     * @param webRoot  Verzeichnis mit dem Unterordner templates/
     * @param settings Einstellungs-Store (wird gelesen und geschrieben)
     * @param state    gemeinsamer Zustand (Bild, Vorschau, SPS-Status)
     */
    /// @param preferTls false = immer HTTP (z.B. fuer HMI-Panels ohne Zertifikatsvertrauen)
    HttpServer(int port, std::string webRoot, SettingsStore& settings, AppState& state,
               bool preferTls = true);
    ~HttpServer();

    HttpServer(const HttpServer&) = delete;
    HttpServer& operator=(const HttpServer&) = delete;

    /// Startet den Server in einem eigenen Thread. false = Start fehlgeschlagen.
    bool start();
    /// Stoppt den Server und wartet auf den Thread.
    void stop();

    /// true, wenn TLS aktiv ist (mit OpenSSL gebaut und Zertifikat vorhanden).
    bool usesTls() const;
    /// Basis-URL für Logausgaben, z.B. "https://localhost:5000".
    std::string url() const;

private:
    struct Impl;
    std::unique_ptr<Impl> impl_;
};
