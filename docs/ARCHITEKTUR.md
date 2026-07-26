# Architektur

Dieses Dokument beschreibt den Datenfluss, die Schichten und die Handshake-Protokolle
zwischen dem C++-Dienst und der SPS.

## Überblick der Schichten

```
┌─────────────────────────────────────────────────────────────┐
│  main.cpp            Bootstrap + Hauptschleife (Polling)     │
├─────────────────────────────────────────────────────────────┤
│  ProjectManager      Orchestrierung (CNC_Daten)             │
│     ├── ProjectFileReader   Dateien finden / Datensätze lesen│
│     ├── GCodeParser         G-Code → Command-Liste          │
│     └── PreviewRenderer     Command-Liste → cv::Mat-Bild    │
├──────────────────────────────┬──────────────────────────────┤
│  SPSController (Fassade)      │  WebServer (webVisu)         │
│     └── SPSDialog             │     eingebettetes CPython    │
│           └── ISPSClient      │     ruft server.py           │
│                ├── OPCClient  │  ─────────────────────────►  │
│                ├── TwincatAds │  Flask (:5000) + ImgManager  │
│                └── TestClient │                              │
└──────────────────────────────┴──────────────────────────────┘
```

Zwei klar getrennte Verantwortungsbereiche:

- **CNC_Daten** kennt die Fachlogik (Projekte, G-Code, Vorschau), aber nicht das
  konkrete Übertragungsprotokoll.
- **interfaces** kapselt die Kommunikation mit der SPS hinter einer typisierten
  Fassade (`SPSController`), sodass CNC_Daten protokollunabhängig bleibt.

## Start-Sequenz (`main.cpp`)

1. **Signal-Handler** registrieren (SIGINT/SIGTERM/SIGBREAK bzw. SIGHUP) →
   setzt das globale `std::atomic<bool> g_running` für sauberes Herunterfahren.
2. **Arbeitsverzeichnis** auf das EXE-Verzeichnis setzen (`getExecutableDirectory`).
3. **Konfiguration laden** aus `config.txt` (`loadSettings`, siehe
   [KONFIGURATION.md](KONFIGURATION.md)).
4. **Einfachinstanz-Sperre** über einen Mutex-Flag (`g_instance_running`).
5. **SPS verbinden**: `SPSController("opc", ipOPC)` + `retry_sps_connect` (Retry
   alle 5 s, abbrechbar über `g_running`).
6. **SPS-Variablen registrieren**: `setupVariables(setupVars)` legt die
   Node-Zuordnung an; initiale Statuswerte (`Info_Text`, `Error`, `StatusCode`).
7. **Heartbeat-Thread** starten (schreibt jede Sekunde `Heartbeat++`).
8. **Webserver** starten (`WebServer` → `startServer()`), Fehler sind nicht fatal.
9. **Hauptschleife** mit `ProjectManager`.

## Hauptschleife und SPS-Trigger

Die Schleife pollt drei bool-Trigger und setzt sie nach Erkennung zurück
(Rendezvous-Muster: SPS setzt Bit → Dienst erkennt → Dienst löscht Bit → Dienst arbeitet):

```cpp
while (g_running) {
    if (readBool("UpdateProjektList")) { writeBool("UpdateProjektList", false);
                                         updateProjectList(MaximaleAnzahlProjekte); }
    if (readBool("showProjektInfo"))   { writeBool("showProjektInfo", false);
                                         showProjectInfo();
                                         writeBool("ProjektInfoWrote", true); }
    if (readBool("ReadProjekt"))       { transferProjectData();
                                         writeBool("ReadProjekt", false); }
    sleep(100ms);
}
```

## Die drei Kernabläufe

### 1. Projektliste aktualisieren — `updateProjectList`

1. `ProjectFileReader::listProjects(maxProjects)` scannt `folderPath` nach
   `.tap`-Dateien.
2. Ergebnis wird als 2D-String-Array in die SPS-Variable `ProjektArray` geschrieben.
3. `ProjektlistUpdated = true` signalisiert der SPS die Fertigstellung.

### 2. Projektinfo & Vorschau — `showProjectInfo`

1. Projektname aus SPS lesen (`ProjektName`).
2. `getProjectInfo()` lädt Metadaten (Existenz, Datensatzanzahl in der `.tab`-Datei).
3. `LenProjekt`, `AnzDatenblöcke`, `ProjektVorhanden` in die SPS schreiben.
4. `RenderParams::loadFromSPS(...)` holt Farb-/Strichstärken-/Offset-Parameter von
   der SPS; `PreviewRenderer::renderPreview()` parst die Datei und zeichnet das Bild.
5. Das gerenderte `cv::Mat` geht über `WebServer::loadImg()` an die Weboberfläche.

### 3. Datenübertragung — `transferProjectData`

Blockweise Übertragung mit Handshake (SPS bestätigt jeden Block):

1. `showProjectInfo()` aufrufen, Datensatzanzahl bestimmen.
2. `AnzDatenblöcke = ceil(recordCount / lenDataArray)`, `IndexData = 0`.
3. Schleife pro Block:
   - Abbruch über `ubertragenAbbrechen` prüfen.
   - Auf `ReadData`-Signal der SPS warten → `ReadData` löschen.
   - `readDataBlock()` liest die nächsten `lenDataArray` Zeilen.
   - Block in `DataArray` schreiben, `IndexData` hochzählen, `WriteData = true`.
4. Nach dem letzten Block: auf die Bestätigung der SPS warten (`WriteData`
   gelöscht oder `ReadData` erneut gesetzt, max. 5 s), dann `UebertgarungBeendet = true`.

### Live-Bild — `LiveImage`

Optionaler Modus: Der Dienst pollt die SPS-Variable `LiveStand` (aktuelle
Datensatznummer im laufenden CNC-Programm). Bei jeder Änderung wird das
Vorschaubild mit hervorgehobenem aktuellem Datensatz (`RenderParams.currentRecord`)
neu gerendert und an den Webserver gegeben. Abbruch über `LiveAbbruch`.
(Die Methode enthält aktuell einen unvollständigen Codeabschnitt — siehe
[MODULE.md](MODULE.md).)

## SPS-Anbindung (interfaces)

`SPSController` ist eine **Fassade**, die anhand des `clientType`-Strings
(`"opc"`, `"twincat"`, `"test"`) eine konkrete `ISPSClient`-Implementierung wählt und
alle typisierten Read/Write-Aufrufe an `SPSDialog` weiterreicht.

- **`SPSDialog`** hält eine Variablen-Registry (`Name → VariableInfo` mit NodeID/Handle)
  und bietet typsichere `readInt16`/`writeBool`/`readString`/… inkl. 2D-Array-Varianten.
- **`OPCClient`** (open62541) ist der aktive Produktivpfad — OPC-UA-Client zur echten SPS.
- **`TwincatAds`** ist die alternative Beckhoff-ADS-Anbindung (Windows-only, im Build
  ausgeschlossen).
- **`OPCUAServer`** ist ein eigenständiger OPC-UA-Server für Tests/Mocking.

Alle Read-Operationen liefern ein `std::tuple<Wert, bool success, int errorCode>`,
alle Writes ein `std::tuple<bool success, int errorCode>` — der Aufrufer prüft immer
erst das `success`-Flag.

## Webvisualisierung (webVisu)

Der C++-`WebServer` **bettet einen CPython-Interpreter ein** (`Python.h`):

1. Python-Installation finden (Registry/Standardpfade, venv-Erkennung über
   `pyvenv.cfg`), `sys.path` um das Skriptverzeichnis erweitern.
2. `server.py` importieren, Klasse `FlaskServer` instanziieren, `start`/`stop` holen.
3. Die **GIL** wird nach der Initialisierung freigegeben (`PyEval_SaveThread`), damit
   der Flask-Thread und `loadImg`-Aufrufe parallel laufen können.
4. `loadImg(cv::Mat&)` skaliert das Bild, kodiert es als JPEG (`cv::imencode`) und
   übergibt die Bytes an Pythons `load_image()`. Der `ImgManager` puffert das Bild
   thread-sicher; Flask liefert es unter `/img` aus.

Flask läuft in einem Daemon-Thread auf `0.0.0.0:5000` (HTTPS, Zertifikat wird bei
Bedarf selbst signiert erzeugt).

## Threading-Modell

| Thread                | Aufgabe                                              |
|-----------------------|------------------------------------------------------|
| Main                  | Bootstrap + Polling-Hauptschleife                    |
| Heartbeat             | schreibt jede Sekunde `Heartbeat` zur SPS            |
| Webserver (C++)       | ruft blockierend Pythons `FlaskServer.start()`       |
| Flask (Python)        | HTTP-Server als Daemon-Thread innerhalb von CPython  |

Synchronisation: `g_running` (atomic) steuert alle Schleifen; die Python-GIL wird
um jeden Interpreter-Zugriff herum ge- `Ensure`/`Release`-t.

## Fehlerbehandlung & Logging

- **`ErrorLoggerSingleton::instance().logError(msg)`** schreibt zeitgestempelte
  Meldungen in `Log.txt` neben der EXE (thread-sicher, neueste Meldung oben).
- Fachliche Fehler werden zusätzlich über `SPSController::pushErrorMessage()` an die
  SPS gemeldet (`Info_Text` / `Error` / `StatusCode`), Info-Meldungen über
  `pushInfoMessage()`.
