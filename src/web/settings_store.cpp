#include "web/settings_store.h"

#include <filesystem>
#include <set>
#include <utility>

#include <sqlite3.h>

// ---------------------------------------------------------------------------
// Schema-Definitionen. Die Daten selbst stammen 1:1 aus der früheren
// settings_db.py und werden generiert (scratchpad/gen_schema.py).
// ---------------------------------------------------------------------------
struct FieldDef
{
    const char* key;
    const char* label;
    const char* type;      // "text" | "int"
    const char* typehint;  // erwarteter Typ (für das ?-Tooltip)
    const char* help;      // Kurzbeschreibung
};

struct GroupDef
{
    const char* name;
    const char* category;  // "Allgemeine Einstellungen" | "OPC-Knoten"
    bool open;             // standardmäßig aufgeklappt
    std::vector<FieldDef> fields;
};

#include "web/settings_schema.inc"

namespace
{
constexpr const char* kCatGeneral = "Allgemeine Einstellungen";
constexpr const char* kCatOpc = "OPC-Knoten";
}  // namespace

// ---------------------------------------------------------------------------

SettingsStore::SettingsStore(std::string dbPath) : dbPath_(std::move(dbPath)) {}

bool SettingsStore::isKnown(const std::string& key)
{
    for (const auto& [k, v] : kDefaults)
    {
        if (k == key)
            return true;
    }
    return false;
}

bool SettingsStore::isLiveKey(const std::string& key)
{
    return kLiveKeys.find(key) != kLiveKeys.end();
}

bool SettingsStore::init()
{
    std::lock_guard<std::mutex> lock(mtx_);

    // Defaults in den Cache legen (Fallback für fehlende DB-Einträge).
    cache_.clear();
    for (const auto& [k, v] : kDefaults)
        cache_[k] = v;

    // Verzeichnis der DB sicherstellen.
    try
    {
        const auto parent = std::filesystem::path(dbPath_).parent_path();
        if (!parent.empty())
            std::filesystem::create_directories(parent);
    }
    catch (const std::exception&)
    {
        // nicht fatal - open() schlägt dann ggf. fehl
    }

    sqlite3* db = nullptr;
    if (sqlite3_open(dbPath_.c_str(), &db) != SQLITE_OK)
    {
        if (db)
            sqlite3_close(db);
        return false;  // ohne DB laufen wir auf reinen Defaults weiter
    }

    // Identisches Schema wie die frühere Python-Version.
    sqlite3_exec(db,
                 "CREATE TABLE IF NOT EXISTS settings ("
                 "key TEXT PRIMARY KEY, value TEXT NOT NULL)",
                 nullptr, nullptr, nullptr);

    // Vorhandene Werte lesen (überschreiben die Defaults).
    sqlite3_stmt* stmt = nullptr;
    if (sqlite3_prepare_v2(db, "SELECT key, value FROM settings", -1, &stmt, nullptr) == SQLITE_OK)
    {
        while (sqlite3_step(stmt) == SQLITE_ROW)
        {
            const unsigned char* k = sqlite3_column_text(stmt, 0);
            const unsigned char* v = sqlite3_column_text(stmt, 1);
            if (k && v)
            {
                const std::string key(reinterpret_cast<const char*>(k));
                // Nur bekannte Schlüssel übernehmen (alte/entfallene ignorieren).
                if (cache_.find(key) != cache_.end())
                    cache_[key] = reinterpret_cast<const char*>(v);
            }
        }
        sqlite3_finalize(stmt);
    }

    // Fehlende Schlüssel seeden (INSERT OR IGNORE wie bisher).
    sqlite3_exec(db, "BEGIN", nullptr, nullptr, nullptr);
    for (const auto& [k, v] : kDefaults)
    {
        sqlite3_stmt* ins = nullptr;
        if (sqlite3_prepare_v2(db, "INSERT OR IGNORE INTO settings (key, value) VALUES (?, ?)",
                               -1, &ins, nullptr) == SQLITE_OK)
        {
            sqlite3_bind_text(ins, 1, k.c_str(), -1, SQLITE_TRANSIENT);
            sqlite3_bind_text(ins, 2, v.c_str(), -1, SQLITE_TRANSIENT);
            sqlite3_step(ins);
            sqlite3_finalize(ins);
        }
    }
    sqlite3_exec(db, "COMMIT", nullptr, nullptr, nullptr);
    sqlite3_close(db);

    rev_.fetch_add(1);
    return true;
}

std::map<std::string, std::string> SettingsStore::getAll() const
{
    std::lock_guard<std::mutex> lock(mtx_);
    return cache_;
}

std::string SettingsStore::get(const std::string& key, const std::string& fallback) const
{
    std::lock_guard<std::mutex> lock(mtx_);
    auto it = cache_.find(key);
    return (it == cache_.end()) ? fallback : it->second;
}

int SettingsStore::getInt(const std::string& key, int fallback) const
{
    try
    {
        const std::string s = get(key);
        return s.empty() ? fallback : std::stoi(s);
    }
    catch (...)
    {
        return fallback;
    }
}

double SettingsStore::getDouble(const std::string& key, double fallback) const
{
    try
    {
        std::string s = get(key);
        if (s.empty())
            return fallback;
        // Dezimalkomma tolerieren
        for (auto& c : s)
        {
            if (c == ',')
                c = '.';
        }
        return std::stod(s);
    }
    catch (...)
    {
        return fallback;
    }
}

bool SettingsStore::writeToDb(const std::map<std::string, std::string>& values)
{
    sqlite3* db = nullptr;
    if (sqlite3_open(dbPath_.c_str(), &db) != SQLITE_OK)
    {
        if (db)
            sqlite3_close(db);
        return false;
    }
    sqlite3_exec(db,
                 "CREATE TABLE IF NOT EXISTS settings ("
                 "key TEXT PRIMARY KEY, value TEXT NOT NULL)",
                 nullptr, nullptr, nullptr);
    sqlite3_exec(db, "BEGIN", nullptr, nullptr, nullptr);
    for (const auto& [k, v] : values)
    {
        sqlite3_stmt* stmt = nullptr;
        if (sqlite3_prepare_v2(db,
                               "INSERT INTO settings (key, value) VALUES (?, ?) "
                               "ON CONFLICT(key) DO UPDATE SET value = excluded.value",
                               -1, &stmt, nullptr) == SQLITE_OK)
        {
            sqlite3_bind_text(stmt, 1, k.c_str(), -1, SQLITE_TRANSIENT);
            sqlite3_bind_text(stmt, 2, v.c_str(), -1, SQLITE_TRANSIENT);
            sqlite3_step(stmt);
            sqlite3_finalize(stmt);
        }
    }
    sqlite3_exec(db, "COMMIT", nullptr, nullptr, nullptr);
    sqlite3_close(db);
    return true;
}

bool SettingsStore::setMany(const std::map<std::string, std::string>& values,
                            std::vector<std::string>* appliedOut)
{
    std::map<std::string, std::string> accepted;
    bool needsReconnect = false;

    {
        std::lock_guard<std::mutex> lock(mtx_);
        for (const auto& [k, v] : values)
        {
            if (cache_.find(k) == cache_.end())
                continue;  // unbekannter Schlüssel -> ignorieren
            if (cache_[k] == v)
                continue;  // unverändert
            cache_[k] = v;
            accepted[k] = v;
            if (!isLiveKey(k))
                needsReconnect = true;
        }
        if (!accepted.empty())
            writeToDb(accepted);
    }

    if (appliedOut)
    {
        for (const auto& [k, v] : accepted)
            appliedOut->push_back(k);
    }
    if (!accepted.empty())
        rev_.fetch_add(1);
    if (needsReconnect)
        bootRev_.fetch_add(1);  // -> OPC-Thread verbindet neu
    return needsReconnect;
}

nlohmann::json SettingsStore::uiSchema() const
{
    const auto current = getAll();

    nlohmann::json general = nlohmann::json::array();
    nlohmann::json opc = nlohmann::json::array();
    std::set<std::string> seen;

    for (const auto& g : kGroups)
    {
        nlohmann::json fields = nlohmann::json::array();
        for (const auto& f : g.fields)
        {
            seen.insert(f.key);
            auto it = current.find(f.key);
            fields.push_back({
                {"key", f.key},
                {"label", f.label},
                {"type", f.type},
                {"typehint", f.typehint},
                {"help", f.help},
                {"value", it == current.end() ? std::string() : it->second},
            });
        }
        nlohmann::json grp = {
            {"name", g.name},
            {"category", g.category},
            {"open", g.open},
            {"fields", fields},
        };
        (std::string(g.category) == kCatGeneral ? general : opc).push_back(grp);
    }

    // Sicherheitsnetz: nicht erfasste Schlüssel anhängen, damit wirklich das
    // komplette Settings-File konfigurierbar bleibt.
    nlohmann::json extra = nlohmann::json::array();
    for (const auto& [k, v] : kDefaults)
    {
        if (seen.find(k) == seen.end())
        {
            auto it = current.find(k);
            extra.push_back({
                {"key", k},
                {"label", k},
                {"type", "int"},
                {"typehint", "Ganzzahl"},
                {"help", ""},
                {"value", it == current.end() ? std::string() : it->second},
            });
        }
    }
    if (!extra.empty())
    {
        opc.push_back({
            {"name", "Sonstige"},
            {"category", kCatOpc},
            {"open", false},
            {"fields", extra},
        });
    }

    // Reihenfolge: erst Allgemeine Einstellungen, dann OPC-Knoten.
    nlohmann::json out = nlohmann::json::array();
    for (auto& g : general)
        out.push_back(g);
    for (auto& g : opc)
        out.push_back(g);
    return out;
}
