import easyocr
import numpy as np
import cv2
import os
import time
from pyzbar.pyzbar import decode, ZBarSymbol

# 📌 Automatisch das aktuelle Skriptverzeichnis bestimmen
SCRIPT_DIR = os.path.dirname(os.path.abspath(__file__))
MODEL_DIR = os.path.join(SCRIPT_DIR, "models")

# 🔧 Sicherstellen, dass die Modelldateien vorhanden sind
REQUIRED_MODELS = ["craft_mlt_25k.pth", "english_g2.pth"]

for model in REQUIRED_MODELS:
    model_path = os.path.join(MODEL_DIR, model)
    if not os.path.exists(model_path):
        raise FileNotFoundError(f"❌ Fehlendes Modell: {model_path}\nBitte stelle sicher, dass alle EasyOCR-Modelle im Ordner 'models/' vorhanden sind.")

# ✅ EasyOCR verwendet NUR lokale Modelle
os.environ["EASYOCR_MODEL_STORAGE_DIR"] = MODEL_DIR

# 🔥 EasyOCR-Modelle laden (KEIN DOWNLOAD ERLAUBT)
reader = easyocr.Reader(['en'], model_storage_directory=MODEL_DIR, download_enabled=False)

def readText(image_bytes):
    """
    Liest Text aus einem Bild mit EasyOCR.
    - Erwartet: Bild als Byte-Array
    - Gibt zurück: Erkannten Text als String
    """
    start = time.time()
    image_np = np.frombuffer(image_bytes, dtype=np.uint8)
    img = cv2.imdecode(image_np, cv2.IMREAD_COLOR)
    if img is None or img.size == 0:
        return "Fehler: Bild konnte nicht dekodiert werden"
    img = cv2.cvtColor(img, cv2.COLOR_BGR2GRAY)
    results = reader.readtext(img, low_text=0.5)
    extracted_texts = [text for (_, text, _) in results]

    end = time.time()
    print(f"OCR took: {end - start:.3f} seconds")
    return " ".join(extracted_texts)


def sharpen_image(image):
    """Verbessert die Bildschärfe mit einem Laplacian-Filter."""
    kernel = np.array([[0, -1, 0], [-1, 5, -1], [0, -1, 0]])
    return cv2.filter2D(image, -1, kernel)





def canny_edge_detection(image):
    """Führt eine Kantenerkennung auf einem Bild mit Canny durch."""
    return cv2.Canny(image, 100, 200)

def barcodeRecognition(image_bytes):
    """
    Dekodiert Barcodes in einem Bild mit pyzbar.
    - Erwartet: Bild als Byte-Array
    - Gibt zurück: Liste der gefundenen Barcodes
    """
    start = time.time()
    image_np = np.frombuffer(image_bytes, dtype=np.uint8)
    img = cv2.imdecode(image_np, cv2.IMREAD_GRAYSCALE)
    cv2.imshow("test", img)
    cv2.waitKey(0)
    cv2.destroyAllWindows()
    if img is None:
        return ""
    decoded_objects = decode(img, symbols=[ZBarSymbol.EAN13])
    if not decoded_objects:
        return ""
    barcodes = [barcode.data.decode('utf-8') for barcode in decoded_objects]
    end = time.time()
    for code in barcodes:
        print(f"Barcode: {code}")
    print(f"Barcode recognition took: {end - start:.3f} seconds")
    return "; ".join(barcodes)

if __name__ == "__main__":
    print("Dieses Skript sollte nicht direkt ausgeführt werden2.")
    img = cv2.imread(r"C:\Users\jan.allmrodt\Downloads\Screenshot 2025-03-14 071807.png")
    img = cv2.cvtColor(img, cv2.COLOR_BGR2GRAY)
    img = cv2.resize(img, (200,100))
    cv2.imshow("vorher", cv2.resize (img, (800,400)) )
 
    #img = sharpen_image(img)
    cv2.imshow("nachher", img) 
    barcodes = barcodeRecognition(cv2.imencode('.png', img)[1].tobytes())
    print("Barcode: ", barcodes)
    print("len: ", len(barcodes))
    print("Super Resolution done")

    cv2.imshow("test", cv2.resize(img, (1920,1080)) )
    cv2.waitKey(0)
    cv2.destroyAllWindows()
