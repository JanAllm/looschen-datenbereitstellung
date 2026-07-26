# Modulreferenz

Modul-für-Modul-Überblick über den Quellcode. Pfade sind relativ zum Projektwurzel.
Header liegen unter `include/` in gespiegelter Struktur zu `src/`.

---

## CNC_Daten — Fachlogik

Der aktive Kernpfad: CNC-Projektdateien lesen, G-Code parsen, Vorschau rendern,
Übertragung zur SPS orchestrieren.

### `ProjectManager` — `src/CNC_Daten/cnc_projekt_manager.cpp`

Orchestrator, der `SPSController` und `WebServer` zusammenbindet. Hält Helfer
`ProjectFileReader` und `PreviewRenderer` (per `unique_ptr`).

Konstruktor:
```cpp
ProjectManager(SPSController& controller, WebServer& webServer,
               const std::string& folderPath,
               int imageSizeX = 1000, int imageSizeY = 2000, int lenDataArray = 5);
```

Öffentliche Methoden:
- `bool updateProjectList(int maxProjects)` — Ordner scannen, Liste zur SPS schreiben.
- `std::tuple<int,bool> showProjectInfo()` — Metadaten + Vorschau; liefert `{recordCount, success}`.
- `bool transferProjectData()` — blockweise Übertragung mit SPS-Handshake.
- `std::tuple<int,bool> LiveImage()` — Live-Vorschau nach `LiveStand`.

> **Bekanntes Problem:** `LiveImage()` enthält am Ende der `while`-Schleife eine
> unvollständige Anweisung (`LiveStand` ohne Verwendung) und einen `catch`-Block, der
> syntaktisch innerhalb der Schleife statt am `try` hängt. Die Methode ist im aktiven
> Ablauf (`main.cpp`) derzeit **nicht verdrahtet**.

### `GCodeParser` — `src/CNC_Daten/gcode_parser.cpp`

Parst G-Code-Dateien/Strings in strukturierte Befehle. Unterstützte Befehlstypen:
`G00` (Eilgang), `G01` (Linear), `G02` (Kreis CW), `G03` (Kreis CCW).

Wichtige Typen:
- `Point2D { double x, y; }` — OpenCV-unabhängiger 2D-Punkt.
- `Command { CommandType type; Point2D endPosition, arcCenter; int lineNumber; ... }`
  mit Prädikaten `isArc()`, `isLinear()`, `isRapid()`.
- `ParseResult { std::vector<Command> commands; std::vector<int> errorLines; ... }`.

Öffentliche Methoden:
- `ParseResult parseFile(const std::string& filename)`
- `ParseResult parseString(const std::string& content)`
- `static CommandType detectCommandType(const std::string& line)`
- `static bool isValidFilePath(const std::string& path)`

### `ProjectFileReader` — `src/CNC_Daten/project_file_reader.cpp`

Liest `.tap`-/`.tab`-Projektdateien aus einem Basisordner.

Typ `ProjectInfo { std::string name; std::filesystem::path filePath; size_t recordCount; bool exists; }`.

Öffentliche Methoden:
- `std::vector<std::string> listProjects(int maxCount)`
- `std::optional<ProjectInfo> getProjectInfo(const std::string& projectName)`
- `size_t countRecords(const std::filesystem::path&)`
- `std::vector<std::string> readDataBlock(path, int blockIndex, int blockSize)`

### `PreviewRenderer` — `src/CNC_Daten/preview_renderer.cpp`

Rendert geparste G-Code-Befehle mit OpenCV in ein `cv::Mat` (Linien und Kreisbögen,
farbcodiert nach G-Typ; Fixed-Point-Rendering mit `FP_SHIFT=8`).

Typ `RenderParams` — Bildgröße, `cv::Scalar colorG0..G3`, `thicknessG0..G3`,
`horizontal/verticalOffset`, `enableMirroring`, `drawReferenceAxis`. Factory:
`static RenderParams loadFromSPS(SPSController&, int defaultSizeX, int defaultSizeY)`.

Öffentliche Methoden:
- `std::tuple<cv::Mat,int> renderPreview(const std::string& projectPath, const RenderParams&)`
- `std::tuple<cv::Mat,int> render(const std::vector<GCodeParser::Command>&, const RenderParams&)`
- `void setPostProcessor(std::function<cv::Mat(cv::Mat)>)`

---

## interfaces — SPS-Kommunikation

Architektur: **`SPSController` (Fassade) → `SPSDialog` (Registry) → `ISPSClient` (Interface)**.

Datentypen in [`include/data_structs.h`](../include/data_structs.h): `LabelIO`,
`ProcessImgParams` (Bildverarbeitungs-Parameter inkl. ROIs/Genauigkeiten),
`KameraSettings`, `KameraDialog`.

### `SPSController` — `src/interfaces/SPSController.cpp`

Wählt anhand `clientType` (`"opc"`, `"twincat"`, `"test"`) den konkreten Client.
Bietet High-Level-Zugriffe und leitet alle typisierten Read/Write an `SPSDialog`
weiter. Alle Reads liefern `tuple<Wert, bool success, int code>`, Writes `tuple<bool, int>`.

- `bool connect()` / `bool disconnect()`
- `bool setupVariables(setupVarInfos&)` — Node-Registrierung
- `readCameraSettings` / `writeCameraSettings`, `readProcessImgParams` / `writeProcessImgParams`
- `pushErrorMessage(msg, code)`, `pushInfoMessage(msg)`
- typisierte Skalar-Zugriffe: `readInt16/Int32/Double/Float/Bool/String/Chars/WString`,
  `writeInt16/Int32/Double/Float/Bool/Chars` und die entsprechenden `*Array`-Varianten.

### `SPSDialog` — `src/interfaces/SPSDialog.cpp` (+ `SPSDialog_Arrays.cpp`)

Variablen-Registry (`unordered_map<string, VariableInfo>`). `registerVariable(name,
DataType, connectionString)` legt die Zuordnung an; generischer Wert über
`SPSValue = std::variant<...>`. Kapselt die typsicheren Read/Write-Aufrufe an den Client.

### `OPCClient` — `src/interfaces/OPC_UA.cpp` (+ `OPC_UA_Arrays.cpp`)

**Aktiver Produktivpfad.** OPC-UA-Client auf Basis von **open62541**. Implementiert
`ISPSClient`: verbindet zu `opc.tcp://…`, liest/schreibt Skalare und 2D-Arrays,
browst Adressraum/Namespaces. Die Array-Read/Write-Implementierungen liegen in
`OPC_UA_Arrays.cpp`.

### `TwincatAds` — `src/interfaces/TwincatAds.cpp` (+ `TwincatAds_Arrays.cpp`)

Alternative Beckhoff-Anbindung über **TwinCAT ADS** (Windows-only, benötigt
`TcAdsDll.dll`). Handle-basierter Zugriff mit Cache, `ipToAmsNetId`, `writeLabelIO`.
**Im Build ausgeschlossen** und im aktuellen `SPSController.h` per include auskommentiert.

### `OPCUAServer` — `src/interfaces/OPCUAServer.cpp`

Eigenständiger OPC-UA-**Server** (open62541) für Tests/Mocking. Fügt Variablen mit
numerischen NodeIDs (`ns=X;i=Y`) hinzu, läuft in eigenem Thread.
`getEndpointUrl()` → `opc.tcp://localhost:PORT`.

---

## webVisu — Weboberfläche

### `WebServer` — `src/webVisu/WebServerDialog.cpp`

C++-Klasse, die einen **CPython-Interpreter einbettet** und `server.py` steuert.

Konstruktor:
```cpp
WebServer(const std::string& pythonPath = "", const std::string& scriptPath = "",
          int webserVerImgSizeX = 500, int webserVerImgSizeY = 1000);
```

- Findet Python (Windows-Registry / Standardpfade / venv über `pyvenv.cfg`),
  initialisiert den Interpreter, erweitert `sys.path`, verwaltet GIL/Threading.
- `int startServer()` — startet Flask (`FlaskServer.start()`) in einem C++-Thread.
- `void stopServer()` — ruft `FlaskServer.stop()` und joint den Thread.
- `int loadImg(cv::Mat&)` — resized das Bild, JPEG-kodiert es und übergibt die Bytes
  an Pythons `load_image()`.

### Python-Backend (`src/webVisu/`)

- **`server.py`** — Klasse `FlaskServer`: startet Flask in Daemon-Thread auf
  `0.0.0.0:5000` (HTTPS via `cert.pem`/`key.pem`). `start()` (PID), `stop()`
  (`os._exit`), `load_image(bytes)`.
- **`flask_app.py`** — `create_app()`-Factory: `ensure_cert()` (selbstsigniert),
  `ImgManager(size=(500,1000))`. Routen: `/`, `/img`, `/health`, `/status`, `/shutdown`.
- **`img_manager.py`** — `ImgManager`: thread-sicherer PIL-Bildpuffer
  (`set_image_bytes`, `get_image_bytes`).
- **`cpp_Wrapper.py`** — alternativer Wrapper über `current_app.img_manager`.
- **`templates/index.html`** — Frontend, das `/img` anzeigt.

---

## bildverarbeitung — OpenCV/OCR-Toolkit (Nebenzweig)

Umfangreiches Bildverarbeitungs-Toolkit. Es gehört zu einem verwandten
Kamera-/Inspektionsprojekt und ist im aktiven Datenbereitstellungs-Pfad
(`main.cpp`) **nicht verdrahtet**, aber Teil der Bibliothek.

- **`Preprocessing`** (`preprocessing/`) — Vorverarbeitung: Threshold (Otsu/adaptiv/
  Canny), Blur (Median/Gauss/Sobel), Farbräume (Gray/HSV/BGR), ROI (Box/Kreis),
  Morphologie (opening/closing), `cylindricalTransform`, `rasterize`, Histogramm,
  Rotation/Spiegelung.
- **`objectdetectionOpenCV`** (`objektdetection/`) — Kontur-/Linienerkennung
  (`findContours`, Hough `FindLines`, `findObjekt`, ConvexHull/-Defects, Rotation/Abstand).
- **`drawOpenCV`** (`drawing/`) — Zeichen-Utilities (`drawLines`, `drawBox`, `drawROI`,
  `drawRotatedBoundingBox`, `writeText`, `drawContours`, `drawCircle`, …).
- **`OCR`** (`deepLearning/`) — C++-Brücke zum Python-OCR-Backend `ocr.py`
  (EasyOCR + pyzbar). `readText(cv::Mat)`, `detectBarcode(...)`, Textvergleich mit
  Zeichenäquivalenzen (LCS-basiert). Erwartet lokale Modelldateien
  `craft_mlt_25k.pth` und `english_g2.pth` in `models/` (siehe unten).

### `src/ocr.py`

Python-OCR-Backend. Nutzt **EasyOCR** (`easyocr.Reader(['en'], download_enabled=False)`
— nur lokale Modelle) und **pyzbar** für Barcodes (EAN13). Wirft `FileNotFoundError`,
wenn die `.pth`-Modelldateien fehlen. Das Top-Level `models/`-Verzeichnis ist im Repo
derzeit leer — Modelle werden über `modleDownload.py` / `offline_packages` bereitgestellt.

---

## include/ — Hilfsheader (header-only)

- **`ErrorLogger.h`** — thread-sicherer Datei-Logger (`logError(msg)` mit Zeitstempel).
- **`ErrorLoggerSingleton.h`** — globaler Logger auf `<EXE-Dir>/Log.txt`; enthält
  `GetExeDirectory()`.
- **`path_utils.h`** — `getExecutableDirectory()` (cross-platform, UTF-8).
- **`load_settings.h`** — `loadSettings(...)` parst `config.txt` in `setupVarInfos` +
  Programmvariablen.
- **`data_structs.h`** — gemeinsame Structs (`ProcessImgParams`, `KameraSettings`, …).
- Weitere: `platform.h`, `interfaces/setupVarInfos.h`, `interfaces/VariablenInfo.h`,
  `interfaces/DataType.h`.

---

## Platzhalter / inaktiver Code

Diese Dateien sind leer oder auskommentiert und gehören nicht zum aktiven Pfad:

| Datei                       | Status |
|-----------------------------|--------|
| `src/BV.cpp`                | leer |
| `src/settings.cpp`          | leer (Logik liegt in `load_settings.h`) |
| `src/MyCppLibrary.cpp`      | leer; Header deklariert C-DLL-Export `ProcessImage(...)` ohne Implementierung |
| `src/postProcessing.cpp`    | vollständig auskommentiert (alter ADS-Nachbearbeitungscode) |
| `include/postProcessing.h`  | leer |
| `include/draw_cnc.h`        | leer/ungenutzt |
| `tests/test_Kamera.cpp` u. a.| leere Test-Platzhalter |

---

## Tests — `tests/`

Framework **GoogleTest** (`gtest_discover_tests`). Executable `MyCppTests` linkt gegen
`MyCppLibrary`, `gtest_main`, Python und OpenCV.

Befüllte Tests:
- `integration_test_opc_ua_client.cpp` — OPC-UA-Client-Integration
- `integration_test_SPS_Controller.cpp` — SPSController-Integration
- `test_SPSController.cpp` — z. B. `ConnectTest`
- `testWebServer.cpp` — WebServer + PreviewRenderer + ProjectManager
- `main.cpp` — Test-Runner

Leere Platzhalter: `test_Bildverarbeitung.cpp`, `test_CamAndPRocessing.cpp`,
`test_ImgQueue.cpp`, `test_Kamera.cpp`, `test_SPSDialog.cpp`.
