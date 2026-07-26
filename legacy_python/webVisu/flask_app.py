# flask_app.py
import os
import signal
import socket
import datetime
import threading
from datetime import timezone
import ipaddress

from flask import (
    Flask, send_file, current_app,
    jsonify, render_template, request, abort
)
from io import BytesIO

import settings_db

from img_manager import ImgManager
import preview_bridge


# --- optional: Self-signed Cert erzeugen (wie in deinem app.py) ---
CERT_FILE = 'cert.pem'
KEY_FILE  = 'key.pem'
VALID_DAYS = 365

def get_local_ip() -> str:
    # ohne echten Netz-Traffic
    with socket.socket(socket.AF_INET, socket.SOCK_DGRAM) as s:
        s.connect(("8.8.8.8", 80))
        return s.getsockname()[0]

def cert_is_valid(path: str) -> bool:
    try:
        from cryptography import x509
        data = open(path, 'rb').read()
        cert = x509.load_pem_x509_certificate(data)
        return cert.not_valid_after_utc > datetime.datetime.now(timezone.utc)
    except Exception:
        return False

def generate_selfsigned_cert():
    # cryptography lazy importieren - schlägt der (native) Import fehl, läuft
    # der Server ohne HTTPS weiter (siehe ensure_cert).
    from cryptography import x509
    from cryptography.x509.oid import NameOID
    from cryptography.hazmat.primitives import hashes, serialization
    from cryptography.hazmat.primitives.asymmetric import rsa

    key = rsa.generate_private_key(public_exponent=65537, key_size=4096)
    ip = get_local_ip()
    name = x509.Name([
        x509.NameAttribute(NameOID.COUNTRY_NAME, u"DE"),
        x509.NameAttribute(NameOID.STATE_OR_PROVINCE_NAME, u"Niedersachsen"),
        x509.NameAttribute(NameOID.LOCALITY_NAME, u"Garrel"),
        x509.NameAttribute(NameOID.ORGANIZATION_NAME, u"Hobohtec GmbH"),
        x509.NameAttribute(NameOID.COMMON_NAME, ip),
    ])
    alt_ips = [ x509.IPAddress(ipaddress.IPv4Address(ip)) for ip in (ip, '127.0.0.1') ]
    alt_dns = [ x509.DNSName(dns) for dns in ('localhost',) ]
    cert = (
        x509.CertificateBuilder()
           .subject_name(name)
           .issuer_name(name)
           .public_key(key.public_key())
           .serial_number(x509.random_serial_number())
           .not_valid_before(datetime.datetime.now(timezone.utc) - datetime.timedelta(days=1))
           .not_valid_after (datetime.datetime.now(timezone.utc) + datetime.timedelta(days=VALID_DAYS))
           .add_extension(x509.SubjectAlternativeName(alt_ips + alt_dns), critical=False)
           .sign(key, hashes.SHA256())
    )
    with open(KEY_FILE,  "wb") as f:
        f.write(key.private_bytes(
            encoding=serialization.Encoding.PEM,
            format=serialization.PrivateFormat.TraditionalOpenSSL,
            encryption_algorithm=serialization.NoEncryption()
        ))
    with open(CERT_FILE,"wb") as f:
        f.write(cert.public_bytes(serialization.Encoding.PEM))

def ensure_cert():
    """Erzeugt bei Bedarf ein selbstsigniertes Zertifikat. Schlägt das fehl
    (z.B. cryptography-Native-Modul nicht ladbar), läuft der Server ohne HTTPS."""
    try:
        if not (os.path.exists(KEY_FILE) and os.path.exists(CERT_FILE) and cert_is_valid(CERT_FILE)):
            generate_selfsigned_cert()
    except Exception as e:
        print(f"[flask_app] Zertifikat konnte nicht erzeugt werden ({e}); Server läuft ohne HTTPS.", flush=True)


# --- die Factory ---
def create_app():
    # 1) optional: Cert prüfen/generieren
    ensure_cert()

    # 2) Flask-Objekt
    app = Flask(__name__)
    app.img_manager = ImgManager(size=(500, 1000))

    # 3) Routen definieren
    @app.route('/')
    def index():
        return render_template('index.html')

    @app.route('/img', methods=['GET'], strict_slashes=False)
    def get_img():
        data = current_app.img_manager.get_image_bytes()
        buf = BytesIO(data)
        return send_file(buf, mimetype='image/png')

    @app.route('/health')
    def health():
        return 'OK', 200

    @app.route('/status')
    def status():
        return jsonify(current_app.img_manager.get_status())

    @app.route('/shutdown', methods=['POST'])
    def shutdown():
        """
        Nutzt den Werkzeug-Shutdown-Hook, um den Server sauber zu beenden.
        Aufruf z.B. via Python.h oder HTTP-Client.
        """
        func = request.environ.get('werkzeug.server.shutdown')
        if func is None:
            abort(500, "Shutdown not available")
        func()
        return 'Server shutting down...', 200

    # ------------------------------------------------------------------
    # Einstellungen (Settings-DB + config.txt-Regenerierung)
    # ------------------------------------------------------------------
    @app.route('/settings')
    def settings_page():
        return render_template('settings.html')

    @app.route('/api/settings', methods=['GET'])
    def api_settings_get():
        return jsonify({"groups": settings_db.ui_schema()})

    @app.route('/api/settings', methods=['PUT'])
    def api_settings_put():
        data = request.get_json(silent=True) or {}
        # Nur bekannte Schlüssel akzeptieren.
        known = set(settings_db.DEFAULTS.keys())
        values = {k: v for k, v in data.items() if k in known}
        if not values:
            abort(400, "Keine gültigen Einstellungen übergeben")

        settings_db.set_many(values)
        settings_db.render_config()

        # Live-Werte (Render-Parameter: Farben/Radius/Stärke) liest der C++-Worker
        # pro Render frisch -> KEIN Neustart nötig. Nur wenn ein Bootstrap-Setting
        # (ipSPS/folderPath/Node-Indizes/Größen) geändert wurde, ist ein Neustart
        # erforderlich.
        restart_needed = any(k not in settings_db.LIVE_KEYS for k in values)

        # Auto-Neustart NUR bei Bootstrap-Änderung UND wenn ein Orchestrator den
        # Prozess wieder hochfährt (Docker: RESTART_ON_SAVE=1 + restart-Policy).
        if restart_needed and os.environ.get("RESTART_ON_SAVE") in ("1", "true", "yes"):
            def _restart():
                os.kill(os.getpid(), signal.SIGTERM)
            threading.Timer(1.0, _restart).start()
            return jsonify({"status": "ok", "restart": True})

        # Nativer Betrieb: gespeichert, aber Neustart muss manuell erfolgen
        # (Bootstrap-Settings wie ipSPS/folderPath liest der C++-Dienst beim Start).
        return jsonify({"status": "ok", "restart": False})

    # ------------------------------------------------------------------
    # Vorschau OHNE SPS - gerendert vom C++-Worker (preview_bridge).
    # Flask reicht die Befehle nur weiter und zeigt Status/Liste/Bild.
    # ------------------------------------------------------------------
    @app.route('/preview')
    def preview_page():
        return render_template('preview.html')

    @app.route('/api/projects')
    def api_projects():
        # Frische Liste anfordern und aktuellen Stand zurückgeben.
        preview_bridge.enqueue({"action": "list"})
        return jsonify(preview_bridge.get_projects())

    @app.route('/api/preview/full', methods=['POST'])
    def api_preview_full():
        project = (request.get_json(silent=True) or {}).get("project")
        if not project:
            abort(400, "project fehlt")
        preview_bridge.enqueue({"action": "render", "project": project})
        return jsonify({"ok": True})

    @app.route('/api/preview/testrun', methods=['POST'])
    def api_preview_testrun():
        data = request.get_json(silent=True) or {}
        project = data.get("project")
        if not project:
            abort(400, "project fehlt")
        try:
            interval = int(data.get("interval_ms", 500))
        except (TypeError, ValueError):
            interval = 500
        interval = max(0, min(interval, 60000))  # 0 = schnellstmöglich
        preview_bridge.enqueue({"action": "testrun", "project": project,
                                "interval_ms": interval})
        return jsonify({"ok": True, "interval_ms": interval})

    @app.route('/api/preview/stop', methods=['POST'])
    def api_preview_stop():
        preview_bridge.request_stop()
        return jsonify({"ok": True})

    @app.route('/api/preview/status')
    def api_preview_status():
        return jsonify(preview_bridge.get_status())

    @app.route('/api/sps_status')
    def api_sps_status():
        return jsonify(preview_bridge.get_sps())

    @app.route('/api/render_params')
    def api_render_params():
        # Aktuelle Web-Render-Werte (Radius/Farben/Stärken) für die Preview-Seite.
        return jsonify(preview_bridge.render_params_dict())

    return app

