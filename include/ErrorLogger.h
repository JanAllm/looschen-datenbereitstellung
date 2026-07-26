#ifndef ERRORLOGGER_H
#define ERRORLOGGER_H

#include <string>
#include <fstream>
#include <chrono>
#include <ctime>
#include <sstream>
#include <filesystem>
#include <mutex>

class ErrorLogger {
public:
    // Konstruktor: bekommt nur einen Dateinamen (absoluter Pfad!)
    inline ErrorLogger(const std::string& filename)
        : filename(filename)
    {
        // Existierenden Inhalt lesen (falls Datei existiert)
        std::ifstream infile(filename);
        if (infile.is_open()) {
            std::stringstream buffer;
            buffer << infile.rdbuf();
            fileContent = buffer.str();
            infile.close();
        }
    }

    // Fehler schreiben
    inline void logError(const std::string& error) {
        std::lock_guard<std::mutex> lock(logMutex);
        
        // Zeitstempel herstellen (cross-platform)
        auto now = std::chrono::system_clock::now();
        std::time_t now_time = std::chrono::system_clock::to_time_t(now);
        
        // Cross-platform time formatting
        char timeStr[64] = {0};
#ifdef _WIN32
        ctime_s(timeStr, sizeof(timeStr), &now_time);
#else
        // Linux/Unix: ctime_r ist thread-safe
        ctime_r(&now_time, timeStr);
#endif
        
        std::string newError = std::string(timeStr) + ": " + error + "\n";
        
        // Datei überschreiben und alten Inhalt voranstellen
        std::ofstream outfile(filename, std::ios::out | std::ios::trunc);
        if (outfile.is_open()) {
            outfile << newError << fileContent;
            fileContent = newError + fileContent;
            outfile.close();
        }
    }

private:
    std::string filename;
    std::string fileContent;
    std::mutex logMutex;  // Thread-safety
};

#endif // ERRORLOGGER_H