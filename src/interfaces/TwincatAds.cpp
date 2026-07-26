#include "interfaces/TwincatAds.h"
#include "TcAdsAPI.h" 
#include <vector>
#include <string>
#include <sstream>
#include <tuple>


#ifndef AMSPORT_R0_PLC_TC3
#define AMSPORT_R0_PLC_TC3 851
#endif
/// !!!!!!!!!!!!!!!!!!!!!!!!ANMERKUNG TRUE UND FALSE ALS RÜCKGABEWERT TASUCHEN !!!!!!!!!!!!!!!!!!!!!!!!
using namespace std;
void TwincatAds::ipToAmsNetId(const   string &ip, uint8_t *netId)
{
      istringstream ss(ip);
      string segment;
    int i = 0;

    // IP-Adresse in Bytes zerlegen
    while (  getline(ss, segment, '.'))
    {
        if (i < 4)
        {
            netId[i++] = static_cast<uint8_t>(  stoi(segment));
        }
    }
    // Standardmäßig AMS-Suffix setzen
    netId[4] = 1; // Erstes Suffix-Byte
    netId[5] = 1; // Zweites Suffix-Byte
}

bool TwincatAds::connect()
{
    long port = AdsPortOpen();
    if (port <= 0)  // ≤ statt nur == 0
    {
        ErrorLoggerSingleton::instance().logError("Fehler beim Öffnen des ADS-Ports!");
        return false;
    }

    // AMS-NetID setzen
    if (ipSPS == "lokal" || ipSPS == "127.0.0.1")
    {
        remoteNetId[0] = 127;
        remoteNetId[1] = 0;
        remoteNetId[2] = 0;
        remoteNetId[3] = 1;
    }
    else
    {
        ipToAmsNetId(ipSPS, remoteNetId);
    }

    // Suffix für TwinCAT-AMS
    remoteNetId[4] = 1;
    remoteNetId[5] = 1;

    std::memcpy(&addr.netId, remoteNetId, sizeof(remoteNetId));
    addr.port = AMSPORT_R0_PLC_TC3;

    // Test: kleine Kommunikationsanfrage zum Check
    uint32_t dummy = 0;
    long nErr = AdsSyncReadReq(&addr, ADSIGRP_SYM_VERSION, 0, sizeof(dummy), &dummy);
    if (nErr != 0)
    {
        std::stringstream ss;
        ss << "Verbindung zur SPS (" << ipSPS << ") fehlgeschlagen. Fehlercode: " << nErr;
        ErrorLoggerSingleton::instance().logError(ss.str());
        AdsPortClose();
        return false;
    }

    return true;
}

/**
 * @brief Registers a variable in the TwinCAT ADS system.
 * 
 * This function attempts to create a handle for the specified variable name.
 * If the handle is successfully created, the function returns true. If the 
 * handle already exists, it returns false. In case of an exception, it logs 
 * the error and returns false.
 * 
 * @param name The name of the variable to register.
 * @param type The type of the variable (not used in the current implementation).
 * @param connectionString The connection string for the TwinCAT ADS system 
 *                         (not used in the current implementation).
 * @return true if the handle was successfully created.
 * @return false if the handle already exists or an exception occurred.
 */
std::tuple<bool, std::optional<long>> TwincatAds::registerVariable(const std::string &name, DataType type, std::string connectionString)
{
    try
    {
       long handle = 0; // Handle initialisieren
        // Hier wird die ADS-Funktion aufgerufen; varName.size() + 1 sichert den Nullterminator
        long nErr = AdsSyncReadWriteReq(
            &addr,
            ADSIGRP_SYM_HNDBYNAME,
            0,
            sizeof(handle),
            &handle,
            connectionString.size() + 1, // Länge inklusive Nullterminator
            (void *)connectionString.c_str());

        if (nErr != 0)
        {
              string errorMsg = "Fehler beim Erstellen des Handles: " + name +
                                   " Fehlercode: " +   to_string(nErr);
            // Fehler loggen
            ErrorLoggerSingleton::instance().logError(errorMsg);
            return {false, -1};
        }

        handleCache[name] = handle;
        return {true, handle};
    }
    catch (const std::exception &e)
    {
        cerr << "Exception: " << e.what() << endl;
        return {false, -1};
    }
}

long TwincatAds::getVariableHandles(const   string &varName)
{
    auto it = handleCache.find(varName); // sucht den Variablennamen im handle Cache
    if (it != handleCache.end())
        return it->second; // Wenn Handle gefunden, wird der handle zurückgegeben
    return -1;             // Handle nicht gefunden, gibt -1 zurück
}

bool TwincatAds::createVariableHandles(const   string &varName)
{
    try
    {
        auto it = handleCache.find(varName); // sucht den Variablennamen im handle Cache
        if (it != handleCache.end())
        {
            return false; // Handle ist bereits vorhanden, keine Aktion erforderlich
        }

        long handle = 0; // Handle initialisieren
        // Hier wird die ADS-Funktion aufgerufen; varName.size() + 1 sichert den Nullterminator
        long nErr = AdsSyncReadWriteReq(
            &addr,
            ADSIGRP_SYM_HNDBYNAME,
            0,
            sizeof(handle),
            &handle,
            varName.size() + 1, // Länge inklusive Nullterminator
            (void *)varName.c_str());

        if (nErr != 0)
        {
              string errorMsg = "Fehler beim Erstellen des Handles: " + varName +
                                   " Fehlercode: " +   to_string(nErr);
            // Fehler loggen
            ErrorLoggerSingleton::instance().logError(errorMsg);
        }

        handleCache[varName] = handle;

        return true;
    }

    catch (const   exception &e)
    {
          cerr << "Exception: " << e.what() <<   endl;
        return false;
    }
}

bool TwincatAds::disconnect()
{
    AdsPortClose();
    return true; // Indicate successful disconnection
}



/**
 * @brief Liest einen 16-Bit-Ganzzahlwert (int16_t) aus einer TwinCAT-ADS-Variable.
 *
 * Diese Funktion liest den Wert einer TwinCAT-ADS-Variable, die durch ihren Namen
 * identifiziert wird, und gibt den Wert zusammen mit einem Fehlerstatus zurück.
 *
 * @param varName Der Name der TwinCAT-ADS-Variable, die gelesen werden soll.
 * @return Ein   pair, das den gelesenen Wert (int16_t) und einen booleschen
 *         Fehlerstatus enthält. Der Fehlerstatus ist `true`, wenn ein Fehler
 *         aufgetreten ist, und `false`, wenn der Wert erfolgreich gelesen wurde.
 *
 *
 *
 * @note Wenn der Handle für die angegebene Variable nicht gefunden wird, wird
 *       ein Fehler ausgegeben und ein Paar mit dem Wert 0 und dem Fehlerstatus
 *       `true` zurückgegeben.
 */
std::tuple<int16_t, bool, int> TwincatAds::readInt16(const VariableInfo &var)
{
    long handle = var.handle.value_or(-1); // Handle aus dem Cache abrufen, -1 falls nicht vorhanden
    if (handle == -1)
    {
        return {0, false, 1};
    }
    long nErr;
    int16_t value;
    uint32_t bytesRead;
    nErr = AdsSyncReadReq(&addr, ADSIGRP_SYM_VALBYHND, handle, sizeof(value), &value);
    if (nErr != 0)
    {
        ErrorLoggerSingleton::instance().logError("Fehler beim Lesen des Wertes");
    }
    return {value, true, 0};
}

std::tuple<std::string, bool, int> TwincatAds::readString(const VariableInfo &var)
{   
    std::cout << "TwincatAds: readString() aufgerufen für Variable: " << var.name << std::endl;
    long handle = var.handle.value_or(-1); // Handle aus dem Cache abrufen, -1 falls nicht vorhanden
    if (handle == -1)
    {
        return {0, false, 1};
    }
    std::cout << "TwincatAds: Handle ist: " << handle << std::endl;
    long nErr;
    string value;
    uint32_t bytesRead;
    nErr = AdsSyncReadReq(&addr, ADSIGRP_SYM_VALBYHND, handle, sizeof(value), &value);
    if (nErr != 0)
    {
        ErrorLoggerSingleton::instance().logError("Fehler beim Lesen des Wertes");
        return {"", false, nErr};
    }
    std::cout << "TwincatAds: Gelesener Wert ist: " << value << std::endl;
    return {value, true, 0};
}
std::tuple<std::wstring, bool, int> TwincatAds::readWString(const VariableInfo &var)
{
    long handle = var.handle.value_or(-1); // Handle aus dem Cache abrufen, -1 falls nicht vorhanden
    if (handle == -1)
    {
        return {L"", false, 1};
    }
    long nErr;
    wstring value;
    uint32_t bytesRead;
    nErr = AdsSyncReadReq(&addr, ADSIGRP_SYM_VALBYHND, handle, sizeof(value), &value);
    if (nErr != 0)
    {
        ErrorLoggerSingleton::instance().logError("Fehler beim Lesen des Wertes");
        return {L"", false, nErr};
    }
    return {value, true, 0};
}

std::tuple<bool, bool, int> TwincatAds::readBool(const VariableInfo &var)
{
    long handle = var.handle.value_or(-1); // Handle aus dem Cache abrufen, -1 falls nicht vorhanden
    if (handle == -1)
    {   
        std::cerr << "kein Handle hinterlegt: " << var.name << std::endl;
        return {false, false, 1};
    }
    long nErr;
    bool value;
    nErr = AdsSyncReadReq(&addr, ADSIGRP_SYM_VALBYHND, handle, sizeof(value), &value);
    if (nErr != 0)
    {
        ErrorLoggerSingleton::instance().logError("Fehler beim Lesen des Wertes");
        return {false, false, nErr};
    }
    return {value, true, 0};
}

std::tuple<float, bool, int> TwincatAds::readFloat(const VariableInfo &var)
{
    long handle = var.handle.value_or(-1); // Handle aus dem Cache abrufen, -1 falls nicht vorhanden
    if (handle == -1)
    {
        return {0, false, 1};
    }

    long nErr;
    float value;
    nErr = AdsSyncReadReq(&addr, ADSIGRP_SYM_VALBYHND, handle, sizeof(value), &value);
    if (nErr != 0)
    {
        ErrorLoggerSingleton::instance().logError("Fehler beim Lesen des Wertes");
        return {0.0f, false, nErr};
    }
    return {value, true, 0};
}

std::tuple<int32_t, bool, int> TwincatAds::readInt32(const VariableInfo &var)
{
    long handle = var.handle.value_or(-1); // Handle aus dem Cache abrufen, -1 falls nicht vorhanden
    if (handle == -1)
    {
        return {0, false, 1}; // false = kein Erfolg
    }

    long nErr;
    int32_t value;
    nErr = AdsSyncReadReq(&addr, ADSIGRP_SYM_VALBYHND, handle, sizeof(value), &value);
    if (nErr != 0)
    {
        ErrorLoggerSingleton::instance().logError("Fehler beim Lesen des Wertes");
        return {0, false, nErr};
    }
    return {value, true, 0}; // true = Erfolg
}

std::tuple<double, bool, int> TwincatAds::readDouble(const VariableInfo &var)
{
    long handle = var.handle.value_or(-1); // Handle aus dem Cache abrufen, -1 falls nicht vorhanden
    if (handle == -1)
    {
        return {0, false, 1};
    }
    long nErr;
    double value;
    nErr = AdsSyncReadReq(&addr, ADSIGRP_SYM_VALBYHND, handle, sizeof(value), &value);
    if (nErr != 0)
    {
        ErrorLoggerSingleton::instance().logError("Fehler beim Lesen des Wertes");
        return {0.0, false, nErr};
    }
    return {value, true, 0};
}

std::tuple<std::string, bool, int> TwincatAds::readChars(const VariableInfo &var, int length)
{
    long handle = var.handle.value_or(-1); // Handle aus dem Cache abrufen, -1 falls nicht vorhanden
    if (handle == -1)
    {
        return {"", false, 1};
    }
    long nErr;
    vector<char> buffer(length);
    uint32_t bytesRead = 0;

    nErr = AdsSyncReadReq(&addr, ADSIGRP_SYM_VALBYHND, handle, length, buffer.data());

    if (nErr != 0)
    {
        return {"", false, nErr};
    }

    return {std::string(buffer.data(), length), true, 0};
}

std::tuple<bool, int> TwincatAds::writeInt16(const VariableInfo &var, int16_t valueToWrite)
{
    long handle = var.handle.value_or(-1); // Handle aus dem Cache abrufen, -1 falls nicht vorhanden
    if (handle == -1)
    {
        return {false, 2};
    }
    long nErr;
    nErr = AdsSyncWriteReq(&addr, ADSIGRP_SYM_VALBYHND, handle, sizeof(valueToWrite), &valueToWrite);
    if (nErr != 0)
    {
        ErrorLoggerSingleton::instance().logError("Fehler beim Schreiben des Wertes");
        return {false, nErr};
    }
    return {true, 0};
}

std::tuple<bool, int> TwincatAds::writeFloat(const VariableInfo &var, float valueToWrite)
{
    long handle = var.handle.value_or(-1); // Handle aus dem Cache abrufen, -1 falls nicht vorhanden
    if (handle == -1)
    {
        return {false, 2};
    }
    long nErr;
    nErr = AdsSyncWriteReq(&addr, ADSIGRP_SYM_VALBYHND, handle, sizeof(valueToWrite), &valueToWrite);
    if (nErr != 0)
    {
        ErrorLoggerSingleton::instance().logError("Fehler beim Schreiben des Wertes");
        return {false, nErr};
    }
    return {true, 0};
}

std::tuple<bool, int> TwincatAds::writeDouble(const VariableInfo &var, double valueToWrite)
{
    long handle = var.handle.value_or(-1); // Handle aus dem Cache abrufen, -1 falls nicht vorhanden
    if (handle == -1)
    {
        return {false, 2};
    }
    long nErr;
    nErr = AdsSyncWriteReq(&addr, ADSIGRP_SYM_VALBYHND, handle, sizeof(valueToWrite), &valueToWrite);
    if (nErr != 0)
    {
        ErrorLoggerSingleton::instance().logError("Fehler beim Schreiben des Wertes");
        return {false, nErr};
    }
    return {true, 0};
}

std::tuple<bool, int> TwincatAds::writeBool(const VariableInfo &var, bool valueToWrite)
{
    long handle = var.handle.value_or(-1); // Handle aus dem Cache abrufen, -1 falls nicht vorhanden
    if (handle == -1)
    {
        return {false, 2};
    }

    long nErr;
    nErr = AdsSyncWriteReq(&addr, ADSIGRP_SYM_VALBYHND, handle, sizeof(valueToWrite), &valueToWrite);
    if (nErr != 0)
    {
        ErrorLoggerSingleton::instance().logError("Fehler beim Schreiben des Wertes");
        return {false, nErr};
    }
    return {true, 0};
}

std::tuple<bool, int> TwincatAds::writeInt32(const VariableInfo &var, int32_t valueToWrite)
{
    long handle = var.handle.value_or(-1); // Handle aus dem Cache abrufen, -1 falls nicht vorhanden
    if (handle == -1)
    {
        return {false, 2};
    }
    long nErr;
    nErr = AdsSyncWriteReq(&addr, ADSIGRP_SYM_VALBYHND, handle, sizeof(valueToWrite), &valueToWrite);
    if (nErr != 0)
    {
        ErrorLoggerSingleton::instance().logError("Fehler beim Schreiben des Wertes");
        return {false, nErr};
    }
    return {true, 0};
}

std::tuple<bool, int> TwincatAds::writeChars(const VariableInfo &var, const std::string &valueToWrite, int length)
{
    long handle = var.handle.value_or(-1); // Handle aus dem Cache abrufen, -1 falls nicht vorhanden
    if (handle == -1)
    {
        return {false, 2};
    }
    long nErr;
    vector<char> charArray(length, '/0');

    copy_n(valueToWrite.begin(), (min)(static_cast<size_t>(length), valueToWrite.size()), charArray.begin());

    nErr = AdsSyncWriteReq(&addr, ADSIGRP_SYM_VALBYHND, handle, length, charArray.data());
    if (nErr != 0)
    {
        ErrorLoggerSingleton::instance().logError("Fehler beim Schreiben des Wertes");
        
        return {false, nErr};
    }
    return {true, 0};
}

std::tuple<bool, int> TwincatAds::writeWString(const VariableInfo &var, const std::wstring &valueToWrite)
{
    std::cout << "muss noch implementiert werden" << std::endl;
    return {true, 0};
}

bool TwincatAds::writeLabelIO(long &handle, LabelIO valueToWrite)
{
    long nErr;
    nErr = AdsSyncWriteReq(&addr, ADSIGRP_SYM_VALBYHND, handle, sizeof(valueToWrite), &valueToWrite);
    if (nErr != 0)
    {
        
        ErrorLoggerSingleton::instance().logError("Fehler beim Schreiben des Wertes" + to_string(nErr));
    }
    return true;
}

bool TwincatAds::checkConnection()
{
    unsigned short adsState = 0;
    unsigned short deviceState = 0;

    long nErr = AdsSyncReadStateReq(&addr, &adsState, &deviceState);
    if (nErr != 0)
    {
        return false;
    }

    if (adsState != ADSSTATE_RUN)
    {
        return false;
    }

    return true;
}
