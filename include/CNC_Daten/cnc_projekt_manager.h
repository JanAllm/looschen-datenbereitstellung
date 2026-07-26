#pragma once

#include "interfaces/SPSController.h"
#include "web/app_state.h"
#include "CNC_Daten/project_file_reader.h"
#include "CNC_Daten/preview_renderer.h"
#include <functional>
#include <memory>
#include <string>
#include <tuple>
#include <vector>

/**
 * @class ProjectManager
 * @brief Verwaltet Projekte: Listing, Info-Anzeige, Datenübertragung
 */
class ProjectManager
{
public:
    /**
     * @brief Konstruktor
     * @param controller SPS-Controller (Referenz)
     * @param state AppState für die Bild-/Statusanzeige im Web (Referenz)
     * @param folderPath Basis-Ordner für Projekte
     * @param imageSizeX Bildbreite für Preview (Standard: 1000)
     * @param imageSizeY Bildhöhe für Preview (Standard: 2000)
     * @param lenDataArray Größe eines Datenblocks (Standard: 5)
     */
    ProjectManager(SPSController& controller,
                  AppState& state,
                  const std::string& folderPath,
                  int imageSizeX = 1000,
                  int imageSizeY = 2000,
                  int lenDataArray = 5);
    
    ~ProjectManager() = default;

    // ========== CONFIGURATION SETTERS ==========
    
    /**
     * @brief Setzt die Bildgröße für Preview
     */
    void setImageSize(int width, int height);
    
    /**
     * @brief Setzt die Datenblock-Größe
     */
    void setDataArrayLength(int length);

    /**
     * @brief Registers a callback invoked periodically inside blocking wait
     * loops (transferProjectData, LiveImage). The callback keeps the SPS
     * heartbeat running while this thread waits for the PLC handshake and
     * returns false when the loop must abort (shutdown or connection loss).
     */
    void setKeepAlive(std::function<bool()> callback);
    
    /**
     * @brief Getter für Bildbreite
     */
    int getImageSizeX() const { return imageSizeX_; }
    
    /**
     * @brief Getter für Bildhöhe
     */
    int getImageSizeY() const { return imageSizeY_; }
    
    /**
     * @brief Getter für Datenblock-Größe
     */
    int getDataArrayLength() const { return lenDataArray_; }

    // ========== PUBLIC METHODS ==========
    
    /**
     * @brief Schreibt Projektliste zur SPS
     * @param maxProjects Maximale Anzahl Projekte
     * @return true bei Erfolg
     */
    bool updateProjectList(int maxProjects);
    
    /**
     * @brief Zeigt Projektinformationen und Preview
     * @return {RecordCount, Success}
     */
    std::tuple<int, bool> showProjectInfo();
    
    /**
     * @brief Überträgt Projektdaten blockweise zur SPS
     * @return true bei Erfolg
     */
    bool transferProjectData();

    /**
     * @brief Live Bild Übertragung
     */
    std::tuple<int, bool>  LiveImage();

private:
    // ========== MEMBER VARIABLES ==========
    SPSController& controller_;
    AppState& state_;
    std::string folderPath_;
    
    // Konfiguration
    int imageSizeX_;
    int imageSizeY_;
    int lenDataArray_;
    std::function<bool()> keepAlive_;
    
    // Helper-Klassen
    std::unique_ptr<ProjectFileReader> fileReader_;
    std::unique_ptr<PreviewRenderer> previewRenderer_;
    
    // ========== PRIVATE HELPERS - SPS Communication ==========
    
    /**
     * @brief Liest Projektname von SPS
     * @return {Projektname, Success}
     */
    std::tuple<std::string, bool> readProjectNameFromSPS();
    
    /**
     * @brief Erstellt RenderParams aus SPS-Parametern
     */
    RenderParams createRenderParamsFromSPS();
    
    /**
     * @brief Schreibt Projektliste zur SPS
     */
    bool writeProjectListToSPS(const std::vector<std::string>& projectNames);
    
    /**
     * @brief Schreibt Projektinfo zur SPS
     */
    bool writeProjectInfoToSPS(const ProjectInfo& info);
    
    /**
     * @brief Schreibt Datenblock zur SPS
     */
    bool writeDataBlockToSPS(const std::vector<std::string>& data, int blockIndex);
    
    /**
     * @brief Setzt Transfer-Flags
     */
    void setTransferFlags(bool writeData, bool readData, bool finished);
    
    /**
     * @brief Hilfsfunktion: Fehler loggen und zur SPS senden
     */
    void logAndPushError(const std::string& message, int code = -1);
    
    /**
     * @brief Hilfsfunktion: Info zur SPS senden
     */
    void pushInfo(const std::string& message);

    
};