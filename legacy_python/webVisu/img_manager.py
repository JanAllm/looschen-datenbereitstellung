# canvas_manager.py
import threading
from io import BytesIO
from PIL import Image

class ImgManager:
    def __init__(self, size, bg_color='black'):
        self.size = size
        self._lock = threading.Lock()
        self._canvas = Image.new('RGB', size, bg_color)

    def get_image_bytes(self, fmt='PNG'):
        with self._lock:
            buf = BytesIO()
            self._canvas.save(buf, format=fmt)
            return buf.getvalue()

    def set_image_bytes(self, image_bytes):
        from io import BytesIO as _B
        status = 0
        try:
            # Bild in ORIGINALAUFLÖSUNG übernehmen (kein Herunterskalieren) -
            # sonst wäre die Vorschau beim Zoomen unscharf. Das Canvas wird durch
            # das neue Bild ersetzt; get_image_bytes kodiert es verlustfrei als PNG.
            img = Image.open(_B(image_bytes)).convert('RGB')
            with self._lock:
                self._canvas = img
        except Exception as e:
            status = 1
            raise ValueError(f"Ungültige Bilddaten: {e}")
        finally:
            return status
