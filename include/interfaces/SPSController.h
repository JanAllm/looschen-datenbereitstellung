#pragma once
#include "SPSDialog.h"
//#include "TwincatAds.h"
#include "OPC_UA.h"
#include "TestClient.h"
#include <memory>
#include "data_structs.h"
#include <map>
#include <string>
#include <vector>
#include <tuple>
#include <optional>
#include "setupVarInfos.h"
/**
 * @class SPSController
 * @brief The SPSController class provides an interface for interacting with an SPS (Steuerungs- und Prozesssystem).
 *
 * This class allows reading and writing of various data types to the SPS, as well as managing camera settings
 * and image processing parameters. It uses a client and dialog mechanism to communicate with the SPS.
 * Inputs : clientType (e.g.,twincat, opc test) and connection string.
 */
class SPSController
{
public:
    /**
     * @brief Constructs an SPSController with the specified client type and connection string.
     * @param clientType The type of client to use (e.g., OPC UA, Twincat ADS).
     * @param connectionString The connection string for the SPS.
     */
    SPSController(const std::string &clientType, const std::string &connectionString);

    /**
     * @brief Default destructor.
     */
    ~SPSController() = default;
    bool connect();
    bool disconnect();
    /**
     * @brief Reads the camera settings from the SPS.
     * @return A tuple containing the KameraSettings, a success flag, and an error code.
     */
    std::tuple<KameraSettings, bool, int> readCameraSettings();

    /**
     * @brief Writes the camera settings to the SPS.
     * @param settings The KameraSettings to write.
     * @return A tuple containing a success flag and an error code.
     */
    std::tuple<bool, int> writeCameraSettings(const KameraSettings &settings);

    /**
     * @brief Psush an Error message to the SPS.
     * @param string Errormessage.
     * @param ErrorCode
     * @return A tuple containing a success flag and an error code.
     */
    std::tuple<bool, int> pushErrorMessage(const std::string &errorMessage, int errorCode);

    /**
     * @brief Psush an Error message to the SPS.
     * @param string InfoMessage.
     * @param
     * @return A tuple containing a success flag and an error code.
     */
    std::tuple<bool, int> pushInfoMessage(const std::string &infoMessage)
    {
        return dialog->writeChars("Info_Text", infoMessage, 255);
    }

    /**
     * @brief Reads the image processing parameters from the SPS.
     * @return A tuple containing the ProcessImgParams, a success flag, and an error code.
     */
    std::tuple<ProcessImgParams, bool, int> readProcessImgParams();

    /**
     * @brief Writes the image processing parameters to the SPS.
     * @param params The ProcessImgParams to write.
     * @return A tuple containing a success flag and an error code.
     */
    std::tuple<bool, int> writeProcessImgParams(const ProcessImgParams &params);

    /**
     * @brief A utility function that always returns true.
     * @return True.
     */
    bool returnTrue() { return true; };
    bool setupVariables(setupVarInfos &setupVars);
    

    bool registerVariable(const std::string &name, DataType type, const std::string &connectionString)
    {
        return dialog->registerVariable(name, type, connectionString); // weitergereicht an SPSDialog
    }
    // -------- Read and Write for Specific Variable Types -----------

    /**
     * @brief Reads an int16_t value from the SPS.
     * @param name Variable name.
     * @return Tuple: int16_t value, success flag, error code.
     */
    std::tuple<int16_t, bool, int> readInt16(const std::string &name)
    {
        return dialog->readInt16(name); // weitergereicht an SPSDialog
    }

    /**
     * @brief Reads an int32_t value from the SPS.
     * @param name Variable name.
     * @return Tuple: int32_t value, success flag, error code.
     */
    std::tuple<int32_t, bool, int> readInt32(const std::string &name)
    {
        return dialog->readInt32(name);
    }

    /**
     * @brief Reads a double value from the SPS.
     * @param name Variable name.
     * @return Tuple: double value, success flag, error code.
     */
    std::tuple<double, bool, int> readDouble(const std::string &name)
    {
        return dialog->readDouble(name);
    }

    /**
     * @brief Reads a float value from the SPS.
     * @param name Variable name.
     * @return Tuple: float value, success flag, error code.
     */
    std::tuple<float, bool, int> readFloat(const std::string &name)
    {
        return dialog->readFloat(name);
    }

    /**
     * @brief Reads a bool value from the SPS.
     * @param name Variable name.
     * @return Tuple: bool value, success flag, error code.
     */
    std::tuple<bool, bool, int> readBool(const std::string &name)
    {
        return dialog->readBool(name);
    }

    /**
     * @brief Reads a string value from the SPS.
     * @param name Variable name.
     * @return Tuple: string value, success flag, error code.
     */
    std::tuple<std::string, bool, int> readString(const std::string &name)
    {
        return dialog->readString(name);
    }

    /**
     * @brief Reads a fixed-length char array as string from the SPS.
     * @param name Variable name.
     * @param length Number of characters to read.
     * @return Tuple: string value, success flag, error code.
     */
    std::tuple<std::string, bool, int> readChars(const std::string &name, int length)
    {
        return dialog->readChars(name, length);
    }

    std::tuple<std::wstring, bool, int> readWString(const std::string &name)
    {
        return dialog->readWString(name);
    }

    /**
     * @brief Writes an int16_t value to the SPS.
     * @param name Variable name.
     * @param valueToWrite Value to write.
     * @return Tuple: success flag, error code.
     */
    std::tuple<bool, int> writeInt16(const std::string &name, int16_t valueToWrite)
    {
        return dialog->writeInt16(name, valueToWrite);
    }

    /**
     * @brief Writes a bool value to the SPS.
     * @param name Variable name.
     * @param valueToWrite Value to write.
     * @return Tuple: success flag, error code.
     */
    std::tuple<bool, int> writeBool(const std::string &name, bool valueToWrite)
    {
        return dialog->writeBool(name, valueToWrite);
    }

    /**
     * @brief Writes a fixed-length char array as string to the SPS.
     * @param name Variable name.
     * @param valueToWrite String value to write.
     * @param length Number of characters to write.
     * @return Tuple: success flag, error code.
     */
    std::tuple<bool, int> writeChars(const std::string &name, const std::string &valueToWrite, int length)
    {
        return dialog->writeChars(name, valueToWrite, length);
    }

    /**
     * @brief Writes a float value to the SPS.
     * @param name Variable name.
     * @param valueToWrite Value to write.
     * @return Tuple: success flag, error code.
     */
    std::tuple<bool, int> writeFloat(const std::string &name, float valueToWrite)
    {
        return dialog->writeFloat(name, valueToWrite);
    }

    /**
     * @brief Writes an int32_t value to the SPS.
     * @param name Variable name.
     * @param valueToWrite Value to write.
     * @return Tuple: success flag, error code.
     */
    std::tuple<bool, int> writeInt32(const std::string &name, int32_t valueToWrite)
    {
        return dialog->writeInt32(name, valueToWrite);
    }

    /**
     * @brief Writes a double value to the SPS.
     * @param name Variable name.
     * @param valueToWrite Value to write.
     * @return Tuple: success flag, error code.
     */
    std::tuple<bool, int> writeDouble(const std::string &name, double valueToWrite)
    {
        return dialog->writeDouble(name, valueToWrite);
    }

    /**
     * @brief Writes a wide string value to the SPS.
     * @param name Variable name.
     * @param valueToWrite Value to write.
     * @return Tuple: success flag, error code.
     */
    std::tuple<bool, int> writeWString(const std::string &name, const std::wstring &valueToWrite)
    {
        return dialog->writeWString(name, valueToWrite);
    }

    // =================Array Read and Write Functions=================
    /**
     * @brief Reads a 2D array of int16_t values from the SPS.
     * @param name Variable name.
     * @param startRow Starting row index.
     * @param rowCount Number of rows to read.
     * @param startCol Starting column index.
     * @param colCount Number of columns to read.
     * @return Tuple: 2D vector of int16_t, success flag, error code.
     */
    std::tuple<std::vector<std::vector<int16_t>>, bool, int> readInt16Array(const std::string &name, size_t startRow = 0, size_t rowCount = SIZE_MAX, size_t startCol = 0, size_t colCount = SIZE_MAX)
    {
        return dialog->readInt16Array(name, startRow, rowCount, startCol, colCount);
    }

    /**
     * @brief Reads a 2D array of int32_t values from the SPS.
     * @param name Variable name.
     * @param startRow Starting row index.
     * @param rowCount Number of rows to read.
     * @param startCol Starting column index.
     * @param colCount Number of columns to read.
     * @return Tuple: 2D vector of int32_t, success flag, error code.
     */
    std::tuple<std::vector<std::vector<int32_t>>, bool, int> readInt32Array(const std::string &name, size_t startRow = 0, size_t rowCount = SIZE_MAX, size_t startCol = 0, size_t colCount = SIZE_MAX)
    {
        return dialog->readInt32Array(name, startRow, rowCount, startCol, colCount);
    }

    /**
     * @brief Reads a 2D array of float values from the SPS.
     * @param name Variable name.
     * @param startRow Starting row index.
     * @param rowCount Number of rows to read.
     * @param startCol Starting column index.
     * @param colCount Number of columns to read.
     * @return Tuple: 2D vector of float, success flag, error code.
     */
    std::tuple<std::vector<std::vector<float>>, bool, int> readFloatArray(const std::string &name, size_t startRow = 0, size_t rowCount = SIZE_MAX, size_t startCol = 0, size_t colCount = SIZE_MAX)
    {
        return dialog->readFloatArray(name, startRow, rowCount, startCol, colCount);
    }

    /**
     * @brief Reads a 2D array of double values from the SPS.
     * @param name Variable name.
     * @param startRow Starting row index.
     * @param rowCount Number of rows to read.
     * @param startCol Starting column index.
     * @param colCount Number of columns to read.
     * @return Tuple: 2D vector of double, success flag, error code.
     */
    std::tuple<std::vector<std::vector<double>>, bool, int> readDoubleArray(const std::string &name, size_t startRow = 0, size_t rowCount = SIZE_MAX, size_t startCol = 0, size_t colCount = SIZE_MAX)
    {
        return dialog->readDoubleArray(name, startRow, rowCount, startCol, colCount);
    }

    /**
     * @brief Reads a 2D array of bool values from the SPS.
     * @param name Variable name.
     * @param startRow Starting row index.
     * @param rowCount Number of rows to read.
     * @param startCol Starting column index.
     * @param colCount Number of columns to read.
     * @return Tuple: 2D vector of bool, success flag, error code.
     */
    std::tuple<std::vector<std::vector<bool>>, bool, int> readBoolArray(const std::string &name, size_t startRow = 0, size_t rowCount = SIZE_MAX, size_t startCol = 0, size_t colCount = SIZE_MAX)
    {
        return dialog->readBoolArray(name, startRow, rowCount, startCol, colCount);
    }

    /**
     * @brief Reads a 2D array of string values from the SPS.
     * @param name Variable name.
     * @param startRow Starting row index.
     * @param rowCount Number of rows to read.
     * @param startCol Starting column index.
     * @param colCount Number of columns to read.
     * @return Tuple: 2D vector of string, success flag, error code.
     */
    std::tuple<std::vector<std::vector<std::string>>, bool, int> readStringArray(const std::string &name, size_t startRow = 0, size_t rowCount = SIZE_MAX, size_t startCol = 0, size_t colCount = SIZE_MAX)
    {
        return dialog->readStringArray(name, startRow, rowCount, startCol, colCount);
    }


    /**
     * @brief Reads a 2D array of wide strings from the specified source.
     *
     * This function attempts to read a two-dimensional array of wide strings (WString) 
     * identified by the given name. The data is read into the provided valueToRead vector.
     * The function supports reading a subset of the array by specifying the starting row 
     * and column, as well as the number of rows and columns to read.
     *
     * @param name The identifier of the array to read.
     * @param valueToRead Reference to a vector where the read values will be stored.
     * @param startRow The index of the first row to read (default is 0).
     * @param rowCount The number of rows to read (default is SIZE_MAX, meaning all rows).
     * @param startCol The index of the first column to read (default is 0).
     * @param colCount The number of columns to read (default is SIZE_MAX, meaning all columns).
     * @return std::tuple<bool, int> A tuple where the first element indicates success (true if successful),
     *         and the second element may contain an error code or additional status information.
     */
    std::tuple<std::vector<std::vector<std::wstring>>, bool, int> readWStringArray(const std::string &name, size_t startRow = 0, size_t rowCount = SIZE_MAX, size_t startCol = 0, size_t colCount = SIZE_MAX)
  
    {
        return dialog->readWStringArray(name, startRow, rowCount, startCol, colCount);
    }
 

    /**
     * @brief Writes a 2D array of int16_t values to the SPS.
     * @param name Variable name.
     * @param valueToWrite 2D vector of int16_t values.
     * @param startRow Starting row index.
     * @param startCol Starting column index.
     * @return Tuple: success flag, error code.
     */
    std::tuple<bool, int> writeInt16Array(const std::string &name, const std::vector<std::vector<int16_t>> &valueToWrite, size_t startRow = 0, size_t startCol = 0)
    {
        return dialog->writeInt16Array(name, valueToWrite, startRow, startCol);
    }

    /**
     * @brief Writes a 2D array of int32_t values to the SPS.
     * @param name Variable name.
     * @param valueToWrite 2D vector of int32_t values.
     * @param startRow Starting row index.
     * @param startCol Starting column index.
     * @return Tuple: success flag, error code.
     */
    std::tuple<bool, int> writeInt32Array(const std::string &name, const std::vector<std::vector<int32_t>> &valueToWrite, size_t startRow = 0, size_t startCol = 0)
    {
        return dialog->writeInt32Array(name, valueToWrite, startRow, startCol);
    }

    /**
     * @brief Writes a 2D array of float values to the SPS.
     * @param name Variable name.
     * @param valueToWrite 2D vector of float values.
     * @param startRow Starting row index.
     * @param startCol Starting column index.
     * @return Tuple: success flag, error code.
     */
    std::tuple<bool, int> writeFloatArray(const std::string &name, const std::vector<std::vector<float>> &valueToWrite, size_t startRow = 0, size_t startCol = 0)
    {
        return dialog->writeFloatArray(name, valueToWrite, startRow, startCol);
    }

    /**
     * @brief Writes a 2D array of double values to the SPS.
     * @param name Variable name.
     * @param valueToWrite 2D vector of double values.
     * @param startRow Starting row index.
     * @param startCol Starting column index.
     * @return Tuple: success flag, error code.
     */
    std::tuple<bool, int> writeDoubleArray(const std::string &name, const std::vector<std::vector<double>> &valueToWrite, size_t startRow = 0, size_t startCol = 0)
    {
        return dialog->writeDoubleArray(name, valueToWrite, startRow, startCol);
    }

    /**
     * @brief Writes a 2D array of bool values to the SPS.
     * @param name Variable name.
     * @param valueToWrite 2D vector of bool values.
     * @param startRow Starting row index.
     * @param startCol Starting column index.
     * @return Tuple: success flag, error code.
     */
    std::tuple<bool, int> writeBoolArray(const std::string &name, const std::vector<std::vector<bool>> &valueToWrite, size_t startRow = 0, size_t startCol = 0)
    {
        return dialog->writeBoolArray(name, valueToWrite, startRow, startCol);
    }

    /**
     * @brief Writes a 2D array of string values to the SPS.
     * @param name Variable name.
     * @param valueToWrite 2D vector of string values.
     * @param startRow Starting row index.
     * @param startCol Starting column index.
     * @return Tuple: success flag, error code.
     */
    std::tuple<bool, int> writeStringArray(const std::string &name, const std::vector<std::vector<std::string>> &valueToWrite, size_t startRow = 0, size_t startCol = 0)
    {
        return dialog->writeStringArray(name, valueToWrite, startRow, startCol);
    }


    /**
     * @brief Writes a 2D array of wide strings to the specified location.
     *
     * This function writes the provided 2D vector of strings (`valueToWrite`) to a target
     * identified by `name`, starting at the specified row (`startRow`) and column (`startCol`).
     *
     * @param name The identifier or name of the target location to write the string array.
     * @param valueToWrite The 2D vector containing the strings to be written.
     * @param startRow The starting row index for writing (default is 0).
     * @param startCol The starting column index for writing (default is 0).
     * @return std::tuple<bool, int> A tuple where the first element indicates success (true) or failure (false),
     *         and the second element may contain an error code or additional status information.
     */
    std::tuple<bool, int> writeWStringArray(const std::string &name, const std::vector<std::vector<std::wstring>> &valueToWrite, size_t startRow = 0, size_t startCol = 0)
    {
        return dialog->writeWStringArray(name, valueToWrite, startRow, startCol);
    }

    /**
     * @brief Checks if the connection to the SPS is established.
     * @return True if connected, false otherwise.
     */
    void printNamespaceArray()
    {
        dialog->printNamespaceArray();
    }
    /// Aktive OPC-Sitzung vorhanden? (echter Sitzungsstatus, nicht Write-Ergebnis)
    bool isConnected() { return dialog && dialog->isConnected(); }

    /// Live-Diagnose ALLER konfigurierten Knoten (fuer /api/opc_nodes).
    /// Arbeitet auf der Konfigurationsliste, nicht auf der Registrierung -
    /// dadurch sieht man auch dann, WELCHER Knoten fehlt, wenn das Setup
    /// fehlgeschlagen ist (genau der Diagnosefall).
    std::vector<NodeDiagnostic> nodeDiagnostics()
    {
        std::vector<NodeDiagnostic> out;
        out.reserve(configuredNodes_.size());
        for (const auto &[name, type, nodeId] : configuredNodes_)
        {
            NodeDiagnostic d;
            d.name = name;
            d.nodeId = nodeId;
            d.type = dataTypeName(type);
            d.exists = dialog->probeNode(nodeId, d.readable, d.writable);
            // One extra read per readable scalar node, every diagnostics pass
            // (10 s). Arrays are skipped inside readNodeValueAsString, so the
            // data block itself is never pulled just to fill the settings page.
            if (d.exists && d.readable)
                d.hasValue = dialog->readNodeValueAsString(nodeId, type, d.value);
            out.push_back(d);
        }
        return out;
    }

    static std::string dataTypeName(DataType t)
    {
        switch (t)
        {
        case DataType::Int16: return "Int16";
        case DataType::Int32: return "Int32";
        case DataType::Float: return "Float";
        case DataType::Double: return "Double";
        case DataType::Bool: return "Bool";
        case DataType::String: return "String";
        case DataType::Int16Array: return "Int16Array";
        case DataType::Int32Array: return "Int32Array";
        case DataType::FloatArray: return "FloatArray";
        case DataType::DoubleArray: return "DoubleArray";
        case DataType::BoolArray: return "BoolArray";
        case DataType::StringArray: return "StringArray";
        case DataType::WStringArray: return "WStringArray";
        default: return "Unknown";
        }
    }

    void printVariableMap()
    {
        dialog->printVariableMap();
    }

    // Alle in setupVariables() konfigurierten Knoten (auch fehlgeschlagene).
    std::vector<std::tuple<std::string, DataType, std::string>> configuredNodes_;
private:
    std::shared_ptr<ISPSClient> client; ///< The client used for communication with the SPS.
    std::unique_ptr<SPSDialog> dialog;  ///< The dialog used for variable interaction.
    

    /**
     * @brief Sets up the variables and registers groups/variables for interaction.
     */
};
