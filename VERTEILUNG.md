# Installation & Verteilung – Schritt für Schritt

Diese Anleitung richtet sich an **Einsteiger ohne Terminal-Erfahrung**. Wenn du
sie von oben nach unten befolgst, läuft die **Looschen Datenbereitstellung** am
Ende im Browser – auch für andere Geräte im Netzwerk.

Du musst **nichts programmieren und nichts bauen**. Es wird ein fertiges Paket
(„Image") aus dem Internet geladen und gestartet.

- **Image:** `janallm/looschen-datenbereitstellung`
- **Läuft auf:** Windows (Docker Desktop) und Linux
- **Bedienung danach:** komplett im Browser

---

## Überblick: Was machen wir?

1. Docker Desktop installieren (einmalig).
2. Den `deploy`-Ordner auf den Rechner kopieren.
3. In der Datei `.env` deinen Projektordner eintragen.
4. Ein Terminal im `deploy`-Ordner öffnen und **einen** Befehl eingeben.
5. Die Weboberfläche im Browser öffnen.

Später brauchst du nur noch Schritt 4/5.

---

## Schritt 1 – Docker Desktop installieren (einmalig)

1. Öffne im Browser: <https://www.docker.com/products/docker-desktop/>
2. Klicke auf **„Download for Windows"** und installiere das Programm
   (einfach den Anweisungen folgen, ggf. Neustart des PCs).
3. Starte **Docker Desktop** über das Startmenü. Warte, bis unten links das
   Wal-Symbol **grün** ist bzw. „Engine running" steht.

> Docker Desktop muss **laufen**, sonst funktionieren die späteren Befehle nicht.
> Es startet nach der Installation normalerweise automatisch mit Windows.

---

## Schritt 2 – Den `deploy`-Ordner bereitlegen

Du hast einen Ordner namens **`deploy`** bekommen (z. B. als ZIP-Datei). Darin
liegen diese Dateien:

```
compose.deploy.yml
compose.windows.yml
compose.linux.yml
.env.example
VERTEILUNG.md   (diese Anleitung)
```

1. Falls es ein ZIP ist: **Rechtsklick → „Alle extrahieren"**.
2. Lege den Ordner an einen einfachen Ort, z. B. `C:\looschen\deploy`.

> Merke dir diesen Ort – wir öffnen ihn gleich.

---

## Schritt 3 – Deinen Projektordner eintragen (`.env`)

Die Anwendung muss wissen, **wo deine G-Code-Dateien liegen**. Das trägst du
einmal in eine kleine Textdatei ein.

1. Öffne den `deploy`-Ordner im Windows-Explorer.
2. Kopiere die Datei **`.env.example`** und benenne die Kopie in **`.env`** um
   (also ohne `.example` am Ende).
   - Falls Windows die Dateiendungen ausblendet: Im Explorer oben unter
     **Ansicht → Dateinamenerweiterungen** einschalten.
3. Öffne die Datei **`.env`** mit einem Doppelklick (oder Rechtsklick →
   „Öffnen mit → Editor/Notepad").
4. Trage bei `PROJECT_FOLDER` den Pfad zu deinem Projektordner ein. **Wichtig:
   Schrägstriche nach vorne (`/`), nicht nach hinten (`\`).**

   Beispiel:
   ```
   PROJECT_FOLDER=D:/CNC/Projekte
   ```
5. Speichern und schließen.

> Ohne diesen Eintrag findet die Anwendung keine Projekte. Der Ordner muss
> wirklich existieren und deine `.tap`/G-Code-Dateien enthalten.

---

## Schritt 4 – Terminal im `deploy`-Ordner öffnen

„Terminal" (auch „PowerShell") ist ein Fenster, in das man Befehle tippt. So
öffnest du es **direkt im richtigen Ordner** (das erspart das Wechseln von
Verzeichnissen):

1. Öffne den `deploy`-Ordner im Windows-Explorer.
2. Klicke oben in die **Adressleiste** (wo der Pfad steht), sodass sie leer/blau
   markiert ist.
3. Tippe dort **`powershell`** und drücke **Enter**.
4. Es öffnet sich ein blaues/schwarzes Fenster – das ist das Terminal. In der
   ersten Zeile sollte der Pfad zu deinem `deploy`-Ordner stehen, z. B.
   `PS C:\looschen\deploy>`.

> Alternativ: Im Explorer **Umschalt (Shift) + Rechtsklick** auf eine freie
> Stelle im Ordner → „PowerShell-Fenster hier öffnen" bzw. „Im Terminal öffnen".

---

## Schritt 5 – Anwendung starten

Tippe im Terminal diesen **einen** Befehl (oder kopiere ihn mit Copy & Paste –
Einfügen im Terminal per **Rechtsklick**) und drücke **Enter**:

```powershell
docker compose -f compose.deploy.yml -f compose.windows.yml up -d
```

Was passiert:
- Beim **ersten Mal** lädt Docker das Image aus dem Internet (kann ein paar
  Minuten dauern – ein Fortschrittsbalken läuft).
- Danach startet der Container. Zum Schluss erscheint eine Zeile wie
  `Container looschen-datenbereitstellung  Started`.

**Prüfen, ob es läuft:**
```powershell
docker ps
```
In der Ausgabe sollte eine Zeile mit `looschen-datenbereitstellung`,
`Up` und `0.0.0.0:5000->5000/tcp` stehen. Fertig gestartet!

---

## Schritt 6 – Weboberfläche öffnen

Öffne einen Browser (Chrome, Edge, Firefox …) und gib ein:

```
https://localhost:5000
```

- Beim ersten Aufruf kommt eine **Sicherheitswarnung** wegen des Zertifikats.
  Das ist normal (die Anwendung nutzt ein selbst erzeugtes HTTPS-Zertifikat):
  **„Erweitert" → „Weiter zu localhost (unsicher)"** klicken.
- Danach siehst du die Oberfläche. Unter **`/settings`** stellst du z. B. die
  SPS-IP ein, unter **`/preview`** kannst du Projekte ohne SPS ansehen.

---

## Von anderen Geräten im Netzwerk zugreifen

Die Oberfläche ist auch von anderen PCs/Tablets im **gleichen Netzwerk**
erreichbar – über die **IP-Adresse des Rechners**, auf dem Docker läuft.

1. IP-Adresse herausfinden (im Terminal):
   ```powershell
   ipconfig
   ```
   Suche unter deinem Netzwerkadapter die **„IPv4-Adresse"**, z. B.
   `192.168.178.24`.
2. Auf dem anderen Gerät im Browser öffnen:
   ```
   https://192.168.178.24:5000
   ```
   (deine IP einsetzen)

**Falls andere Geräte nicht verbinden können**, blockiert meist die
**Windows-Firewall** den Port. Einmalig freigeben:

1. Startmenü → **„PowerShell"** suchen → **Rechtsklick → „Als Administrator
   ausführen"**.
2. Diesen Befehl eingeben und Enter:
   ```powershell
   New-NetFirewallRule -DisplayName "Looschen Web 5000" -Direction Inbound -Protocol TCP -LocalPort 5000 -Action Allow
   ```

> Tipp: Die IP-Adresse kann sich nach einem Neustart ändern. Damit die Adresse
> gleich bleibt, im Router eine **feste IP / DHCP-Reservierung** für diesen
> Rechner einrichten.

---

## Alltag: Stoppen, Starten, Aktualisieren

Immer zuerst das Terminal im `deploy`-Ordner öffnen (Schritt 4).

**Läuft es? / Status ansehen:**
```powershell
docker ps
```

**Stoppen (Container entfernen, Einstellungen bleiben erhalten):**
```powershell
docker compose -f compose.deploy.yml -f compose.windows.yml down
```

**Wieder starten:**
```powershell
docker compose -f compose.deploy.yml -f compose.windows.yml up -d
```

**Auf eine neue Version aktualisieren:**
```powershell
docker pull janallm/looschen-datenbereitstellung:latest
docker compose -f compose.deploy.yml -f compose.windows.yml up -d
```
Deine Einstellungen (im Ordner `deploy\data`) bleiben dabei erhalten.

---

## Häufige Probleme (in einfachen Worten)

| Was du siehst | Was los ist / Lösung |
|---|---|
| `... is already in use by container ...` | Es läuft schon ein Container mit dem Namen. Entferne ihn: `docker rm -f looschen-datenbereitstellung`, dann erneut starten. |
| `docker: unknown command: delete` | Der Befehl heißt **`docker rm`**, nicht `docker delete`. Zum Entfernen: `docker rm -f looschen-datenbereitstellung`. |
| `open compose.deploy.yml: ... nicht finden` | Das Terminal ist im falschen Ordner. Terminal **im `deploy`-Ordner** öffnen (Schritt 4). |
| `Cannot connect to the Docker daemon` | **Docker Desktop läuft nicht.** Erst Docker Desktop starten (Wal-Symbol grün), dann erneut versuchen. |
| Browser: Zertifikatswarnung | Normal. Über „Erweitert → Weiter/Fortfahren" bestätigen. |
| Keine Projekte in der Vorschau | In der `.env` ist `PROJECT_FOLDER` nicht (richtig) gesetzt, oder der Ordner ist leer. Schrägstriche `/` verwenden. |
| Anderes Gerät kommt nicht drauf | Firewall-Regel für Port 5000 anlegen (siehe oben) und richtige IP verwenden. |

---

## Ganz ohne Compose-Datei (Kurzform für Fortgeschrittene)

Man kann den Container auch mit einem einzigen Befehl starten – die Angaben aus
den Compose-Dateien werden dann direkt mitgegeben. Passe die beiden Pfade an:

```powershell
docker run -d --name looschen-datenbereitstellung --restart unless-stopped -p 5000:5000 -v D:/looschen-data:/app/data -v D:/CNC/Projekte:/data/projects:ro janallm/looschen-datenbereitstellung:latest
```

- `-p 5000:5000` = Weboberfläche erreichbar (nötig)
- `-v ...:/data/projects:ro` = dein G-Code-Ordner (nur lesend)
- `-v ...:/app/data` = Einstellungen dauerhaft speichern

---

## Kurzreferenz (zum Nachschlagen)

| Aktion | Befehl (im `deploy`-Ordner) |
|---|---|
| Starten | `docker compose -f compose.deploy.yml -f compose.windows.yml up -d` |
| Status | `docker ps` |
| Stoppen | `docker compose -f compose.deploy.yml -f compose.windows.yml down` |
| Aktualisieren | `docker pull janallm/looschen-datenbereitstellung:latest` danach `... up -d` |
| Konflikt lösen | `docker rm -f looschen-datenbereitstellung` |

> **Linux statt Windows?** Überall `compose.windows.yml` durch
> `compose.linux.yml` ersetzen. Terminal öffnen mit „im Ordner ein Terminal
> öffnen"; der Rest ist identisch.
