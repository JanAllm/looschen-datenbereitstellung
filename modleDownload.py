import os
import easyocr

# 📌 Aktuellen Skript-Pfad bestimmen
SCRIPT_DIR = os.path.dirname(os.path.abspath(__file__))
MODEL_DIR = os.path.join(SCRIPT_DIR, "models")

# 🔧 Modellverzeichnis setzen
os.environ["EASYOCR_MODEL_STORAGE_DIR"] = MODEL_DIR

def main():
    """Lädt die EasyOCR-Modelle mit der internen Funktion herunter."""
    print(f"📂 Speichere Modelle in: {MODEL_DIR}")

    # Initialisiere EasyOCR (dies triggert den Download der Modelle)
    reader = easyocr.Reader(['en'], model_storage_directory=MODEL_DIR)

    print("✅ EasyOCR-Modelle erfolgreich heruntergeladen!")

if __name__ == "__main__":
    main()
