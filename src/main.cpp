#include <iomanip>
#include <filesystem>
#include <cstdlib>
#include <future>
#include <stdio.h>
#include <chrono>
#include <vector>
#include <sstream>
#include <atomic>
#include <csignal>
#include <mutex>
#include <clocale>

// ==================Eigene Header================
#include "ErrorLoggerSingleton.h"
#include "interfaces/SPSController.h"
#include "data_structs.h"
#include "interfaces/setupVarInfos.h"
#include "web/http_server.h"
#include "web/app_state.h"
#include "web/settings_store.h"
#include "web/image_sink.h"
#include "path_utils.h"
#include "load_settings.h"
#include "CNC_Daten/cnc_projekt_manager.h"
#include "CNC_Daten/arc_flatten.h"
#include <nlohmann/json.hpp>

#ifdef _WIN32
#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <timeapi.h>  // timeBeginPeriod / timeEndPeriod (winmm)
#else
#include <netdb.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <fcntl.h>
#include <unistd.h>
#endif

namespace fs = std::filesystem;
using json = nlohmann::json;

// =================== Globale Variablen ===================
std::string ProgrammId = "";
std::string ipOPC = "opc.tcp://CP-5E1BE0:4840";
std::string folderPath = "";
setupVarInfos setupVars;
int MaximaleAnzahlProjekte = 10;
int lenDataArray = 5;
int sizeObjx = 1000;
int sizeObjy = 2000;
int webserVerImgSizeX = 500;
int webserVerImgSizeY = 1000;
// Vorschau (ohne SPS) rendert in hoher Auflösung, damit man scharf zoomen kann.
// Unabhängig von der SPS-Live-Ansicht (webserVerImgSize*). Seitenverhältnis 1:2.
int previewImgSizeX = 2000;
int previewImgSizeY = 4000;
// Verzeichnis mit templates/ (und cert.pem/key.pem) fuer den Webserver.
std::string webRoot = "";
int webPort = 5000;

// Cross-Platform Shutdown-Signal
std::atomic<bool> g_running{true};
std::mutex g_instance_mutex;
bool g_instance_running = false;

// SPS-Verbindungsstatus (für die Web-Anzeige)
std::atomic<bool> g_sps_connected{false};

// Signal Handler (Cross-Platform)
void signalHandler(int signal)
{
    ErrorLoggerSingleton::instance().logError(
        "Signal empfangen: " + std::to_string(signal) + " - Beende Programm...");
    g_running.store(false);
}

// =================== SPS Verbindung ===================
// =================== Vorschau-Worker (rendert mit C++, ohne SPS) ===================

// Auftrags-Generation: pro Render/Testdurchlauf erhöht, damit das Frontend das
// Ende auch sehr schneller Rendervorgänge zuverlässig erkennt.
static std::atomic<long> s_previewGen{0};

// Meldet den Fortschritt an die Weboberflaeche.
static void previewStatus(AppState &state, bool running, int step, int total,
                          const std::string &project, const std::string &msg = "")
{
    state.setStatus(running, step, total, project, msg);
}

// Skaliert/verschiebt die Befehle so, dass der gesamte Werkzeugweg in die Canvas
// passt. Vorgehen: Koordinaten mit `scale` multiplizieren und die Translation über
// die (vom Renderer addierten) Offsets abbilden -> der Renderer-Startpunkt (0,0)
// landet korrekt am transformierten Ursprung.
static std::vector<GCodeParser::Command>
fitCommands(const std::vector<GCodeParser::Command> &cmds, int W, int H,
            double margin, int &hOffOut, int &vOffOut)
{
    hOffOut = 0;
    vOffOut = 0;
    if (cmds.empty())
        return cmds;

    double minx = 0.0, maxx = 0.0, miny = 0.0, maxy = 0.0;  // (0,0) = Renderer-Start
    for (const auto &c : cmds)
    {
        minx = std::min(minx, c.endPosition.x);
        maxx = std::max(maxx, c.endPosition.x);
        miny = std::min(miny, c.endPosition.y);
        maxy = std::max(maxy, c.endPosition.y);
    }
    double spanx = std::max(1.0, maxx - minx);
    double spany = std::max(1.0, maxy - miny);
    double scale = std::min((W - 2 * margin) / spanx, (H - 2 * margin) / spany);
    if (scale <= 0.0)
        scale = 1.0;

    double centerX = (W - spanx * scale) / 2.0;
    double centerY = (H - spany * scale) / 2.0;
    hOffOut = static_cast<int>(std::llround(centerX - minx * scale));
    vOffOut = static_cast<int>(std::llround(centerY - miny * scale));

    std::vector<GCodeParser::Command> out = cmds;
    for (auto &c : out)
    {
        c.endPosition.x *= scale;
        c.endPosition.y *= scale;
        c.arcCenter.x *= scale;  // relativer Offset -> nur skalieren
        c.arcCenter.y *= scale;
    }
    return out;
}

// Glättet kleine Bögen auf Command-Ebene in ECHTEN mm - identisch zur an die SPS
// gesendeten Logik (arc_flatten.h). MUSS VOR fitCommands laufen, damit der Radius
// unskaliert (mm) mit dem Schwellwert verglichen wird und nicht im Pixelraum.
// Ein geglätteter G02/G03 wird zu G01 -> renderCommand zeichnet ihn danach in
// der Linien-Farbe/-Stärke (G01), so wie die zur SPS gesendeten G01-Daten.
static std::vector<GCodeParser::Command>
flattenCommandsMM(const std::vector<GCodeParser::Command> &cmds, double thresholdMM)
{
    if (thresholdMM <= 0.0)
        return cmds;
    using CT = GCodeParser::CommandType;
    std::vector<GCodeParser::Command> out = cmds;
    for (auto &c : out)
    {
        if (c.type == CT::G02 || c.type == CT::G03)
        {
            const double r = arc_flatten::radiusFromIJ(c.arcCenter.x, c.arcCenter.y);
            if (arc_flatten::shouldFlatten(r, thresholdMM))
                c.type = CT::G01;  // wird ab jetzt als Linie behandelt
        }
    }
    return out;
}

// "R,G,B" (0-255) -> cv::Scalar in OpenCV-BGR-Reihenfolge.
static cv::Scalar parseColorRGB(const std::string &s, const cv::Scalar &fallback)
{
    int vals[3];
    int n = 0;
    std::stringstream ss(s);
    std::string tok;
    while (n < 3 && std::getline(ss, tok, ','))
    {
        try
        {
            vals[n++] = std::stoi(tok);
        }
        catch (...)
        {
            return fallback;
        }
    }
    if (n != 3)
        return fallback;
    return cv::Scalar(vals[2], vals[1], vals[0]);  // R,G,B -> B,G,R
}

// Bootstrap-Werte aus dem Settings-Store uebernehmen. Ersetzt das fruehere
// Lesen der config.txt - die Datenbank ist jetzt die einzige Quelle.
static void applySettings(const SettingsStore &s)
{
    ipOPC = s.get("ipSPS", ipOPC);
    ProgrammId = s.get("ProgrammId", ProgrammId);
    setupVars.ns = s.getInt("ns", setupVars.ns);
    folderPath = s.get("folderPath", folderPath);
    sizeObjx = s.getInt("sizeObjx", sizeObjx);
    sizeObjy = s.getInt("sizeObjy", sizeObjy);
    webserVerImgSizeX = s.getInt("webserVerImgSizeX", webserVerImgSizeX);
    webserVerImgSizeY = s.getInt("webserVerImgSizeY", webserVerImgSizeY);
    setupVars.iError = s.getInt("iError", setupVars.iError);
    setupVars.iInfo = s.getInt("iInfo", setupVars.iInfo);
    setupVars.iHeartbeat = s.getInt("iHeartbeat", setupVars.iHeartbeat);
    setupVars.iStatusCode = s.getInt("iStatusCode", setupVars.iStatusCode);
    setupVars.iDataArry = s.getInt("iDataArry", setupVars.iDataArry);
    setupVars.iProjektArray = s.getInt("iProjektArray", setupVars.iProjektArray);
    setupVars.iUpdateProjektList = s.getInt("iUpdateProjektList", setupVars.iUpdateProjektList);
    setupVars.iProjektlistUpdated = s.getInt("iProjektlistUpdated", setupVars.iProjektlistUpdated);
    setupVars.iProjektName = s.getInt("iProjektName", setupVars.iProjektName);
    setupVars.iReadProjekt = s.getInt("iReadProjekt", setupVars.iReadProjekt);
    setupVars.iLenProjekt = s.getInt("iLenProjekt", setupVars.iLenProjekt);
    setupVars.iAnzDatenblocke = s.getInt("iAnzDatenblöcke", setupVars.iAnzDatenblocke);
    setupVars.iWriteData = s.getInt("iWriteData", setupVars.iWriteData);
    setupVars.iIndexData = s.getInt("iIndexData", setupVars.iIndexData);
    setupVars.iReadData = s.getInt("iReadData", setupVars.iReadData);
    MaximaleAnzahlProjekte = s.getInt("MaximaleAnzahlProjekte", MaximaleAnzahlProjekte);
    lenDataArray = s.getInt("LenDataArray", lenDataArray);
    setupVars.iabbruchUbertragen = s.getInt("iabbruchUbertragen", setupVars.iabbruchUbertragen);
    setupVars.ishowProjektInfo = s.getInt("showProjektInfo", setupVars.ishowProjektInfo);
    setupVars.iProjektInfoWrote = s.getInt("ProjektInfoWrote", setupVars.iProjektInfoWrote);
    setupVars.iProjektVorhanden = s.getInt("ProjektVorhanden", setupVars.iProjektVorhanden);
    setupVars.iUebertgarungBeendet = s.getInt("iUebertgarungBeendet", setupVars.iUebertgarungBeendet);
    setupVars.iUebertragungslaege = s.getInt("iUebertragungslaege", setupVars.iUebertragungslaege);
    setupVars.icolorG0 = s.getInt("iFarbeG0", setupVars.icolorG0);
    setupVars.istreghtsG0 = s.getInt("iStaerkeG0", setupVars.istreghtsG0);
    setupVars.icolorG1 = s.getInt("iFarbeG1", setupVars.icolorG1);
    setupVars.istreghtsG1 = s.getInt("iStaerkeG1", setupVars.istreghtsG1);
    setupVars.icolorG2 = s.getInt("iFarbeG2", setupVars.icolorG2);
    setupVars.istreghtsG2 = s.getInt("iStaerkeG2", setupVars.istreghtsG2);
    setupVars.icolorG3 = s.getInt("iFarbeG3", setupVars.icolorG3);
    setupVars.istreghtsG3 = s.getInt("iStaerkeG3", setupVars.istreghtsG3);
    setupVars.objektgroesseX = s.getInt("objektgroesseX", setupVars.objektgroesseX);
    setupVars.objektgroesseY = s.getInt("objektgroesseY", setupVars.objektgroesseY);
    setupVars.iFarbeLive = s.getInt("iFarbeLive", setupVars.iFarbeLive);
    setupVars.iFarbeErledigt = s.getInt("iFarbeErledigt", setupVars.iFarbeErledigt);
    setupVars.iStaerkeLive = s.getInt("iStaerkeLive", setupVars.iStaerkeLive);
    setupVars.iStaerkeErledigt = s.getInt("iStaerkeErledigt", setupVars.iStaerkeErledigt);
    setupVars.iLiveStand = s.getInt("iLiveStand", setupVars.iLiveStand);
    setupVars.iLiveAbbruch = s.getInt("iLiveBreak", setupVars.iLiveAbbruch);
    setupVars.iMinArcRadius = s.getInt("iMinArcRadius", setupVars.iMinArcRadius);
}

// Mirror image of applySettings: same keys, but read back from the runtime
// variables instead of the database. This is what /api/active_settings serves,
// so the settings page can show stored and in-use values side by side.
//
// managerBlockSize >= 0 overrides LenDataArray with the value the running
// ProjectManager actually holds. That is exactly where the block size drifted
// apart in the field report of 20.07.2026: the database said 100 while the
// manager still worked with the 5 it had been constructed with.
static nlohmann::json activeSettingsJson(int managerBlockSize = -1)
{
    nlohmann::json v = nlohmann::json::object();
    auto put = [&v](const char *key, int value) { v[key] = std::to_string(value); };

    v["ipSPS"] = ipOPC;
    v["ProgrammId"] = ProgrammId;
    put("ns", setupVars.ns);
    v["folderPath"] = folderPath;
    put("sizeObjx", sizeObjx);
    put("sizeObjy", sizeObjy);
    put("webserVerImgSizeX", webserVerImgSizeX);
    put("webserVerImgSizeY", webserVerImgSizeY);
    put("iError", setupVars.iError);
    put("iInfo", setupVars.iInfo);
    put("iHeartbeat", setupVars.iHeartbeat);
    put("iStatusCode", setupVars.iStatusCode);
    put("iDataArry", setupVars.iDataArry);
    put("iProjektArray", setupVars.iProjektArray);
    put("iUpdateProjektList", setupVars.iUpdateProjektList);
    put("iProjektlistUpdated", setupVars.iProjektlistUpdated);
    put("iProjektName", setupVars.iProjektName);
    put("iReadProjekt", setupVars.iReadProjekt);
    put("iLenProjekt", setupVars.iLenProjekt);
    put("iAnzDatenblöcke", setupVars.iAnzDatenblocke);
    put("iWriteData", setupVars.iWriteData);
    put("iIndexData", setupVars.iIndexData);
    put("iReadData", setupVars.iReadData);
    put("MaximaleAnzahlProjekte", MaximaleAnzahlProjekte);
    put("LenDataArray", managerBlockSize >= 0 ? managerBlockSize : lenDataArray);
    put("iabbruchUbertragen", setupVars.iabbruchUbertragen);
    put("showProjektInfo", setupVars.ishowProjektInfo);
    put("ProjektInfoWrote", setupVars.iProjektInfoWrote);
    put("ProjektVorhanden", setupVars.iProjektVorhanden);
    put("iUebertgarungBeendet", setupVars.iUebertgarungBeendet);
    put("iUebertragungslaege", setupVars.iUebertragungslaege);
    put("iFarbeG0", setupVars.icolorG0);
    put("iStaerkeG0", setupVars.istreghtsG0);
    put("iFarbeG1", setupVars.icolorG1);
    put("iStaerkeG1", setupVars.istreghtsG1);
    put("iFarbeG2", setupVars.icolorG2);
    put("iStaerkeG2", setupVars.istreghtsG2);
    put("iFarbeG3", setupVars.icolorG3);
    put("iStaerkeG3", setupVars.istreghtsG3);
    put("objektgroesseX", setupVars.objektgroesseX);
    put("objektgroesseY", setupVars.objektgroesseY);
    put("iFarbeLive", setupVars.iFarbeLive);
    put("iFarbeErledigt", setupVars.iFarbeErledigt);
    put("iStaerkeLive", setupVars.iStaerkeLive);
    put("iStaerkeErledigt", setupVars.iStaerkeErledigt);
    put("iLiveStand", setupVars.iLiveStand);
    put("iLiveBreak", setupVars.iLiveAbbruch);
    put("iMinArcRadius", setupVars.iMinArcRadius);
    return v;
}

// Render-Werte aus der Settings-DB als JSON (ersetzt die fruehere Python-Bruecke).
static std::string renderParamsJson(const SettingsStore &s)
{
    static const std::pair<const char *, const char *> keys[] = {
        {"minArcRadius", "previewMinArcRadius"},
        {"colorG0", "previewColorG0"},   {"colorG1", "previewColorG1"},
        {"colorG2", "previewColorG2"},   {"colorG3", "previewColorG3"},
        {"thicknessG0", "previewThicknessG0"}, {"thicknessG1", "previewThicknessG1"},
        {"thicknessG2", "previewThicknessG2"}, {"thicknessG3", "previewThicknessG3"},
        {"liveColor", "previewLiveColor"},     {"doneColor", "previewDoneColor"},
        {"liveThickness", "previewLiveThickness"}, {"doneThickness", "previewDoneThickness"},
    };
    json j;
    for (const auto &[jk, dk] : keys)
        j[jk] = s.get(dk);
    return j.dump();
}

static void previewWorkerLoop(AppState *statePtr, SettingsStore *settingsPtr,
                              std::string folderPath,
                              int sizeX, int sizeY, int maxProjects)
{
    AppState &state = *statePtr;
    const SettingsStore &settings = *settingsPtr;

    ProjectFileReader reader(folderPath);
    PreviewRenderer renderer;
    GCodeParser parser;

    auto listProjects = [&]()
    {
        json out;
        out["folder"] = folderPath;
        out["projects"] = json::array();
        try
        {
            for (const auto &f : reader.listProjects(maxProjects))
                out["projects"].push_back(f);
        }
        catch (const std::exception &)
        {
        }
        state.setProjects(out["folder"].get<std::string>(),
                          out["projects"].get<std::vector<std::string>>());
    };

    // Render-Parameter FRISCH aus der Settings-DB holen (web-editierbar, ohne SPS).
    auto makeParams = [&](int hOff, int vOff)
    {
        RenderParams p;
        p.imageSizeX = sizeX;
        p.imageSizeY = sizeY;
        p.horizontalOffset = hOff;
        p.verticalOffset = vOff;
        p.drawReferenceAxis = false;  // ohne SPS keine sinnvolle Achse
        p.antialiased = false;        // Vorschau: schnell (ohne Kantenglättung)

        std::string js = renderParamsJson(settings);
        if (!js.empty())
        {
            try
            {
                json j = json::parse(js);
                auto getd = [&](const char *k, double d)
                { try { return std::stod(j.value(k, std::string())); } catch (...) { return d; } };
                auto geti = [&](const char *k, int d)
                { try { return std::stoi(j.value(k, std::string())); } catch (...) { return d; } };

                p.minArcRadius = getd("minArcRadius", 0.0);
                p.colorG0 = parseColorRGB(j.value("colorG0", std::string()), p.colorG0);
                p.colorG1 = parseColorRGB(j.value("colorG1", std::string()), p.colorG1);
                p.colorG2 = parseColorRGB(j.value("colorG2", std::string()), p.colorG2);
                p.colorG3 = parseColorRGB(j.value("colorG3", std::string()), p.colorG3);
                p.thicknessG0 = geti("thicknessG0", p.thicknessG0);
                p.thicknessG1 = geti("thicknessG1", p.thicknessG1);
                p.thicknessG2 = geti("thicknessG2", p.thicknessG2);
                p.thicknessG3 = geti("thicknessG3", p.thicknessG3);
                // Fortschritts-Farben (Testdurchlauf): aktuell + erledigt.
                p.liveColor = parseColorRGB(j.value("liveColor", std::string()), p.liveColor);
                p.doneColor = parseColorRGB(j.value("doneColor", std::string()), p.doneColor);
                p.liveThickness = geti("liveThickness", p.liveThickness);
                p.doneThickness = geti("doneThickness", p.doneThickness);
            }
            catch (...)
            {
            }
        }
        return p;
    };

    listProjects();  // beim Start einmal listen

    while (g_running.load())
    {
        state.setSpsConnected(g_sps_connected.load());

        auto cmdOpt = state.takeCommand();
        if (!cmdOpt)
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(30));
            continue;
        }
        const std::string action = cmdOpt->action;

        if (action == "list")
        {
            listProjects();
        }
        else if (action == "render" || action == "testrun")
        {
            const std::string project = cmdOpt->project;
            auto infoOpt = reader.getProjectInfo(project);
            if (!infoOpt)
            {
                previewStatus(state, false, 0, 0, project, "Projekt nicht gefunden");
                continue;
            }

            state.bumpGeneration();  // neuer Auftrag (für die Frontend-Fertig-Erkennung)

            auto parseResult = parser.parseFile(infoOpt->filePath.string());

            // Bogenglättung in ECHTEN mm auf Command-Ebene (vor der Skalierung),
            // exakt wie die an die SPS gesendeten Daten. thrMM = Web-Wert (mm).
            const double thrMM = makeParams(0, 0).minArcRadius;
            auto flattened = flattenCommandsMM(parseResult.commands, thrMM);

            int hOff = 0, vOff = 0;
            auto commands = fitCommands(flattened, sizeX, sizeY, 40.0, hOff, vOff);
            RenderParams params = makeParams(hOff, vOff);
            params.minArcRadius = 0.0;  // Glättung bereits (in mm) erledigt -> im
                                        // Renderer nicht nochmal (skaliert) glätten
            int total = static_cast<int>(commands.size());

            // Persistente (ungespiegelte) Canvas in Anzeigegröße; für die Anzeige
            // wird eine gespiegelte Kopie geschoben (wie enableMirroring im Renderer).
            cv::Mat canvas(cv::Size(sizeX, sizeY), CV_8UC3, cv::Scalar(0, 0, 0));
            cv::Point2d pos(0.0, 0.0);
            // full=true: native Auflösung (scharf, für Endbild/„Jetzt rendern").
            // full=false: halbe Auflösung -> ~4x billiger zu kodieren/übertragen,
            // damit die Testlauf-Animation flüssig (viele Frames) bleibt.
            auto pushMirrored = [&](bool full = true)
            {
                cv::Mat disp;
                cv::flip(canvas, disp, 0);
                if (full)
                {
                    publishImage(state, disp);
                }
                else
                {
                    publishImageHalf(state, disp);
                }
            };

            if (action == "render")
            {
                previewStatus(state, true, 0, total, project, "Rendere ...");
                renderer.renderRange(canvas, commands, 0,
                                     static_cast<std::size_t>(total), pos, params);
                pushMirrored();
                previewStatus(state, false, total, total, project);
            }
            else  // testrun - Fortschritts-Sweep (wie SPS-Live: erledigt/aktuell/offen)
            {
                int interval = cmdOpt->intervalMs;
                state.takeStop();  // Stop-Flag zurücksetzen
                previewStatus(state, true, 0, total, project);
                bool stopped = false;

                // 1) Basisbild: ALLE Befehle in Normalfarbe (offen) + Startposition
                //    jedes Befehls merken (für das spätere Einzel-Umfärben).
                std::vector<cv::Point2d> starts(static_cast<std::size_t>(std::max(0, total)));
                {
                    RenderParams base = params;
                    base.currentRecord = -1;
                    cv::Point2d p(0.0, 0.0);
                    for (int k = 0; k < total; ++k)
                    {
                        starts[static_cast<std::size_t>(k)] = p;
                        p = renderer.renderRange(canvas, commands,
                                                 static_cast<std::size_t>(k),
                                                 static_cast<std::size_t>(k + 1), p, params);
                    }
                }
                pushMirrored();  // Vollbild (alles offen) sofort zeigen

                // 2) Sweep: aktuelle Position hervorheben, Trail als erledigt.
                RenderParams liveP = params;
                RenderParams doneP = params;
                int prevK = -1;
                int lastK = 0;
                // Der Bild-Push (2000x4000 PNG) ist teuer (~50-100 ms). Nicht jeden
                // Schritt pushen, sondern ZEITGETAKTET (~7/s). So bestimmt das
                // Intervall das Tempo, nicht die Bildübertragung. Bei großem
                // Intervall (Zeitlupe) pusht die Zeitschranke ohnehin jeden Schritt.
                // Schritt-Fallback (stepChunk): im Schnellmodus (Intervall 0) greift
                // die Zeitschranke nie (µs-Schritte) -> dann ~40 Frames über die
                // Schrittzahl, sonst sähe man nur das Endbild.
                auto lastPush = std::chrono::steady_clock::now();
                int lastPushK = 0;
                // Halbauflösende Sweep-Frames sind ~4x billiger -> dichter takten
                // (~11/s) für eine flüssige Animation.
                const auto pushEvery = std::chrono::milliseconds(90);
                const int stepChunk = std::max(1, total / 60);

                for (int k = 0; k < total && g_running.load() && !stopped; ++k)
                {
                    if (prevK >= 0)  // vorherige Position -> erledigt
                    {
                        doneP.currentRecord = commands[static_cast<std::size_t>(prevK)].recordIndex + 1;
                        renderer.renderRange(canvas, commands,
                                             static_cast<std::size_t>(prevK),
                                             static_cast<std::size_t>(prevK + 1),
                                             starts[static_cast<std::size_t>(prevK)], doneP);
                    }
                    // aktuelle Position -> Highlight
                    liveP.currentRecord = commands[static_cast<std::size_t>(k)].recordIndex;
                    renderer.renderRange(canvas, commands,
                                         static_cast<std::size_t>(k),
                                         static_cast<std::size_t>(k + 1),
                                         starts[static_cast<std::size_t>(k)], liveP);
                    prevK = k;
                    lastK = k + 1;

                    auto now = std::chrono::steady_clock::now();
                    if (now - lastPush >= pushEvery || (k - lastPushK) >= stepChunk)
                    {
                        pushMirrored(false);  // Animationsframe: halbe Auflösung
                        previewStatus(state, true, k + 1, total, project);
                        lastPush = now;
                        lastPushK = k;
                    }

                    if (interval > 0)
                    {
                        int slept = 0;
                        while (slept < interval && g_running.load())
                        {
                            if (state.takeStop()) { stopped = true; break; }
                            int chunk = std::min(50, interval - slept);
                            std::this_thread::sleep_for(std::chrono::milliseconds(chunk));
                            slept += chunk;
                        }
                    }
                    else if ((k % 128) == 0 && state.takeStop())
                    {
                        stopped = true;  // Schnellmodus: nur selten auf Stop prüfen
                    }
                }

                // Normal beendet: auch die letzte Position als erledigt einfärben.
                if (!stopped && prevK >= 0)
                {
                    doneP.currentRecord = commands[static_cast<std::size_t>(prevK)].recordIndex + 1;
                    renderer.renderRange(canvas, commands,
                                         static_cast<std::size_t>(prevK),
                                         static_cast<std::size_t>(prevK + 1),
                                         starts[static_cast<std::size_t>(prevK)], doneP);
                }

                pushMirrored();  // Endstand sicher anzeigen
                previewStatus(state, false, lastK, total, project,
                              stopped ? "Gestoppt" : "");
            }
        }
    }
}

// Schnelle Erreichbarkeitspruefung (TCP) fuer "opc.tcp://host:port".
// Grund: UA_Client_connect() blockiert bei nicht erreichbarem Endpunkt sehr
// lange. Mit dieser Vorpruefung bleibt der SPS-Thread reaktionsfaehig und
// bemerkt Einstellungsaenderungen zeitnah.
static bool endpointReachable(const std::string &url, int timeoutMs)
{
    std::string rest = url;
    const auto scheme = rest.find("://");
    if (scheme != std::string::npos)
        rest = rest.substr(scheme + 3);
    const auto slash = rest.find('/');
    if (slash != std::string::npos)
        rest = rest.substr(0, slash);
    std::string host = rest;
    std::string port = "4840";
    const auto colon = rest.rfind(':');
    if (colon != std::string::npos)
    {
        host = rest.substr(0, colon);
        port = rest.substr(colon + 1);
    }
    if (host.empty())
        return false;

    addrinfo hints{};
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    addrinfo *res = nullptr;
    if (::getaddrinfo(host.c_str(), port.c_str(), &hints, &res) != 0 || !res)
        return false;  // Name nicht aufloesbar

    bool ok = false;
#ifdef _WIN32
    SOCKET s = ::socket(res->ai_family, res->ai_socktype, res->ai_protocol);
    if (s != INVALID_SOCKET)
    {
        u_long nb = 1;
        ::ioctlsocket(s, FIONBIO, &nb);
        ::connect(s, res->ai_addr, static_cast<int>(res->ai_addrlen));
        fd_set wf;
        FD_ZERO(&wf);
        FD_SET(s, &wf);
        timeval tv{timeoutMs / 1000, (timeoutMs % 1000) * 1000};
        ok = ::select(0, nullptr, &wf, nullptr, &tv) > 0;
        ::closesocket(s);
    }
#else
    int s = ::socket(res->ai_family, res->ai_socktype, res->ai_protocol);
    if (s >= 0)
    {
        int flags = ::fcntl(s, F_GETFL, 0);
        ::fcntl(s, F_SETFL, flags | O_NONBLOCK);
        ::connect(s, res->ai_addr, res->ai_addrlen);
        fd_set wf;
        FD_ZERO(&wf);
        FD_SET(s, &wf);
        timeval tv{timeoutMs / 1000, (timeoutMs % 1000) * 1000};
        if (::select(s + 1, nullptr, &wf, nullptr, &tv) > 0)
        {
            int err = 0;
            socklen_t len = sizeof(err);
            ::getsockopt(s, SOL_SOCKET, SO_ERROR, &err, &len);
            ok = (err == 0);
        }
        ::close(s);
    }
#endif
    ::freeaddrinfo(res);
    return ok;
}

// =================== SPS-Worker (eigener Thread) ===================
// Verbindet, registriert die Knoten und faehrt die Betriebsschleife.
// Robust: ein falscher/nicht erreichbarer Endpunkt beendet den Dienst NICHT.
// Neu: Aendert sich eine Bootstrap-Einstellung (ipSPS, ns, Knoten-Index), wird
// die Verbindung automatisch neu aufgebaut - OHNE Dienst-Neustart.
static void spsWorkerLoop(SettingsStore *settingsPtr, AppState *statePtr)
{
    SettingsStore &settings = *settingsPtr;
    AppState &state = *statePtr;

    auto setStatus = [&](bool connected, const std::string &msg) {
        g_sps_connected.store(connected);
        state.setSpsConnected(connected);
        state.setSpsMessage(msg);
    };

    // Ohne OPC-Sitzung sind die Knoten-Diagnosewerte ungueltig. Sie MUESSEN
    // geloescht werden, sonst zeigt die Oberflaeche veraltete "vorhanden"-
    // Meldungen an, obwohl gar keine Verbindung besteht.
    auto clearDiag = [&]() { state.setNodeDiagnostics(nlohmann::json::array()); };

    // Wartet in kleinen Schritten: Beenden und Einstellungsaenderungen greifen sofort.
    auto wait = [&](int ms, long long bootRev) {
        int waited = 0;
        while (waited < ms && g_running.load() && settings.bootstrapRevision() == bootRev)
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(200));
            waited += 200;
        }
    };

    while (g_running.load())
    {
        // Aktuelle Einstellungen uebernehmen (ipSPS, Namespace, Knoten-Indizes).
        applySettings(settings);
        state.setActiveSettings(activeSettingsJson());
        const long long bootRev = settings.bootstrapRevision();

        // Betriebsschalter: OPC-Verbindung komplett deaktiviert?
        if (settings.getInt("opcEnabled", 1) == 0)
        {
            clearDiag();
            setStatus(false, "SPS-Verbindung deaktiviert");
            for (int i = 0; i < 10 && g_running.load(); ++i)
                std::this_thread::sleep_for(std::chrono::milliseconds(200));
            continue;
        }

        ErrorLoggerSingleton::instance().logError("SPS: verbinde mit " + ipOPC);
        clearDiag();
        setStatus(false, "verbinde mit " + ipOPC + " ...");

        // Erst schnell pruefen, ob der Endpunkt ueberhaupt erreichbar ist.
        if (!endpointReachable(ipOPC, 2000))
        {
            clearDiag();
            setStatus(false, "nicht erreichbar: " + ipOPC);
            wait(3000, bootRev);
            continue;
        }

        SPSController spsController("opc", ipOPC);

        if (!spsController.connect())
        {
            clearDiag();
            setStatus(false, "nicht erreichbar: " + ipOPC);
            wait(5000, bootRev);
            continue;
        }

        setStatus(true, "verbunden, registriere Knoten ...");
        ErrorLoggerSingleton::instance().logError("SPS verbunden");

        // Knoten registrieren (liest die Knoten LIVE vom Server). Schlaegt das
        // fehl (falscher Namespace/Index/Typ), NICHT abstuerzen, sondern erneut
        // versuchen - der Anwender kann die Werte im Web korrigieren.
        auto collectDiag = [&](SPSController &ctrl) {
            json arr = json::array();
            for (const auto &d : ctrl.nodeDiagnostics())
            {
                arr.push_back({{"name", d.name},
                               {"nodeId", d.nodeId},
                               {"type", d.type},
                               {"exists", d.exists},
                               {"readable", d.readable},
                               {"writable", d.writable},
                               {"hasValue", d.hasValue},
                               {"value", d.value}});
            }
            state.setNodeDiagnostics(arr);
        };

        if (!spsController.setupVariables(setupVars))
        {
            // WICHTIG: Diagnose gerade jetzt erheben - so sieht man im Web,
            // welcher Knoten fehlt bzw. nicht les-/schreibbar ist.
            collectDiag(spsController);
            ErrorLoggerSingleton::instance().logError(
                "SPS-Setup fehlgeschlagen (Endpunkt/Knoten pruefen) - erneuter Versuch");
            setStatus(false, "Knoten-Setup fehlgeschlagen - Namespace/Indizes pruefen");
            spsController.disconnect();
            wait(5000, bootRev);
            continue;
        }

        setStatus(true, "verbunden");
        spsController.printVariableMap();
        spsController.writeChars("Info_Text", "Service gestartet", 255);
        spsController.writeBool("Error", false);
        spsController.writeInt16("StatusCode", 0);

        ErrorLoggerSingleton::instance().logError("SPS-Betriebsschleife startet");
        // sizeObjx/sizeObjy/lenDataArray MUST be passed on - without them the
        // manager falls back to its defaults (block size 5!) and the block
        // count written to the PLC has nothing to do with LenDataArray.
        ProjectManager projectManager(spsController, state, folderPath,
                                      sizeObjx, sizeObjy, lenDataArray);

        // Ab hier ist der Manager die maßgebliche Quelle der Blockgröße. Wenn
        // sie je wieder von der Einstellung abweicht, steht es jetzt auf der
        // Einstellungsseite, statt erst in der Blockzahl auf der SPS aufzufallen.
        state.setActiveSettings(activeSettingsJson(projectManager.getDataArrayLength()));

        collectDiag(spsController);
        auto lastDiag = std::chrono::steady_clock::now();

        // Watchdog in EIGENEM Thread. Grund: Faellt der Server weg, ohne die
        // Verbindung zu schliessen (Kabel ab, VM/Container gestoppt), versickern
        // die Pakete - open62541 blockiert dann bis zum TCP-Retransmit-Timeout
        // (~2 min). Die Betriebsschleife haengt in genau so einem Lesevorgang
        // fest und kann sich nicht selbst pruefen. Der Watchdog meldet den
        // Verlust deshalb unabhaengig davon innerhalb weniger Sekunden an die
        // Oberflaeche; die Schleife bricht ab, sobald ihr Aufruf zurueckkehrt.
        std::atomic<bool> loopRunning{true};
        std::atomic<bool> linkDown{false};
        std::thread watchdog([&]() {
            while (loopRunning.load() && g_running.load())
            {
                if (!endpointReachable(ipOPC, 2000))
                {
                    if (!linkDown.exchange(true))
                    {
                        ErrorLoggerSingleton::instance().logError(
                            "SPS nicht mehr erreichbar (Watchdog)");
                    }
                    clearDiag();
                    setStatus(false, "nicht erreichbar: " + ipOPC);
                }
                else
                {
                    linkDown.store(false);
                }
                for (int i = 0; i < 15 && loopRunning.load() && g_running.load(); ++i)
                    std::this_thread::sleep_for(std::chrono::milliseconds(200));
            }
        });

        int16_t heartbeat = 0;
        auto lastBeat = std::chrono::steady_clock::now();

        // transferProjectData()/LiveImage() block this thread while waiting
        // for the PLC handshake. This callback keeps the heartbeat running
        // during those waits and aborts them on shutdown or dead link.
        projectManager.setKeepAlive([&]() {
            auto now = std::chrono::steady_clock::now();
            if (now - lastBeat >= std::chrono::seconds(1))
            {
                auto [hbOk, hbEc] = spsController.writeInt16("Heartbeat", heartbeat++);
                g_sps_connected.store(hbOk);
                state.setSpsConnected(hbOk);
                lastBeat = now;
                if (!hbOk)
                    return false;
            }
            return g_running.load() && !linkDown.load();
        });

        while (g_running.load())
        {
            // Einstellungen geaendert? -> Verbindung neu aufbauen.
            if (settings.bootstrapRevision() != bootRev)
            {
                ErrorLoggerSingleton::instance().logError(
                    "SPS: Einstellungen geaendert - Reconnect");
                break;
            }

            // Knoten-Diagnose alle 10 s auffrischen.
            if (std::chrono::steady_clock::now() - lastDiag >= std::chrono::seconds(10))
            {
                collectDiag(spsController);
                lastDiag = std::chrono::steady_clock::now();
            }

            // Verbindungsverlust erkennen. WICHTIG: Weder der Rueckgabewert des
            // Writes noch UA_Client_getState() sind hier verlaesslich - faellt
            // der Server weg, ohne die Verbindung sauber zu schliessen (Kabel
            // ab, Container/VM gestoppt), versickern die Pakete und open62541
            // blockiert bis zum TCP-Retransmit-Timeout (~2 min). Deshalb prueft
            // eine eigene, schnelle TCP-Probe alle 5 s die Erreichbarkeit.
            if (linkDown.load())
                break;  // Watchdog hat den Verlust erkannt -> neu verbinden

            // Betriebsschalter zur Laufzeit auswerten.
            if (settings.getInt("opcEnabled", 1) == 0)
            {
                ErrorLoggerSingleton::instance().logError(
                    "SPS-Verbindung per Einstellung deaktiviert - trenne");
                break;
            }
            // Datenaustausch pausiert: Heartbeat und Verbindung laufen weiter,
            // damit die SPS den Dienst weiterhin als verfuegbar sieht.
            const bool dataOn = settings.getInt("dataExchangeEnabled", 1) != 0;

            // Heartbeat ~1x/s; schlägt der Write fehl, ist die SPS weg -> reconnect.
            auto now = std::chrono::steady_clock::now();
            if (now - lastBeat >= std::chrono::seconds(1))
            {
                auto [hbOk, hbEc] = spsController.writeInt16("Heartbeat", heartbeat++);
                g_sps_connected.store(hbOk);
                state.setSpsConnected(hbOk);
                if (hbOk)
                    state.setSpsMessage(dataOn ? "verbunden"
                                               : "verbunden - Datenaustausch pausiert");
                lastBeat = now;
                if (!hbOk)
                {
                    ErrorLoggerSingleton::instance().logError(
                        "SPS-Verbindung verloren - versuche Reconnect");
                    break;
                }
            }

        if (!dataOn)
        {
            // pausiert -> nur Heartbeat/Statuspflege oben
            std::this_thread::sleep_for(std::chrono::milliseconds(200));
            continue;
        }

        // Projektliste aktualisieren
        auto [updateRequested, updateSuccess, updateCode] =
            spsController.readBool("UpdateProjektList");
        
        if (updateSuccess && updateRequested)
        {
            spsController.writeBool("UpdateProjektList", false);
            projectManager.updateProjectList(MaximaleAnzahlProjekte);
        }

        // Projektinfo anzeigen
        auto [showRequested, showSuccess, showCode] =
            spsController.readBool("showProjektInfo");

        if (showSuccess && showRequested)
        {
            spsController.writeBool("showProjektInfo", false);
            projectManager.showProjectInfo();
            spsController.writeBool("ProjektInfoWrote", true);
        }

        // Daten übertragen
        auto [readRequested, readSuccess, readCode] =
            spsController.readBool("ReadProjekt");

        if (readSuccess && readRequested)
        {
            projectManager.transferProjectData();
            spsController.writeBool("ReadProjekt", false);
        }

            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }  // Ende Betriebsschleife

        loopRunning.store(false);
        if (watchdog.joinable())
            watchdog.join();

        // Verbindung sauber trennen; oben wird ggf. neu verbunden.
        spsController.writeChars("Info_Text", "Service gestoppt", 255);
        spsController.disconnect();
        clearDiag();
        setStatus(false, g_running.load() ? "Einstellungen geaendert - verbinde neu" : "beendet");
    }
}

// =================== Hauptprogramm ===================
int main(int argc, char* argv[])
{
#ifdef _WIN32
    // Feinere Scheduler-Timer-Auflösung (1 ms).
    timeBeginPeriod(1);
#endif

    // UTF-8 für die CRT aktivieren (nur LC_CTYPE, sonst bricht std::stod).
    std::setlocale(LC_CTYPE, ".UTF-8");

    // Signal Handler registrieren
#ifdef _WIN32
    signal(SIGINT, signalHandler);
    signal(SIGTERM, signalHandler);
    signal(SIGBREAK, signalHandler);
#else
    signal(SIGINT, signalHandler);
    signal(SIGTERM, signalHandler);
    signal(SIGHUP, signalHandler);
#endif

    ErrorLoggerSingleton::instance().logError("Programmstart...");

    // Arbeitsverzeichnis setzen
    std::string exeDir = getExecutableDirectory();
    std::filesystem::current_path(exeDir);
    ErrorLoggerSingleton::instance().logError("EXE-Dir: " + exeDir);

    // Web-Wurzel: enthält templates/ sowie cert.pem/key.pem (selbstsigniert).
    if (webRoot.empty())
    {
#ifdef _WIN32
        webRoot = exeDir + "\\webVisu";
#else
        webRoot = exeDir + "/webVisu";
#endif
    }

    // Einstellungs-Datenbank (dieselbe Datei wie bisher, im persistenten Volume).
    std::string settingsDbPath = std::getenv("SETTINGS_DB")
                                     ? std::getenv("SETTINGS_DB")
                                     : std::string("/app/data/settings.db");

    // Instanz-Check (Mutex)
    {
        std::lock_guard<std::mutex> lock(g_instance_mutex);
        if (g_instance_running)
        {
            ErrorLoggerSingleton::instance().logError("Programm läuft bereits");
            return 1;
        }
        g_instance_running = true;
    }

    // Gemeinsamer Zustand + Einstellungen (nativ, ohne Python).
    // Die SQLite-DB ist die einzige Konfigurationsquelle - keine config.txt mehr.
    static AppState appState;
    static SettingsStore settingsStore(settingsDbPath);
    settingsStore.init();
    applySettings(settingsStore);
    // Ohne SPS gibt es noch keinen ProjectManager, daher hier ohne Blockgroesse
    // aus dem Manager. Die Seite zeigt trotzdem schon die geltenden Werte.
    appState.setActiveSettings(activeSettingsJson());
    ErrorLoggerSingleton::instance().logError(
        "Einstellungen geladen (" + settingsDbPath + "): folderPath=" + folderPath +
        ", ipSPS=" + ipOPC);

    // Webserver ZUERST starten - unabhängig von der SPS. So sind Einstellungen
    // und Vorschau auch ohne SPS-Verbindung erreichbar.
    HttpServer *webServer = nullptr;
    try
    {
        ErrorLoggerSingleton::instance().logError("Starte Webserver...");
        bool useTls = settingsStore.getInt("webUseTls", 1) != 0;
        webServer = new HttpServer(webPort, webRoot, settingsStore, appState, useTls);
        if (!webServer->start())
        {
            ErrorLoggerSingleton::instance().logError("Webserver-Start fehlgeschlagen");
            delete webServer;
            webServer = nullptr;
        }
        else
        {
            ErrorLoggerSingleton::instance().logError(
                "Webserver gestartet: " + webServer->url() +
              (webServer->usesTls() ? " (HTTPS)" : " (HTTP)"));
        }
    }
    catch (const std::exception &e)
    {
        ErrorLoggerSingleton::instance().logError("Webserver-Exception: " + std::string(e.what()));
        if (webServer)
        {
            delete webServer;
            webServer = nullptr;
        }
    }

    // Vorschau-Worker (rendert mit C++, unabhängig von der SPS).
    std::thread previewThread;
    if (webServer)
    {
        // In hoher Auflösung rendern (scharfes Zoomen); loadImg(..., true) pusht
        // das Bild verlustfrei (PNG) und ohne Herunterskalieren.
        previewThread = std::thread(previewWorkerLoop, &appState, &settingsStore, folderPath,
                                    previewImgSizeX, previewImgSizeY,
                                    MaximaleAnzahlProjekte);
        ErrorLoggerSingleton::instance().logError("Vorschau-Worker gestartet");
    }

    // ---- SPS in eigenem Thread (verbindet bei Aenderungen selbst neu) -----
    ErrorLoggerSingleton::instance().logError("SPS-Verbindungsverwaltung startet");
    std::thread spsThread(spsWorkerLoop, &settingsStore, &appState);

    // Hauptthread wartet auf das Beenden und beobachtet die HTTPS/HTTP-Umschaltung.
    // Wird webUseTls geaendert, muss der Webserver neu gebunden werden (anderer
    // Socket-Typ) - das erledigen wir hier live, ohne Container-Neustart.
    bool tlsWanted = settingsStore.getInt("webUseTls", 1) != 0;
    while (g_running.load() && !appState.shutdownRequested())
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(200));

        const bool nowWanted = settingsStore.getInt("webUseTls", 1) != 0;
        if (webServer && nowWanted != tlsWanted)
        {
            tlsWanted = nowWanted;
            ErrorLoggerSingleton::instance().logError(
                std::string("Webserver wird umgestellt auf ") + (nowWanted ? "HTTPS" : "HTTP"));
            webServer->stop();
            delete webServer;
            webServer = new HttpServer(webPort, webRoot, settingsStore, appState, nowWanted);
            if (webServer->start())
            {
                ErrorLoggerSingleton::instance().logError(
                    "Webserver neu gestartet: " + webServer->url());
            }
        }
    }
    g_running.store(false);
    if (spsThread.joinable())
        spsThread.join();

    // ---- Cleanup (nur beim echten Beenden, g_running=false) ----------------
    ErrorLoggerSingleton::instance().logError("Beenden - starte Cleanup...");

    // Vorschau-Worker beenden, BEVOR der Interpreter (WebServer) abgebaut wird.
    if (previewThread.joinable())
    {
        previewThread.join();
    }

    if (webServer)
    {
        webServer->stop();
        delete webServer;
    }

    ErrorLoggerSingleton::instance().logError("Programm sauber beendet");
    return 0;
}