#ifndef OCR_H
#define OCR_H

#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <cctype>
#include <unordered_map>
#include <unordered_set>
#include <stdexcept>
#include <filesystem>
#include <cstdlib>
#include <array>
#include <memory>
#include <mutex>
#include "ErrorLoggerSingleton.h"
#include <unordered_map>
#include <unordered_set>
#include <opencv2/core/types.hpp>

struct _object;
typedef _object PyObject;

class OCR {
public:
    OCR(const std::string& scriptPath);
    ~OCR();
    std::string readText(const cv::Mat& img);
    std::pair<std::vector<cv::Rect_<float>>, std::vector<cv::Mat>> findTextOpenCV(std::vector<cv::Mat> images,cv::Size KernelSize, int minX, int minY, int offsetX, int offsetY);
    
    static bool compareText(const std::string& text1,
        const std::string& text2,
        const std::unordered_map<char, std::unordered_set<char>>& equivalences,
        double threshold = 0.8);
        double matchPercentageWithEquivalences(const std::string& text1,
        const std::string& text2,
        const std::unordered_map<char, std::unordered_set<char>>& equivalences);
    
    std::string detectBarcode(const cv::Mat& img);
    std::string detectBarcodeOPenCV(const cv::Mat& img);
private:
    
    void setPythonEnvironment();
    std::vector<uchar> convertMatToBytes(const cv::Mat& img);
    std::string autoFindVenv();
    std::string scriptDir;
    PyObject* pModule;
    PyObject* pFunc;
    PyObject* pBarcodeFunc;
     /**
     * @brief Bereinigt den Text, indem nur alphanumerische Zeichen beibehalten
     *        und in Kleinbuchstaben umgewandelt werden.
     *
     * @param text Eingabetext.
     * @return Bereinigter Text.
     */
    static std::string cleanText(const std::string& text);

    /**
     * @brief Prüft, ob zwei Zeichen identisch oder äquivalent sind (basierend auf der Mapping-Tabelle).
     *
     * @param a Erstes Zeichen.
     * @param b Zweites Zeichen.
     * @param equivalences Mapping für Zeichenäquivalenzen.
     * @return true, wenn a und b gleich bzw. äquivalent sind.
     */
    static bool areEquivalent(char a,
                              char b,
                              const std::unordered_map<char, std::unordered_set<char>>& equivalences);

    /**
     * @brief Berechnet die Länge der längsten gemeinsamen Teilsequenz (LCS) zweier Strings,
     *        wobei Zeichenäquivalenzen berücksichtigt werden.
     *
     * @param s1 Erster bereinigter String.
     * @param s2 Zweiter bereinigter String.
     * @param equivalences Mapping für Zeichenäquivalenzen.
     * @return Länge der längsten gemeinsamen Teilsequenz.
     */
    static int computeLCS(const std::string& s1,
                          const std::string& s2,
                          const std::unordered_map<char, std::unordered_set<char>>& equivalences);

    
};

#endif // OCR_H

