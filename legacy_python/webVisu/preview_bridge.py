"""
preview_bridge.py
=================
Vermittler zwischen Flask (Python) und dem C++-Preview-Worker.

Ablauf (rendern mit C++, ohne SPS):
  Flask-Route  --enqueue(cmd)-->  [Queue]  --poll()-->  C++-Worker
  C++-Worker   --set_status / set_projects / set_sps-->  [State]  --get_*-->  Flask

Der C++-Worker importiert dasselbe Modul (aus sys.modules), daher ist der
Zustand zwischen Flask-Thread und C++-Thread geteilt. Alle Zugriffe unter Lock.
Befehle/Status werden als JSON-Strings über die C++-Grenze gereicht.
"""

import json
import threading

_lock = threading.Lock()

_cmd = None  # ausstehender Befehl (dict) oder None
_stop = False  # Stop-Anforderung für einen laufenden Testdurchlauf
_status = {"running": False, "step": 0, "total": 0, "project": None, "message": "", "gen": 0}
_projects = {"folder": "", "projects": []}
_sps = {"connected": False}


# ---- Flask -> Worker ----
def enqueue(cmd):
    """Flask legt einen Befehl ab (dict), z.B. {'action':'render','project':'x.tap'}."""
    global _cmd
    with _lock:
        _cmd = cmd


def request_stop():
    """Flask fordert Abbruch eines laufenden Testdurchlaufs an."""
    global _stop
    with _lock:
        _stop = True


# ---- Worker (C++) ----
def poll():
    """C++ holt den nächsten Befehl (JSON-String) und leert die Queue. '' = keiner."""
    global _cmd
    with _lock:
        c = _cmd
        _cmd = None
    return json.dumps(c) if c else ""


def take_stop():
    """C++ liest die Stop-Anforderung und setzt sie zurück (1/0 als String)."""
    global _stop
    with _lock:
        s = _stop
        _stop = False
    return "1" if s else "0"


def set_status(js):
    """C++ meldet den Fortschritt (JSON-String)."""
    global _status
    try:
        val = json.loads(js)
    except (ValueError, TypeError):
        return
    with _lock:
        _status = val


def set_projects(js):
    """C++ meldet die Projektliste (JSON-String: {folder, projects[]})."""
    global _projects
    try:
        val = json.loads(js)
    except (ValueError, TypeError):
        return
    with _lock:
        _projects = val


def set_sps(connected):
    """C++ meldet den SPS-Verbindungsstatus (bool/int)."""
    with _lock:
        _sps["connected"] = bool(int(connected)) if isinstance(connected, str) else bool(connected)


# ---- Worker -> Flask ----
def get_status():
    with _lock:
        return dict(_status)


def get_projects():
    with _lock:
        return dict(_projects)


def get_sps():
    with _lock:
        return dict(_sps)


# ---- Render-Parameter (Web-Werte aus der Settings-DB) ----
_RENDER_KEYS = {
    "minArcRadius": "previewMinArcRadius",
    "colorG0": "previewColorG0", "colorG1": "previewColorG1",
    "colorG2": "previewColorG2", "colorG3": "previewColorG3",
    "thicknessG0": "previewThicknessG0", "thicknessG1": "previewThicknessG1",
    "thicknessG2": "previewThicknessG2", "thicknessG3": "previewThicknessG3",
    # Fortschritts-Farben (Testdurchlauf): aktuelle Position + erledigt.
    "liveColor": "previewLiveColor", "doneColor": "previewDoneColor",
    # Strichstärke der Fortschritts-Zustände.
    "liveThickness": "previewLiveThickness", "doneThickness": "previewDoneThickness",
}


def render_params_dict():
    """Aktuelle Render-Werte aus der Settings-DB (für Preview-Seite / C++-Worker)."""
    import settings_db
    s = settings_db.get_all()
    return {k: s.get(dbkey, "") for k, dbkey in _RENDER_KEYS.items()}


def get_render_params():
    """Vom C++-Worker aufgerufen: Render-Werte als JSON-String."""
    return json.dumps(render_params_dict())
