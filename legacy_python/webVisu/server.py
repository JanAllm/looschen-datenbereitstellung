import threading
from flask_app import create_app
import os
import numpy as np
from PIL import Image
import io
import time
import sys

class FlaskServer:
    def __init__(self, host='0.0.0.0', port=5000, debug=False, threaded=True):
        self.host = host
        self.port = port
        self.debug = debug
        self.threaded = threaded
        self._app = create_app()
        self._thread = None
        
    def start(self):
        def run():
            cert_file = os.path.join(os.getcwd(), 'cert.pem')
            key_file  = os.path.join(os.getcwd(), 'key.pem')
            ssl_ctx = (cert_file, key_file) if os.path.exists(cert_file) and os.path.exists(key_file) else None
            
            # URL ausgeben
            protocol = "https" if ssl_ctx else "http"
            display_host = "localhost" if self.host == "0.0.0.0" else self.host
            
            print("=" * 60)
            print(f"🚀 Flask WebServer gestartet!")
            print(f"📍 URL: {protocol}://{display_host}:{self.port}")
            print(f"🌐 Alle Netzwerk-Interfaces: {protocol}://{self.host}:{self.port}")
            print(f"🔒 HTTPS: {'Ja (Zertifikat gefunden)' if ssl_ctx else 'Nein'}")
            print("=" * 60)
            
            self._app.run(
                host=self.host,
                port=self.port,
                debug=self.debug,
                threaded=self.threaded,
                use_reloader=False,
                ssl_context=ssl_ctx
            )
        
        self._thread = threading.Thread(target=run, daemon=True)
        self._thread.start()
        
        # Kurz warten, damit der Server startet
        time.sleep(0.5)
        
        return os.getpid()
    
    def stop(self):
        print("\n🛑 Flask WebServer wird beendet...")
        os._exit(0)
    
    def load_image(self, image_bytes):
        """
        Lädt ein Bild in den ImgManager.
        Erwartet 'image_bytes' als Bytes-Objekt.
        """
        status = self._app.img_manager.set_image_bytes(image_bytes)
        return status
        
if __name__ == '__main__':
    # Nur zum Testen: Server direkt starten
    server = FlaskServer(debug=False)
    server.start()
    
    try:
        print("\n✅ Server läuft. Drücke Ctrl+C zum Beenden.\n")
        
        while True:
            # Erstelle ein zufälliges RGB-Bild (z.B. 128x128 Pixel)
            random_array = np.random.randint(0, 256, (128, 128, 3), dtype=np.uint8)
            img = Image.fromarray(random_array, 'RGB')
            buf = io.BytesIO()
            img.save(buf, format='PNG')
            testImageBytes = buf.getvalue()
            results = server.load_image(testImageBytes)
            print(f"📸 Testbild geladen (Status: {results})")
            time.sleep(10)
            
    except KeyboardInterrupt:
        server.stop()
