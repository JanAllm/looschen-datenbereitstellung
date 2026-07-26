#!/usr/bin/env python3
"""
entrypoint.py
=============
Container-Einstiegspunkt. Läuft VOR dem C++-Dienst:

  1. Settings-DB initialisieren/seeden (SQLite).
  2. config.txt aus der DB rendern (der C++-Dienst liest sie beim Start).
  3. Den C++-Dienst per exec starten (übernimmt die PID -> Signale/SIGTERM
     erreichen ihn direkt, sauberer Neustart bei Settings-Änderung).
"""

import os
import sys

APP_DIR = os.environ.get("APP_DIR", "/app")
WEBVISU_DIR = os.path.join(APP_DIR, "webVisu")
EXECUTABLE = os.environ.get("APP_EXECUTABLE", os.path.join(APP_DIR, "MyCppExecutable"))

# settings_db liegt im webVisu-Verzeichnis.
sys.path.insert(0, WEBVISU_DIR)
import settings_db  # noqa: E402


def main():
    settings_db.init_db()
    cfg_path = settings_db.render_config()
    print(f"[entrypoint] Settings-DB: {settings_db.DB_PATH}", flush=True)
    print(f"[entrypoint] config.txt generiert: {cfg_path}", flush=True)

    if not os.path.exists(EXECUTABLE):
        print(f"[entrypoint] FEHLER: Executable nicht gefunden: {EXECUTABLE}",
              file=sys.stderr, flush=True)
        sys.exit(1)

    os.chdir(APP_DIR)
    print(f"[entrypoint] Starte Dienst: {EXECUTABLE}", flush=True)
    # Ersetzt den aktuellen Prozess -> der Dienst bekommt die PID.
    os.execv(EXECUTABLE, [EXECUTABLE])


if __name__ == "__main__":
    main()
