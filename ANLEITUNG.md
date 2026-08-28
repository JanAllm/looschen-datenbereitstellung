# Anleitung – Looschen Datenbereitstellung (Docker + Weboberfläche)

Diese Anleitung führt von der Docker-Installation bis zur Bedienung der
Weboberfläche. Sie richtet sich an Erstinstallation und laufenden Betrieb.

---

## 1. Was macht die Anwendung?

Der Dienst stellt einer **SPS** (über **OPC UA**) CNC-Projektdaten bereit: Er liest
G-Code-Projektdateien (`.tap`) aus einem Ordner, überträgt sie zur SPS und rendert
Vorschaubilder. Zusätzlich gibt es eine **Weboberfläche** zum Konfigurieren und einen
**Vorschaumodus, der komplett ohne SPS funktioniert** (Bilder ansehen, Testdurchlauf).

Der Webserver läuft auf **Port 5000** und ist auch dann erreichbar, wenn (noch) keine
SPS verbunden ist.

---

## 2. Voraussetzungen

- **Windows:** [Docker Desktop](https://www.docker.com/products/docker-desktop/) installieren
  (inkl. WSL2-Backend). Nach der Installation Docker Desktop starten.
- **Linux:** Docker Engine + Compose-Plugin
  (`sudo apt install docker.io docker-compose-plugin` o. ä.), Docker-Dienst starten.
- Prüfen, dass Docker läuft:
  ```bash
  docker --version
  docker compose version
  ```

---

## 3. Projektordner (Mount)

Der Container liest die CNC-Projektdateien aus einem **gemounteten Host-Ordner**. Der
Mount wird beim Start festgelegt (nicht zur Laufzeit änderbar).

- **Standard:** der mitgelieferte Testdatensatz `./BeispielDatensatz` wird nach
  `/data/projects` im Container gemountet – zum Ausprobieren ohne eigene Daten.
- **Eigener Ordner:** die Umgebungsvariable `PROJECT_FOLDER` auf den Host-Pfad setzen
  (siehe unten). Der Ordner sollte die `.tap`-Dateien direkt enthalten.

> Im Container heißt der Ordner immer `/data/projects` – in den Einstellungen steht
> `folderPath = /data/projects`. Ändere diesen Wert nur, wenn du einen Unterordner
> darunter verwenden willst.

---

## 4. Installation & Start

Im Projektverzeichnis (dort, wo `docker-compose.yml` liegt):

### Windows (Docker Desktop)

```bash
# Bauen (einmalig bzw. nach Code-Änderungen)
docker compose -f docker-compose.yml -f compose.windows.yml build

# Starten (im Hintergrund)
docker compose -f docker-compose.yml -f compose.windows.yml up -d
```

Eigenen Projektordner mounten (statt Testdatensatz):

```bash
# PowerShell
$env:PROJECT_FOLDER = "D:/Pfad/zu/meinen/Projekten"
docker compose -f docker-compose.yml -f compose.windows.yml up -d
```

### Linux (Server)

```bash
docker compose -f docker-compose.yml -f compose.linux.yml build
docker compose -f docker-compose.yml -f compose.linux.yml up -d

# Eigener Ordner:
PROJECT_FOLDER=/srv/cnc/projekte docker compose -f docker-compose.yml -f compose.linux.yml up -d
```

*(Linux nutzt das Host-Netzwerk, damit die SPS im LAN direkt erreichbar ist.)*

### Verteilung über Docker Hub (ohne Quellcode / ohne Build)

Auf einem Zielrechner, der das Projekt **nicht** selbst bauen soll, wird das
fertige Image direkt von Docker Hub gezogen. Dafür genügen die Dateien
`compose.deploy.yml`, `compose.windows.yml` bzw. `compose.linux.yml` und (optional)
ein Projektordner – **kein Quellcode nötig**.

```bash
# Image ziehen
docker pull janallm/looschen-datenbereitstellung:latest

# Windows (Docker Desktop)
docker compose -f compose.deploy.yml -f compose.windows.yml up -d

# Linux (Server)
docker compose -f compose.deploy.yml -f compose.linux.yml up -d
```

Eigenen Projektordner mounten – wie oben über `PROJECT_FOLDER`:

```bash
# PowerShell (Windows)
$env:PROJECT_FOLDER = "D:/Pfad/zu/meinen/Projekten"
docker compose -f compose.deploy.yml -f compose.windows.yml up -d

# Linux
PROJECT_FOLDER=/srv/cnc/projekte docker compose -f compose.deploy.yml -f compose.linux.yml up -d
```

> `compose.deploy.yml` verweist auf `image: janallm/looschen-datenbereitstellung:latest`
> (statt lokal zu bauen). Ein fester Stand lässt sich über das Versions-Tag ziehen,
> z. B. `janallm/looschen-datenbereitstellung:1.0`.
> Repository: <https://hub.docker.com/r/janallm/looschen-datenbereitstellung>

### Image selbst hosten (ohne Docker Hub)

Wenn das Image nicht über Docker Hub laufen soll — etwa weil die Anlage keinen
Internetzugang hat oder ein eigener Stand verteilt wird — gibt es zwei Wege.
In beiden Fällen wird das Image zuerst aus dem Quellcode gebaut
(`docker compose build`, ergibt `looschen-datenbereitstellung:latest`).

**Weg 1 – Image als Datei übergeben (offline, der einfachste Weg):**

```bash
# Auf dem Build-Rechner: Image in eine Datei packen
docker save looschen-datenbereitstellung:latest | gzip > looschen-image.tar.gz

# Datei auf den Zielrechner bringen (USB-Stick, scp, …) und dort laden:
docker load < looschen-image.tar.gz

# Unter dem Namen bereitstellen, den compose.deploy.yml erwartet:
docker tag looschen-datenbereitstellung:latest janallm/looschen-datenbereitstellung:latest
docker compose -f compose.deploy.yml -f compose.linux.yml up -d
```

> Für den **Raspberry Pi** muss das Image als ARM64 gebaut werden, bevor es
> gepackt wird: `docker buildx build --platform linux/arm64 -t
> looschen-datenbereitstellung:latest --load .`

**Weg 2 – eigene Registry im Netzwerk** (wenn mehrere Rechner regelmäßig
ziehen sollen):

```bash
# Einmalig auf einem Server im Netz. Port 5001, denn 5000 belegt die Anwendung selbst:
docker run -d --restart=always --name registry -p 5001:5000 \
  -v /srv/registry:/var/lib/registry registry:2

# Image dorthin schieben:
docker tag looschen-datenbereitstellung:latest <server-ip>:5001/looschen-datenbereitstellung:latest
docker push <server-ip>:5001/looschen-datenbereitstellung:latest
```

Jeder Zielrechner muss die Registry einmalig als HTTP-Registry erlauben — in
`/etc/docker/daemon.json` (Docker Desktop: Settings → Docker Engine):

```json
{ "insecure-registries": ["<server-ip>:5001"] }
```

danach Docker neu starten. Ziehen und starten dann so:

```bash
docker pull <server-ip>:5001/looschen-datenbereitstellung:latest
docker tag <server-ip>:5001/looschen-datenbereitstellung:latest janallm/looschen-datenbereitstellung:latest
docker compose -f compose.deploy.yml -f compose.linux.yml up -d
```

> „insecure" heißt hier nur: HTTP statt HTTPS. Im eigenen, vertrauenswürdigen
> Netz ist das üblich; soll die Registry über Netzgrenzen erreichbar sein,
> braucht sie ein TLS-Zertifikat (siehe Docker-Registry-Doku).

### Aufrufen

Im Browser öffnen:

```
https://localhost:5000        (Windows / lokal)
https://<host-ip>:5000        (Linux-Server im Netz)
```

> **Zertifikatswarnung:** Der Server nutzt ein **selbstsigniertes HTTPS-Zertifikat**.
> Der Browser warnt beim ersten Aufruf – „Erweitert → Trotzdem fortfahren" wählen.

---

## 5. Erste Konfiguration (Weboberfläche)

Alle wichtigen Einstellungen werden über **`/settings`** vorgenommen und in einer
kleinen Datenbank gespeichert (bleibt über Neustarts erhalten). Eine OPC-/SPS-Verbindung
ist dafür **nicht** nötig.

1. `https://localhost:5000/settings` öffnen.
2. Oben rechts zeigt eine Anzeige den **SPS-Status** (grün „verbunden" / rot „getrennt").
3. Unter **Allgemeine Einstellungen** mindestens setzen:
   - **ipSPS** – der OPC-UA-Endpunkt der SPS, z. B. `opc.tcp://192.168.155.4:4840`
   - **folderPath** – bleibt i. d. R. `/data/projects` (der gemountete Ordner)
4. **„Speichern & Übernehmen"** klicken. Änderungen an SPS-Verbindung/Ordner werden
   **sofort übernommen – ohne Neustart**. Ändert sich die SPS-Verbindung oder ein
   Knoten-Index, baut der Dienst die OPC-Verbindung von selbst neu auf.

Jedes Feld hat ein **?**-Symbol – ein Klick öffnet ein Popup mit Typ und Beschreibung.

Die Einstellungen sind in zwei Bereiche unterteilt:
- **Allgemeine Einstellungen** – echte Programmwerte (SPS-IP, Ordner, Größen,
  Darstellung/Farben/Radius für die Vorschau).
- **OPC-Knoten** – die Node-Index-Zuordnungen für die SPS-Kommunikation.

---

## 6. Die Weboberfläche im Überblick

### `/` – Startseite (leitet auf die Einstellungen weiter)

Ruft man den Server ohne Pfad auf, landet man direkt unter `/settings`.

### `/live` – Live-Bild (SPS-Visualisierung)
Zeigt ausschließlich das aktuelle Bild (das auch die SPS ausliest) auf schwarzem
Hintergrund, ohne Bedienelemente. Für die reine Anzeige an der Maschine gedacht.

### `/settings` – Einstellungen

Ganz oben zeigt **OPC-Knoten – Live-Diagnose** für jeden konfigurierten Knoten,
ob er auf der SPS **existiert** und ob er laut Server **lesbar/schreibbar** ist
(gelesen wird nur das AccessLevel-Attribut – es wird nichts auf die SPS geschrieben).
Fehlende Knoten sind rot markiert; so sieht man sofort, welcher Index/Namespace
falsch gesetzt ist.

Komplette Konfiguration (siehe Abschnitt 5): SPS-Endpunkt, Projektordner, Größen,
Darstellung (Farben/Radius) und die OPC-Node-Indizes. Mit SPS-Statusanzeige und
`?`-Hilfe zu jedem Feld.

### `/preview` – Vorschau **ohne SPS**
Rendert die G-Code-Projekte mit demselben Renderer wie die SPS-Visualisierung – ohne
SPS-Verbindung:

- **Projektliste** – zeigt alle `.tap`-Dateien aus dem Projektordner; „Liste
  aktualisieren" liest neu.
- **Gesamtbild** – „Jetzt rendern" zeichnet das komplette Projekt.
- **Testdurchlauf** – zeichnet den Werkzeugweg **Schritt für Schritt** mit einstellbarem
  **Zeitabstand (ms)**; „Stop" bricht ab.
- **Darstellung** – Farben pro G-Code-Typ (Legende):
  - **G0** – Eilgang · **G1** – Linear · **G2** – Kreis im Uhrzeigersinn ·
    **G3** – Kreis gegen den Uhrzeigersinn
  - **Bogenglättung-Radius (mm)** – Bögen mit kleinerem Radius werden als Linie
    gezeichnet (`0` = aus). Farben und Radius wirken **sofort** (kein Neustart).
- **Zoom** im Bild: **Mausrad** = Zoom, **Ziehen** = Verschieben,
  **Doppelklick** = zurücksetzen.

---

## 7. Betrieb

```bash
# Logs ansehen
docker compose -f docker-compose.yml -f compose.windows.yml logs -f
#   (oder:) docker logs -f looschen-datenbereitstellung

# Status
docker ps

# Neu starten
docker compose -f docker-compose.yml -f compose.windows.yml restart

# Stoppen und entfernen
docker compose -f docker-compose.yml -f compose.windows.yml down
```

- Der Container startet automatisch neu (`restart: unless-stopped`), u. a. nach dem
  Speichern von Einstellungen.
- **Persistenz:** Die Einstellungs-Datenbank liegt im Volume `./data` auf dem Host und
  überlebt Neustarts/Updates.

---

## 8. Troubleshooting

| Problem | Ursache / Lösung |
|--------|------------------|
| Browser zeigt Zertifikatswarnung | Selbstsigniertes HTTPS – „Trotzdem fortfahren". |
| SPS-Status bleibt „getrennt" | `ipSPS` prüfen; SPS im Netz erreichbar? Unter Windows/Docker-Desktop muss die SPS vom Host aus erreichbar sein. |
| Keine Projekte in `/preview` | Ist der richtige Ordner gemountet? `PROJECT_FOLDER` beim Start gesetzt? Enthält der Ordner `.tap`-Dateien? |
| Einstellungen wirken nicht | Alle Werte wirken sofort. Bei SPS-Problemen die Tabelle **OPC-Knoten – Live-Diagnose** unter `/settings` prüfen: dort steht je Knoten, ob er existiert und les-/schreibbar ist. |
| Port 5000 belegt | Anderen Dienst beenden oder Port-Mapping in `compose.windows.yml` anpassen. |

---

## 9. Kurzreferenz

| | |
|---|---|
| Weboberfläche | `https://<host>:5000` |
| Startseite (→ Einstellungen) | `/` |
| Live-Bild | `/live` |
| Bild (rohes PNG) | `/image` |
| Einstellungen | `/settings` |
| Vorschau (ohne SPS) | `/preview` |
| Projektordner im Container | `/data/projects` (Host-Mount) |
| Einstellungs-DB (Host) | `./data/settings.db` |
| Bauen | `docker compose -f docker-compose.yml -f compose.<os>.yml build` |
| Starten | `docker compose -f docker-compose.yml -f compose.<os>.yml up -d` |
