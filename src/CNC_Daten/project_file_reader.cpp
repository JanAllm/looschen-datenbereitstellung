#include "CNC_Daten/project_file_reader.h"
#include <fstream>
#include <algorithm>

// ========== ALTE FUNKTIONEN DIREKT ÜBERNOMMEN ==========

// Deine existierende countTabRecords Funktion
size_t ProjectFileReader::countRecords(const std::filesystem::path& filePath)
{
    std::ifstream file(filePath);
    if (!file.is_open())
    {
        throw std::runtime_error("Kann Datei nicht öffnen: " + filePath.string());
    }

    std::size_t count = 0;
    std::string line;
    bool firstLine = true;

    while (std::getline(file, line))
    {
        // Header überspringen
        if (firstLine)
        {
            firstLine = false;
            continue;
        }
        
        // Leere oder nur Whitespace-Zeilen überspringen
        bool onlySpace = true;
        for (char ch : line)
        {
            if (!std::isspace(static_cast<unsigned char>(ch)))
            {
                onlySpace = false;
                break;
            }
        }
        if (onlySpace)
            continue;

        ++count;
    }
    return count;
}

// Deine existierende Logik aus writeProjektList
std::vector<std::string> ProjectFileReader::listProjects(int maxCount)
{
    namespace fs = std::filesystem;
    
    std::vector<std::string> projektList;
    
    if (!fs::exists(basePath_))
    {
        throw std::runtime_error("Ordnerpfad existiert nicht: " + basePath_);
    }
    
    for (const auto& entry : fs::directory_iterator(basePath_))
    {
        if (entry.is_regular_file() && entry.path().extension() == ".tap")
        {
            std::string fileName = entry.path().filename().string();
            projektList.push_back(fileName);
        }
    }
    
    if (projektList.size() > static_cast<size_t>(maxCount))
    {
        projektList.resize(maxCount);
    }
    
    return projektList;
}

// Deine existierende Logik aus showProjektInfo
std::optional<ProjectInfo> ProjectFileReader::getProjectInfo(const std::string& projectName)
{
    namespace fs = std::filesystem;
    
    ProjectInfo info;
    info.name = projectName;
    
    // 1) Pfad zum Projektordner prüfen
    std::string combined = basePath_ + "/" + projectName;
    fs::path tabFile{combined};

    if (!fs::exists(tabFile))
    {
        info.exists = false;
        return std::nullopt;
    }
    
    info.filePath = tabFile;
    info.exists = true;
    
    // 2) Records zählen
    try
    {
        info.recordCount = countRecords(tabFile);
    }
    catch (const std::exception&)
    {
        info.exists = false;
        return std::nullopt;
    }
    
    return info;
}

// Deine existierende Logik aus writeData
std::vector<std::string> ProjectFileReader::readDataBlock(
    const std::filesystem::path& filePath,
    int blockIndex,
    int blockSize)
{
    std::ifstream file(filePath);
    if (!file.is_open())
    {
        throw std::runtime_error("Kann .tab-Datei nicht öffnen: " + filePath.string());
    }
    
    std::vector<std::string> block;
    std::string line;
    bool firstLine = true;
    int rowCount = 0;
    
    while (std::getline(file, line))
    {
        // Zeilenende bereinigen: Bei Windows-Dateien (CRLF) trennt getline nur am
        // LF und laesst das CR stehen. Das wuerde als CR mit in die SPS
        // uebertragen (dort als "$R" sichtbar). Auch sonstige nachlaufende
        // Leerzeichen/Tabs entfernen.
        while (!line.empty() &&
               (line.back() == '\r' || line.back() == '\n' ||
                line.back() == ' ' || line.back() == '\t'))
        {
            line.pop_back();
        }

        // Skip the header line - must match countRecords(), otherwise every
        // block is shifted by one line and the PLC receives the header as its
        // first record while the last G-code line is never transferred.
        if (firstLine)
        {
            firstLine = false;
            continue;
        }
        
        // Reine Leerzeilen überspringen
        bool onlySpace = true;
        for (char ch : line)
        {
            if (!std::isspace(static_cast<unsigned char>(ch)))
            {
                onlySpace = false;
                break;
            }
        }
        if (onlySpace)
            continue;

        // Nur Zeilen im gewünschten Blockbereich aufnehmen
        if (rowCount >= blockIndex * blockSize && 
            rowCount < (blockIndex + 1) * blockSize)
        {
            block.push_back(line);
        }
        
        ++rowCount;
        
        if (rowCount >= (blockIndex + 1) * blockSize)
            break;
    }
    
    return block;
}

std::optional<std::filesystem::path> ProjectFileReader::findTabFile(
    const std::string& projectName)
{
    // Deine existierende Logik - hier sehr simpel
    std::string combined = basePath_ + "/" + projectName;
    std::filesystem::path tabFile{combined};
    
    if (std::filesystem::exists(tabFile))
    {
        return tabFile;
    }
    
    return std::nullopt;
}