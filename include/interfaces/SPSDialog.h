#pragma once
#include <map>
#include <string>
#include <vector>
#include <memory>
#include <variant>
#include <tuple>
#include <optional>
#include <unordered_map>
#include "ISPSClient.h"
#include <iostream>
#include "ErrorLoggerSingleton.h"
#include "VariablenInfo.h"







using SPSValue = std::variant<int16_t, int32_t, float, double, bool, std::string>;

class SPSDialog
{
public:
    SPSDialog(std::shared_ptr<ISPSClient> client);
    ~SPSDialog() = default;

    bool registerVariable(const std::string &name, DataType type, const std::string &connectionString);
    std::tuple<SPSValue, bool, int> readAnyValue(const std::string &name, std::optional<DataType> expectedType = std::nullopt);
    bool writeAnyValue(const std::string &name, const SPSValue &value);

    //std::tuple<bool, int> registerVariable(const std::string &name, const std::string &type, const std::string &connectionString);

    std::tuple<int16_t, bool, int> readInt16(const std::string &name);
    std::tuple<int, bool, int> readInt32(const std::string &name);
    std::tuple<float, bool, int> readFloat(const std::string &name);
    std::tuple<double, bool, int> readDouble(const std::string &name);
    std::tuple<bool, bool, int> readBool(const std::string &name);
    std::tuple<std::string, bool, int> readString(const std::string &name);
    std::tuple<std::string, bool, int> readChars(const std::string &name, int length);
    std::tuple<std::wstring, bool, int> readWString(const std::string &name);

    std::tuple<bool, int> writeInt16(const std::string &name, int16_t valueToWrite);
    std::tuple<bool, int> writeFloat(const std::string &name, float valueToWrite);
    std::tuple<bool, int> writeBool(const std::string &name, bool valueToWrite);
    std::tuple<bool, int> writeInt32(const std::string &name, int32_t valueToWrite);
    std::tuple<bool, int> writeDouble(const std::string &name, double valueToWrite);
    std::tuple<bool, int> writeChars(const std::string &name, const std::string &valueToWrite, int length);
    std::tuple<bool, int> writeString(const std::string &name, const std::wstring &valueToWrite);
    std::tuple<bool, int> writeWString(const std::string &name, const std::wstring &valueToWrite);

    std::tuple<std::vector<std::vector<int16_t>>, bool, int> readInt16Array(const std::string &name, size_t startRow = 0, size_t rowCount = SIZE_MAX, size_t startCol = 0, size_t colCount = SIZE_MAX);
    std::tuple<std::vector<std::vector<int32_t>>, bool, int> readInt32Array(const std::string &name, size_t startRow = 0, size_t rowCount = SIZE_MAX, size_t startCol = 0, size_t colCount = SIZE_MAX); 
    std::tuple<std::vector<std::vector<float>>, bool, int> readFloatArray(const std::string &name, size_t startRow = 0, size_t rowCount = SIZE_MAX, size_t startCol = 0, size_t colCount = SIZE_MAX);
    std::tuple<std::vector<std::vector<double>>, bool, int> readDoubleArray(const std::string &name, size_t startRow = 0, size_t rowCount = SIZE_MAX, size_t startCol = 0, size_t colCount = SIZE_MAX);
    std::tuple<std::vector<std::vector<bool>>, bool, int> readBoolArray(const std::string &name, size_t startRow = 0, size_t rowCount = SIZE_MAX, size_t startCol = 0, size_t colCount = SIZE_MAX);
    std::tuple<std::vector<std::vector<std::string>>, bool, int> readStringArray(const std::string &name, size_t startRow = 0, size_t rowCount = SIZE_MAX, size_t startCol = 0, size_t colCount = SIZE_MAX);
    std::tuple<std::vector<std::vector<std::wstring>>, bool, int> readWStringArray(const std::string &name, size_t startRow = 0, size_t rowCount = SIZE_MAX, size_t startCol = 0, size_t colCount = SIZE_MAX);


    std::tuple<bool, int> writeInt16Array(const std::string &name, const std::vector<std::vector<int16_t>> &valueToWrite, size_t startRow = 0,  size_t startCol = 0);
    std::tuple<bool, int> writeInt32Array(const std::string &name, const std::vector<std::vector<int32_t>> &valueToWrite, size_t startRow = 0,  size_t startCol = 0 );
    std::tuple<bool, int> writeFloatArray(const std::string &name, const std::vector<std::vector<float>> &valueToWrite, size_t startRow = 0, size_t startCol = 0);
    std::tuple<bool, int> writeDoubleArray(const std::string &name, const std::vector<std::vector<double>> &valueToWrite, size_t startRow = 0, size_t startCol = 0);
    std::tuple<bool, int> writeBoolArray(const std::string &name, const std::vector<std::vector<bool>> &valueToWrite, size_t startRow = 0, size_t startCol = 0);
    std::tuple<bool, int> writeStringArray(const std::string &name, const std::vector<std::vector<std::string>> &valueToWrite, size_t startRow = 0, size_t startCol = 0);
    std::tuple<bool, int> writeWStringArray(const std::string &name, const std::vector<std::vector<std::wstring>> &valueToWrite, size_t startRow = 0, size_t startCol = 0);

    void printVariableMap();

    /// Live-Diagnose aller registrierten Knoten (AccessLevel-Attribut).
    std::vector<NodeDiagnostic> nodeDiagnostics();

    /// Prueft einen Knoten direkt (AccessLevel), ohne ihn registrieren zu muessen.
    bool probeNode(const std::string &nodeId, bool &readable, bool &writable);

    /// Aktive OPC-Sitzung vorhanden?
    bool isConnected() { return client && client->isConnected(); }

    void printNamespaceArray();
    void browseAddressSpace();
private:
    std::shared_ptr<ISPSClient> client;
    std::unordered_map<std::string, VariableInfo> variableMap;
};
