# Looschen_Datenbereitstellung

C++-Dienst, der einer **SPS** (Beckhoff, angebunden über **OPC UA**) CNC-Projektdaten
bereitstellt. Der Dienst liest CNC-/G-Code-Projektdateien von der Platte, zählt und
überträgt deren Datensätze blockweise zur SPS, rendert aus dem G-Code ein Vorschaubild
und stellt dieses (sowie ein Live-Bild des aktuellen Bearbeitungsstands) über einen
eingebetteten **Flask-Webserver** im Browser dar.

Der Name „Datenbereitstellung“ beschreibt genau diese Rolle: Die Anwendung ist der
Vermittler zwischen dem Dateisystem (CNC-Projekte) und der SPS.

---

## Was die Anwendung tut

```
   CNC-Projektdateien (.tap / .tab)                 SPS (OPC UA Server)
        auf der Festplatte                        opc.tcp://<ip>:4840
                │                                          │
                │  lesen / zählen / Block lesen            │  Trigger-Bits lesen
                ▼                                          │  Daten/Listen schreiben
        ┌───────────────────────────────────────────────────────────┐
        │                    MyCppExecutable                         │
        │                                                            │
        │  ProjectFileReader ─▶ GCodeParser ─▶ PreviewRenderer       │
        │            │                              │ (cv::Mat)      │
        │            ▼                              ▼                │
        │        ProjectManager ◀───────────▶ SPSController          │
        │                          │                                 │
        │                          ▼ (JPEG-Bytes)                    │
        │                     WebServer (eingebettetes CPython)      │
        └────────────────────────────┬──────────────────────────────┘
                                     │  HTTPS :5000
                                     ▼
                             Browser (Live-Vorschau)
```

Die Hauptschleife in [`src/main.cpp`](src/main.cpp) pollt zyklisch (alle 100 ms)
Trigger-Bits der SPS und reagiert darauf:

| SPS-Trigger (bool)   | Aktion im Dienst                                              |
|----------------------|--------------------------------------------------------------|
| `UpdateProjektList`  | Ordner scannen, Projektliste als String-Array zur SPS schreiben |
| `showProjektInfo`    | Datensatzanzahl ermitteln, Vorschaubild rendern & im Web anzeigen |
| `ReadProjekt`        | Projektdaten blockweise (Handshake) zur SPS übertragen        |

Parallel läuft ein **Heartbeat-Thread** (schreibt jede Sekunde einen hochzählenden
`Heartbeat`-Wert) und der **Webserver-Thread**.

---

## Projektstruktur

```
Looschen_Datenbereitstellung/
├── CMakeLists.txt          # Build-Konfiguration (cross-platform)
├── src/
│   ├── main.cpp            # Einstiegspunkt, Hauptschleife
│   ├── config.txt          # Laufzeit-Konfiguration (SPS-IP, Node-Indizes, Größen)
│   ├── CNC_Daten/          # G-Code parsen, Projekte lesen, Vorschau rendern
│   ├── interfaces/         # SPS-Anbindung (OPC UA / TwinCAT ADS) + Test-Server
│   ├── webVisu/            # C++↔Flask-Brücke + Python-Webserver
│   ├── bildverarbeitung/   # OpenCV/OCR-Toolkit (Nebenzweig, s. docs/MODULE.md)
│   └── ocr.py              # Python-OCR-Backend (EasyOCR + pyzbar)
├── include/                # Header (gespiegelte Struktur zu src/)
├── tests/                  # GoogleTest-Unit- und Integrationstests
├── third_party/open62541/  # OPC-UA-Bibliothek (als Submodul/Quelle)
├── Libarys/                # vorkompilierte DLLs (OpenCV, Torch, IC4, …)
├── models/                 # OCR-Modelle (.pth) – müssen bereitgestellt werden
└── docs/                   # ausführliche Dokumentation (siehe unten)
```

---

## Dokumentation

| Dokument | Inhalt |
|----------|--------|
| [docs/ARCHITEKTUR.md](docs/ARCHITEKTUR.md)   | Datenfluss, Schichten, Threading, SPS-Handshakes im Detail |
| [docs/KONFIGURATION.md](docs/KONFIGURATION.md) | Jeder `config.txt`-Parameter, SPS-Variablen und Dialog-Protokoll |
| [docs/MODULE.md](docs/MODULE.md)             | Modul-für-Modul-Referenz (Klassen, wichtige Methoden) |

---

## Build

### Voraussetzungen

- **CMake ≥ 3.18**, C++20-Compiler (MSVC unter Windows, GCC/Clang unter Linux), **x64**
- **Python 3.10–3.14** mit `pybind11`, `numpy` sowie den Runtime-Paketen aus
  [`requirements.txt`](requirements.txt) (Flask, OpenCV-Python, EasyOCR, pyzbar, cryptography …)
- **OpenCV** (Windows: erwartet unter `D:/opencv/build`, siehe `CMakeLists.txt`)
- **open62541** unter `third_party/open62541`
  (Windows: wird per `add_subdirectory` gebaut; Linux: vorgebautes `libopen62541.a` erwartet)
- `nlohmann/json` wird automatisch per CMake `FetchContent` geladen

Ein Python-`venv` unter `.venv/` wird bevorzugt (siehe `CMakeLists.txt`,
Abschnitt „Python venv konfigurieren“).

### Bauen

```bash
cmake -S . -B build
cmake --build build --config Release
```

Erzeugte Artefakte (in `build/bin` bzw. `build/tests`):

- `MyCppExecutable` – die Anwendung
- `MyCppTests` – die Testsuite
- `MyCppLibrary` – Kernbibliothek, gegen die beide gelinkt werden

Unter Windows kopiert CMake nach dem Build automatisch die benötigten DLLs
(OpenCV, Python), das Verzeichnis `config/`, sowie `src/webVisu/` neben die EXE.

> **Hinweis:** `TwincatAds.cpp`, `TwincatAds_Arrays.cpp` und `baslerKameras.cpp`
> werden im Build **ausgeschlossen** (Windows-only Hardware). Der aktive
> SPS-Pfad ist OPC UA.

---

## Ausführen

1. [`src/config.txt`](src/config.txt) anpassen — mindestens:
   - `ipSPS` = OPC-UA-Endpunkt der SPS (z. B. `opc.tcp://192.168.155.4:4840`)
   - `folderPath` = Ordner mit den CNC-Projektdateien
   - Die SPS-**Node-Indizes** (`iError`, `iInfo`, `iProjektArray`, …) müssen mit
     dem SPS-Programm übereinstimmen (siehe [docs/KONFIGURATION.md](docs/KONFIGURATION.md)).
2. Sicherstellen, dass das Verzeichnis mit `server.py` (`webVisu/`) neben der EXE liegt.
3. `MyCppExecutable` starten. Beim Start:
   - verbindet sich der Dienst mit der SPS (Retry alle 5 s),
   - meldet „Service gestartet“ über die Info-Variable,
   - startet den Webserver auf **Port 5000** (HTTPS, selbstsigniertes Zertifikat).
4. Live-Vorschau im Browser: `https://<host-ip>:5000/`

Beenden über `Ctrl+C` (SIGINT) — der Dienst fährt Heartbeat, Webserver und
SPS-Verbindung sauber herunter und meldet „Service gestoppt“.

Fehler- und Ablaufmeldungen werden in `Log.txt` neben der EXE geschrieben.

---

## Tests

```bash
cd build
ctest --output-on-failure
```

Framework: **GoogleTest**. Befüllt sind u. a. Integrationstests für den
OPC-UA-Client, den `SPSController` und den `WebServer`/Renderer. Einige Testdateien
(`test_Kamera.cpp`, `test_Bildverarbeitung.cpp`, …) sind derzeit leere Platzhalter.
