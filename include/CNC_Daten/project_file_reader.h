#pragma once

#include <string>
#include <vector>
#include <filesystem>
#include <optional>

/**
 * @struct ProjectInfo
 * @brief Informationen über ein Projekt
 */
struct ProjectInfo
{
    std::string name;
    std::filesystem::path filePath;
    size_t recordCount{0};
    bool exists{false};
};

/**
 * @class ProjectFileReader
 * @brief Liest Projektdateien (.tap/.tab)
 */
class ProjectFileReader
{
public:
    explicit ProjectFileReader(std::string basePath)
        : basePath_(std::move(basePath))
    {}
    
    /**
     * @brief Listet alle .tap-Dateien im Ordner
     * @param maxCount Maximale Anzahl
     * @return Liste von Dateinamen
     */
    std::vector<std::string> listProjects(int maxCount);
    
    /**
     * @brief Lädt Projektinformationen
     * @param projectName Name des Projekts
     * @return ProjectInfo oder nullopt
     */
    std::optional<ProjectInfo> getProjectInfo(const std::string& projectName);
    
    /**
     * @brief Zählt Datensätze in .tab-Datei
     * @param filePath Pfad zur Datei
     * @return Anzahl Records
     */
    size_t countRecords(const std::filesystem::path& filePath);
    
    /**
     * @brief Liest Datenblock aus Datei
     * @param filePath Pfad zur Datei
     * @param blockIndex Block-Index (0-basiert)
     * @param blockSize Anzahl Zeilen pro Block
     * @return Zeilen des Blocks
     */
    std::vector<std::string> readDataBlock(const std::filesystem::path& filePath,
                                           int blockIndex,
                                           int blockSize);

private:
    std::string basePath_;
    
    /**
     * @brief Sucht .tab-Datei für Projekt
     * @param projectName Projektname
     * @return Pfad oder nullopt
     */
    std::optional<std::filesystem::path> findTabFile(const std::string& projectName);
};