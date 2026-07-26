#include "../include/bildverarbeitung/deepLearning/OCR.h"
#include <Python.h>
#include <opencv2/opencv.hpp>

#ifdef _WIN32
    #include <cstdlib>  // _putenv
#else
    #include <cstdlib>  // setenv
#endif

// 🔄 **Globale Initialisierung von Python (nur einmal pro Prozess)**
std::once_flag pythonInitialized;
static std::mutex gilMutex;  // 🔹 Globaler Mutex für GIL-Verwaltung

// 🔍 **Hilfsfunktion: Python-Executable automatisch finden**
std::string getPythonExecutable() {
    std::array<char, 128> buffer;
    std::string result;

#ifdef _WIN32
    FILE* pipe = _popen("python -c \"import sys; print(sys.executable)\"", "r");
#else
    FILE* pipe = popen("python3 -c \"import sys; print(sys.executable)\"", "r");
#endif

    if (!pipe) {
        std::cerr << "Fehler: Konnte 'python' nicht finden.\n";
        ErrorLoggerSingleton::instance().logError("Fehler: Konnte 'python' nicht finden.");
        return "";
    }

    while (fgets(buffer.data(), buffer.size(), pipe) != nullptr) {
        result += buffer.data();
    }

#ifdef _WIN32
    _pclose(pipe);
#else
    pclose(pipe);
#endif

    if (result.empty()) {
#ifdef _WIN32
        pipe = _popen("py -c \"import sys; print(sys.executable)\"", "r");
#else
        pipe = popen("python -c \"import sys; print(sys.executable)\"", "r");
#endif
        if (!pipe) {
            std::cerr << "Fehler: Konnte 'py' nicht finden.\n";
            ErrorLoggerSingleton::instance().logError("Fehler: Konnte 'py' nicht finden.");
            return "";
        }
        while (fgets(buffer.data(), buffer.size(), pipe) != nullptr) {
            result += buffer.data();
        }
#ifdef _WIN32
        _pclose(pipe);
#else
        pclose(pipe);
#endif
    }

    result.erase(std::remove(result.begin(), result.end(), '\n'), result.end());
    return result;
}

// 🛠 **Python-Umgebung setzen**
void OCR::setPythonEnvironment() {
    std::string pythonExe = getPythonExecutable();

    if (pythonExe.empty()) {
        throw std::runtime_error("❌ Keine Python-Installation gefunden. Stelle sicher, dass Python installiert ist.");
    }

    std::string pythonDir = std::filesystem::path(pythonExe).parent_path().string();

#ifdef _WIN32
    std::string torchLibPath = pythonDir + "\\Lib\\site-packages\\torch\\lib";
    
    _putenv("PYTHONHOME=");
    _putenv("PYTHONPATH=");
    
    const char* currentPath = std::getenv("PATH");
    std::string newPath = pythonDir + ";" + torchLibPath + ";" + (currentPath ? currentPath : "");
    _putenv(("PATH=" + newPath).c_str());
    _putenv(("PYTHONEXECUTABLE=" + pythonExe).c_str());
#else
    std::string torchLibPath = pythonDir + "/../lib/python3.10/site-packages/torch/lib";
    
    setenv("PYTHONHOME", "", 1);
    setenv("PYTHONPATH", "", 1);
    
    const char* currentPath = getenv("PATH");
    std::string newPath = pythonDir + ":" + torchLibPath + ":" + (currentPath ? currentPath : "");
    setenv("PATH", newPath.c_str(), 1);
    setenv("PYTHONEXECUTABLE", pythonExe.c_str(), 1);
#endif

    std::cout << "✅ Python-Executable gesetzt!\n";
}

// 🏗 **Konstruktor: Python-Interpreter starten**
OCR::OCR(const std::string& scriptPath) : scriptDir(scriptPath), pModule(nullptr), pFunc(nullptr), pBarcodeFunc(nullptr) {
    std::cout << "🔄 Initialisiere OCR-Instanz...\n";
    
    std::call_once(pythonInitialized, []() {
        Py_Initialize();
        PyEval_SaveThread();  // GIL nach der Initialisierung freigeben
    });

    std::cout << "⏳ Versuche GIL zu sperren...\n";
    std::lock_guard<std::mutex> gilLock(gilMutex);
    PyGILState_STATE gstate = PyGILState_Ensure();
    std::cout << "✅ GIL erfolgreich gesperrt.\n";

    setPythonEnvironment();
    
    std::string pythonCmd = 
        "import sys\n"
        "sys.path.append('" + scriptPath + "')\n";
    
    PyRun_SimpleString(pythonCmd.c_str());

    PyObject* pName = PyUnicode_DecodeFSDefault("ocr");
    pModule = PyImport_Import(pName);
    Py_DECREF(pName);

    if (!pModule) {
        PyErr_Print();
        std::cerr << "❌ Fehler: Python-Modul 'ocr' nicht importierbar.\n";
        PyGILState_Release(gstate);
        return;
    }

    pFunc = PyObject_GetAttrString(pModule, "readText");
    pBarcodeFunc = PyObject_GetAttrString(pModule, "barcodeRecognition");

    if (!pFunc || !PyCallable_Check(pFunc)) {
        PyErr_Print();
        Py_DECREF(pModule);
        PyGILState_Release(gstate);
        return;
    }

    if (!pBarcodeFunc || !PyCallable_Check(pBarcodeFunc)) {
        PyErr_Print();
        Py_DECREF(pModule);
        PyGILState_Release(gstate);
        return;
    }

    PyGILState_Release(gstate);
    std::cout << "✅ OCR-Instanz erfolgreich initialisiert.\n";
}

// 🧹 **Destruktor: Python-Speicher freigeben**
OCR::~OCR() {
    std::cout << "🔒 Versuche GIL zu sperren im Destruktor...\n";
    std::lock_guard<std::mutex> gilLock(gilMutex);
    PyGILState_STATE gstate = PyGILState_Ensure();
    std::cout << "✅ GIL erfolgreich gesperrt.\n";

    Py_XDECREF(pFunc);
    Py_XDECREF(pBarcodeFunc);
    Py_XDECREF(pModule);

    PyGILState_Release(gstate);
    std::cout << "OCR-Instanz beendet.\n";
}

// Convert a cv::Mat image to a byte vector
std::vector<uchar> OCR::convertMatToBytes(const cv::Mat& img) {
    std::vector<uchar> buffer;
    cv::imencode(".png", img, buffer);  // Alternatively: ".png"
    return buffer;
}

/** 
 * @brief Find text regions in images using OpenCV.
 *
 * Diese Funktion sucht nach Textregionen in einer Liste von Bildern unter Verwendung von OpenCV.
 * Sie konvertiert die Bilder in Graustufen, binarisiert sie und führt eine morphologische Schließung durch,
 * um die Textregionen zu finden. Die gefundenen Regionen werden als Bounding Boxes zurückgegeben.
 *
 * @param images Eine Liste von Eingabebildern, in denen nach Text gesucht werden soll.
 * @param KernelSize Die Größe des strukturellen Elements, das für die morphologische Schließung verwendet wird.
 * @param minX Die minimale Breite einer Bounding Box, damit sie als Textregion betrachtet wird.
 * @param minY Die minimale Höhe einer Bounding Box, damit sie als Textregion betrachtet wird.
 * @param offsetX Der horizontale Offset, der zu jeder Bounding Box hinzugefügt wird.
 * @param offsetY Der vertikale Offset, der zu jeder Bounding Box hinzugefügt wird.
 * @return Ein Paar bestehend aus:
 *         - Einem Vektor von normalisierten Bounding Boxes (cv::Rect_<float>), die die gefundenen Textregionen darstellen.
 *         - Einem Vektor von cv::Mat, die die ausgeschnittenen Textregionen enthalten.
 */
std::pair<std::vector<cv::Rect_<float>>, std::vector<cv::Mat>> OCR::findTextOpenCV(std::vector<cv::Mat> images,cv::Size KernelSize, int minX, int minY, int offsetX, int offsetY) {
    std::vector<cv::Mat> line_images;
    std::vector<cv::Rect_<float>> normalized_bounding_boxes;
    
    for (auto& image : images) {
        cv::Mat gray_image;
        
        if (image.channels() == 3) {
            cv::cvtColor(image, gray_image, cv::COLOR_BGR2GRAY);
        } else {
            gray_image = image.clone();
        }
        
        cv::Mat binary_image;
        cv::threshold(gray_image, binary_image, 0, 255, cv::THRESH_BINARY_INV | cv::THRESH_OTSU);
        
        cv::Mat kernel = cv::getStructuringElement(cv::MORPH_RECT, KernelSize);
        cv::Mat morph_image;
        cv::morphologyEx(binary_image, morph_image, cv::MORPH_CLOSE, kernel);
        
        std::vector<std::vector<cv::Point>> contours;
        cv::findContours(morph_image, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);
        
        for (const auto& contour : contours) {
            cv::Rect bbox = cv::boundingRect(contour);
            
            // Überprüfen, ob die Bounding Box groß genug ist
            if (bbox.width >= minX && bbox.height >= minY) {
                // Offset zur Bounding Box hinzufügen
                bbox.x = std::max(0, bbox.x - offsetX);
                bbox.y = std::max(0, bbox.y - offsetY);
                bbox.width = std::min(bbox.width + 2 * offsetX, image.cols - bbox.x);
                bbox.height = std::min(bbox.height + 2 * offsetY, image.rows - bbox.y);

                // Ensure the bounding box is within image bounds
                bbox.x = std::max(0, bbox.x);
                bbox.y = std::max(0, bbox.y);
                bbox.width = std::min(bbox.width, image.cols - bbox.x);
                bbox.height = std::min(bbox.height, image.rows - bbox.y);

                cv::Rect_<float> normalized_bbox(
                    static_cast<float>(bbox.x) / image.cols,
                    static_cast<float>(bbox.y) / image.rows,
                    static_cast<float>(bbox.width) / image.cols,
                    static_cast<float>(bbox.height) / image.rows
                );
                if (bbox.width > 0 && bbox.height > 0) {
                    cv::Mat line_image = image(bbox).clone();
                    line_images.push_back(line_image);
                    normalized_bounding_boxes.push_back(normalized_bbox);
                }
            }
        }
    }
        return std::make_pair(normalized_bounding_boxes, line_images);
}

/**
 * @brief Liest Text aus einem Bild mithilfe einer Python-Funktion.
 *
 * Diese Funktion verwendet eine Python-Funktion namens 'process_image', um Text aus einem OpenCV-Mat-Bild zu extrahieren.
 * Die Python-Funktion muss vorher initialisiert und aufrufbar sein.
 *
 * @param img Das OpenCV-Mat-Bild, aus dem der Text gelesen werden soll.
 * @return Ein std::string, der den erkannten Text enthält. Im Fehlerfall wird "Fehler" zurückgegeben.
 *
 * Fehlerbehandlung:
 * - Gibt "Fehler" zurück, wenn die Python-Funktion nicht verfügbar ist.
 * - Gibt "Fehler" zurück, wenn ein Fehler beim Aufruf der Python-Funktion auftritt.
 */
std::string OCR::readText(const cv::Mat& img) {
    if (img.empty()) {
        std::cerr << "❌ Fehler: Das Eingangsbild ist leer!" << std::endl;
        ErrorLoggerSingleton::instance().logError("Fehler: Leeres Bild in OCR::readText");
        return "Fehler: Leeres Bild";
    }
    
    if (!pFunc || !PyCallable_Check(pFunc)) {
        std::cerr << "❌ Fehler: Python-Funktion 'process_image' nicht verfügbar!" << std::endl;
        ErrorLoggerSingleton::instance().logError("Python-Funktion 'process_image' nicht verfügbar!");
        return "Fehler";
    }

    std::vector<uchar> imageData = convertMatToBytes(img);
    if (imageData.empty()) {
        std::cerr << "❌ Fehler: Bilddaten konnten nicht konvertiert werden!" << std::endl;
        ErrorLoggerSingleton::instance().logError("Fehler: Bilddaten konnten nicht konvertiert werden!");
        return "Fehler: Konvertierung fehlgeschlagen";
    }

    std::lock_guard<std::mutex> gilLock(gilMutex);
    PyGILState_STATE gstate = PyGILState_Ensure();

    PyObject* pBytes = PyBytes_FromStringAndSize(reinterpret_cast<const char*>(imageData.data()), imageData.size());
    PyObject* pArgs = PyTuple_Pack(1, pBytes);

    PyObject* pValue = PyObject_CallObject(pFunc, pArgs);

    Py_DECREF(pArgs);
    Py_DECREF(pBytes);

    std::string textResult = "Fehler";
    if (pValue != nullptr) {
        PyObject* pStr = PyUnicode_AsEncodedString(pValue, "utf-8", "strict");
        const char* result = PyBytes_AsString(pStr);
        textResult = std::string(result);
        Py_DECREF(pStr);
        Py_DECREF(pValue);
    } else {
        PyErr_Print();
        std::cerr << "❌ Fehler beim Aufruf der Python-Funktion 'process_image'!" << std::endl;
        ErrorLoggerSingleton::instance().logError("Fehler beim Aufruf der Python-Funktion 'process_image'!");   
    }

    PyGILState_Release(gstate);
    return textResult;
}


// Entfernt alle nicht-alphanumerischen Zeichen und wandelt den Text in Kleinbuchstaben um.
std::string OCR::cleanText(const std::string& text) {
    std::string result;
    for (char c : text) {
        if (std::isalnum(static_cast<unsigned char>(c))) {
            result.push_back(std::tolower(c));
        }
    }
    return result;
}

// Prüft, ob zwei Zeichen identisch oder äquivalent sind (basierend auf der Mapping-Tabelle).
bool OCR::areEquivalent(char a, char b, const std::unordered_map<char, std::unordered_set<char>>& equivalences) {
    if (a == b) return true;
    auto it = equivalences.find(a);
    if (it != equivalences.end() && it->second.count(b) > 0) return true;
    auto it2 = equivalences.find(b);
    if (it2 != equivalences.end() && it2->second.count(a) > 0) return true;
    return false;
}

// Berechnet die Länge der längsten gemeinsamen Teilsequenz (LCS) unter Berücksichtigung von Zeichenäquivalenzen.
int OCR::computeLCS(const std::string& s1, const std::string& s2, 
               const std::unordered_map<char, std::unordered_set<char>>& equivalences) {
    size_t m = s1.size();
    size_t n = s2.size();
    std::vector<std::vector<int>> dp(m + 1, std::vector<int>(n + 1, 0));

    for (size_t i = 1; i <= m; ++i) {
        for (size_t j = 1; j <= n; ++j) {
            if (areEquivalent(s1[i - 1], s2[j - 1], equivalences))
                dp[i][j] = dp[i - 1][j - 1] + 1;
            else
                dp[i][j] = std::max(dp[i - 1][j], dp[i][j - 1]);
        }
    }
    return dp[m][n];
}

/**
 * @brief Vergleicht zwei Texte anhand des LCS-Ansatzes und gibt true zurück,
 *        wenn der relative Übereinstimmungsgrad (bezogen auf den Originaltext) einen bestimmten Schwellwert erreicht.
 *
 * @param text1 Originaltext.
 * @param text2 OCR-Text.
 * @param equivalences Mapping für Zeichenäquivalenzen.
 * @param threshold Schwellwert für die Übereinstimmung (Standard: 0.8).
 * @return true, wenn der berechnete Übereinstimmungsgrad >= threshold ist, sonst false.
 */
bool OCR::compareText(const std::string& text1, const std::string& text2, 
                 const std::unordered_map<char, std::unordered_set<char>>& equivalences,
                 double threshold) {
    std::string cleaned1 = cleanText(text1);
    std::string cleaned2 = cleanText(text2);

    if (cleaned1.empty()) return false;

    int lcs = computeLCS(cleaned1, cleaned2, equivalences);
    double similarity = static_cast<double>(lcs) / cleaned1.size();

    return similarity >= threshold;
}

/**
 * @brief Berechnet den prozentualen Übereinstimmungsgrad zwischen zwei Texten unter
 *        Berücksichtigung von Zeichenäquivalenzen anhand der LCS-Methode.
 *
 * @param text1 Originaltext.
 * @param text2 OCR-Text.
 * @param equivalences Mapping für Zeichenäquivalenzen.
 * @return Übereinstimmungsgrad als Wert zwischen 0.0 und 1.0.
 */
double OCR::matchPercentageWithEquivalences(const std::string& text1, 
                                         const std::string& text2, 
                                         const std::unordered_map<char, std::unordered_set<char>>& equivalences) {
    std::string cleaned1 = cleanText(text1);
    std::string cleaned2 = cleanText(text2);

    if (cleaned1.empty()) return 0.0;

    int lcs = computeLCS(cleaned1, cleaned2, equivalences);
    return static_cast<double>(lcs) / cleaned1.size();
}


// Barcode-Erkennung
/**
 * @brief Detects a barcode in the given image using a Python function.
 * 
 * This function takes an OpenCV matrix (cv::Mat) as input, converts it to a byte array,
 * and calls a Python function 'barcodeRecognition' to detect the barcode in the image.
 * 
 * @param img The input image in which the barcode needs to be detected.
 * @return A string containing the detected barcode or "Fehler" in case of an error.
 * 
 * @note The Python function 'barcodeRecognition' must be available and callable.
 *       The Global Interpreter Lock (GIL) is acquired and released within this function.
 * 
 * @warning If the Python function is not available or an error occurs during the call,
 *          an error message is printed to the standard error stream and "Fehler" is returned.
 */
std::string OCR::detectBarcode(const cv::Mat& img) {
    // Überprüfe, ob das Eingangsbild leer ist
    if (img.empty()) {
        std::cerr << "❌ Fehler: Eingangsbild ist leer!" << std::endl;
        return "Fehler: Leeres Bild";
    }

    // Überprüfe, ob die Python-Funktion verfügbar ist
    if (!pBarcodeFunc || !PyCallable_Check(pBarcodeFunc)) {
        std::cerr << "❌ Fehler: Python-Funktion 'barcodeRecognition' nicht verfügbar!" << std::endl;
        return "Fehler";
    }

    // Konvertiere das Bild in einen Byte-Vektor (verwende deine konvertierende Funktion)
    std::vector<uchar> imageData = convertMatToBytes(img);
    if (imageData.empty()) {
        std::cerr << "❌ Fehler: Bilddaten konnten nicht konvertiert werden!" << std::endl;
        return "Fehler: Konvertierung fehlgeschlagen";
    }

    // Sicherstellung, dass der Python GIL erworben wird
    std::lock_guard<std::mutex> gilLock(gilMutex);
    PyGILState_STATE gstate = PyGILState_Ensure();

    // Erstelle ein Python-Bytes-Objekt aus den Bilddaten
    PyObject* pBytes = PyBytes_FromStringAndSize(reinterpret_cast<const char*>(imageData.data()), imageData.size());
    if (!pBytes) {
        std::cerr << "❌ Fehler: Konnte PyBytes nicht erstellen!" << std::endl;
        ErrorLoggerSingleton::instance().logError("Fehler: PyBytes-Erstellung fehlgeschlagen");
        PyGILState_Release(gstate);
        return "Fehler: PyBytes-Erstellung fehlgeschlagen";
    }

    // Verpacke das Bytes-Objekt in ein Tupel als Argument
    PyObject* pArgs = PyTuple_Pack(1, pBytes);
    if (!pArgs) {
        std::cerr << "❌ Fehler: Konnte PyTuple_Pack nicht erstellen!" << std::endl;
        Py_DECREF(pBytes);
        PyGILState_Release(gstate);
        ErrorLoggerSingleton::instance().logError("Fehler: PyTuple-Erstellung fehlgeschlagen");
        return "Fehler: PyTuple-Erstellung fehlgeschlagen";
    }

    // Rufe die Python-Funktion auf
    PyObject* pValue = PyObject_CallObject(pBarcodeFunc, pArgs);

    Py_DECREF(pArgs);
    Py_DECREF(pBytes);

    std::string textResult = "Fehler";
    if (pValue != nullptr) {
        // Konvertiere das Ergebnis in einen UTF-8 codierten String
        PyObject* pStr = PyUnicode_AsEncodedString(pValue, "utf-8", "strict");
        if (pStr) {
            const char* result = PyBytes_AsString(pStr);
            if (result) {
                textResult = std::string(result);
            } else {
                std::cerr << "❌ Fehler: Konnte Ergebnisstring nicht abrufen!" << std::endl;
                ErrorLoggerSingleton::instance().logError("Fehler: Konnte Ergebnisstring nicht abrufen in 'barcodeRecognition'");
            }
            Py_DECREF(pStr);
        } else {
            std::cerr << "❌ Fehler: Konnte Ergebnis nicht konvertieren!" << std::endl;
            ErrorLoggerSingleton::instance().logError("Fehler: Konnte Ergebnis nicht konvertieren in 'barcodeRecognition'");
        }
        Py_DECREF(pValue);
    } else {
        PyErr_Print();
        std::cerr << "❌ Fehler beim Aufruf der Python-Funktion 'barcodeRecognition'!" << std::endl;
        ErrorLoggerSingleton::instance().logError("Fehler beim Aufruf der Python-Funktion 'barcodeRecognition'");
    }

    PyGILState_Release(gstate);
    return textResult;
}


