# pragma once
#include <string>
#include <fstream>
#include "ErrorLoggerSingleton.h"
#include "interfaces/setupVarInfos.h"




void loadSettings(const std::string &filename , setupVarInfos &setupVars, 
                    std::string &ipOPC, 
                    std::string &ProgrammId,
                    std::string &folderPath, int &sizeObjx, int &sizeObjy,
                    int &webserVerImgSizeX, int &webserVerImgSizeY,
                    int &MaximaleAnzahlProjekte, int &lenDataArray)
{
    std::ifstream file(filename);
    std::string line;
    
    if (!file.is_open())
    {
        ErrorLoggerSingleton::instance().logError("Config nicht gefunden: " + filename);
        return;
    }
    std::cerr << "\n=== SPSController::setupVariables() AUFGERUFEN" << std::endl;
    while (std::getline(file, line))
    {
        size_t pos = line.find("=");
        if (pos != std::string::npos)
        {
            std::string key = line.substr(0, pos);
            std::string value = line.substr(pos + 1);
            
            try
            {
                if (key == "ipSPS")
                    ipOPC = value;
                else if (key == "ProgrammId")
                    ProgrammId = value;
                else if (key == "ns")
                    setupVars.ns = std::stoi(value);
                else if (key == "folderPath")
                    folderPath = value;
                else if (key == "sizeObjx")
                    sizeObjx = std::stoi(value);
                else if (key == "sizeObjy")
                    sizeObjy = std::stoi(value);
                else if (key == "webserVerImgSizeX")
                    webserVerImgSizeX = std::stoi(value);
                else if (key == "webserVerImgSizeY")
                    webserVerImgSizeY = std::stoi(value);
                else if (key == "iError")
                    setupVars.iError = std::stoi(value);
                else if (key == "iInfo")
                    setupVars.iInfo = std::stoi(value);
                else if (key == "iHeartbeat")
                    setupVars.iHeartbeat = std::stoi(value);
                else if (key == "iStatusCode")
                    setupVars.iStatusCode = std::stoi(value);
                else if (key == "iDataArry")
                    setupVars.iDataArry = std::stoi(value);
                else if (key == "iProjektArray")
                    setupVars.iProjektArray = std::stoi(value);
                else if (key == "iUpdateProjektList")
                    setupVars.iUpdateProjektList = std::stoi(value);
                else if (key == "iProjektlistUpdated")
                    setupVars.iProjektlistUpdated = std::stoi(value);
                else if (key == "iProjektName")
                    setupVars.iProjektName = std::stoi(value);
                else if (key == "iReadProjekt")
                    setupVars.iReadProjekt = std::stoi(value);
                else if (key == "iLenProjekt")
                    setupVars.iLenProjekt = std::stoi(value);
                else if (key == "iAnzDatenblcke")
                    setupVars.iAnzDatenblocke = std::stoi(value);
                else if (key == "iWriteData")
                    setupVars.iWriteData = std::stoi(value);
                else if (key == "iIndexData")
                    setupVars.iIndexData = std::stoi(value);
                else if (key == "iReadData")
                    setupVars.iReadData = std::stoi(value);
                else if (key == "MaximaleAnzahlProjekte")
                    MaximaleAnzahlProjekte = std::stoi(value);
                else if (key == "LenDataArray")
                    lenDataArray = std::stoi(value);
                else if (key == "iabbruchUbertragen")
                    setupVars.iabbruchUbertragen = std::stoi(value);
                else if (key == "showProjektInfo")
                    setupVars.ishowProjektInfo = std::stoi(value);
                else if (key == "ProjektInfoWrote")
                    setupVars.iProjektInfoWrote = std::stoi(value);
                else if (key == "ProjektVorhanden")
                    setupVars.iProjektVorhanden = std::stoi(value);
                else if (key == "iAnzDatenblöcke")
                    setupVars.iAnzDatenblocke = std::stoi(value);
                else if (key == "iUebertgarungBeendet")
                    setupVars.iUebertgarungBeendet = std::stoi(value);
                else if (key == "iUebertragungslaege")
                    setupVars.iUebertragungslaege = std::stoi(value);
                else if (key == "iFarbeG0")
                    setupVars.icolorG0 = std::stoi(value);
                else if (key == "iStaerkeG0")
                    setupVars.istreghtsG0 = std::stoi(value);
                else if (key == "iFarbeG1")
                    setupVars.icolorG1 = std::stoi(value);
                else if (key == "iStaerkeG1")
                    setupVars.istreghtsG1 = std::stoi(value);
                else if (key == "iFarbeG2")
                    setupVars.icolorG2 = std::stoi(value);
                else if (key == "iStaerkeG2")
                    setupVars.istreghtsG2 = std::stoi(value);
                else if (key == "iFarbeG3")
                    setupVars.icolorG3 = std::stoi(value);
                else if (key == "iStaerkeG3")
                    setupVars.istreghtsG3 = std::stoi(value);
                else if (key == "objektgroesseX")
                    setupVars.objektgroesseX = std::stoi(value);
                else if (key == "objektgroesseY")
                    setupVars.objektgroesseY = std::stoi(value);
                else if (key == "iFarbeLive")
                    setupVars.iFarbeLive = std::stoi(value);
                else if (key == "iFarbeErledigt")
                    setupVars.iFarbeErledigt = std::stoi(value);
                else if (key == "iStaerkeLive")
                    setupVars.iStaerkeLive = std::stoi(value);
                else if (key == "iStaerkeErledigt")
                    setupVars.iStaerkeErledigt = std::stoi(value);
                else if (key == "iLiveStand")
                    setupVars.iLiveStand = std::stoi(value);
                else if (key == "iLiveBreak")
                    setupVars.iLiveAbbruch = std::stoi(value);
                else if (key == "iMinArcRadius")
                    setupVars.iMinArcRadius = std::stoi(value);
            }
            catch (const std::exception &e)
            {
                ErrorLoggerSingleton::instance().logError(
                    "Parse-Fehler '" + key + "': " + e.what());
            }
        }
    }
    
    file.close();
}