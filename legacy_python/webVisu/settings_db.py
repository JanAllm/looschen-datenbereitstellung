"""
settings_db.py
==============
Kleine SQLite-basierte Einstellungsverwaltung (Single Source of Truth) für die
Bootstrap-Konfiguration des C++-Dienstes.

Fluss:
  Web-UI  --(PUT /api/settings)-->  SQLite  --(render_config)-->  config.txt
  Der C++-Dienst liest beim Start unverändert seine config.txt.

Die DB liegt in einem persistenten Volume (Default /app/data/settings.db).
Beim ersten Start werden fehlende Schlüssel aus DEFAULTS geseedet.
"""

import os
import sqlite3

# ---------------------------------------------------------------------------
# Pfade (per Env überschreibbar)
# ---------------------------------------------------------------------------
DB_PATH = os.environ.get("SETTINGS_DB", "/app/data/settings.db")
CONFIG_PATH = os.environ.get("CONFIG_PATH", "/app/config.txt")

# ---------------------------------------------------------------------------
# Standardwerte (entsprechen der bisherigen config.txt; ipSPS/folderPath für Docker)
# ---------------------------------------------------------------------------
DEFAULTS = {
    # Verbindung & Ordner
    "ProgrammId": "OPCDATENVERARBEITUNG",
    "ipSPS": "opc.tcp://192.168.155.4:4840",
    # Namespace-Index der SPS-Variablen (gilt für ALLE Knoten: ns=<Wert>;i=<Index>)
    "ns": "1",
    "folderPath": "/data/projects",
    "MaximaleAnzahlProjekte": "16",
    "LenDataArray": "100",
    # Größen / WebServer
    "sizeObjx": "1000",
    "sizeObjy": "2000",
    "webserVerImgSizeX": "500",
    "webserVerImgSizeY": "1000",
    # Bogenglättung
    "iMinArcRadius": "214",
    # Allgemeine Node-Indizes
    "iError": "120",
    "iInfo": "122",
    "iHeartbeat": "123",
    "iStatusCode": "124",
    "iDataArry": "19",
    "iProjektArray": "138",
    "iUpdateProjektList": "125",
    "iProjektlistUpdated": "126",
    "showProjektInfo": "127",
    "iProjektName": "128",
    "iLenProjekt": "129",
    "iAnzDatenblöcke": "130",
    "ProjektVorhanden": "131",
    "ProjektInfoWrote": "132",
    "iReadProjekt": "133",
    "iWriteData": "134",
    "iIndexData": "135",
    "iReadData": "136",
    "iabbruchUbertragen": "137",
    "iUebertgarungBeendet": "139",
    "iUebertragungslaege": "140",
    # Zeichnungs-Design
    "iFarbeG0": "200",
    "iStaerkeG0": "201",
    "iFarbeG1": "202",
    "iStaerkeG1": "203",
    "iFarbeG2": "204",
    "iStaerkeG2": "205",
    "iFarbeG3": "206",
    "iStaerkeG3": "207",
    "iFarbeLive": "210",
    "iFarbeErledigt": "215",
    "iStaerkeLive": "211",
    "iStaerkeErledigt": "216",
    "iLiveStand": "212",
    "iLiveBreak": "213",
    # Canvas
    "objektgroesseX": "208",
    "objektgroesseY": "209",
    # Darstellung / Rendering (Web-Werte für die Vorschau OHNE SPS)
    "previewMinArcRadius": "0",
    "previewColorG0": "128,128,128",
    "previewColorG1": "0,255,0",
    "previewColorG2": "255,0,0",
    "previewColorG3": "0,0,255",
    "previewThicknessG0": "1",
    "previewThicknessG1": "2",
    "previewThicknessG2": "2",
    "previewThicknessG3": "2",
    # Fortschritts-Farben (aktuelle Position + erledigt) für Testdurchlauf/SPS-Live
    "previewLiveColor": "255,255,0",
    "previewDoneColor": "90,90,90",
    # Strichstärke der Fortschritts-Zustände (0 = normale Stärke behalten)
    "previewLiveThickness": "3",
    "previewDoneThickness": "0",
}

# ---------------------------------------------------------------------------
# UI-Metadaten: KOMPLETTES Settings-File, gruppiert nach den Kommentar-
# Abschnitten der config.txt. Jedes Feld: (key, label, type, typehint, help).
#   type     : "text" | "int"  -> Eingabefeld-Typ
#   typehint : erwarteter Typ (für das ?-Tooltip)
#   help     : Kurzbeschreibung (aus config.txt / setupVarInfos.h)
# Beschreibungen stammen aus dem C++-Programm (config.txt-Kommentare,
# include/interfaces/setupVarInfos.h, OPC-Datentypen aus SPSController.cpp).
#
# GROUPS: (Gruppenname, Kategorie, standardmäßig_aufgeklappt, [felder...])
# Kategorie trennt echte Programmwerte ("Allgemeine Einstellungen") von den
# OPC-UA-Node-Index-Zuordnungen ("OPC-Knoten").
# ---------------------------------------------------------------------------
CAT_GENERAL = "Allgemeine Einstellungen"
CAT_OPC = "OPC-Knoten"

GROUPS = [
    ("Programmeinstellungen", CAT_GENERAL, True, [
        ("ProgrammId", "ProgrammId", "text", "Text",
         "Programmeinstellungen / Kennung des Dienstes."),
        ("ipSPS", "ipSPS", "text", "Text (OPC-UA URL)",
         "SPS-IP-Adresse (OPC-UA-Endpunkt)."),
        ("folderPath", "folderPath", "text", "Text (Pfad)",
         "Ordner mit den CNC-Projektdateien. Im Container der Mount /data/projects."),
        ("MaximaleAnzahlProjekte", "MaximaleAnzahlProjekte", "int", "Ganzzahl",
         "Gibt die Maximalgröße der im Folder befindlichen Objekte vor "
         "(Das Array für den OPC Server darf nicht kleiner sein)."),
        ("LenDataArray", "LenDataArray", "int", "Ganzzahl",
         "Größe des Arrays welches zum Datenübertragen genutzt wird."),
    ]),
    ("OPC-Namespace", CAT_OPC, True, [
        ("ns", "ns", "int", "Ganzzahl (Namespace-Index)",
         "Namespace-Index der SPS-Variablen. Gilt für ALLE Knoten - jede Node-ID "
         "wird als ns=<Wert>;i=<Index> gebildet. Standard bei vielen SPSen: 1 "
         "(TIA/S7 oft 3 oder 4). Muss zum OPC-UA-Server der SPS passen."),
    ]),
    ("Allgemeine Variablen Knoten (Dialog etc)", CAT_OPC, False, [
        ("iError", "iError", "int", "Node-Index → Bool (write)",
         "Index der Fehlervariable (write, bool)."),
        ("iInfo", "iInfo", "int", "Node-Index → String (write)",
         "Index der Informationsvariable (write, string)."),
        ("iHeartbeat", "iHeartbeat", "int", "Node-Index → Int (write)",
         "Index der Herzschlagvariable (write, int)."),
        ("iStatusCode", "iStatusCode", "int", "Node-Index → Int16 (write)",
         "Index des Statuscode (write, int16_t)."),
    ]),
    ("Arrays", CAT_OPC, False, [
        ("iDataArry", "iDataArry", "int", "Node-Index → Float-Array (write)",
         "Index des Datenarrays (write, float array)."),
        ("iProjektArray", "iProjektArray", "int", "Node-Index → String-Array (write)",
         "Index des Projektarrays (write, string array)."),
    ]),
    ("Dialog: Liste der verfügbaren Projekte aktualisieren", CAT_OPC, False, [
        ("iUpdateProjektList", "iUpdateProjektList", "int", "Node-Index → Bool (read)",
         "Trigger der Aktualisierungsliste des Projekts (read, bool)."),
        ("iProjektlistUpdated", "iProjektlistUpdated", "int", "Node-Index → Bool (write)",
         "Liste wurde geupdated (write, bool)."),
    ]),
    ("Dialog: Informationen zum ausgewählten Projekt", CAT_OPC, False, [
        ("showProjektInfo", "showProjektInfo", "int", "Node-Index → Bool (read)",
         "Trigger Variable für die Info (read, bool)."),
        ("iProjektName", "iProjektName", "int", "Node-Index → String (read)",
         "Projekt aus dem String Array (read, string)."),
        ("iLenProjekt", "iLenProjekt", "int", "Node-Index → Int16 (write)",
         "Info zur Länge der Auswahl (write, int16_t)."),
        ("iAnzDatenblöcke", "iAnzDatenblöcke", "int", "Node-Index → Int16 (write)",
         "Anzahl der Datenblöcke die das Projekt zur Übertragung benötigt (write, int16_t)."),
        ("ProjektVorhanden", "ProjektVorhanden", "int", "Node-Index → Bool (write)",
         "Projekt vorhanden (write, bool)."),
        ("ProjektInfoWrote", "ProjektInfoWrote", "int", "Node-Index → Bool (write)",
         "Gibt an dass die Infos geschrieben wurden (write, bool)."),
    ]),
    ("Dialog: Ausgewähltes Projekt laden", CAT_OPC, False, [
        ("iReadProjekt", "iReadProjekt", "int", "Node-Index → Bool (read)",
         "Trigger: Ausgewähltes Projekt laden (read, bool)."),
        ("iWriteData", "iWriteData", "int", "Node-Index → Bool (write)",
         "Ausgabe dass ein Datenblock geschrieben wurde (write, bool)."),
        ("iIndexData", "iIndexData", "int", "Node-Index → Int16 (write)",
         "Index des aktuellen Datenblocks (write, int16_t)."),
        ("iReadData", "iReadData", "int", "Node-Index → Bool",
         "Wenn true wurde der Block von der SPS gelesen und der nächste wird geschrieben."),
        ("iabbruchUbertragen", "iabbruchUbertragen", "int", "Node-Index → Bool (read)",
         "Bricht die Datenübertragung ab (read, bool)."),
        ("iUebertgarungBeendet", "iUebertgarungBeendet", "int", "Node-Index → Bool (write)",
         "Variable dass die Übertragung beendet ist und die Anwendung wieder frei ist."),
    ]),
    ("Länge der auf einmal übertragenden Daten (Array Len)", CAT_OPC, False, [
        ("iUebertragungslaege", "iUebertragungslaege", "int", "Node-Index → Int16",
         "Länge der auf einmal übertragenden Daten (Array Len)."),
    ]),
    ("Designs der Zeichnungen", CAT_OPC, False, [
        ("iFarbeG0", "iFarbeG0", "int", "Node-Index → Int-Array [G,B,R]",
         "Farbe G0 [G(255),B(255),R(255)] of Int."),
        ("iStaerkeG0", "iStaerkeG0", "int", "Node-Index → Int",
         "Stärke G0 Int (Tipp: Werte zwischen 2 und 10)."),
        ("iFarbeG1", "iFarbeG1", "int", "Node-Index → Int-Array [G,B,R]",
         "Farbe G1 [G(255),B(255),R(255)] of Int."),
        ("iStaerkeG1", "iStaerkeG1", "int", "Node-Index → Int",
         "Stärke G1 Int."),
        ("iFarbeG2", "iFarbeG2", "int", "Node-Index → Int-Array [G,B,R]",
         "Farbe G2 [G(255),B(255),R(255)] of Int."),
        ("iStaerkeG2", "iStaerkeG2", "int", "Node-Index → Int",
         "Stärke G2 Int."),
        ("iFarbeG3", "iFarbeG3", "int", "Node-Index → Int-Array [G,B,R]",
         "Farbe G3 [G(255),B(255),R(255)] of Int."),
        ("iStaerkeG3", "iStaerkeG3", "int", "Node-Index → Int",
         "Stärke G3 Int."),
        ("iFarbeLive", "iFarbeLive", "int", "Node-Index → Int-Array [G,B,R]",
         "Farbe der AKTUELLEN Position [G(255),B(255),R(255)] of Int."),
        ("iFarbeErledigt", "iFarbeErledigt", "int", "Node-Index → Int-Array [G,B,R] (OPTIONAL)",
         "Farbe bereits ABGEARBEITETER Abschnitte [G(255),B(255),R(255)] of Int. "
         "OPTIONAL: fehlt der Knoten auf der SPS, wird der Default (Grau) genutzt "
         "und das Setup schlägt NICHT fehl."),
        ("iStaerkeLive", "iStaerkeLive", "int", "Node-Index → Int",
         "Stärke der AKTUELLEN Position Int (Tipp: Werte zwischen 2 und 10)."),
        ("iStaerkeErledigt", "iStaerkeErledigt", "int", "Node-Index → Int (OPTIONAL)",
         "Stärke bereits ABGEARBEITETER Abschnitte Int. OPTIONAL: fehlt der Knoten, "
         "wird der Default (0 = normale Stärke) genutzt und das Setup schlägt NICHT fehl."),
    ]),
    ("Live Bild", CAT_OPC, False, [
        ("iLiveStand", "iLiveStand", "int", "Node-Index → Int (read)",
         "Aktueller Stand im Programm."),
        ("iLiveBreak", "iLiveBreak", "int", "Node-Index → Bool (read)",
         "abbruchLive - bricht das Live-Bild ab."),
    ]),
    ("Bogenglättung", CAT_OPC, True, [
        ("iMinArcRadius", "iMinArcRadius", "int", "Node-Index → Float (read)",
         "Schwellwert-Radius (float): Bögen (G02/G03) mit kleinerem Radius werden zu "
         "Linien (G01) umgewandelt - in der Visualisierung UND in den gesendeten Daten. "
         "Wert 0 = deaktiviert."),
    ]),
    ("Canvas Größe", CAT_OPC, False, [
        ("objektgroesseX", "objektgroesseX", "int", "Node-Index → Int (read)",
         "Größe des Objekts in X-Richtung (read, int)."),
        ("objektgroesseY", "objektgroesseY", "int", "Node-Index → Int (read)",
         "Größe des Objekts in Y-Richtung (read, int)."),
    ]),
    ("Darstellung (Vorschau ohne SPS)", CAT_GENERAL, True, [
        ("previewMinArcRadius", "Bogenglättung-Radius (mm)", "text", "Zahl (mm), 0 = aus",
         "Bögen (G02/G03) mit kleinerem Radius werden als Linie gezeichnet. "
         "Web-Wert für die Vorschau ohne SPS. 0 = deaktiviert."),
        ("previewColorG0", "Farbe G0 – Eilgang (R,G,B)", "text", "R,G,B (0-255)",
         "Farbe für Eilbewegungen (G00)."),
        ("previewColorG1", "Farbe G1 – Linear (R,G,B)", "text", "R,G,B (0-255)",
         "Farbe für Linearbewegungen (G01)."),
        ("previewColorG2", "Farbe G2 – Kreis CW (R,G,B)", "text", "R,G,B (0-255)",
         "Farbe für Kreisbögen im Uhrzeigersinn (G02)."),
        ("previewColorG3", "Farbe G3 – Kreis CCW (R,G,B)", "text", "R,G,B (0-255)",
         "Farbe für Kreisbögen gegen den Uhrzeigersinn (G03)."),
        ("previewThicknessG0", "Strichstärke G0", "int", "Ganzzahl (2-10)",
         "Linienstärke für G00."),
        ("previewThicknessG1", "Strichstärke G1", "int", "Ganzzahl (2-10)",
         "Linienstärke für G01."),
        ("previewThicknessG2", "Strichstärke G2", "int", "Ganzzahl (2-10)",
         "Linienstärke für G02."),
        ("previewThicknessG3", "Strichstärke G3", "int", "Ganzzahl (2-10)",
         "Linienstärke für G03."),
        ("previewLiveColor", "Farbe AKTUELLE Position (R,G,B)", "text", "R,G,B (0-255)",
         "Fortschritts-Farbe für die aktuell abgearbeitete Position im "
         "Testdurchlauf (und SPS-Livebild)."),
        ("previewDoneColor", "Farbe ERLEDIGT (R,G,B)", "text", "R,G,B (0-255)",
         "Fortschritts-Farbe für bereits abgearbeitete Abschnitte im "
         "Testdurchlauf (und SPS-Livebild)."),
        ("previewLiveThickness", "Strichstärke AKTUELLE Position", "int", "Ganzzahl (1-10)",
         "Strichstärke der aktuell abgearbeiteten Position (hervorgehoben)."),
        ("previewDoneThickness", "Strichstärke ERLEDIGT", "int", "Ganzzahl (0-10)",
         "Strichstärke bereits abgearbeiteter Abschnitte. 0 = normale Stärke behalten."),
    ]),
    ("WebServer", CAT_GENERAL, True, [
        ("sizeObjx", "sizeObjx", "int", "Ganzzahl (px)",
         "Render-Breite des Vorschaubilds."),
        ("sizeObjy", "sizeObjy", "int", "Ganzzahl (px)",
         "Render-Höhe des Vorschaubilds."),
        ("webserVerImgSizeX", "webserVerImgSizeX", "int", "Ganzzahl (px)",
         "Breite, auf die das Bild für die Web-Anzeige skaliert wird."),
        ("webserVerImgSizeY", "webserVerImgSizeY", "int", "Ganzzahl (px)",
         "Höhe, auf die das Bild für die Web-Anzeige skaliert wird."),
    ]),
]


# Live-Werte: werden vom C++-Worker pro Render frisch gelesen -> wirken sofort,
# ohne Dienst-Neustart. Alle übrigen Schlüssel (Bootstrap: ipSPS/folderPath/
# Node-Indizes/Größen) brauchen einen Neustart.
LIVE_KEYS = {
    "previewMinArcRadius",
    "previewColorG0", "previewColorG1", "previewColorG2", "previewColorG3",
    "previewThicknessG0", "previewThicknessG1", "previewThicknessG2", "previewThicknessG3",
    "previewLiveColor", "previewDoneColor",
    "previewLiveThickness", "previewDoneThickness",
}


def _connect():
    os.makedirs(os.path.dirname(DB_PATH) or ".", exist_ok=True)
    conn = sqlite3.connect(DB_PATH)
    conn.execute(
        "CREATE TABLE IF NOT EXISTS settings (key TEXT PRIMARY KEY, value TEXT NOT NULL)"
    )
    return conn


def init_db():
    """Legt die Tabelle an und seedet fehlende Schlüssel aus DEFAULTS."""
    conn = _connect()
    try:
        for key, value in DEFAULTS.items():
            conn.execute(
                "INSERT OR IGNORE INTO settings (key, value) VALUES (?, ?)",
                (key, value),
            )
        conn.commit()
    finally:
        conn.close()


def get_all():
    """Alle Settings als dict (DEFAULTS als Fallback für fehlende Schlüssel)."""
    conn = _connect()
    try:
        rows = conn.execute("SELECT key, value FROM settings").fetchall()
    finally:
        conn.close()
    result = dict(DEFAULTS)
    result.update({k: v for k, v in rows})
    return result


def set_many(values):
    """Speichert mehrere Settings (dict key->value)."""
    conn = _connect()
    try:
        for key, value in values.items():
            conn.execute(
                "INSERT INTO settings (key, value) VALUES (?, ?) "
                "ON CONFLICT(key) DO UPDATE SET value = excluded.value",
                (key, str(value)),
            )
        conn.commit()
    finally:
        conn.close()


def render_config(path=None):
    """Schreibt die aktuellen Settings als key=value nach config.txt."""
    path = path or CONFIG_PATH
    settings = get_all()
    lines = ["# Automatisch generiert aus settings.db - NICHT manuell editieren.\n"]
    for key, value in settings.items():
        lines.append(f"{key}={value}\n")
    os.makedirs(os.path.dirname(path) or ".", exist_ok=True)
    with open(path, "w", encoding="utf-8") as f:
        f.writelines(lines)
    return path


def ui_schema():
    """Gruppierte Feld-Metadaten für die Web-UI. Deckt das KOMPLETTE Settings-
    File ab: jeder DEFAULTS-Schlüssel erscheint in genau einer Gruppe."""
    current = get_all()
    seen = set()
    groups = []
    for name, category, is_open, fields in GROUPS:
        out_fields = []
        for key, label, ftype, typehint, help_text in fields:
            seen.add(key)
            out_fields.append({
                "key": key, "label": label, "type": ftype,
                "typehint": typehint, "help": help_text,
                "value": current.get(key, ""),
            })
        groups.append({"name": name, "category": category,
                       "open": is_open, "fields": out_fields})

    # Sicherheitsnetz: jeden nicht erfassten DEFAULTS-Schlüssel anhängen,
    # damit wirklich das komplette Settings-File konfigurierbar ist.
    extra = [k for k in DEFAULTS if k not in seen]
    if extra:
        groups.append({
            "name": "Sonstige", "category": CAT_OPC, "open": False,
            "fields": [{"key": k, "label": k, "type": "int",
                        "typehint": "Ganzzahl", "help": "",
                        "value": current.get(k, "")} for k in extra],
        })

    # Nach Kategorie sortieren: erst Allgemeine Einstellungen, dann OPC-Knoten
    # (stabil -> Reihenfolge innerhalb der Kategorie bleibt erhalten).
    order = {CAT_GENERAL: 0, CAT_OPC: 1}
    groups.sort(key=lambda g: order.get(g["category"], 99))
    return groups
