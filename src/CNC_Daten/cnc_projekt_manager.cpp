#include "CNC_Daten/cnc_projekt_manager.h"
#include "web/image_sink.h"
#include "CNC_Daten/arc_flatten.h"
#include "ErrorLoggerSingleton.h"
#include <iostream>
#include <cmath>
#include <thread>
#include <chrono>

// ========== CONSTRUCTOR ==========

ProjectManager::ProjectManager(SPSController &controller,
                               AppState &state,
                               const std::string &folderPath,
                               int imageSizeX,
                               int imageSizeY,
                               int lenDataArray)
    : controller_(controller), state_(state), folderPath_(folderPath), imageSizeX_(imageSizeX), imageSizeY_(imageSizeY), lenDataArray_(lenDataArray), fileReader_(std::make_unique<ProjectFileReader>(folderPath)), previewRenderer_(std::make_unique<PreviewRenderer>())
{
}

// ========== CONFIGURATION SETTERS ==========

void ProjectManager::setImageSize(int width, int height)
{
    imageSizeX_ = width;
    imageSizeY_ = height;
}

void ProjectManager::setDataArrayLength(int length)
{
    lenDataArray_ = length;
}

// ========== PUBLIC METHODS ==========

bool ProjectManager::updateProjectList(int maxProjects)
{
    try
    {
        // 1. Projekte auflisten
        auto projectNames = fileReader_->listProjects(maxProjects);

        if (projectNames.empty())
        {
            logAndPushError("Keine Projekte im Ordner gefunden: " + folderPath_);
            return false;
        }

        if (projectNames.size() > static_cast<size_t>(maxProjects))
        {
            logAndPushError("Zu viele Projekte im Ordner: " + folderPath_);
            return false;
        }

        // 2. Zur SPS schreiben
        return writeProjectListToSPS(projectNames);
    }
    catch (const std::exception &e)
    {
        logAndPushError("Fehler bei updateProjectList: " + std::string(e.what()));
        return false;
    }
}

std::tuple<int, bool> ProjectManager::showProjectInfo()
{
    try
    {
        // 1. Projektname lesen
        auto [projectName, success] = readProjectNameFromSPS();
        if (!success)
        {
            return {0, false};
        }

        // 2. Projektinfo laden
        auto infoOpt = fileReader_->getProjectInfo(projectName);
        if (!infoOpt)
        {
            controller_.writeBool("ProjektVorhanden", false);
            logAndPushError("Projekt nicht gefunden: " + projectName);
            return {0, false};
        }

        ProjectInfo info = *infoOpt;

        // 3. Projektinfo zur SPS schreiben
        if (!writeProjectInfoToSPS(info))
        {
            return {0, false};
        }

        // 4. Preview rendern
        RenderParams params = createRenderParamsFromSPS();
        auto [previewImg, statusCode] = previewRenderer_->renderPreview(
            info.filePath.string(),
            params);

        if (statusCode != 0)
        {
            controller_.writeInt16("StatusCode", static_cast<int16_t>(statusCode));
            logAndPushError("Fehler beim Rendern der Vorschau: " + std::to_string(statusCode));
        }
        else
        {
            // Preview zum WebServer laden
            publishImage(state_, previewImg);
        }

        return {static_cast<int>(info.recordCount), true};
    }
    catch (const std::exception &e)
    {
        logAndPushError("Fehler bei showProjectInfo: " + std::string(e.what()));
        return {0, false};
    }
}

bool ProjectManager::transferProjectData()
{
    try
    {
        // 1. Projektinfo holen
        auto [recordCount, success] = showProjectInfo();
        if (!success || recordCount == 0)
        {
            return false;
        }

        // 2. Projektname lesen
        auto [projectName, nameSuccess] = readProjectNameFromSPS();
        if (!nameSuccess)
        {
            return false;
        }

        // 3. Projektinfo laden
        auto infoOpt = fileReader_->getProjectInfo(projectName);
        if (!infoOpt)
        {
            return false;
        }

        ProjectInfo info = *infoOpt;

        // 4. Übertragung vorbereiten
        int anzBlocke = static_cast<int>(
            std::ceil(static_cast<double>(recordCount) / lenDataArray_));

        controller_.writeInt16("AnzDatenblöcke", static_cast<int16_t>(anzBlocke));
        controller_.writeInt16("IndexData", 0);
        setTransferFlags(false, true, false);

        // Schwellwert für die Bogenglättung einmalig lesen (0 = deaktiviert).
        auto [minArcRadius, okArc, codeArc] = controller_.readFloat("minArcRadius");
        const double arcThreshold = okArc ? static_cast<double>(minArcRadius) : 0.0;

        int currentBlock = 0;

        // 5. Blockweise übertragen
        while (true)
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(10));

            // Abbruch prüfen
            auto [abbruch, succAbbruch, codeAbbruch] =
                controller_.readBool("ubertragenAbbrechen");

            if (succAbbruch && abbruch)
            {
                controller_.writeBool("ubertragenAbbrechen", false);
                break;
            }

            // Auf ReadData-Signal warten
            auto [readData, succRead, codeRead] = controller_.readBool("ReadData");

            if (succRead && readData)
            {
                controller_.writeBool("ReadData", false);
                controller_.writeInt16("IndexData", static_cast<int16_t>(currentBlock + 1));

                // Block lesen
                auto blockData = fileReader_->readDataBlock(
                    info.filePath,
                    currentBlock,
                    lenDataArray_ // ← Verwende Klassenvariable
                );

                // Kleine Bögen (G02/G03) zu Linien (G01) umwandeln - identische
                // Logik wie in der Visualisierung (arc_flatten.h). Bei
                // arcThreshold == 0 bleiben die Zeilen unverändert.
                for (auto& lineStr : blockData)
                {
                    lineStr = arc_flatten::flattenArcLine(lineStr, arcThreshold);
                }

                // Block schreiben
                if (!writeDataBlockToSPS(blockData, currentBlock))
                {
                    break;
                }

                controller_.writeBool("WriteData", true);
                ++currentBlock;
            }

            // Fertig?
            if (currentBlock >= anzBlocke)
            {
                pushInfo("Alle Blöcke übertragen.");
                break;
            }
        }

        // 6. Abschluss
        setTransferFlags(false, false, true);
        return true;
    }
    catch (const std::exception &e)
    {
        logAndPushError("Fehler bei transferProjectData: " + std::string(e.what()));
        setTransferFlags(false, false, true);
        return false;
    }
}

// ========== PRIVATE HELPERS - SPS Communication ==========

std::tuple<std::string, bool> ProjectManager::readProjectNameFromSPS()
{
    auto [name, success, errorCode] = controller_.readString("ProjektName");

    if (!success)
    {
        logAndPushError("Fehler beim Lesen des Projektnamens: " + std::to_string(errorCode));
        return {"", false};
    }

    if (name.empty())
    {
        logAndPushError("Kein Projektname angegeben");
        return {"", false};
    }

    return {name, true};
}

RenderParams ProjectManager::createRenderParamsFromSPS()
{
    return RenderParams::loadFromSPS(controller_, imageSizeX_, imageSizeY_);
}

bool ProjectManager::writeProjectListToSPS(const std::vector<std::string> &projectNames)
{
    std::vector<std::vector<std::string>> array2D = {projectNames};

    auto [success, code] = controller_.writeStringArray("ProjektArray", array2D, 0, 0);

    if (!success)
    {
        logAndPushError("Fehler beim Schreiben der Projektliste", code);
        return false;
    }

    controller_.writeBool("ProjektlistUpdated", true);
    pushInfo("Projektliste aktualisiert");
    return true;
}

bool ProjectManager::writeProjectInfoToSPS(const ProjectInfo &info)
{
    if (!info.exists)
    {
        controller_.writeBool("ProjektVorhanden", false);
        logAndPushError("Projekt existiert nicht: " + info.name);
        return false;
    }

    controller_.writeInt16("LenProjekt", static_cast<int16_t>(info.recordCount));
    controller_.writeBool("ProjektVorhanden", true);

    int16_t anzBlocke = static_cast<int16_t>(
        std::ceil(static_cast<double>(info.recordCount) / lenDataArray_) // ← Verwende Klassenvariable
    );
    controller_.writeInt16("AnzDatenblöcke", anzBlocke);

    pushInfo("Projektinformationen erfolgreich angezeigt: " + info.name);
    return true;
}

bool ProjectManager::writeDataBlockToSPS(const std::vector<std::string> &data, int blockIndex)
{
    std::vector<std::vector<std::string>> array2D = {data};

    auto [success, code] = controller_.writeStringArray("DataArray", array2D, 0, 0);

    if (!success)
    {
        logAndPushError("Fehler beim Schreiben des Datenblocks: " + std::to_string(code), code);
        return false;
    }

    return true;
}

void ProjectManager::setTransferFlags(bool writeData, bool readData, bool finished)
{
    controller_.writeBool("WriteData", writeData);
    controller_.writeBool("ReadData", readData);
    controller_.writeBool("UebertgarungBeendet", finished);
}

void ProjectManager::logAndPushError(const std::string &message, int code)
{
    ErrorLoggerSingleton::instance().logError(message);
    controller_.pushErrorMessage(message, code);
}

void ProjectManager::pushInfo(const std::string &message)
{
    controller_.pushInfoMessage(message);
}

std::tuple<int, bool> ProjectManager::LiveImage()
{

    // 1. Projektname lesen

    // Variablen aus SPS read readProjectNameFromSPS holt das ausgewähle Programm
    // Zählerstand gibt den aktuellen Stand innerhalb des Programms an
    // Abbruchvariable um das Livebild abbrechen zu können
    // der Webserver stellt dann immer das Li8ve Bild dar
    // bricht ab wenn die Abbruchvariable gesetzt ist oder das Programm zu Ende ist  oder wenn das Programm zu Ende ist

    try
    {
        // 1. Projektname lesen
        auto [projectName, success] = readProjectNameFromSPS();
        if (!success)
        {
            return {0, false};
        }

        // 2. Projektinfo laden
        auto infoOpt = fileReader_->getProjectInfo(projectName);
        if (!infoOpt)
        {
            controller_.writeBool("ProjektVorhanden", false);
            logAndPushError("Projekt nicht gefunden: " + projectName);
            return {0, false};
        }

        ProjectInfo info = *infoOpt;
        
        // 3. Projektinfo zur SPS schreiben
        if (!writeProjectInfoToSPS(info))
        {
            return {0, false};
        }

        // 4. Preview rendern
        RenderParams params = createRenderParamsFromSPS();
        auto [previewImg, statusCode] = previewRenderer_->renderPreview(
            info.filePath.string(),
            params);

        if (statusCode != 0)
        {
            controller_.writeInt16("StatusCode", static_cast<int16_t>(statusCode));
            logAndPushError("Fehler beim Rendern der Vorschau: " + std::to_string(statusCode));
        }
        else
        {
            // Preview zum WebServer laden
            publishImage(state_, previewImg);
        }
        int LiveStand = -1;
        while (true)
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(10));

            // Abbruch prüfen
            auto [abbruch, succAbbruch, codeAbbruch] =
                controller_.readBool("LiveAbbruch");

            if (succAbbruch && abbruch)
            {
                controller_.writeBool("LiveAbbruch", false);
                break;
            }

            if (LiveStand == static_cast<int>(info.recordCount))
            {
                // Ende des Programms erreicht
                break;
            }
            auto [stand_sps, succStand, codeStand] = controller_.readInt16("LiveStand");

            if (!succStand)
            {
                logAndPushError("Fehler beim Lesen des LiveStand: " + std::to_string(codeStand));
            }
            if (stand_sps != LiveStand)
            {
                LiveStand = stand_sps;
                // Rendern des Livebilds basierend auf dem aktuellen Stand
                RenderParams liveParams = createRenderParamsFromSPS();
                liveParams.currentRecord = LiveStand; // Setze den aktuellen Datensatz für das Livebild

                auto [liveImg, liveStatusCode] = previewRenderer_->renderPreview(
                    info.filePath.string(),
                    liveParams);

                if (liveStatusCode != 0)
                {
                    controller_.writeInt16("StatusCode", static_cast<int16_t>(liveStatusCode));
                    logAndPushError("Fehler beim Rendern des Livebilds: " + std::to_string(liveStatusCode));
                }
                else
                {
                    // Livebild zum WebServer laden
                    publishImage(state_, liveImg);
                }
            }
            else
            {
                // Keine Änderung im Stand, überspringen
                continue;
            }

            // das Livebild immer dann aktualisieren wenn die Zählervariable sich ändert
        }
    }
    catch (const std::exception &e)
    {
        logAndPushError("Fehler bei LiveImage: " + std::string(e.what()));
        return {0, false};
    }

    return {0, true};
}
