

#include <gtest/gtest.h>
#include <interfaces/SPSController.h>
#include <fstream>
#include <string>
#include "interfaces/VariablenInfo.h"
#include "interfaces/DataType.h"

/*
TEST(OPC, TEST_Connection)
{
    SPSController client("opc", "opc.tcp://192.168.155.4:4840");
    // SPSController client("opc", "opc.tcp://192.168.124.169:4840");
    EXPECT_TRUE(client.connect()) << "Failed to connect to OPC server";

    client.printNamespaceArray();
    EXPECT_TRUE(client.connect()) << "Failed to connect to OPC server";
}

TEST(OPC, TEST_READ_VAR)
{
    SPSController controller("opc", "opc.tcp://192.168.155.4:4840");
    // SPSController controller("opc", "opc.tcp://lokalhost:4840");
    EXPECT_TRUE(controller.connect()) << "Failed to connect to OPC server";
    std::string variableName = "TestInt";
    controller.registerVariable(variableName, DataType::Int16, "ns=4;i=4005");
    auto [value, success, errorCode] = controller.readInt16(variableName);
    EXPECT_TRUE(success) << "Failed to read variable '" << variableName << "' with error code: " << errorCode << "value: " << value;
    // EXPECT_EQ(value, 42) << "Unexpected value read from variable '" << variableName << "'";
    controller.disconnect();
}

TEST(OPC, TEST_WRITE_VAR)
{
    SPSController controller("opc", "opc.tcp://192.168.155.4:4840");
    // SPSController controller("opc", "opc.tcp://lokalhost:4840");
    EXPECT_TRUE(controller.connect()) << "Failed to connect to OPC server";
    std::string variableName = "TestInt";
    controller.registerVariable(variableName, DataType::Int16, "ns=4;i=4005");
    auto [value, success, errorCode] = controller.readInt16(variableName);
    EXPECT_TRUE(success) << "Failed to read variable '" << variableName << "' with error code: " << errorCode << "value: " << value;
    // EXPECT_EQ(value, 42) << "Unexpected value read from variable '" << variableName << "'";

    int16_t newValue = value + 10;
    auto [writeSuccess, writeErrorCode] = controller.writeInt16(variableName, newValue);
    EXPECT_TRUE(writeSuccess) << "Failed to write variable '" << variableName << "' with error code: " << writeErrorCode;
    auto [readValue, readSuccess, readErrorCode] = controller.readInt16(variableName);
    EXPECT_TRUE(readSuccess) << "Failed to read variable '" << variableName << "' after write with error code: " << readErrorCode;
    EXPECT_EQ(readValue, newValue) << "Unexpected value read from variable '" << variableName << "' after write";
    controller.disconnect();
}

TEST(OPC, TEST_READ_WRITE_STRIGN)
{
    SPSController controller("opc", "opc.tcp://192.168.155.4:4840");
    // SPSController controller("opc", "opc.tcp://lokalhost:4840");
    EXPECT_TRUE(controller.connect()) << "Failed to connect to OPC server";
    std::string variableName = "TEST_STRING";
    controller.registerVariable(variableName, DataType::String, "ns=4;i=4004");
    auto [value, success, errorCode] = controller.readString(variableName);
    EXPECT_TRUE(success) << "Failed to read variable '" << variableName << "' with error code: " << errorCode << " value: " << value;
    std::cerr << "Read value: " << value << std::endl;

    std::string newValue = "Moin Moin!";
    auto [writeSuccess, writeErrorCode] = controller.writeChars(variableName, newValue, 255);
    EXPECT_TRUE(writeSuccess) << "Failed to write variable '" << variableName << "' with error code: " << writeErrorCode;

    auto [readValue, readSuccess, readErrorCode] = controller.readString(variableName);
    EXPECT_TRUE(readSuccess) << "Failed to read variable '" << variableName << "' after write with error code: " << readErrorCode;
    EXPECT_EQ(readValue, newValue) << "Unexpected value read from variable '" << variableName << "' after write, expected: " << newValue << ", got: " << readValue;
    controller.disconnect();
}
TEST(OPC, TEST_READ_WRITE_BOOL)
{
    SPSController controller("opc", "opc.tcp://192.168.155.4:4840");
    // SPSController controller("opc", "opc.tcp://lokalhost:4840");
    EXPECT_TRUE(controller.connect()) << "Failed to connect to OPC server";
    std::string variableName = "TestBool";
    controller.registerVariable(variableName, DataType::Bool, "ns=4;i=4006");
    auto [value, success, errorCode] = controller.readBool(variableName);
    EXPECT_TRUE(success) << "Failed to read variable '" << variableName << "' with error code: " << errorCode << " value: " << value;
    std::cerr << "Read value: " << value << std::endl;
    bool newValue = !value; // Toggle the boolean value
    auto [writeSuccess, writeErrorCode] = controller.writeBool(variableName, newValue);
    EXPECT_TRUE(writeSuccess) << "Failed to write variable '" << variableName << "' with error code: " << writeErrorCode;
    auto [readValue, readSuccess, readErrorCode] = controller.readBool(variableName);
    EXPECT_TRUE(readSuccess) << "Failed to read variable '" << variableName << "' after write with error code: " << readErrorCode;
    EXPECT_EQ(readValue, newValue) << "Unexpected value read from variable '" << variableName << "' after write, expected: " << newValue << ", got: " << readValue;

    controller.disconnect();
}

TEST(OPC, TEST_READ_WRITE_FLOAT)
{
    SPSController controller("opc", "opc.tcp://192.168.155.4:4840");
    // SPSController controller("opc", "opc.tcp://lokalhost:4840");
    EXPECT_TRUE(controller.connect()) << "Failed to connect to OPC server";
    std::string variableName = "TestFloat";
    controller.registerVariable(variableName, DataType::Float, "ns=4;i=4716");
    auto [value, success, errorCode] = controller.readFloat(variableName);
    EXPECT_TRUE(success) << "Failed to read variable '" << variableName << "' with error code: " << errorCode << " value: " << value;
    std::cerr << "Read value: " << value << std::endl;
    float newValue = value + 11.11f;
    auto [writeSuccess, writeErrorCode] = controller.writeFloat(variableName, newValue);
    EXPECT_TRUE(writeSuccess) << "Failed to write variable '" << variableName << "' with error code: " << writeErrorCode;
    auto [readValue, readSuccess, readErrorCode] = controller.readFloat(variableName);
    EXPECT_TRUE(readSuccess) << "Failed to read variable '" << variableName << "' after write with error code: " << readErrorCode;
    EXPECT_EQ(readValue, newValue) << "Unexpected value read from variable '" << variableName << "' after write, expected: " << newValue << ", got: " << readValue;
    controller.disconnect();
}

TEST(OPC, TEST_READ_ARRAY)
{
    SPSController controller("opc", "opc.tcp://192.168.155.4:4840");
    // SPSController controller("opc", "opc.tcp://lokalhost:4840");
    EXPECT_TRUE(controller.connect()) << "Failed to connect to OPC server";
    std::string variableName = "TestArray";
    EXPECT_TRUE(controller.registerVariable(variableName, DataType::StringArray, "ns=4;i=2")) << "Failed to register variable '" << variableName << "'";

    auto [value, success, errorCode] = controller.readStringArray(variableName, 0, 1, 1, 9);
    std::vector<std::string> strings = value[0];
    EXPECT_TRUE(success) << "Failed to read variable '" << variableName << "' with errorCode: " << errorCode;

    try
    {
        std::cerr << "value shape: " << strings.size() << std::endl;
        std::cerr << "Read value: " << strings[0] << std::endl;
    }
    catch (const std::exception &e)
    {
        std::cerr << "Exception occurred while reading value: " << e.what() << std::endl;
        FAIL() << "Exception occurred while reading value";
    }
}
TEST(OPC, TEST_Write_ARRAY)
{
    try
    {

        SPSController controller("opc", "opc.tcp://192.168.155.4:4840");
        // SPSController controller("opc", "opc.tcp://lokalhost:4840");
        EXPECT_TRUE(controller.connect()) << "Failed to connect to OPC server";
        std::string variableName = "TestArray";
        EXPECT_TRUE(controller.registerVariable(variableName, DataType::StringArray, "ns=4;i=2")) << "Failed to register variable '" << variableName << "'";
        std::vector<std::string> strings = {"Hallo ", "Nico ", "hier ", "ist ", "der ", "einig ", "wahre ", "Jan ", "!!!!!!!"};
        std::vector<std::vector<std::string>> stringArray = {strings};
        auto [writeSuccess, writeErrorCode] = controller.writeStringArray(variableName, stringArray, 0, 0);
        EXPECT_TRUE(writeSuccess) << "Failed to write variable '" << variableName << "' with error code: " << writeErrorCode;

        auto [readValue, readSuccess, readErrorCode] = controller.readStringArray(variableName, 0, 1, 0, 9);
        EXPECT_TRUE(readSuccess) << "Failed to read variable '" << variableName << "' after write with error code: " << readErrorCode;
        std::vector<std::string> readStrings = readValue[0];
        EXPECT_EQ(readStrings.size(), strings.size()) << "Unexpected size of read array from variable '" << variableName << "' after write, expected: " << strings.size() << ", got: " << readStrings.size();
        for (size_t i = 0; i < strings.size(); ++i)
        {
            EXPECT_EQ(readStrings[i], strings[i]) << "Unexpected value at index " << i << " in read array from variable '" << variableName << "' after write, expected: " << strings[i] << ", got: " << readStrings[i];
        }
        controller.disconnect();
    }
    catch (const std::exception &e)
    {
        std::cerr << "Exception occurred while writing value: " << e.what() << std::endl;
        FAIL() << "Exception occurred while writing value";
    }
}

TEST(OPC, TEST_READ_WRITE_2DFLOAT_ARRAY)
{
    try
    {
        SPSController controller("opc", "opc.tcp://192.168.155.4:4840");
        // SPSController controller("opc", "opc.tcp://lokalhost:4840");
        EXPECT_TRUE(controller.connect()) << "Failed to connect to OPC server";
        std::string variableName = "Test2DFloatArray";
        EXPECT_TRUE(controller.registerVariable(variableName, DataType::FloatArray, "ns=4;i=4008")) << "Failed to register variable '" << variableName << "'";
        auto [value, success, errorCode] = controller.readFloatArray(variableName, 0, 2, 0, 3);
        EXPECT_TRUE(success) << "Failed to read variable '" << variableName << "' with error code: " << errorCode;

        std::vector<std::vector<float>> floatArray;
        float wert  = 0.0f;
        for (int i = 0; i < 10; ++i)
        {
            std::vector<float> floatRow;
            for (int j = 0; j < 1; ++j)
            {
                //wert += 0.10f;
                wert = 100;
                floatRow.push_back(wert);
                
            }
            floatArray.push_back(floatRow);
        }
        auto [writeSuccess, writeErrorCode] = controller.writeFloatArray(variableName, floatArray, 5, 5);
        EXPECT_TRUE(writeSuccess) << "Failed to write variable '" << variableName << "' with error code: " << writeErrorCode;
        controller.disconnect();
    }
    catch (const std::exception &e)
    {
        std::cerr << "Exception occurred while writing value: " << e.what() << std::endl;
        FAIL() << "Exception occurred while writing value";
    }
}
*/