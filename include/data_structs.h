# pragma once

// Struktur für LabelIO
struct LabelIO {
    int16_t LabelIO;
    int16_t DateIO;
    int16_t ProductIdIO;
    int16_t BarcodeIO;
};

// =====================Struktur für Bildverarbeitungs-Parameter=======================
struct ProcessImgParams {
    short MedianBlurr = 5;
    short Threshold = 120;
    short MaxAngle = 10;
    short LabelSizeX = 50;
    short LabelSizeY = 50;
    short opening = 11;
    short MinDivToSize = 500;
    short RasterX = 4;
    short RasterY = 4;
    float ROIImgX = 0.0f;
    float ROIImgY = 0.0f;
    float ROIImgWidth = 0.99f;
    float ROIImgHeight = 0.99f;
    float ROIDateX = 0.12f;
    float ROIDateY = 0.76f;
    float ROIDateWidth = 0.4f;
    float ROIDateHeight = 0.16f;
    float ROIBarcodeX = 0.52f;
    float ROIBarcodeY = 0.7f;
    float ROIBarcodeWidth = 0.47f;
    float ROIBarcodeHeight = 0.25f;
    float ROIBarcodeTextHeight = 0.18f;
    float GenauigkeitDate = 0.95f;
    float GenauigkeitBarcode = 0.9f;
    std::string Datum;
    std::string Produktnummer;
    std::string Barcode;
    bool turnImg180;
    bool turnImg90;
    bool turnImg270;
    bool mirrorImgX;
    bool mirrorImgY;
    int thresholdLabel = 10; // threshhold zum erkennen ob sich das Bild bewegt
    ProcessImgParams() = default; // Expliziter Standardkonstruktor
};

//=====================Strucktur für Kamera Settings=======================
struct KameraSettings
{
    short LineRate; // Zeilengeschwindigkeit der Zeilenkamera (SPS)
    short exposureTime;    // set exposure Time in Camera (in µs)
    short Img_Height;
    short Img_Width;
    char ColorMode[20]; // RGB, BGR, Grey
    bool autoGain; // true = Auto Gain, false = Manual Gain
    float gain; // Gain in dB
    bool autoExposure; // true = Auto Exposure, false = Manual Exposure
    
                     
};

struct KameraDialog
{
    bool close = false; // Beendet die Kamera Software
    bool trigger = false; // Trigger für die Kamera
    bool busy = false; // Kamera ist beschäftigt
    bool done = false; // Kamera hat das Bild aufgenommen
    bool error = false; // Fehlerflag
    short errorCode = 0; // Fehlercode
    char info_text[70]; // Info Text
    bool readVars = false; // Lädt alle Input Werte für die Bildverarbeitung aus der SPS neu
    bool ready = false; // Kamera ist bereit
    bool takingImg = false; // Solange True nimmt die Kamera ein Bild auf (sollte etwa genau so lange sein wie die Zeit für ein Durchgang)
    bool Speicher_Label_Img = false; // öffnet aus c++ ein FileDialog der ein Bild in dem definierten Speicherpfad ablegt
    bool new_Img_Loaded = false; // Neues Bild wurde geladen
    bool LoadImg;
    bool LoadImgDone; 
};

