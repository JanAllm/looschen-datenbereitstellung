#pragma once
#include <iostream>
#include <string>
#include <vector>
#include <chrono>
#include <tuple>
#include <optional>
#include "VariablenInfo.h"

class ISPSClient
{
public:
    virtual ~ISPSClient() = default;
    // Verbindung
    virtual bool connect() = 0;
    virtual bool disconnect() = 0;
    // Namespace-Handling
    virtual std::tuple<bool, std::optional<long>> registerVariable(const std::string &name, const DataType type, const std::string connectionString) = 0;
    virtual bool isNamespaceUriPresent(const std::string &namespaceUri) = 0;
    virtual void browseAddressSpace() = 0;

    /// Besteht eine aktive Sitzung? Default: true (Clients ohne Sitzungsbegriff).
    virtual bool isConnected() { return true; }

    /// Liest das AccessLevel-Attribut eines Knotens (kein Schreibzugriff!).
    /// Rueckgabe: true, wenn der Knoten existiert. Default: nicht unterstuetzt.
    virtual bool readAccessLevel(const std::string & /*nodeId*/,
                                 bool &readable, bool &writable)
    {
        readable = false;
        writable = false;
        return false;
    }
    // virtual void browseNamespaceNodes(const UA_NodeId &nodeId, uint16_t nsIndex, int indent = 0) = 0;
    //  Generischer Zugriff
    //  Gelesene Werte (typisiert)
    //  Gelesene Werte (typisiert) anhand von VariableInfo
    virtual std::tuple<int16_t, bool, int> readInt16(const VariableInfo &var) = 0;
    virtual std::tuple<int32_t, bool, int> readInt32(const VariableInfo &var) = 0;
    virtual std::tuple<float, bool, int> readFloat(const VariableInfo &var) = 0;
    virtual std::tuple<double, bool, int> readDouble(const VariableInfo &var) = 0;
    virtual std::tuple<bool, bool, int> readBool(const VariableInfo &var) = 0;
    virtual std::tuple<std::string, bool, int> readString(const VariableInfo &var) = 0;
    virtual std::tuple<std::string, bool, int> readChars(const VariableInfo &var, int length) = 0;
    virtual std::tuple<std::wstring, bool, int> readWString(const VariableInfo &var) = 0;

    virtual std::tuple<std::vector<std::vector<int16_t>>, bool, int>
    readInt16Array(const VariableInfo &var,
                   size_t startRow = 0, size_t rowCount = SIZE_MAX,
                   size_t startCol = 0, size_t colCount = SIZE_MAX) = 0;

    virtual std::tuple<std::vector<std::vector<int32_t>>, bool, int>
    readInt32Array(const VariableInfo &var,
                   size_t startRow = 0, size_t rowCount = SIZE_MAX,
                   size_t startCol = 0, size_t colCount = SIZE_MAX) = 0;

    virtual std::tuple<std::vector<std::vector<float>>, bool, int>
    readFloatArray(const VariableInfo &var,
                   size_t startRow = 0, size_t rowCount = SIZE_MAX,
                   size_t startCol = 0, size_t colCount = SIZE_MAX) = 0;

    virtual std::tuple<std::vector<std::vector<double>>, bool, int>
    readDoubleArray(const VariableInfo &var,
                    size_t startRow = 0, size_t rowCount = SIZE_MAX,
                    size_t startCol = 0, size_t colCount = SIZE_MAX) = 0;

    virtual std::tuple<std::vector<std::vector<bool>>, bool, int>
    readBoolArray(const VariableInfo &var,
                  size_t startRow = 0, size_t rowCount = SIZE_MAX,
                  size_t startCol = 0, size_t colCount = SIZE_MAX) = 0;

    virtual std::tuple<std::vector<std::vector<std::string>>, bool, int>
    readStringArray(const VariableInfo &var,
                    size_t startRow = 0, size_t rowCount = SIZE_MAX,
                    size_t startCol = 0, size_t colCount = SIZE_MAX) = 0;
    virtual std::tuple<std::vector<std::vector<std::wstring>>, bool, int>
    readWStringArray(const VariableInfo &var,
                    size_t startRow = 0, size_t rowCount = SIZE_MAX,
                    size_t startCol = 0, size_t colCount = SIZE_MAX) = 0;
    // Schreiben von Werten
    virtual std::tuple<bool, int> writeInt16(const VariableInfo &var, int16_t value) = 0;
    virtual std::tuple<bool, int> writeInt32(const VariableInfo &var, int32_t value) = 0;
    virtual std::tuple<bool, int> writeFloat(const VariableInfo &var, float value) = 0;
    virtual std::tuple<bool, int> writeDouble(const VariableInfo &var, double value) = 0;
    virtual std::tuple<bool, int> writeBool(const VariableInfo &var, bool value) = 0;
    virtual std::tuple<bool, int> writeChars(const VariableInfo &var, const std::string &value, int length) = 0;
    
    virtual std::tuple<bool, int> writeWString(const VariableInfo &var, const std::wstring &value) = 0;
    virtual void printNamespaceArray() = 0; // Zum Debuggen, gibt die Namespace-Array aus

    virtual std::tuple<bool, int> writeInt16Array(const VariableInfo &var,
                                                  const std::vector<std::vector<int16_t>> &data,
                                                  size_t startRow = 0, size_t startCol = 0) = 0;

    virtual std::tuple<bool, int> writeInt32Array(const VariableInfo &var,
                                                  const std::vector<std::vector<int32_t>> &data,
                                                  size_t startRow = 0, size_t startCol = 0) = 0;

    virtual std::tuple<bool, int> writeFloatArray(const VariableInfo &var,
                                                  const std::vector<std::vector<float>> &data,
                                                  size_t startRow = 0, size_t startCol = 0) = 0;

    virtual std::tuple<bool, int> writeDoubleArray(const VariableInfo &var,
                                                   const std::vector<std::vector<double>> &data,
                                                   size_t startRow = 0, size_t startCol = 0) = 0;

    virtual std::tuple<bool, int> writeBoolArray(const VariableInfo &var,
                                                 const std::vector<std::vector<bool>> &data,
                                                 size_t startRow = 0, size_t startCol = 0) = 0;

    virtual std::tuple<bool, int> writeStringArray(const VariableInfo &var,
                                                   const std::vector<std::vector<std::string>> &data,
                                                   size_t startRow = 0, size_t startCol = 0) = 0;
    virtual std::tuple<bool, int> writeWStringArray(const VariableInfo &var,
                                                    const std::vector<std::vector<std::wstring>> &data,
                                                    size_t startRow = 0, size_t startCol = 0) = 0;
};