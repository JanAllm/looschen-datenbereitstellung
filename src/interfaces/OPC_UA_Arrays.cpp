#include <cstring>  // für std::memcpy
#include "interfaces/OPC_UA.h"

//  ======================Array Lese Bool ARRAY ======================
//
//
std::tuple<std::vector<std::vector<bool>>, bool, int>
OPCClient::readBoolArray(const VariableInfo &var,
                         size_t startRow, size_t rowCount,
                         size_t startCol, size_t colCount)
{
    UA_Variant variant;
    UA_Variant_init(&variant);

    UA_NodeId node = parseNodeId(var.fullName);
    if (UA_NodeId_equal(&node, &UA_NODEID_NULL))
    {
        return {{}, false, 1};
    }

    UA_StatusCode status = UA_Client_readValueAttribute(client, node, &variant);
    if (status != UA_STATUSCODE_GOOD)
    {
        return {{}, false, 2};
    }

    if (variant.arrayLength == 0 || variant.data == nullptr)
    {
        return {{}, false, 3};
    }
    UA_Boolean *dataArray = static_cast<UA_Boolean *>(variant.data);
    size_t totalElements = variant.arrayLength;

    size_t dim1 = 1, dim2 = totalElements;
    if (variant.arrayDimensionsSize == 0)
    {
        dim1 = 1;
        dim2 = totalElements;
    }
    else if (variant.arrayDimensionsSize == 1)
    {
        dim1 = 1;
        dim2 = variant.arrayDimensions[0];
    }
    else if (variant.arrayDimensionsSize == 2)
    {
        dim1 = variant.arrayDimensions[0];
        dim2 = variant.arrayDimensions[1];
    }
    else
    {
        UA_Variant_clear(&variant);
        return {{}, false, 4};
    }

    if (startRow >= dim1 || startCol >= dim2)
    {
        UA_Variant_clear(&variant);
        return {{}, false, 5};
    }

    size_t endRow = (std::min)(dim1, startRow + rowCount);
    size_t endCol = (std::min)(dim2, startCol + colCount);

    std::vector<std::vector<bool>> result;

    for (size_t row = startRow; row < endRow; ++row)
    {
        std::vector<bool> rowData;
        for (size_t col = startCol; col < endCol; ++col)
        {
            size_t index = row * dim2 + col;
            if (index >= totalElements)
                break;
            rowData.push_back(dataArray[index]);
        }
        result.push_back(std::move(rowData));
    }

    UA_Variant_clear(&variant);
    return {result, true, 0};
}

// ======================Array Lese String ARRAY ======================
//
//
std::tuple<std::vector<std::vector<std::string>>, bool, int>
OPCClient::readStringArray(const VariableInfo &var,
                           size_t startRow, size_t rowCount,
                           size_t startCol, size_t colCount)
{
    UA_Variant variant;
    UA_Variant_init(&variant);

    UA_NodeId node = parseNodeId(var.fullName);
    if (UA_NodeId_equal(&node, &UA_NODEID_NULL))
    {
        return {{}, false, 1};
    }

    UA_StatusCode status = UA_Client_readValueAttribute(client, node, &variant);
    if (status != UA_STATUSCODE_GOOD)
    {
        return {{}, false, 2};
    }

    if (variant.arrayLength == 0 || variant.data == nullptr)
    {
        return {{}, false, 3};
    }
    UA_String *stringArray = static_cast<UA_String *>(variant.data);
    size_t totalElements = variant.arrayLength;

    // Optional: Dimension bestimmen
    size_t dim1 = 1, dim2 = totalElements;
    if (variant.arrayDimensionsSize == 0)
    {
        dim1 = 1;
        dim2 = totalElements;
    }
    else if (variant.arrayDimensionsSize == 1)
    {
        dim1 = 1;
        dim2 = variant.arrayDimensions[0];
    }
    else if (variant.arrayDimensionsSize == 2)
    {
        dim1 = variant.arrayDimensions[0];
        dim2 = variant.arrayDimensions[1];
    }
    else
    {
        UA_Variant_clear(&variant);
        return {{}, false, 4};
    }
    // Gültigkeit der Anfrage prüfen
    if (startRow >= dim1 || startCol >= dim2)
    {
        UA_Variant_clear(&variant);
        return {{}, false, 5};
    }

    if (startRow >= dim1)
        startRow = dim1;
    if (startCol >= dim2)
        startCol = dim2;

    size_t endRow = (std::min)(dim1, startRow + rowCount);
    size_t endCol = (std::min)(dim2, startCol + colCount);

    std::vector<std::vector<std::string>> result;

    for (size_t row = startRow; row < endRow; ++row)
    {
        std::vector<std::string> rowData;
        for (size_t col = startCol; col < endCol; ++col)
        {
            size_t index = row * dim2 + col;
            if (index >= totalElements)
                break;

            const UA_String &uaStr = stringArray[index];
            std::string str;
            if (uaStr.data && uaStr.length > 0)
            {
                str.assign(reinterpret_cast<const char *>(uaStr.data), uaStr.length);
            }
            else
            {
                str.assign("");
            }
            rowData.push_back(str);
        }
        result.push_back(std::move(rowData));
    }

    UA_Variant_clear(&variant);
    return {result, true, 0};
}
// ======================Array Lese Int ARRAY ======================
//
//
std::tuple<std::vector<std::vector<int16_t>>, bool, int>
OPCClient::readInt16Array(const VariableInfo &var,
                          size_t startRow, size_t rowCount,
                          size_t startCol, size_t colCount)
{
    UA_Variant variant;
    UA_Variant_init(&variant);

    UA_NodeId node = parseNodeId(var.fullName);
    if (UA_NodeId_equal(&node, &UA_NODEID_NULL))
    {
        return {{}, false, 1};
    }

    UA_StatusCode status = UA_Client_readValueAttribute(client, node, &variant);
    if (status != UA_STATUSCODE_GOOD)
    {
        return {{}, false, 2};
    }

    if (variant.arrayLength == 0 || variant.data == nullptr)
    {
        return {{}, false, 3};
    }
    int16_t *dataArray = static_cast<int16_t *>(variant.data);
    size_t totalElements = variant.arrayLength;

    size_t dim1 = 1, dim2 = totalElements;
    if (variant.arrayDimensionsSize == 0)
    {
        dim1 = 1;
        dim2 = totalElements;
    }
    else if (variant.arrayDimensionsSize == 1)
    {
        dim1 = 1;
        dim2 = variant.arrayDimensions[0];
    }
    else if (variant.arrayDimensionsSize == 2)
    {
        dim1 = variant.arrayDimensions[0];
        dim2 = variant.arrayDimensions[1];
    }
    else
    {
        UA_Variant_clear(&variant);
        return {{}, false, 4};
    }

    if (startRow >= dim1 || startCol >= dim2)
    {
        UA_Variant_clear(&variant);
        return {{}, false, 5};
    }

    size_t endRow = (std::min)(dim1, startRow + rowCount);
    size_t endCol = (std::min)(dim2, startCol + colCount);

    std::vector<std::vector<int16_t>> result;

    for (size_t row = startRow; row < endRow; ++row)
    {
        std::vector<int16_t> rowData;
        for (size_t col = startCol; col < endCol; ++col)
        {
            size_t index = row * dim2 + col;
            if (index >= totalElements)
                break;
            rowData.push_back(dataArray[index]);
        }
        result.push_back(std::move(rowData));
    }

    UA_Variant_clear(&variant);
    return {result, true, 0};
}

// ======================Array Lese Int32 ARRAY ======================
//
//
std::tuple<std::vector<std::vector<int32_t>>, bool, int>
OPCClient::readInt32Array(const VariableInfo &var,
                          size_t startRow, size_t rowCount,
                          size_t startCol, size_t colCount)
{
    UA_Variant variant;
    UA_Variant_init(&variant);

    UA_NodeId node = parseNodeId(var.fullName);
    if (UA_NodeId_equal(&node, &UA_NODEID_NULL))
    {
        return {{}, false, 1};
    }

    UA_StatusCode status = UA_Client_readValueAttribute(client, node, &variant);
    if (status != UA_STATUSCODE_GOOD)
    {
        return {{}, false, 2};
    }

    if (variant.arrayLength == 0 || variant.data == nullptr)
    {
        return {{}, false, 3};
    }
    int32_t *dataArray = static_cast<int32_t *>(variant.data);
    size_t totalElements = variant.arrayLength;

    size_t dim1 = 1, dim2 = totalElements;
    if (variant.arrayDimensionsSize == 0)
    {
        dim1 = 1;
        dim2 = totalElements;
    }
    else if (variant.arrayDimensionsSize == 1)
    {
        dim1 = 1;
        dim2 = variant.arrayDimensions[0];
    }
    else if (variant.arrayDimensionsSize == 2)
    {
        dim1 = variant.arrayDimensions[0];
        dim2 = variant.arrayDimensions[1];
    }
    else
    {
        UA_Variant_clear(&variant);
        return {{}, false, 4};
    }

    if (startRow >= dim1 || startCol >= dim2)
    {
        UA_Variant_clear(&variant);
        return {{}, false, 5};
    }

    size_t endRow = (std::min)(dim1, startRow + rowCount);
    size_t endCol = (std::min)(dim2, startCol + colCount);

    std::vector<std::vector<int32_t>> result;

    for (size_t row = startRow; row < endRow; ++row)
    {
        std::vector<int32_t> rowData;
        for (size_t col = startCol; col < endCol; ++col)
        {
            size_t index = row * dim2 + col;
            if (index >= totalElements)
                break;
            rowData.push_back(dataArray[index]);
        }
        result.push_back(std::move(rowData));
    }

    UA_Variant_clear(&variant);
    return {result, true, 0};
}
// ======================Array Lese Float ARRAY ======================
//
//
std::tuple<std::vector<std::vector<float>>, bool, int>
OPCClient::readFloatArray(const VariableInfo &var,
                          size_t startRow, size_t rowCount,
                          size_t startCol, size_t colCount)
{
    UA_Variant variant;
    UA_Variant_init(&variant);

    UA_NodeId node = parseNodeId(var.fullName);
    if (UA_NodeId_equal(&node, &UA_NODEID_NULL))
    {
        return {{}, false, 1};
    }

    UA_StatusCode status = UA_Client_readValueAttribute(client, node, &variant);
    if (status != UA_STATUSCODE_GOOD)
    {
        return {{}, false, 2};
    }

    if (variant.arrayLength == 0 || variant.data == nullptr)
    {
        return {{}, false, 3};
    }
    float *dataArray = static_cast<float *>(variant.data);
    size_t totalElements = variant.arrayLength;

    size_t dim1 = 1, dim2 = totalElements;
    if (variant.arrayDimensionsSize == 0)
    {
        dim1 = 1;
        dim2 = totalElements;
    }
    else if (variant.arrayDimensionsSize == 1)
    {
        dim1 = 1;
        dim2 = variant.arrayDimensions[0];
    }
    else if (variant.arrayDimensionsSize == 2)
    {
        dim1 = variant.arrayDimensions[0];
        dim2 = variant.arrayDimensions[1];
    }
    else
    {
        UA_Variant_clear(&variant);
        return {{}, false, 4};
    }

    if (startRow >= dim1 || startCol >= dim2)
    {
        UA_Variant_clear(&variant);
        return {{}, false, 5};
    }

    size_t endRow = (std::min)(dim1, startRow + rowCount);
    size_t endCol = (std::min)(dim2, startCol + colCount);

    std::vector<std::vector<float>> result;

    for (size_t row = startRow; row < endRow; ++row)
    {
        std::vector<float> rowData;
        for (size_t col = startCol; col < endCol; ++col)
        {
            size_t index = row * dim2 + col;
            if (index >= totalElements)
                break;
            rowData.push_back(dataArray[index]);
        }
        result.push_back(std::move(rowData));
    }

    UA_Variant_clear(&variant);
    return {result, true, 0};
}
// ======================Array Lese Double ARRAY ======================
//
//
std::tuple<std::vector<std::vector<double>>, bool, int>
OPCClient::readDoubleArray(const VariableInfo &var,
                           size_t startRow, size_t rowCount,
                           size_t startCol, size_t colCount)
{
    UA_Variant variant;
    UA_Variant_init(&variant);

    UA_NodeId node = parseNodeId(var.fullName);
    if (UA_NodeId_equal(&node, &UA_NODEID_NULL))
    {
        return {{}, false, 1};
    }

    UA_StatusCode status = UA_Client_readValueAttribute(client, node, &variant);
    if (status != UA_STATUSCODE_GOOD)
    {
        return {{}, false, 2};
    }

    if (variant.arrayLength == 0 || variant.data == nullptr)
    {
        return {{}, false, 3};
    }
    double *dataArray = static_cast<double *>(variant.data);
    size_t totalElements = variant.arrayLength;

    size_t dim1 = 1, dim2 = totalElements;
    if (variant.arrayDimensionsSize == 0)
    {
        dim1 = 1;
        dim2 = totalElements;
    }
    else if (variant.arrayDimensionsSize == 1)
    {
        dim1 = 1;
        dim2 = variant.arrayDimensions[0];
    }
    else if (variant.arrayDimensionsSize == 2)
    {
        dim1 = variant.arrayDimensions[0];
        dim2 = variant.arrayDimensions[1];
    }
    else
    {
        UA_Variant_clear(&variant);
        return {{}, false, 4};
    }

    if (startRow >= dim1 || startCol >= dim2)
    {
        UA_Variant_clear(&variant);
        return {{}, false, 5};
    }

    size_t endRow = (std::min)(dim1, startRow + rowCount);
    size_t endCol = (std::min)(dim2, startCol + colCount);

    std::vector<std::vector<double>> result;

    for (size_t row = startRow; row < endRow; ++row)
    {
        std::vector<double> rowData;
        for (size_t col = startCol; col < endCol; ++col)
        {
            size_t index = row * dim2 + col;
            if (index >= totalElements)
                break;
            rowData.push_back(dataArray[index]);
        }
        result.push_back(std::move(rowData));
    }

    UA_Variant_clear(&variant);
    return {result, true, 0};
}

std::tuple<std::vector<std::vector<std::wstring>>, bool, int>
OPCClient::readWStringArray(const VariableInfo &var,
                            size_t startRow, size_t rowCount,
                            size_t startCol, size_t colCount)
{
    UA_Variant variant;
    UA_Variant_init(&variant);

    UA_NodeId node = parseNodeId(var.fullName);
    if (UA_NodeId_equal(&node, &UA_NODEID_NULL))
    {
        return {{}, false, 1};
    }

    UA_StatusCode status = UA_Client_readValueAttribute(client, node, &variant);
    if (status != UA_STATUSCODE_GOOD)
    {
        return {{}, false, 2};
    }

    if (variant.arrayLength == 0 || variant.data == nullptr)
    {
        return {{}, false, 3};
    }
    UA_ByteString *stringArray = static_cast<UA_ByteString *>(variant.data);
    size_t totalElements = variant.arrayLength;

    // Optional: Dimension bestimmen
    size_t dim1 = 1, dim2 = totalElements;
    if (variant.arrayDimensionsSize == 0)
    {
        dim1 = 1;
        dim2 = totalElements;
    }
    else if (variant.arrayDimensionsSize == 1)
    {
        dim1 = 1;
        dim2 = variant.arrayDimensions[0];
    }
    else if (variant.arrayDimensionsSize == 2)
    {
        dim1 = variant.arrayDimensions[0];
        dim2 = variant.arrayDimensions[1];
    }
    else
    {
        UA_Variant_clear(&variant);
        return {{}, false, 4};
    }
    // Gültigkeit der Anfrage prüfen
    if (startRow >= dim1 || startCol >= dim2)
    {
        UA_Variant_clear(&variant);
        return {{}, false, 5};
    }

    if (startRow >= dim1)
        startRow = dim1;
    if (startCol >= dim2)
        startCol = dim2;

    size_t endRow = (std::min)(dim1, startRow + rowCount);
    size_t endCol = (std::min)(dim2, startCol + colCount);

    std::vector<std::vector<std::wstring>> result;

    for (size_t row = startRow; row < endRow; ++row)
    {
        std::vector<std::wstring> rowData;
        for (size_t col = startCol; col < endCol; ++col)
        {
            size_t index = row * dim2 + col;
            if (index >= totalElements)
                break;

            const UA_ByteString &bs = stringArray[index];
            std::wstring str;
            if (bs.data && bs.length > 0)
            {
                size_t wcharCount = bs.length / sizeof(wchar_t);
                const wchar_t *wptr = reinterpret_cast<const wchar_t *>(bs.data);
                str.assign(wptr, wptr + wcharCount);
            }
            else
            {
                str.clear();
            }
            rowData.push_back(str);
        }
        result.push_back(std::move(rowData));
    }

    UA_Variant_clear(&variant);
    return {result, true, 0};
}

//  ======================Array Schreiben ARRAY ======================
//
//
// ===== Write Int16 Array =====
//
//
std::tuple<bool, int> OPCClient::writeInt16Array(const VariableInfo &var, const std::vector<std::vector<int16_t>> &data, size_t startRow, size_t startCol)
{
    UA_NodeId node = parseNodeId(var.fullName);
    if (UA_NodeId_equal(&node, &UA_NODEID_NULL))
        return {false, 1};

    UA_Variant variant;
    UA_Variant_init(&variant);
    UA_StatusCode status = UA_Client_readValueAttribute(client, node, &variant);
    if (status != UA_STATUSCODE_GOOD || variant.type != &UA_TYPES[UA_TYPES_INT16] || variant.arrayLength == 0)
        return {false, 2};

    size_t dim1 = 1, dim2 = variant.arrayLength;
    if (variant.arrayDimensionsSize == 2)
    {
        dim1 = variant.arrayDimensions[0];
        dim2 = variant.arrayDimensions[1];
    }
    else if (variant.arrayDimensionsSize == 1)
    {
        dim1 = 1;
        dim2 = variant.arrayDimensions[0];
    }

    size_t rows = data.size();
    size_t cols = rows > 0 ? data[0].size() : 0;
    if ((startRow + rows > dim1) || (startCol + cols > dim2))
    {
        UA_Variant_clear(&variant);
        return {false, 3};
    }

    int16_t *existing = static_cast<int16_t *>(variant.data);
    std::vector<int16_t> values(existing, existing + variant.arrayLength);

    for (size_t row = 0; row < rows; ++row)
        for (size_t col = 0; col < cols; ++col)
            values[(startRow + row) * dim2 + (startCol + col)] = data[row][col];

    UA_Variant newVariant;
    UA_Variant_init(&newVariant);
    status = UA_Variant_setArrayCopy(&newVariant, values.data(), values.size(), &UA_TYPES[UA_TYPES_INT16]);
    if (status != UA_STATUSCODE_GOOD)
    {
        UA_Variant_clear(&variant);
        return {false, 4};
    }

    newVariant.arrayDimensionsSize = variant.arrayDimensionsSize;
    newVariant.arrayDimensions = (UA_UInt32 *)UA_Array_new(newVariant.arrayDimensionsSize, &UA_TYPES[UA_TYPES_UINT32]);
    for (size_t i = 0; i < newVariant.arrayDimensionsSize; ++i)
        newVariant.arrayDimensions[i] = variant.arrayDimensions[i];

    status = UA_Client_writeValueAttribute(client, node, &newVariant);
    UA_Variant_clear(&variant);
    UA_Variant_clear(&newVariant);

    return {status == UA_STATUSCODE_GOOD, status == UA_STATUSCODE_GOOD ? 0 : 5};
}

// ===== Write Int32 Array =====
//
//
std::tuple<bool, int> OPCClient::writeInt32Array(const VariableInfo &var, const std::vector<std::vector<int32_t>> &data, size_t startRow, size_t startCol)
{
    UA_NodeId node = parseNodeId(var.fullName);
    if (UA_NodeId_equal(&node, &UA_NODEID_NULL))
        return {false, 1};

    UA_Variant variant;
    UA_Variant_init(&variant);
    UA_StatusCode status = UA_Client_readValueAttribute(client, node, &variant);
    if (status != UA_STATUSCODE_GOOD || variant.type != &UA_TYPES[UA_TYPES_INT32] || variant.arrayLength == 0)
        return {false, 2};

    size_t dim1 = 1, dim2 = variant.arrayLength;
    if (variant.arrayDimensionsSize == 2)
    {
        dim1 = variant.arrayDimensions[0];
        dim2 = variant.arrayDimensions[1];
    }
    else if (variant.arrayDimensionsSize == 1)
    {
        dim1 = 1;
        dim2 = variant.arrayDimensions[0];
    }

    size_t rows = data.size();
    size_t cols = rows > 0 ? data[0].size() : 0;
    if ((startRow + rows > dim1) || (startCol + cols > dim2))
    {
        UA_Variant_clear(&variant);
        return {false, 3};
    }

    int32_t *existing = static_cast<int32_t *>(variant.data);
    std::vector<int32_t> values(existing, existing + variant.arrayLength);

    for (size_t row = 0; row < rows; ++row)
        for (size_t col = 0; col < cols; ++col)
            values[(startRow + row) * dim2 + (startCol + col)] = data[row][col];

    UA_Variant newVariant;
    UA_Variant_init(&newVariant);
    status = UA_Variant_setArrayCopy(&newVariant, values.data(), values.size(), &UA_TYPES[UA_TYPES_INT32]);
    if (status != UA_STATUSCODE_GOOD)
    {
        UA_Variant_clear(&variant);
        return {false, 4};
    }

    newVariant.arrayDimensionsSize = variant.arrayDimensionsSize;
    newVariant.arrayDimensions = (UA_UInt32 *)UA_Array_new(newVariant.arrayDimensionsSize, &UA_TYPES[UA_TYPES_UINT32]);
    for (size_t i = 0; i < newVariant.arrayDimensionsSize; ++i)
        newVariant.arrayDimensions[i] = variant.arrayDimensions[i];

    status = UA_Client_writeValueAttribute(client, node, &newVariant);
    UA_Variant_clear(&variant);
    UA_Variant_clear(&newVariant);

    return {status == UA_STATUSCODE_GOOD, status == UA_STATUSCODE_GOOD ? 0 : 5};
}
// ===== Write Bool Array =====
//
//
std::tuple<bool, int> OPCClient::writeBoolArray(const VariableInfo &var, const std::vector<std::vector<bool>> &data, size_t startRow, size_t startCol)
{
    // BOOL MUSS NOCH IMPLEMENTIERT WERDEN
    return {false, -100};
}

// ===== Write Float Array =====
//
//
std::tuple<bool, int> OPCClient::writeFloatArray(const VariableInfo &var, const std::vector<std::vector<float>> &data, size_t startRow, size_t startCol)
{
    UA_NodeId node = parseNodeId(var.fullName);
    if (UA_NodeId_equal(&node, &UA_NODEID_NULL))
        return {false, 1};

    UA_Variant variant;
    UA_Variant_init(&variant);
    UA_StatusCode status = UA_Client_readValueAttribute(client, node, &variant);
    if (status != UA_STATUSCODE_GOOD || variant.type != &UA_TYPES[UA_TYPES_FLOAT] || variant.arrayLength == 0)
        return {false, 2};

    size_t dim1 = 1, dim2 = variant.arrayLength;
    if (variant.arrayDimensionsSize == 2)
    {
        dim1 = variant.arrayDimensions[0];
        dim2 = variant.arrayDimensions[1];
    }
    else if (variant.arrayDimensionsSize == 1)
    {
        dim1 = 1;
        dim2 = variant.arrayDimensions[0];
    }

    size_t rows = data.size();
    size_t cols = rows > 0 ? data[0].size() : 0;
    if ((startRow + rows > dim1) || (startCol + cols > dim2))
    {
        UA_Variant_clear(&variant);
        return {false, 3};
    }

    float *existing = static_cast<float *>(variant.data);
    std::vector<float> values(existing, existing + variant.arrayLength);

    for (size_t row = 0; row < rows; ++row)
        for (size_t col = 0; col < cols; ++col)
            values[(startRow + row) * dim2 + (startCol + col)] = data[row][col];

    UA_Variant newVariant;
    UA_Variant_init(&newVariant);
    status = UA_Variant_setArrayCopy(&newVariant, values.data(), values.size(), &UA_TYPES[UA_TYPES_FLOAT]);
    if (status != UA_STATUSCODE_GOOD)
    {
        UA_Variant_clear(&variant);
        return {false, 4};
    }

    newVariant.arrayDimensionsSize = variant.arrayDimensionsSize;
    newVariant.arrayDimensions = (UA_UInt32 *)UA_Array_new(newVariant.arrayDimensionsSize, &UA_TYPES[UA_TYPES_UINT32]);
    for (size_t i = 0; i < newVariant.arrayDimensionsSize; ++i)
        newVariant.arrayDimensions[i] = variant.arrayDimensions[i];

    status = UA_Client_writeValueAttribute(client, node, &newVariant);
    UA_Variant_clear(&variant);
    UA_Variant_clear(&newVariant);

    return {status == UA_STATUSCODE_GOOD, status == UA_STATUSCODE_GOOD ? 0 : 5};
}

// ===== Write Double Array =====
//
//
std::tuple<bool, int> OPCClient::writeDoubleArray(const VariableInfo &var, const std::vector<std::vector<double>> &data, size_t startRow, size_t startCol)
{
    UA_NodeId node = parseNodeId(var.fullName);
    if (UA_NodeId_equal(&node, &UA_NODEID_NULL))
        return {false, 1};

    UA_Variant variant;
    UA_Variant_init(&variant);
    UA_StatusCode status = UA_Client_readValueAttribute(client, node, &variant);
    if (status != UA_STATUSCODE_GOOD || variant.type != &UA_TYPES[UA_TYPES_DOUBLE] || variant.arrayLength == 0)
        return {false, 2};

    size_t dim1 = 1, dim2 = variant.arrayLength;
    if (variant.arrayDimensionsSize == 2)
    {
        dim1 = variant.arrayDimensions[0];
        dim2 = variant.arrayDimensions[1];
    }
    else if (variant.arrayDimensionsSize == 1)
    {
        dim1 = 1;
        dim2 = variant.arrayDimensions[0];
    }

    size_t rows = data.size();
    size_t cols = rows > 0 ? data[0].size() : 0;
    if ((startRow + rows > dim1) || (startCol + cols > dim2))
    {
        UA_Variant_clear(&variant);
        return {false, 3};
    }

    double *existing = static_cast<double *>(variant.data);
    std::vector<double> values(existing, existing + variant.arrayLength);

    for (size_t row = 0; row < rows; ++row)
        for (size_t col = 0; col < cols; ++col)
            values[(startRow + row) * dim2 + (startCol + col)] = data[row][col];

    UA_Variant newVariant;
    UA_Variant_init(&newVariant);
    status = UA_Variant_setArrayCopy(&newVariant, values.data(), values.size(), &UA_TYPES[UA_TYPES_DOUBLE]);
    if (status != UA_STATUSCODE_GOOD)
    {
        UA_Variant_clear(&variant);
        return {false, 4};
    }

    newVariant.arrayDimensionsSize = variant.arrayDimensionsSize;
    newVariant.arrayDimensions = (UA_UInt32 *)UA_Array_new(newVariant.arrayDimensionsSize, &UA_TYPES[UA_TYPES_UINT32]);
    for (size_t i = 0; i < newVariant.arrayDimensionsSize; ++i)
        newVariant.arrayDimensions[i] = variant.arrayDimensions[i];

    status = UA_Client_writeValueAttribute(client, node, &newVariant);
    UA_Variant_clear(&variant);
    UA_Variant_clear(&newVariant);

    return {status == UA_STATUSCODE_GOOD, status == UA_STATUSCODE_GOOD ? 0 : 5};
}

// ======================Array Schreiben String ARRAY ======================
//
//
std::tuple<bool, int> OPCClient::writeStringArray(const VariableInfo &var, const std::vector<std::vector<std::string>> &data, size_t startRow, size_t startCol)
{
    UA_NodeId node = parseNodeId(var.fullName);
    if (UA_NodeId_equal(&node, &UA_NODEID_NULL))
    {
        return {false, 1};
    }

    UA_Variant variant;
    UA_Variant_init(&variant);
    UA_StatusCode status = UA_Client_readValueAttribute(client, node, &variant);
    if (status != UA_STATUSCODE_GOOD || variant.type != &UA_TYPES[UA_TYPES_STRING] || variant.arrayLength == 0)
    {
        UA_Variant_clear(&variant);
        return {false, 2};
    }

    size_t dim1 = 1, dim2 = variant.arrayLength;
    if (variant.arrayDimensionsSize == 2)
    {
        dim1 = variant.arrayDimensions[0];
        dim2 = variant.arrayDimensions[1];
    }
    else if (variant.arrayDimensionsSize == 1)
    {
        dim1 = 1;
        dim2 = variant.arrayDimensions[0];
    }

    size_t rows = data.size();
    size_t cols = rows > 0 ? data[0].size() : 0;
    if ((startRow + rows > dim1) || (startCol + cols > dim2))
    {
        UA_Variant_clear(&variant);
        return {false, 3};
    }

    // Patch the requested window in place. `variant` keeps ownership of every
    // element (old and newly allocated), so the single UA_Variant_clear below
    // frees everything. The previous copy-based version leaked the variant and
    // the allocated strings on every successful call.
    UA_String *elements = static_cast<UA_String *>(variant.data);
    for (size_t row = 0; row < rows; ++row)
    {
        for (size_t col = 0; col < cols; ++col)
        {
            size_t index = (startRow + row) * dim2 + (startCol + col);
            UA_String_clear(&elements[index]);
            elements[index] = UA_STRING_ALLOC(data[row][col].c_str());
        }
    }

    status = UA_Client_writeValueAttribute(client, node, &variant);
    UA_Variant_clear(&variant);

    if (status == UA_STATUSCODE_GOOD)
    {
        return {true, 0};
    }
    else
    {
        return {false, 5};
    }
}

// ======================Array Schreiben WString ARRAY ======================
//
//
std::tuple<bool, int>
OPCClient::writeWStringArray(const VariableInfo &var,
                             const std::vector<std::vector<std::wstring>> &data,
                             size_t startRow, size_t startCol)
{
    UA_NodeId node = parseNodeId(var.fullName);
    if (UA_NodeId_equal(&node, &UA_NODEID_NULL)) {
        return { false, 1 };
    }

    UA_Variant   variant;
    UA_Variant_init(&variant);
    UA_StatusCode status = UA_Client_readValueAttribute(client, node, &variant);
    if (status != UA_STATUSCODE_GOOD ||
        variant.type != &UA_TYPES[UA_TYPES_BYTESTRING] ||
        variant.arrayLength == 0)
    {
        UA_Variant_clear(&variant);
        return { false, 2 };
    }

    size_t dim1 = 1, dim2 = variant.arrayLength;
    if (variant.arrayDimensionsSize == 2) {
        dim1 = variant.arrayDimensions[0];
        dim2 = variant.arrayDimensions[1];
    }
    else if (variant.arrayDimensionsSize == 1) {
        dim2 = variant.arrayDimensions[0];
    }

    size_t rows = data.size();
    size_t cols = rows ? data[0].size() : 0;
    if (startRow + rows > dim1 || startCol + cols > dim2) {
        UA_Variant_clear(&variant);
        return { false, 3 };
    }

    // Patch the requested window in place (same ownership model as
    // writeStringArray). The previous copy-based version cleared the aliased
    // element copies AND the variant afterwards - a double free of every
    // element outside the patched window.
    UA_ByteString *elements = static_cast<UA_ByteString *>(variant.data);
    for (size_t r = 0; r < rows; ++r) {
        for (size_t c = 0; c < cols; ++c) {
            size_t idx = (startRow + r) * dim2 + (startCol + c);
            UA_ByteString_clear(&elements[idx]);

            const std::wstring &ws = data[r][c];
            size_t byteLen = ws.size() * sizeof(wchar_t);
            UA_ByteString bs;
            bs.length = byteLen;
            bs.data = static_cast<UA_Byte *>(UA_malloc(byteLen));
            memcpy(bs.data, ws.data(), byteLen);

            elements[idx] = bs;
        }
    }

    status = UA_Client_writeValueAttribute(client, node, &variant);
    UA_Variant_clear(&variant);

    if (status == UA_STATUSCODE_GOOD) {
        return { true, 0 };
    }
    else {
        return { false, 5 };
    }
}
