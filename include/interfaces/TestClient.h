#pragma once

#include "ISPSClient.h"
#include <memory>
#include <string>
#include <tuple>
#include <optional>
#include "VariablenInfo.h"
class TestClient : public ISPSClient
{
public:
    TestClient(const std::string &connectionString)
    {
        connectionString_ = connectionString;
    }

    bool connect() override
    {
        if (connectionString_.empty())
        {
            return false;
        }
        return true;
    }

    bool disconnect() override
    {
        return true;
    }

    std::tuple<bool, std::optional<long>> registerVariable(const std::string &name, DataType type, const std::string connectionString) override
    {
        return {true, 0}; // Dummy-Handle zurückgeben
    }

    bool isNamespaceUriPresent(const std::string &namespaceUri) override
    {
        return true;
    }

    void browseAddressSpace() override
    {
    }

    std::tuple<int16_t, bool, int> readInt16(const VariableInfo &var) override
    {
        return {42, false, 0};
    }

    std::tuple<int32_t, bool, int> readInt32(const VariableInfo &var) override
    {
        return {424242, false, 0};
    }

    std::tuple<float, bool, int> readFloat(const VariableInfo &var) override
    {
        return {3.14f, false, 0};
    }

    std::tuple<double, bool, int> readDouble(const VariableInfo &var) override
    {
        return {6.28, false, 0};
    }

    std::tuple<bool, bool, int> readBool(const VariableInfo &var) override
    {
        return {true, false, 0};
    }

    std::tuple<std::string, bool, int> readString(const VariableInfo &var) override
    {
        return {"TestString", false, 0};
    }

    std::tuple<std::string, bool, int> readChars(const VariableInfo &var, int length) override
    {
        return {std::string(length, 'A'), false, 0};
    }

    std::tuple<bool, int> writeInt16(const VariableInfo &var, int16_t value) override
    {
        return {false, 0};
    }

    std::tuple<bool, int> writeInt32(const VariableInfo &var, int32_t value) override
    {
        return {false, 0};
    }

    std::tuple<bool, int> writeFloat(const VariableInfo &var, float value) override
    {
        return {false, 0};
    }

    std::tuple<bool, int> writeDouble(const VariableInfo &var, double value) override
    {
        return {false, 0};
    }

    std::tuple<bool, int> writeBool(const VariableInfo &var, bool value) override
    {
        return {false, 0};
    }

    std::tuple<bool, int> writeChars(const VariableInfo &var, const std::string &value, int length) override
    {
        return {false, 0};
    }
    std::tuple<std::wstring, bool, int> readWString(const VariableInfo &var) override
    {
        return {L"TestWString", false, 0};
    }
    std::tuple<bool, int> writeWString(const VariableInfo &var, const std::wstring &value) override
    {
        return {false, 0};
    }
    void printNamespaceArray() override
    {
        std::cout << "Namespace Array: [TestNamespace]" << std::endl;
    }

    std::tuple<std::vector<std::vector<int16_t>>, bool, int>
    readInt16Array(const VariableInfo &var, size_t startRow = 0,
                   size_t rowCount = SIZE_MAX,
                   size_t startCol = 0, size_t colCount = SIZE_MAX) override
    {
        return {{}, false, 0}; // Dummy-Array zurückgeben
    }
    std::tuple<std::vector<std::vector<int32_t>>, bool, int>
    readInt32Array(const VariableInfo &var, size_t startRow = 0,

                   size_t rowCount = SIZE_MAX,
                   size_t startCol = 0, size_t colCount = SIZE_MAX) override
    {
        return {{}, false, 0}; // Dummy-Array zurückgeben
    }

    std::tuple<std::vector<std::vector<float>>, bool, int>
    readFloatArray(const VariableInfo &var, size_t startRow = 0,
                   size_t rowCount = SIZE_MAX,
                   size_t startCol = 0, size_t colCount = SIZE_MAX) override
    {
        return {{}, false, 0}; // Dummy-Array zurückgeben
    }
    std::tuple<std::vector<std::vector<double>>, bool, int>
    readDoubleArray(const VariableInfo &var, size_t startRow = 0,
                    size_t rowCount = SIZE_MAX,
                    size_t startCol = 0, size_t colCount = SIZE_MAX) override
    {
        return {{}, false, 0}; // Dummy-Array zurückgeben
    }
    std::tuple<std::vector<std::vector<bool>>, bool, int>
    readBoolArray(const VariableInfo &var, size_t startRow = 0,
                  size_t rowCount = SIZE_MAX,
                  size_t startCol = 0, size_t colCount = SIZE_MAX) override
    {
        return {{}, false, 0}; // Dummy-Array zurückgeben
    }
    std::tuple<std::vector<std::vector<std::string>>, bool, int>
    readStringArray(const VariableInfo &var, size_t startRow = 0,
                    size_t rowCount = SIZE_MAX,
                    size_t startCol = 0, size_t colCount = SIZE_MAX) override
    {
        return {{}, false, 0}; // Dummy-Array zurückgeben
    }   
    std::tuple<bool, int> writeInt16Array(const VariableInfo &var,
                                           const std::vector<std::vector<int16_t>> &data,
                                           size_t startRow = 0, size_t startCol = 0) override
    {
        return {false, 0}; // Dummy-Erfolg zurückgeben
    }
    std::tuple<bool, int> writeInt32Array(const VariableInfo &var,
                                           const std::vector<std::vector<int32_t>> &data,
                                           size_t startRow = 0, size_t startCol = 0) override
    {
        return {false, 0}; // Dummy-Erfolg zurückgeben
    }
    std::tuple<bool, int> writeFloatArray(const VariableInfo &var,
                                           const std::vector<std::vector<float>> &data,
                                           size_t startRow = 0, size_t startCol = 0) override
    {
        return {false, 0}; // Dummy-Erfolg zurückgeben
    }
    std::tuple<bool, int> writeDoubleArray(const VariableInfo &var,
                                            const std::vector<std::vector<double>> &data,
                                            size_t startRow = 0, size_t startCol = 0) override
    {
        return {false, 0}; // Dummy-Erfolg zurückgeben
    }
    std::tuple<bool, int> writeBoolArray(const VariableInfo &var,
                                          const std::vector<std::vector<bool>> &data,
                                          size_t startRow = 0, size_t startCol = 0) override
    {
        return {false, 0}; // Dummy-Erfolg zurückgeben
    }
    std::tuple<bool, int> writeStringArray(const VariableInfo &var,
                                            const std::vector<std::vector<std::string>> &data,
                                            size_t startRow = 0, size_t startCol = 0) override
    {
        return {false, 0}; // Dummy-Erfolg zurückgeben
    }


    
    std::tuple<std::vector<std::vector<std::wstring>>, bool, int>
    readWStringArray(const VariableInfo &var, size_t startRow = 0,
                     size_t rowCount = SIZE_MAX,
                     size_t startCol = 0, size_t colCount = SIZE_MAX) override
    {
        return {{}, false, 0}; // Dummy-Array zurückgeben
    }
    std::tuple<bool, int> writeWStringArray(const VariableInfo &var,
                                             const std::vector<std::vector<std::wstring>> &data,
                                             size_t startRow = 0, size_t startCol = 0) override
    {
        return {false, 0}; // Dummy-Erfolg zurückgeben
    }

    
private:
    std::string connectionString_;
};
