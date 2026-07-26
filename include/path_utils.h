#pragma once
#include <string>
#include <filesystem>

#ifdef _WIN32
    #include <windows.h>
#else
    #include <unistd.h>
    #include <linux/limits.h>
#endif

inline std::string getExecutableDirectory()
{
#ifdef _WIN32
    // Windows
    wchar_t buffer[MAX_PATH];
    GetModuleFileNameW(NULL, buffer, MAX_PATH);
    std::wstring path(buffer);
    size_t pos = path.find_last_of(L"\\/");
    std::wstring dir = path.substr(0, pos);
    int size_needed = WideCharToMultiByte(CP_UTF8, 0, dir.c_str(), (int)dir.size(), NULL, 0, NULL, NULL);
    std::string dir_utf8(size_needed, 0);
    WideCharToMultiByte(CP_UTF8, 0, dir.c_str(), (int)dir.size(), &dir_utf8[0], size_needed, NULL, NULL);
    return dir_utf8;
#else
    // Linux
    char buffer[PATH_MAX];
    ssize_t len = readlink("/proc/self/exe", buffer, sizeof(buffer) - 1);
    if (len != -1) {
        buffer[len] = '\0';
        std::filesystem::path exePath(buffer);
        return exePath.parent_path().string();
    }
    // Fallback
    return std::filesystem::current_path().string();
#endif
}