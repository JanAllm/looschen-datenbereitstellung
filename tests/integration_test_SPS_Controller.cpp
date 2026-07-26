
/*
#include <gtest/gtest.h>
#include <interfaces/SPSController.h>
#include <fstream>
#include <string>
#include "interfaces/VariablenInfo.h"
#include "interfaces/DataType.h"

std::string load2IPAdress(const std::string &filename)
{
    std::ifstream file(filename);
    std::string line;
    std::string ipSPS = "lokal"; // fallback

    if (!file.is_open())
    {
        ErrorLoggerSingleton::instance().logError("Fehler beim Öffnen der Settings Datei!");
        return ipSPS;
    }

    while (std::getline(file, line))
    {
        size_t pos = line.find('=');
        if (pos != std::string::npos)
        {
            std::string key = line.substr(0, pos);
            std::string value = line.substr(pos + 1);
            if (key == "ipSPS")
                ipSPS = value;
        }
    }
    return ipSPS;
}

TEST(SPSController, integrationTest_connect)
{
    std::string ip = load2IPAdress("camera_config.txt");
    std::cout << "[INFO] Verbindung zu TwinCAT-SPS: " << ip << std::endl;

    try
    {
        SPSController controller("twincat", ip);
        auto connected = controller.connect();
        EXPECT_TRUE(connected) << "Verbindung zur SPS konnte nicht aufgebaut werden.";
        std::cout << "[INFO] Verbindung erfolgreich hergestellt." << std::endl;
    }
    catch (const std::exception &e)
    {
        std::cerr << "[ERROR] Ausnahme beim Test: " << e.what() << std::endl;
        FAIL() << "Exception: " << e.what();
    }
}
TEST(SPSController, integrationTest_Variable_handling)
{
    std::string ip = load2IPAdress("camera_config.txt");
    std::cout << "[INFO] Verbindung zu TwinCAT-SPS: " << ip << std::endl;

    try
    {
        SPSController controller("twincat", ip);
        auto connected = controller.connect();
        EXPECT_TRUE(connected) << "Verbindung zur SPS konnte nicht aufgebaut werden.";
        std::cout << "[INFO] Verbindung erfolgreich hergestellt." << std::endl;
        DataType typeBool = DataType::Bool;
        bool success = controller.registerVariable("TestBool", typeBool, "GVL_Kamera.TestBool");
        EXPECT_TRUE(success) << "Fehler bei der Registrierung der Variable.";
        auto [results, error, errorCode] = controller.readBool("TestBool");
        EXPECT_FALSE(error) << "Fehler beim Lesen der Bool-Variable: " << errorCode;

        auto [errorWrite, errorCodeWrite] = controller.writeBool("TestBool", true);
        EXPECT_FALSE(errorWrite) << "Fehler beim Schreiben der Bool-Variable: " << errorCodeWrite;
        auto [resultsRead, errorRead, errorCodeRead] = controller.readBool("TestBool");
        EXPECT_FALSE(errorRead) << "Fehler beim Lesen der Bool-Variable: " << errorCodeRead;
        EXPECT_EQ(resultsRead, true) << "Unerwarteter Wert für TestBool: " << resultsRead;

        bool successInt = controller.registerVariable("TestInt", DataType::Int16, "GVL_Kamera.TestInt");
        EXPECT_TRUE(successInt) << "Fehler bei der Registrierung der Int16-Variable.";

        auto [errorWriteInt, errorCodeWriteInt] = controller.writeInt16("TestInt", 42);
        EXPECT_FALSE(errorWriteInt) << "Fehler beim Schreiben der Int16-Variable: " << errorCodeWriteInt;

        auto [resultsInt, errorInt, errorCodeInt] = controller.readInt16("TestInt");
        EXPECT_FALSE(errorInt) << "Fehler beim Lesen der Int16-Variable: " << errorCodeInt;
        EXPECT_EQ(resultsInt, 42) << "Unerwarteter Wert für GVL_Kamera.TestInt: " << resultsInt;
    }
    catch (const std::exception &e)
    {
        std::cerr << "[ERROR] Ausnahme beim Test: " << e.what() << std::endl;
        FAIL() << "Exception: " << e.what();
    }
}
TEST(SPSController, integrationTest_SetupVariables)
{
    std::string ip = load2IPAdress("camera_config.txt");
    std::cout << "[INFO] Verbindung zu TwinCAT-SPS: " << ip << std::endl;

    try
    {
        SPSController controller("twincat", ip);
        auto connected = controller.connect();
        EXPECT_TRUE(connected) << "Verbindung zur SPS konnte nicht aufgebaut werden.";
        std::cout << "[INFO] Verbindung erfolgreich hergestellt." << std::endl;
        //bool sucsses = controller.setupVariables();
        //EXPECT_TRUE(sucsses) << "Fehler beim Einrichten der Variablen.";
    }
    catch (const std::exception &e)
    {
        std::cerr << "[ERROR] Ausnahme beim Test: " << e.what() << std::endl;
        FAIL() << "Exception: " << e.what();
    }
}
TEST(SPSController, integrationTest_readCameraSettings)
{
    std::string ip = load2IPAdress("camera_config.txt");
    std::cout << "[INFO] Verbindung zu TwinCAT-SPS: " << ip << std::endl;

    try
    {
        SPSController controller("twincat", ip);
        auto connected = controller.connect();
        //controller.setupVariables();
        EXPECT_TRUE(connected) << "Verbindung zur SPS konnte nicht aufgebaut werden.";
        std::cout << "[INFO] Verbindung erfolgreich hergestellt." << std::endl;
        auto [settings, error, errorCode] = controller.readCameraSettings();
        EXPECT_FALSE(error) << "Fehler beim Lesen der Kameraeinstellungen: " << errorCode;
        // EXPECT_EQ(settings.exposureTime, 1000) << "Unerwarteter Wert für exposureTime: " << settings.exposureTime;
        // EXPECT_FLOAT_EQ(settings.gain, 1.5f) << "Unerwarteter Wert für gain: " << settings.gain;
        // EXPECT_EQ(settings.autoGain, true) << "Unerwarteter Wert für autoGain: " << settings.autoGain;
        // EXPECT_EQ(settings.autoExposure, false) << "Unerwarteter Wert für autoExposure: " << settings.autoExposure;
    }
    catch (const std::exception &e)
    {
        std::cerr << "[ERROR] Ausnahme beim Test: " << e.what() << std::endl;
        FAIL() << "Exception: " << e.what();
    }
}

TEST(SPSController, integrationTest_readProcessImgParams)
{
    std::string ip = load2IPAdress("camera_config.txt");
    std::cout << "[INFO] Verbindung zu TwinCAT-SPS: " << ip << std::endl;

    try
    {
        SPSController controller("twincat", ip);
        auto connected = controller.connect();
        EXPECT_TRUE(connected) << "Verbindung zur SPS konnte nicht aufgebaut werden.";
        std::cout << "[INFO] Verbindung erfolgreich hergestellt." << std::endl;
        auto [params, error, errorCode] = controller.readProcessImgParams();
        EXPECT_FALSE(error) << "Fehler beim Lesen der Bildverarbeitungsparameter: " << errorCode;
        // EXPECT_EQ(params.MedianBlurr, 5) << "Unerwarteter Wert für MedianBlurr: " << params.MedianBlurr;
        // EXPECT_EQ(params.Threshold, 128) << "Unerwarteter Wert für Threshold: " << params.Threshold;
        // EXPECT_EQ(params.MaxAngle, 45) << "Unerwarteter Wert für MaxAngle: " << params.MaxAngle;
    }
    catch (const std::exception &e)
    {
        std::cerr << "[ERROR] Ausnahme beim Test: " << e.what() << std::endl;
        FAIL() << "Exception: " << e.what();
    }
}

TEST(SPSController, integrationsTest_PushErrorMessage)
{
    std::string ip = load2IPAdress("camera_config.txt");
    std::cout << "[INFO] Verbindung zu TwinCAT-SPS: " << ip << std::endl;

    try
    {
        SPSController controller("twincat", ip);
        auto connected = controller.connect();
        EXPECT_TRUE(connected) << "Verbindung zur SPS konnte nicht aufgebaut werden.";
        //controller.setupVariables();
        std::cout << "[INFO] Verbindung erfolgreich hergestellt." << std::endl;
        auto [error, errorCode] = controller.pushErrorMessage("Test Error", 1);
        EXPECT_FALSE(error) << "Fehler beim Pushen der Fehlermeldung: " << errorCode;
    }
    catch (const std::exception &e)
    {
        std::cerr << "[ERROR] Ausnahme beim Test: " << e.what() << std::endl;
        FAIL() << "Exception: " << e.what();
    }
}
*/