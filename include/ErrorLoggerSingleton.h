#pragma once
#include "ErrorLogger.h"
#include <filesystem>
#include <string>

#ifdef _WIN32
    #include <windows.h>
#else
    #include <unistd.h>
    #include <linux/limits.h>
#endif

inline std::string GetExeDirectory() {
#ifdef _WIN32
    wchar_t buffer[MAX_PATH];
    GetModuleFileNameW(nullptr, buffer, MAX_PATH);
    return std::filesystem::path(buffer).parent_path().string();
#else
    char buffer[PATH_MAX];
    ssize_t len = readlink("/proc/self/exe", buffer, sizeof(buffer) - 1);
    if (len != -1) {
        buffer[len] = '\0';
        return std::filesystem::path(buffer).parent_path().string();
    }
    // Fallback: aktuelles Verzeichnis
    return std::filesystem::current_path().string();
#endif
}

class ErrorLoggerSingleton {
public:
    static ErrorLogger& instance() {
        static ErrorLogger instance(GetExeDirectory() + 
            std::string(1, std::filesystem::path::preferred_separator) + "Log.txt");
        return instance;
    }
    
    ErrorLoggerSingleton(const ErrorLoggerSingleton&) = delete;
    ErrorLoggerSingleton& operator=(const ErrorLoggerSingleton&) = delete;

private:
    ErrorLoggerSingleton() = default;
};