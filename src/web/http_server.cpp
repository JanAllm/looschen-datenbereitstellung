#include "web/http_server.h"

#include <atomic>
#include <cstdio>
#include <filesystem>
#include <fstream>
#include <sstream>
#include <thread>

#include <httplib.h>
#include <nlohmann/json.hpp>

#include "web/app_state.h"
#include "web/settings_store.h"

#ifdef CPPHTTPLIB_OPENSSL_SUPPORT
#include <openssl/evp.h>
#include <openssl/pem.h>
#include <openssl/x509v3.h>
#endif

#ifdef _WIN32
#include <winsock2.h>
#include <ws2tcpip.h>
#else
#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>
#endif

using json = nlohmann::json;

namespace
{

/// Datei komplett einlesen. Leerer String = nicht lesbar.
std::string readFile(const std::filesystem::path& p)
{
    std::ifstream f(p, std::ios::binary);
    if (!f)
        return {};
    std::ostringstream ss;
    ss << f.rdbuf();
    return ss.str();
}

/// Lokale LAN-IP ermitteln (ohne echten Traffic) - für Zertifikat/SAN.
std::string localIp()
{
#ifdef _WIN32
    WSADATA wsa;
    static bool inited = false;
    if (!inited)
    {
        WSAStartup(MAKEWORD(2, 2), &wsa);
        inited = true;
    }
#endif
    std::string ip = "127.0.0.1";
    int s = static_cast<int>(::socket(AF_INET, SOCK_DGRAM, 0));
    if (s < 0)
        return ip;
    sockaddr_in remote{};
    remote.sin_family = AF_INET;
    remote.sin_port = htons(53);
    remote.sin_addr.s_addr = inet_addr("8.8.8.8");
    if (::connect(s, reinterpret_cast<sockaddr*>(&remote), sizeof(remote)) == 0)
    {
        sockaddr_in local{};
#ifdef _WIN32
        int len = sizeof(local);
#else
        socklen_t len = sizeof(local);
#endif
        if (::getsockname(s, reinterpret_cast<sockaddr*>(&local), &len) == 0)
        {
            char buf[INET_ADDRSTRLEN] = {0};
            if (inet_ntop(AF_INET, &local.sin_addr, buf, sizeof(buf)))
                ip = buf;
        }
    }
#ifdef _WIN32
    ::closesocket(s);
#else
    ::close(s);
#endif
    return ip;
}

#ifdef CPPHTTPLIB_OPENSSL_SUPPORT
/// Erzeugt ein selbstsigniertes Zertifikat (ersetzt Python-`cryptography`).
bool generateSelfSignedCert(const std::string& certPath, const std::string& keyPath,
                            const std::string& extraHosts)
{
    EVP_PKEY* pkey = EVP_RSA_gen(2048);
    if (!pkey)
        return false;

    X509* x509 = X509_new();
    if (!x509)
    {
        EVP_PKEY_free(pkey);
        return false;
    }

    X509_set_version(x509, 2);  // v3 (nötig für Extensions)
    ASN1_INTEGER_set(X509_get_serialNumber(x509), 1);
    X509_gmtime_adj(X509_getm_notBefore(x509), -24L * 3600);
    X509_gmtime_adj(X509_getm_notAfter(x509), 365L * 24 * 3600);
    X509_set_pubkey(x509, pkey);

    const std::string ip = localIp();
    // Erste konfigurierte Adresse als CN verwenden - das ist die, unter der die
    // Oberflaeche tatsaechlich aufgerufen wird.
    std::string cn = ip;
    if (!extraHosts.empty())
    {
        const auto comma = extraHosts.find(',');
        cn = (comma == std::string::npos) ? extraHosts : extraHosts.substr(0, comma);
    }
    X509_NAME* name = X509_get_subject_name(x509);
    auto addName = [&](const char* field, const char* value) {
        X509_NAME_add_entry_by_txt(name, field, MBSTRING_ASC,
                                   reinterpret_cast<const unsigned char*>(value), -1, -1, 0);
    };
    addName("C", "DE");
    addName("O", "Hobohtec GmbH");
    addName("CN", cn.c_str());
    X509_set_issuer_name(x509, name);

    std::string san = "DNS:localhost,IP:127.0.0.1";
    if (!ip.empty() && ip != "127.0.0.1")
        san += ",IP:" + ip;
    // Konfigurierte Adressen ergaenzen: rein numerische -> IP, sonst DNS.
    {
        std::string item;
        std::istringstream hs(extraHosts);
        while (std::getline(hs, item, ','))
        {
            while (!item.empty() && (item.front() == ' ' || item.front() == '	')) item.erase(item.begin());
            while (!item.empty() && (item.back() == ' ' || item.back() == '	')) item.pop_back();
            if (item.empty()) continue;
            const bool isIp = item.find_first_not_of("0123456789.") == std::string::npos;
            san += (isIp ? ",IP:" : ",DNS:") + item;
        }
    }
    X509V3_CTX ctx;
    X509V3_set_ctx_nodb(&ctx);
    X509V3_set_ctx(&ctx, x509, x509, nullptr, nullptr, 0);
    if (X509_EXTENSION* ext =
            X509V3_EXT_conf_nid(nullptr, &ctx, NID_subject_alt_name, san.c_str()))
    {
        X509_add_ext(x509, ext, -1);
        X509_EXTENSION_free(ext);
    }

    bool ok = X509_sign(x509, pkey, EVP_sha256()) != 0;

    if (ok)
    {
        if (FILE* f = std::fopen(keyPath.c_str(), "wb"))
        {
            ok = PEM_write_PrivateKey(f, pkey, nullptr, nullptr, 0, nullptr, nullptr) != 0;
            std::fclose(f);
        }
        else
            ok = false;
    }
    if (ok)
    {
        if (FILE* f = std::fopen(certPath.c_str(), "wb"))
        {
            ok = PEM_write_X509(f, x509) != 0;
            std::fclose(f);
        }
        else
            ok = false;
    }

    X509_free(x509);
    EVP_PKEY_free(pkey);
    return ok;
}

/// Zertifikat vorhanden und noch gültig?
bool certValid(const std::string& certPath)
{
    FILE* f = std::fopen(certPath.c_str(), "rb");
    if (!f)
        return false;
    X509* x = PEM_read_X509(f, nullptr, nullptr, nullptr);
    std::fclose(f);
    if (!x)
        return false;
    const bool ok = X509_cmp_current_time(X509_getm_notAfter(x)) > 0;
    X509_free(x);
    return ok;
}
#endif  // CPPHTTPLIB_OPENSSL_SUPPORT

/// Render-Werte (Web) -> JSON-Schlüssel, die die Preview-Seite erwartet.
const std::pair<const char*, const char*> kRenderKeys[] = {
    {"minArcRadius", "previewMinArcRadius"},
    {"colorG0", "previewColorG0"},
    {"colorG1", "previewColorG1"},
    {"colorG2", "previewColorG2"},
    {"colorG3", "previewColorG3"},
    {"thicknessG0", "previewThicknessG0"},
    {"thicknessG1", "previewThicknessG1"},
    {"thicknessG2", "previewThicknessG2"},
    {"thicknessG3", "previewThicknessG3"},
    {"liveColor", "previewLiveColor"},
    {"doneColor", "previewDoneColor"},
    {"liveThickness", "previewLiveThickness"},
    {"doneThickness", "previewDoneThickness"},
};

}  // namespace

// ---------------------------------------------------------------------------

struct HttpServer::Impl
{
    int port;
    std::filesystem::path webRoot;
    SettingsStore& settings;
    AppState& state;

    std::thread thread;
    std::atomic<bool> tls{false};

#ifdef CPPHTTPLIB_OPENSSL_SUPPORT
    std::unique_ptr<httplib::SSLServer> ssl;
#endif
    std::unique_ptr<httplib::Server> plain;

    bool preferTls{true};

    Impl(int p, std::string root, SettingsStore& s, AppState& st, bool tls)
        : port(p), webRoot(std::move(root)), settings(s), state(st), preferTls(tls)
    {
    }

    std::string templatePath(const char* file) const
    {
        return (webRoot / "templates" / file).string();
    }

    /// Registriert alle Routen auf einem beliebigen httplib-Server.
    template <typename Server>
    void routes(Server& svr)
    {
        auto html = [this](const char* file, httplib::Response& res) {
            const std::string body = readFile(templatePath(file));
            if (body.empty())
                res.status = 404;
            else
                res.set_content(body, "text/html; charset=utf-8");
        };

        // Startseite -> Einstellungen (wenn keine Seite angegeben wurde).
        svr.Get("/", [](const httplib::Request&, httplib::Response& res) {
            res.set_redirect("/settings", 302);
        });

        svr.Get("/settings", [html](const httplib::Request&, httplib::Response& res) {
            html("settings.html", res);
        });
        svr.Get("/preview", [html](const httplib::Request&, httplib::Response& res) {
            html("preview.html", res);
        });
        // Bildansicht mit Auto-Refresh (für die SPS-Anzeige).
        svr.Get("/live", [html](const httplib::Request&, httplib::Response& res) {
            html("index.html", res);
        });

        // Aktuelles Bild. /img bleibt als Alias erhalten.
        auto sendImage = [this](const httplib::Request&, httplib::Response& res) {
            auto png = state.image();
            if (png.empty())
            {
                res.status = 503;
                res.set_content("noch kein Bild", "text/plain; charset=utf-8");
                return;
            }
            res.set_content(reinterpret_cast<const char*>(png.data()), png.size(), "image/png");
            res.set_header("Cache-Control", "no-store");
        };
        svr.Get("/image", sendImage);
        svr.Get("/img", sendImage);

        svr.Get("/health", [](const httplib::Request&, httplib::Response& res) {
            res.set_content("OK", "text/plain");
        });

        svr.Get("/status", [this](const httplib::Request&, httplib::Response& res) {
            res.set_content(state.status().dump(), "application/json");
        });

        svr.Post("/shutdown", [this](const httplib::Request&, httplib::Response& res) {
            state.requestShutdown();
            res.set_content("Server shutting down...", "text/plain");
        });

        // ---- Einstellungen ----
        svr.Get("/api/settings", [this](const httplib::Request&, httplib::Response& res) {
            res.set_content(json{{"groups", settings.uiSchema()}}.dump(), "application/json");
        });

        svr.Put("/api/settings", [this](const httplib::Request& req, httplib::Response& res) {
            json body;
            try
            {
                body = json::parse(req.body);
            }
            catch (const std::exception&)
            {
                res.status = 400;
                res.set_content(json{{"error", "ungültiges JSON"}}.dump(), "application/json");
                return;
            }
            std::map<std::string, std::string> values;
            for (auto it = body.begin(); it != body.end(); ++it)
            {
                if (!SettingsStore::isKnown(it.key()))
                    continue;
                values[it.key()] = it.value().is_string()
                                       ? it.value().get<std::string>()
                                       : it.value().dump();
            }
            if (values.empty())
            {
                res.status = 400;
                res.set_content(json{{"error", "Keine gültigen Einstellungen übergeben"}}.dump(),
                                "application/json");
                return;
            }
            settings.setMany(values);
            // Kein Neustart mehr nötig: Bootstrap-Änderungen übernimmt der
            // OPC-Thread selbst (Reconnect), Live-Werte wirken sofort.
            res.set_content(json{{"status", "ok"}, {"restart", false}}.dump(), "application/json");
        });

        // ---- Vorschau ----
        svr.Get("/api/projects", [this](const httplib::Request&, httplib::Response& res) {
            state.enqueue(PreviewCommand{"list", "", 0});
            res.set_content(state.projects().dump(), "application/json");
        });

        auto readProject = [](const httplib::Request& req, std::string& project, int& interval) {
            try
            {
                auto b = json::parse(req.body);
                project = b.value("project", std::string());
                interval = b.value("interval_ms", 500);
            }
            catch (const std::exception&)
            {
                project.clear();
            }
        };

        svr.Post("/api/preview/full",
                 [this, readProject](const httplib::Request& req, httplib::Response& res) {
                     std::string project;
                     int interval = 0;
                     readProject(req, project, interval);
                     if (project.empty())
                     {
                         res.status = 400;
                         res.set_content(json{{"error", "project fehlt"}}.dump(), "application/json");
                         return;
                     }
                     state.enqueue(PreviewCommand{"render", project, 0});
                     res.set_content(json{{"ok", true}}.dump(), "application/json");
                 });

        svr.Post("/api/preview/testrun",
                 [this, readProject](const httplib::Request& req, httplib::Response& res) {
                     std::string project;
                     int interval = 500;
                     readProject(req, project, interval);
                     if (project.empty())
                     {
                         res.status = 400;
                         res.set_content(json{{"error", "project fehlt"}}.dump(), "application/json");
                         return;
                     }
                     if (interval < 0) interval = 0;
                     if (interval > 60000) interval = 60000;
                     state.enqueue(PreviewCommand{"testrun", project, interval});
                     res.set_content(json{{"ok", true}, {"interval_ms", interval}}.dump(),
                                     "application/json");
                 });

        svr.Post("/api/preview/stop", [this](const httplib::Request&, httplib::Response& res) {
            state.requestStop();
            res.set_content(json{{"ok", true}}.dump(), "application/json");
        });

        svr.Get("/api/preview/status", [this](const httplib::Request&, httplib::Response& res) {
            res.set_content(state.status().dump(), "application/json");
        });

        svr.Get("/api/sps_status", [this](const httplib::Request&, httplib::Response& res) {
            res.set_content(json{{"connected", state.spsConnected()},
                                 {"message", state.spsMessage()}}
                                .dump(),
                            "application/json");
        });

        // Live-Diagnose der OPC-Knoten: existiert / lesbar / schreibbar.
        svr.Get("/api/opc_nodes", [this](const httplib::Request&, httplib::Response& res) {
            res.set_content(json{{"connected", state.spsConnected()},
                                 {"message", state.spsMessage()},
                                 {"nodes", state.nodeDiagnostics()}}
                                .dump(),
                            "application/json");
        });

        svr.Get("/api/render_params", [this](const httplib::Request&, httplib::Response& res) {
            json out = json::object();
            for (const auto& [jsonKey, dbKey] : kRenderKeys)
                out[jsonKey] = settings.get(dbKey);
            res.set_content(out.dump(), "application/json");
        });
    }
};

// ---------------------------------------------------------------------------

HttpServer::HttpServer(int port, std::string webRoot, SettingsStore& settings, AppState& state,
                       bool preferTls)
    : impl_(std::make_unique<Impl>(port, std::move(webRoot), settings, state, preferTls))
{
}

HttpServer::~HttpServer()
{
    stop();
}

bool HttpServer::usesTls() const
{
    return impl_->tls.load();
}

std::string HttpServer::url() const
{
    return std::string(impl_->tls.load() ? "https" : "http") + "://localhost:" +
           std::to_string(impl_->port);
}

bool HttpServer::start()
{
#ifdef CPPHTTPLIB_OPENSSL_SUPPORT
    // preferTls=false: bewusst unverschlüsselt (z.B. HMI-Panel, dessen Browser
    // dem selbstsignierten Zertifikat nicht vertraut).
    if (impl_->preferTls)
    {
        // Zertifikat neben der Settings-DB ablegen (persistentes Volume).
        const std::string certPath = (impl_->webRoot / "cert.pem").string();
        const std::string keyPath = (impl_->webRoot / "key.pem").string();
        const std::string hostsPath = (impl_->webRoot / "cert.hosts").string();
        const std::string hosts = impl_->settings.get("webCertHosts");

        // Zertifikat neu erzeugen, wenn es fehlt/abgelaufen ist ODER die
        // konfigurierten Adressen sich geaendert haben (sonst passt der Name nicht).
        const bool hostsChanged = (readFile(hostsPath) != hosts);
        if (!certValid(certPath) || hostsChanged)
        {
            if (generateSelfSignedCert(certPath, keyPath, hosts))
            {
                std::ofstream hf(hostsPath, std::ios::binary);
                hf << hosts;
            }
        }

        if (certValid(certPath))
        {
            impl_->ssl = std::make_unique<httplib::SSLServer>(certPath.c_str(), keyPath.c_str());
            if (impl_->ssl->is_valid())
            {
                impl_->routes(*impl_->ssl);
                impl_->tls.store(true);
                impl_->thread = std::thread([this] { impl_->ssl->listen("0.0.0.0", impl_->port); });
                return true;
            }
            impl_->ssl.reset();  // ungültig -> HTTP-Fallback
        }
    }
#endif

    impl_->plain = std::make_unique<httplib::Server>();
    impl_->routes(*impl_->plain);
    impl_->tls.store(false);
    impl_->thread = std::thread([this] { impl_->plain->listen("0.0.0.0", impl_->port); });
    return true;
}

void HttpServer::stop()
{
    if (!impl_)
        return;
#ifdef CPPHTTPLIB_OPENSSL_SUPPORT
    if (impl_->ssl)
        impl_->ssl->stop();
#endif
    if (impl_->plain)
        impl_->plain->stop();
    if (impl_->thread.joinable())
        impl_->thread.join();
}
