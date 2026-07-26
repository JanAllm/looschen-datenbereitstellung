
#pragma once
#include <interfaces/ISPSClient.h>
#include <windows.h>
#include <cstdint>
#include <string>
#include <TcAdsDef.h>
#include <TcAdsAPI.h>
#include <iostream>
#include <sstream>
#include "ErrorLoggerSingleton.h"
#include "data_structs.h"
#include <unordered_map>
#include "VariablenInfo.h"

class TwincatAds : public ISPSClient
{
public:
    TwincatAds(const std::string ipSPS) : ipSPS(ipSPS) {  };
    ~TwincatAds() {};
    bool connect() override;
    bool disconnect() override;
    std::tuple<bool, std::optional<long>> registerVariable(const std::string &name, const DataType type, const std::string connectionString) override;
   // std::tuple<std::string, bool, int> readAnyValue(const std::string &nodeId) override;
    std::tuple<int16_t, bool, int> readInt16(const VariableInfo& var) override;
    std::tuple<std::string, bool, int> readString(const VariableInfo& var) override;
    std::tuple<bool, bool, int> readBool(const VariableInfo& var) override;
    std::tuple<int32_t, bool, int> readInt32(const VariableInfo& var) override;
    std::tuple<double, bool, int> readDouble(const VariableInfo& var) override;
    std::tuple<std::string, bool, int> readChars(const VariableInfo& var, int length) override;
    std::tuple<float, bool, int> readFloat(const VariableInfo& var) override;
    std::tuple<std::wstring, bool, int> readWString(const VariableInfo& var) override;
    std::tuple<std::vector<std::vector<int16_t>>, bool, int>readInt16Array(const VariableInfo& var, size_t startRow = 0, size_t rowCount = SIZE_MAX, size_t startCol = 0, size_t colCount = SIZE_MAX) override;
    std::tuple<std::vector<std::vector<int32_t>>, bool, int>readInt32Array(const VariableInfo& var, size_t startRow = 0, size_t rowCount = SIZE_MAX, size_t startCol = 0, size_t colCount = SIZE_MAX) override;
    std::tuple<std::vector<std::vector<float>>, bool, int>readFloatArray(const VariableInfo& var, size_t startRow = 0, size_t rowCount = SIZE_MAX, size_t startCol = 0, size_t colCount = SIZE_MAX) override;
    std::tuple<std::vector<std::vector<double>>, bool, int>readDoubleArray(const VariableInfo& var, size_t startRow = 0, size_t rowCount = SIZE_MAX, size_t startCol = 0, size_t colCount = SIZE_MAX) override;
    std::tuple<std::vector<std::vector<bool>>, bool, int>readBoolArray(const VariableInfo& var, size_t startRow = 0, size_t rowCount = SIZE_MAX, size_t startCol = 0, size_t colCount = SIZE_MAX) override;
    std::tuple<std::vector<std::vector<std::string>>, bool, int>readStringArray(const VariableInfo& var, size_t startRow = 0, size_t rowCount = SIZE_MAX, size_t startCol = 0, size_t colCount = SIZE_MAX) override;
    std::tuple<std::vector<std::vector<std::wstring>>, bool, int>readWStringArray(const VariableInfo& var, size_t startRow = 0, size_t rowCount = SIZE_MAX, size_t startCol = 0, size_t colCount = SIZE_MAX) override;
    
    std::tuple<bool, int> writeInt16(const VariableInfo& var, int16_t valueToWrite) override;
    std::tuple<bool, int> writeFloat(const VariableInfo& var, float valueToWrite) override;
    //  std::tuple<bool, int> writeString(const   std::string& nodeId, const   std::string& valueToWrite)override ;
    std::tuple<bool, int> writeBool(const VariableInfo& var, bool valueToWrite) override;
    std::tuple<bool, int> writeInt32(const VariableInfo& var, int32_t valueToWrite) override;
    std::tuple<bool, int> writeDouble(const VariableInfo& var, double valueToWrite) override;
    std::tuple<bool, int> writeChars(const VariableInfo& var, const std::string &valueToWrite, const int length) override;
    std::tuple<bool, int> writeWString(const VariableInfo& var, const std::wstring &valueToWrite) override;
    
    std::tuple<bool, int> writeInt16Array(const VariableInfo& var,const std::vector<std::vector<int16_t>>& data, size_t startRow = 0, size_t startCol = 0) override;
    std::tuple<bool, int> writeInt32Array(const VariableInfo& var, const std::vector<std::vector<int32_t>>& data, size_t startRow = 0, size_t startCol = 0) override;
    std::tuple<bool, int> writeFloatArray(const VariableInfo& var, const std::vector<std::vector<float>>& data, size_t startRow = 0, size_t startCol = 0) override;
    std::tuple<bool, int> writeDoubleArray(const VariableInfo& var, const std::vector<std::vector<double>>& data, size_t startRow = 0, size_t startCol = 0) override;
    std::tuple<bool, int> writeBoolArray(const VariableInfo& var, const std::vector<std::vector<bool>>& data, size_t startRow = 0, size_t startCol = 0) override;
    std::tuple<bool, int> writeStringArray(const VariableInfo& var, const std::vector<std::vector<std::string>>& data, size_t startRow = 0, size_t startCol = 0) override;
    std::tuple<bool, int> writeWStringArray(const VariableInfo& var, const std::vector<std::vector<std::wstring>>& data, size_t startRow = 0, size_t startCol = 0) override;
    
    bool isNamespaceUriPresent(const std::string &namespaceUri) override { return false; };
    void browseAddressSpace() override { std::cout << "browseAddressSpace" << std::endl; };
    // Generischer Zugriff soll gelüscht werden
    void ipToAmsNetId(const std::string &ip, uint8_t *netId);
    bool writeLabelIO(long &handle, LabelIO valueToWrite);
    bool checkConnection();
    void printNamespaceArray() override {
        std::cout << "TwincatAds: printNamespaceArray" << std::endl;
    }
private:
    bool createVariableHandles(const std::string &nodeId);
    long getVariableHandles(const std::string &nodeId);
    uint8_t remoteNetId[6];
    AmsAddr addr;
    const std::string ipSPS;
    std::unordered_map<std::string, long> handleCache;
};
