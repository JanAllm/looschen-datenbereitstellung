#pragma once

#include <atomic>
#include <map>
#include <mutex>
#include <string>
#include <vector>

#include <nlohmann/json.hpp>

/**
 * @file settings_store.h
 * @brief Native (C++) Einstellungsverwaltung - ersetzt src/webVisu/settings_db.py.
 *
 * Persistenz: dieselbe SQLite-Datei und dasselbe Schema wie bisher
 *   CREATE TABLE settings (key TEXT PRIMARY KEY, value TEXT NOT NULL)
 * Eine bestehende data/settings.db wird dadurch nahtlos weiterverwendet.
 *
 * Die Werte werden zusätzlich im RAM gehalten (cache_), damit Render-/OPC-
 * Threads sie ohne DB-Zugriff sehr häufig lesen können.
 *
 * Thread-safe: alle öffentlichen Methoden sind durch mtx_ geschützt.
 */
class SettingsStore
{
public:
    explicit SettingsStore(std::string dbPath);

    /// Tabelle anlegen und fehlende Schlüssel aus den Defaults ergänzen.
    bool init();

    // ---- Lesen ----
    std::map<std::string, std::string> getAll() const;
    std::string get(const std::string& key, const std::string& fallback = "") const;
    int getInt(const std::string& key, int fallback = 0) const;
    double getDouble(const std::string& key, double fallback = 0.0) const;

    /**
     * @brief Speichert mehrere Werte (nur bekannte Schlüssel).
     * @param values     key -> value
     * @param appliedOut optional: tatsächlich übernommene Schlüssel
     * @return true, wenn mindestens ein NICHT-Live-Schlüssel geändert wurde
     *         (dann ist ein OPC-Reconnect nötig; Live-Werte wirken sofort).
     */
    bool setMany(const std::map<std::string, std::string>& values,
                 std::vector<std::string>* appliedOut = nullptr);

    /// Gruppiertes UI-Schema inkl. aktueller Werte (wie ui_schema() in Python).
    nlohmann::json uiSchema() const;

    /// Ist der Schlüssel überhaupt bekannt (in den Defaults)?
    static bool isKnown(const std::string& key);
    /// Live-Wert = ohne Neustart/Reconnect wirksam (Render-Farben/Stärken).
    static bool isLiveKey(const std::string& key);

    /// Steigt bei JEDER Änderung. Andere Threads erkennen so neue Werte billig.
    long long revision() const { return rev_.load(); }
    /// Steigt nur bei Bootstrap-Änderungen (ipSPS, ns, Knoten-Indizes, Pfade).
    /// Der OPC-Thread verbindet daraufhin neu - Live-Werte lösen das NICHT aus.
    long long bootstrapRevision() const { return bootRev_.load(); }

private:
    std::string dbPath_;
    mutable std::mutex mtx_;
    std::map<std::string, std::string> cache_;
    std::atomic<long long> rev_{0};
    std::atomic<long long> bootRev_{0};

    // Erwartet gehaltenes mtx_.
    bool writeToDb(const std::map<std::string, std::string>& values);
};
