/*
#include <gtest/gtest.h>
#include <interfaces/SPSController.h>
#include <iostream>

TEST(SPSControllerTest, ConnectTest) {
    std::cout << "[TEST] Starte ConnectTest\n";

    try {
        SPSController client("test", "testString");

        std::cout << "[TEST] Versuche Verbindung herzustellen...\n";
        bool connected = client.connect();
        std::cout << "[TEST] Verbindung: " << std::boolalpha << connected << std::endl;

        EXPECT_TRUE(connected) << "Verbindung zur SPS konnte nicht hergestellt werden.";

        std::cout << "[TEST] Rufe returnTrue() auf...\n";
        bool test = client.returnTrue();
        std::cout << "[TEST] returnTrue: " << std::boolalpha << test << std::endl;

        ASSERT_TRUE(test) << "returnTrue() hat false zurückgegeben.";

    } catch (const std::exception& e) {
        FAIL() << "Exception aufgetreten: " << e.what();
    } catch (...) {
        FAIL() << "Unbekannter Fehler (nicht-catchbare Exception)";
    }

    std::cout << "[TEST] ConnectTest abgeschlossen.\n";
}

TEST(SPSControllerTest, Test_Variable_handling) {
    std::cout << "[TEST] Starte Test_Variable_handling\n";

    try {
        SPSController client("test", "testString");
        client.connect();

        // Test for Int16
        EXPECT_TRUE(client.registerVariable("testVariableInt16", DataType::Int16, "GVL.TestInt16"));
        auto [varInt16, errorInt16, errorCodeInt16] = client.readInt16("testVariableInt16");
        EXPECT_FALSE(errorInt16) << "Fehler beim Lesen der Int16-Variable: " << errorCodeInt16;
        EXPECT_EQ(varInt16, 42) << "Unerwarteter Wert für testVariableInt16: " << varInt16;

        // Adjusted all tests to use appropriate data types and methods
        EXPECT_TRUE(client.registerVariable("testVariableInt32", DataType::Int32, "GVL.TestInt32"));
        auto [varInt32, errorInt32, errorCodeInt32] = client.readInt32("testVariableInt32");
        EXPECT_FALSE(errorInt32) << "Fehler beim Lesen der Int32-Variable: " << errorCodeInt32;
        EXPECT_EQ(varInt32, 424242) << "Unerwarteter Wert für testVariableInt32: " << varInt32;

        EXPECT_TRUE(client.registerVariable("testVariableFloat", DataType::Float, "GVL.TestFloat"));
        auto [varFloat, errorFloat, errorCodeFloat] = client.readFloat("testVariableFloat");
        EXPECT_FALSE(errorFloat) << "Fehler beim Lesen der Float-Variable: " << errorCodeFloat;
        EXPECT_FLOAT_EQ(varFloat, 3.14) << "Unerwarteter Wert für testVariableFloat: " << varFloat;

        EXPECT_TRUE(client.registerVariable("testVariableBool", DataType::Bool, "GVL.TestBool"));
        auto [varBool, errorBool, errorCodeBool] = client.readBool("testVariableBool");
        EXPECT_FALSE(errorBool) << "Fehler beim Lesen der Bool-Variable: " << errorCodeBool;
        EXPECT_EQ(varBool, true) << "Unerwarteter Wert für testVariableBool: " << varBool;

        EXPECT_TRUE(client.registerVariable("testVariableString", DataType::String, "GVL.TestString"));
        auto [varString, errorString, errorCodeString] = client.readString("testVariableString");
        EXPECT_FALSE(errorString) << "Fehler beim Lesen der String-Variable: " << errorCodeString;
        EXPECT_EQ(varString, "TestString") << "Unerwarteter Wert für testVariableString: " << varString;

        auto [varFalse, errorFalse, errorCodeFalse] = client.readInt16("NichtVorhanden");
        EXPECT_TRUE(errorFalse) << "Fehler beim Lesen der nicht vorhandenen Variable: " << errorCodeFalse;
        EXPECT_EQ(varFalse, 0) << "Unerwarteter Wert für nicht vorhandene Variable: " << varFalse;

    } catch (const std::exception& e) {
        FAIL() << "Exception aufgetreten: " << e.what();
    } catch (...) {
        FAIL() << "Unbekannter Fehler (nicht-catchbare Exception)";
    }
}
*/