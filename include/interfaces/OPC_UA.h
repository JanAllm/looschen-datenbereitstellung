#pragma once
#include <interfaces/ISPSClient.h>

// open62541 Amalgamation Header
#ifdef __cplusplus
extern "C" {
#endif
#include <open62541.h>
#ifdef __cplusplus
}
#endif

#include <string>
#include <iostream>
#include <vector>
#include <tuple>
#include "VariablenInfo.h"
#include <algorithm>

class OPCClient : public ISPSClient{
public:
    OPCClient(const std::string& serverURL);
    ~OPCClient();
    bool connect()override;
    bool disconnect()override;
    std::tuple<bool, std::optional<long>> registerVariable(const std::string &name, const DataType type, const std::string connectionString) override;
    std::tuple<int16_t, bool, int> readInt16(const VariableInfo &var) override;
    std::tuple<std::string, bool, int> readString(const VariableInfo& var) override;
    std::tuple<bool, bool, int> readBool(const VariableInfo& var) override;
    std::tuple<int32_t, bool, int> readInt32(const VariableInfo& var) override;
    std::tuple<double, bool, int> readDouble(const VariableInfo& var) override;
    std::tuple<std::string, bool, int> readChars(const VariableInfo& var, int length) override;
    std::tuple<float, bool, int> readFloat(const VariableInfo& var) override;
    std::tuple<std::wstring, bool, int> readWString(const VariableInfo& var) override;
    

    std::tuple<std::vector<std::vector<int16_t>>, bool, int> readInt16Array(const VariableInfo& var, size_t startRow = 0, size_t rowCount = SIZE_MAX, size_t startCol = 0, size_t colCount = SIZE_MAX) override;
    std::tuple<std::vector<std::vector<int32_t>>, bool, int> readInt32Array(const VariableInfo& var, size_t startRow = 0, size_t rowCount = SIZE_MAX, size_t startCol = 0, size_t colCount = SIZE_MAX) override;
    std::tuple<std::vector<std::vector<float>>, bool, int> readFloatArray(const VariableInfo& var, size_t startRow = 0, size_t rowCount = SIZE_MAX, size_t startCol = 0, size_t colCount = SIZE_MAX) override;
    std::tuple<std::vector<std::vector<double>>, bool, int> readDoubleArray(const VariableInfo& var, size_t startRow = 0, size_t rowCount = SIZE_MAX, size_t startCol = 0, size_t colCount = SIZE_MAX) override;
    std::tuple<std::vector<std::vector<bool>>, bool, int> readBoolArray(const VariableInfo& var, size_t startRow = 0, size_t rowCount = SIZE_MAX, size_t startCol = 0, size_t colCount = SIZE_MAX) override;
    std::tuple<std::vector<std::vector<std::string>>, bool, int> readStringArray(const VariableInfo& var, size_t startRow = 0, size_t rowCount = SIZE_MAX, size_t startCol = 0, size_t colCount = SIZE_MAX) override;
    std::tuple<std::vector<std::vector<std::wstring>>, bool, int> readWStringArray(const VariableInfo& var, size_t startRow = 0, size_t rowCount = SIZE_MAX, size_t startCol = 0, size_t colCount = SIZE_MAX) override;

    //  std::tuple<bool, int> writeString(const   std::string& nodeId, const   std::string& valueToWrite)override ;
    std::tuple<bool, int> writeBool(const VariableInfo& var, bool valueToWrite) override;
    std::tuple<bool, int> writeInt32(const VariableInfo& var, int32_t valueToWrite) override;
    std::tuple<bool, int> writeDouble(const VariableInfo& var, double valueToWrite) override;
    std::tuple<bool, int> writeChars(const VariableInfo& var, const std::string &valueToWrite, const int length) override;
    std::tuple<bool, int> writeInt16(const VariableInfo& var, int16_t valueToWrite) override;
    std::tuple<bool, int> writeFloat(const VariableInfo& var, float valueToWrite) override;
    std::tuple<bool, int> writeWString(const VariableInfo& var, const std::wstring &valueToWrite) override;

    std::tuple<bool, int> writeInt16Array(const VariableInfo& var, const std::vector<std::vector<int16_t>>& data, size_t startRow = 0, size_t startCol = 0) override;
    std::tuple<bool, int> writeInt32Array(const VariableInfo& var, const std::vector<std::vector<int32_t>>& data, size_t startRow = 0, size_t startCol = 0) override;
    std::tuple<bool, int> writeFloatArray(const VariableInfo& var, const std::vector<std::vector<float>>& data, size_t startRow = 0, size_t startCol = 0) override;
    std::tuple<bool, int> writeDoubleArray(const VariableInfo& var, const std::vector<std::vector<double>>& data, size_t startRow = 0, size_t startCol = 0) override;
    std::tuple<bool, int> writeBoolArray(const VariableInfo& var, const std::vector<std::vector<bool>>& data, size_t startRow = 0, size_t startCol = 0) override;
    std::tuple<bool, int> writeStringArray(const VariableInfo& var, const std::vector<std::vector<std::string>>& data, size_t startRow = 0, size_t startCol = 0) override;
    std::tuple<bool, int> writeWStringArray(const VariableInfo& var, const std::vector<std::vector<std::wstring>>& data, size_t startRow = 0, size_t startCol = 0) override;
    
    void browseAddressSpace();
    bool isNamespaceUriPresent(const std::string &namespaceUri)override;
    bool readAccessLevel(const std::string &nodeIdStr, bool &readable, bool &writable) override;
    bool isConnected() override;
    //void browseNamespaceNodes(const UA_NodeId& nodeId, uint16_t nsIndex, int indent = 0) override;
    void printNamespaceArray();
   

private:
    std::string serverURL;
    UA_Client* client;
    void browseRecursive(const UA_NodeId& nodeId, int indent);
    UA_NodeId parseNodeId(const std::string &nodeIdStr);
};