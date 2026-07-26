# Konfiguration & SPS-Protokoll

Die Laufzeitkonfiguration liegt in [`src/config.txt`](../src/config.txt) und wird beim
Start von [`loadSettings`](../include/load_settings.h) im Format `schlüssel=wert`
geparst (Zeilen mit `#` sind Kommentare). Unbekannte Schlüssel werden ignoriert,
Parse-Fehler ins `Log.txt` geschrieben.

Beim Build kopiert CMake `config.txt` neben die EXE bzw. in `build/tests/`.

## Allgemeine Parameter

| Schlüssel               | Beispiel                            | Bedeutung |
|-------------------------|-------------------------------------|-----------|
| `ProgrammId`            | `OPCDATENVERARBEITUNG`              | Kennung des Dienstes |
| `ipSPS`                 | `opc.tcp://192.168.155.4:4840`      | OPC-UA-Endpunkt der SPS |
| `folderPath`            | `C:/Users/.../TestOrdner`           | Ordner mit den CNC-Projektdateien |
| `MaximaleAnzahlProjekte`| `16`                                | max. Projekte in der Liste (≤ SPS-Array-Größe) |
| `LenDataArray`          | `100`                               | Zeilen pro Übertragungsblock (`DataArray`-Größe) |

## WebServer & Canvas

| Schlüssel          | Beispiel | Bedeutung |
|--------------------|----------|-----------|
| `sizeObjx`         | `1000`   | Renderbreite des Vorschaubilds (Pixel) |
| `sizeObjy`         | `2000`   | Renderhöhe des Vorschaubilds (Pixel) |
| `webserVerImgSizeX`| `500`    | Breite, auf die das Bild fürs Web skaliert wird |
| `webserVerImgSizeY`| `1000`   | Höhe, auf die das Bild fürs Web skaliert wird |
| `objektgroesseX`   | `208`    | SPS-Node-Index der Canvas-Breite |
| `objektgroesseY`   | `209`    | SPS-Node-Index der Canvas-Höhe |

## SPS-Node-Indizes

Die SPS-Variablen werden **nicht über Namen, sondern über numerische OPC-UA-NodeIDs**
adressiert. `config.txt` bildet einen sprechenden Namen (der im C++-Code verwendet
wird) auf den Node-Index ab. Diese Indizes **müssen mit dem SPS-Programm
übereinstimmen** — andernfalls werden falsche oder nicht existierende Nodes gelesen.

Die Zuordnung landet in der Struktur `setupVarInfos`
([`include/interfaces/setupVarInfos.h`](../include/interfaces/setupVarInfos.h)) und wird
per `SPSController::setupVariables()` registriert.

### Allgemeine Variablen (Status / Dialog)

| config-Schlüssel | Index | Richtung | Typ    | Bedeutung |
|------------------|-------|----------|--------|-----------|
| `iError`         | 120   | write    | bool   | Fehlerflag |
| `iInfo`          | 122   | write    | string | Info-/Status-Text (`Info_Text`) |
| `iHeartbeat`     | 123   | write    | int16  | Lebenszeichen (jede Sekunde +1) |
| `iStatusCode`    | 124   | write    | int16  | Statuscode / Fehlercode |

### Datenarrays

| config-Schlüssel | Index | Richtung | Typ          | Bedeutung |
|------------------|-------|----------|--------------|-----------|
| `iDataArry`      | 19    | write    | float array  | Übertragungsblock (`DataArray`) |
| `iProjektArray`  | 138   | write    | string array | Liste der verfügbaren Projekte (`ProjektArray`) |

### Ablauf „Projektliste aktualisieren“

| config-Schlüssel     | Index | Richtung | Typ  | Bedeutung |
|----------------------|-------|----------|------|-----------|
| `iUpdateProjektList` | 125   | read     | bool | Trigger: Liste aktualisieren |
| `iProjektlistUpdated`| 126   | write    | bool | Liste wurde geschrieben |

### Ablauf „Projektinfo anzeigen“

| config-Schlüssel  | Index | Richtung | Typ    | Bedeutung |
|-------------------|-------|----------|--------|-----------|
| `showProjektInfo` | 127   | read     | bool   | Trigger: Info anzeigen |
| `iProjektName`    | 128   | read     | string | ausgewählter Projektname |
| `iLenProjekt`     | 129   | write    | int16  | Anzahl Datensätze im Projekt |
| `iAnzDatenblöcke` | 130   | write    | int16  | Anzahl benötigter Übertragungsblöcke |
| `ProjektVorhanden`| 131   | write    | bool   | Projekt existiert |
| `ProjektInfoWrote`| 132   | write    | bool   | Infos wurden geschrieben |

### Ablauf „Projektdaten übertragen“

| config-Schlüssel      | Index | Richtung | Typ   | Bedeutung |
|-----------------------|-------|----------|-------|-----------|
| `iReadProjekt`        | 133   | read     | bool  | Trigger: Datenübertragung starten |
| `iWriteData`          | 134   | write    | bool  | ein Block wurde geschrieben |
| `iIndexData`          | 135   | write    | int16 | Index des aktuellen Blocks |
| `iReadData`           | 136   | read     | bool  | SPS hat Block gelesen → nächsten senden |
| `iabbruchUbertragen`  | 137   | read     | bool  | Übertragung abbrechen |
| `iUebertgarungBeendet`| 139   | write    | bool  | Übertragung beendet, Dienst wieder frei |
| `iUebertragungslaege` | 140   | –        | int   | Länge der auf einmal übertragenen Daten |

### Zeichnungs-Design (Vorschau-Rendering)

Farben sind als Integer im Format `[G, B, R]` hinterlegt; Strichstärken als Integer
(empfohlen 2–10). G0–G3 entsprechen den G-Code-Befehlstypen.

| config-Schlüssel | Index | Bedeutung |
|------------------|-------|-----------|
| `iFarbeG0`       | 200   | Farbe G00 (Eilgang) |
| `iStaerkeG0`     | 201   | Strichstärke G00 |
| `iFarbeG1`       | 202   | Farbe G01 (Linear) |
| `iStaerkeG1`     | 203   | Strichstärke G01 |
| `iFarbeG2`       | 204   | Farbe G02 (Kreis CW) |
| `iStaerkeG2`     | 205   | Strichstärke G02 |
| `iFarbeG3`       | 206   | Farbe G03 (Kreis CCW) |
| `iStaerkeG3`     | 207   | Strichstärke G03 |
| `iFarbeLive`     | 210   | Farbe des Live-Stands |
| `iStaerkeLive`   | 211   | Strichstärke des Live-Stands |

### Live-Bild

| config-Schlüssel | Index | Richtung | Typ   | Bedeutung |
|------------------|-------|----------|-------|-----------|
| `iLiveStand`     | 212   | read     | int16 | aktueller Datensatz im laufenden Programm |
| `iLiveBreak`     | 213   | read     | bool  | Live-Modus abbrechen |

### Bogenglättung

| config-Schlüssel | Index | Richtung | Typ   | Bedeutung |
|------------------|-------|----------|-------|-----------|
| `iMinArcRadius`  | 214   | read     | float | Schwellwert-Radius (mm): Bögen (G02/G03) mit kleinerem Radius werden zu Linien (G01) — in Visualisierung **und** gesendeten Daten. **0 = deaktiviert.** |

## Weitere Konfigurationsdateien

- **`src/camera_config.txt`** — Kamera-/Bildverarbeitungs-Parameter (Belichtung, ROI,
  Threshold/Filter-Indizes). Gehört zum Bildverarbeitungs-Nebenzweig
  (`src/bildverarbeitung/`), nicht zum aktiven Datenbereitstellungs-Pfad.
- **`config/` (Top-Level)** — wird beim Build kopiert, ist aktuell leer.
- **`src/webVisu/WebServerConfig.txt`** — Platzhalter (leer).
