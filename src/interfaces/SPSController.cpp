
#include "interfaces/SPSController.h"
using namespace std;
/**
 * @brief Konstruktor für die SPSController-Klasse.
 *
 * Erstellt einen SPS-Client basierend auf dem angegebenen clientType und connectionString.
 * Unterstützte Client-Typen sind "twincat", "opc" und "test".
 * Bei unbekanntem Client-Typ wird eine Ausnahme ausgelöst.
 * Nach erfolgreicher Erstellung des Clients wird ein SPSDialog mit dem Client initialisiert.
 *
 * @param clientType Typ des SPS-Clients ("twincat", "opc", "test").
 * @param connectionString Verbindungszeichenfolge für den jeweiligen SPS-Client.
 *
 * @throws std::runtime_error Wenn ein unbekannter Client-Typ angegeben wird.
 */
SPSController::SPSController(const std::string &clientType, const std::string &connectionString)
{
    if (clientType == "twincat")
    {
       // client = std::make_shared<TwincatAds>(connectionString);
       std::cerr << "TwincatAds client is currently not implemented." << std::endl;
    }
    else if (clientType == "opc")
    {
        client = std::make_shared<OPCClient>(connectionString);
    }
    else if (clientType == "test")
    {
        client = std::make_shared<TestClient>(connectionString);
    }
    else
    {
        throw std::runtime_error("Unbekannter Client-Typ: " + clientType);
    }
    std::cout << "SPSController: Client erstellt: " << clientType << std::endl;
    std::cout << "SPSController: Verbindung: " << connectionString << std::endl;
    std::cout << client << std::endl;
    dialog = std::make_unique<SPSDialog>(client); // dialog erwartet ja einen fertigen Client
    std::cout << "SPSController: Dialog erstellt" << std::endl;
}

bool SPSController::connect()
{
    return client->connect();
}
bool SPSController::disconnect()
{
    return client->disconnect();
}

bool SPSController::setupVariables(setupVarInfos &setupVars)
{
    std::cerr << "\n=== SPSController::setupVariables() GESTARTET ===" << std::endl;
    std::cerr << "Namespace: " << setupVars.ns << std::endl;
    std::cerr << "Registriere " << 30 << " Variablen..." << std::endl;
    
    bool globalSuccess = true;
    int successCount = 0;
    int failCount = 0;
    
    // Helper lambda für Fehlerausgabe
    configuredNodes_.clear();
    auto registerAndLog = [&](const std::string& name, DataType type, const std::string& nodeId) {
        // Jeden Versuch merken - auch fehlgeschlagene, damit die Web-Diagnose
        // zeigen kann, WELCHER Knoten fehlt.
        configuredNodes_.emplace_back(name, type, nodeId);
        bool success = dialog->registerVariable(name, type, nodeId);
        if (success) {
            successCount++;
            std::cerr << "  ✓ " << name << " -> " << nodeId << std::endl;
        } else {
            failCount++;
            std::cerr << "  ✗ FEHLER: " << name << " -> " << nodeId << " (Typ: " << static_cast<int>(type) << ")" << std::endl;
        }
        return success;
    };
    
    // Verwende setupVars.ns statt hardcoded "4"
    std::string ns = "ns=" + std::to_string(setupVars.ns) + ";i=";
    
    std::cerr << "\n--- Allgemeine Variablen ---" << std::endl;
    globalSuccess &= registerAndLog("Error", DataType::Bool, ns + std::to_string(setupVars.iError));
    globalSuccess &= registerAndLog("Info_Text", DataType::String, ns + std::to_string(setupVars.iInfo));
    globalSuccess &= registerAndLog("Heartbeat", DataType::Int16, ns + std::to_string(setupVars.iHeartbeat));
    globalSuccess &= registerAndLog("StatusCode", DataType::Int16, ns + std::to_string(setupVars.iStatusCode));
    
    std::cerr << "\n--- Arrays ---" << std::endl;
    globalSuccess &= registerAndLog("DataArray", DataType::StringArray, ns + std::to_string(setupVars.iDataArry));
    globalSuccess &= registerAndLog("ProjektArray", DataType::StringArray, ns + std::to_string(setupVars.iProjektArray));
    
    std::cerr << "\n--- Dialog Variablen ---" << std::endl;
    globalSuccess &= registerAndLog("UpdateProjektList", DataType::Bool, ns + std::to_string(setupVars.iUpdateProjektList));
    globalSuccess &= registerAndLog("ProjektlistUpdated", DataType::Bool, ns + std::to_string(setupVars.iProjektlistUpdated));
    globalSuccess &= registerAndLog("ProjektName", DataType::String, ns + std::to_string(setupVars.iProjektName));
    globalSuccess &= registerAndLog("ReadProjekt", DataType::Bool, ns + std::to_string(setupVars.iReadProjekt));
    globalSuccess &= registerAndLog("LenProjekt", DataType::Int16, ns + std::to_string(setupVars.iLenProjekt));
    globalSuccess &= registerAndLog("AnzDatenblöcke", DataType::Int16, ns + std::to_string(setupVars.iAnzDatenblocke));
    globalSuccess &= registerAndLog("WriteData", DataType::Bool, ns + std::to_string(setupVars.iWriteData));
    globalSuccess &= registerAndLog("IndexData", DataType::Int16, ns + std::to_string(setupVars.iIndexData));
    globalSuccess &= registerAndLog("ReadData", DataType::Bool, ns + std::to_string(setupVars.iReadData));
    globalSuccess &= registerAndLog("showProjektInfo", DataType::Bool, ns + std::to_string(setupVars.ishowProjektInfo));
    // ProjektInfoWrote ist OPTIONAL (kein globalSuccess &=): Fehlt der Knoten
    // auf der SPS, laeuft der Dienst trotzdem - der Schreibvorgang schlaegt
    // dann lediglich fehl und wird ignoriert.
    registerAndLog("ProjektInfoWrote", DataType::Bool, ns + std::to_string(setupVars.iProjektInfoWrote));
    globalSuccess &= registerAndLog("ProjektVorhanden", DataType::Bool, ns + std::to_string(setupVars.iProjektVorhanden));
    globalSuccess &= registerAndLog("ubertragenAbbrechen", DataType::Bool, ns + std::to_string(setupVars.iabbruchUbertragen));
    globalSuccess &= registerAndLog("UebertgarungBeendet", DataType::Bool, ns + std::to_string(setupVars.iUebertgarungBeendet));
    globalSuccess &= registerAndLog("Uebertragungslaege", DataType::Int16, ns + std::to_string(setupVars.iUebertragungslaege));
    
    std::cerr << "\n--- Farben und Stärken ---" << std::endl;
    globalSuccess &= registerAndLog("colorG0", DataType::Int16Array, ns + std::to_string(setupVars.icolorG0));
    globalSuccess &= registerAndLog("streghtsG0", DataType::Int16, ns + std::to_string(setupVars.istreghtsG0));
    globalSuccess &= registerAndLog("colorG1", DataType::Int16Array, ns + std::to_string(setupVars.icolorG1));
    globalSuccess &= registerAndLog("streghtsG1", DataType::Int16, ns + std::to_string(setupVars.istreghtsG1));
    globalSuccess &= registerAndLog("colorG2", DataType::Int16Array, ns + std::to_string(setupVars.icolorG2));
    globalSuccess &= registerAndLog("streghtsG2", DataType::Int16, ns + std::to_string(setupVars.istreghtsG2));
    globalSuccess &= registerAndLog("colorG3", DataType::Int16Array, ns + std::to_string(setupVars.icolorG3));
    globalSuccess &= registerAndLog("streghtsG3", DataType::Int16, ns + std::to_string(setupVars.istreghtsG3));
    globalSuccess &= registerAndLog("FarbeLive", DataType::Int16Array, ns + std::to_string(setupVars.iFarbeLive));
    // FarbeErledigt ist OPTIONAL: schlägt die Registrierung fehl (Knoten fehlt auf
    // der SPS), NICHT globalSuccess beeinflussen -> Setup bleibt erfolgreich, im
    // Renderer greift der Default (Grau). So bricht eine ältere SPS-Konfiguration
    // ohne diesen Knoten nicht das gesamte Setup ab.
    registerAndLog("FarbeErledigt", DataType::Int16Array, ns + std::to_string(setupVars.iFarbeErledigt));
    globalSuccess &= registerAndLog("StaerkeLive", DataType::Int16, ns + std::to_string(setupVars.iStaerkeLive));
    // StaerkeErledigt ebenfalls OPTIONAL (kein globalSuccess &=): fehlt der Knoten,
    // greift der Default (0 = normale Stärke) statt das Setup zu brechen.
    registerAndLog("StaerkeErledigt", DataType::Int16, ns + std::to_string(setupVars.iStaerkeErledigt));

    globalSuccess &= registerAndLog("LiveStand", DataType::Int16, ns + std::to_string(setupVars.iLiveStand));
    // LiveAbbruch ist ein BOOL: der Dienst liest/schreibt ihn mit readBool/
    // writeBool. Die frühere Int16-Registrierung passte nicht zur SPS.
    globalSuccess &= registerAndLog("LiveAbbruch", DataType::Bool, ns + std::to_string(setupVars.iLiveAbbruch));

    std::cerr << "\n--- Bogenglättung ---" << std::endl;
    globalSuccess &= registerAndLog("minArcRadius", DataType::Float, ns + std::to_string(setupVars.iMinArcRadius));


    
    std::cerr << "\n--- Canvas ---" << std::endl;
    globalSuccess &= registerAndLog("objektgroesseX", DataType::Int16, ns + std::to_string(setupVars.objektgroesseX));
    globalSuccess &= registerAndLog("objektgroesseY", DataType::Int16, ns + std::to_string(setupVars.objektgroesseY));
    
    std::cerr << "\n=== ZUSAMMENFASSUNG ===" << std::endl;
    std::cerr << "✓ Erfolgreich: " << successCount << std::endl;
    std::cerr << "✗ Fehlgeschlagen: " << failCount << std::endl;
    std::cerr << "Gesamt-Ergebnis: " << (globalSuccess ? "SUCCESS" : "FAILED") << std::endl;
    std::cerr << "==============================\n" << std::endl;

    return globalSuccess;
}
tuple<KameraSettings, bool, int> SPSController::readCameraSettings()
{
    KameraSettings settings;
    auto [exposureTime, error, errorCode] = dialog->readInt16("exposure");
    if (error)
    {
        return {settings, true, errorCode};
    }
    settings.exposureTime = exposureTime;
    auto [gain, errorGain, errorCodeGain] = dialog->readFloat("gain");
    if (errorGain)
    {
        return {settings, true, errorCodeGain};
    }
    settings.gain = gain;
    auto [autoGain, errorAutoGain, errorCodeAutoGain] = dialog->readBool("autoGain");
    if (errorAutoGain)
    {
        return {settings, true, errorCodeAutoGain};
    }
    settings.autoGain = autoGain;
    auto [autoExposure, errorAutoExposure, errorCodeAutoExposure] = dialog->readBool("autoExposure");
    if (errorAutoExposure)
    {
        return {settings, true, errorCodeAutoExposure};
    }
    settings.autoExposure = autoExposure;

    return {settings, false, 0}; // alles ok
}

tuple<bool, int> SPSController::writeCameraSettings(const KameraSettings &settings)
{
    auto [success, errorCode] = dialog->writeInt16("exposure", settings.exposureTime);
    if (success)
    {
        return {true, 0}; // alles ok
    }
    else
    {
        return {false, errorCode}; // Fehler beim Schreiben
    }
}

tuple<ProcessImgParams, bool, int> SPSController::readProcessImgParams()
{
    ProcessImgParams params;
    auto [medianBlurr, error, errorCode] = dialog->readInt16("MedianBlurr");
    if (error)
    {
        return {params, true, errorCode};
    }
    params.MedianBlurr = medianBlurr;
    auto [threshold, errorThreshold, errorCodeThreshold] = dialog->readInt16("Threshold");
    if (errorThreshold)
    {
        return {params, true, errorCodeThreshold};
    }
    params.Threshold = threshold;
    auto [maxAngle, errorMaxAngle, errorCodeMaxAngle] = dialog->readInt16("MaxAngle");
    if (errorMaxAngle)
    {
        return {params, true, errorCodeMaxAngle};
    }
    params.MaxAngle = maxAngle;
    auto [labelSizeX, errorLabelSizeX, errorCodeLabelSizeX] = dialog->readInt16("LabelSizeX");
    if (errorLabelSizeX)
    {
        return {params, true, errorCodeLabelSizeX};
    }
    params.LabelSizeX = labelSizeX;
    auto [labelSizeY, errorLabelSizeY, errorCodeLabelSizeY] = dialog->readInt16("LabelSizeY");
    if (errorLabelSizeY)
    {
        return {params, true, errorCodeLabelSizeY};
    }
    params.LabelSizeY = labelSizeY;

    // Weitere Parameter hier lesen und in params speichern

    return {params, false, 0}; // alles ok
}
tuple<bool, int> SPSController::writeProcessImgParams(const ProcessImgParams &params)
{
    auto [success, errorCode] = dialog->writeInt16("MedianBlurr", params.MedianBlurr);
    if (success)
    {
        return {true, 0}; // alles ok
    }
    else
    {
        return {false, errorCode}; // Fehler beim Schreiben
    }
}

tuple<bool, int> SPSController::pushErrorMessage(const std::string &errorMessage, int errorCode)
{
    bool error = false;
    auto [error_error, errorCodeError] = dialog->writeBool("Error", true);
    auto [error_Message, errorCodeerrorMessage] = dialog->writeChars("Info_Text", errorMessage, 255);
    auto [error_errorCode, errorCodeerrorCode_] = dialog->writeInt16("StatusCode", errorCode);
    

    if (error_Message || error_errorCode || error_error)
    {
        error = true;
    }
    if (error)
    {
        return {true, 0}; // alles ok
    }
    else
    {
        return {false, errorCode}; // Fehler beim Schreiben
    }
}
