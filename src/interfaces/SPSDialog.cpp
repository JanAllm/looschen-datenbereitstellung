#include "interfaces/SPSDialog.h"
#include <algorithm>
#include <iomanip>
#include <sstream>
using namespace std;
SPSDialog::SPSDialog(std::shared_ptr<ISPSClient> client)
    : client(std::move(client)) {
    }

bool SPSDialog::registerVariable(const std::string& name,
                                 DataType           type,
                                 const std::string& connectionString)
{
    if (!client) {
      
        ErrorLoggerSingleton::instance()
            .logError("SPSDialog: client nicht gesetzt!");
        throw std::runtime_error("SPSDialog: client nicht gesetzt!");
    }

    auto [success, handle] = client->registerVariable(name, type, connectionString);
    if (!success) {
        std::string err = "SPSDialog: Fehler bei der Registrierung der Variable: " + name;
       
        ErrorLoggerSingleton::instance().logError(err);
        return false;
    }

    VariableInfo var;
    var.name     = name;
    var.type     = type;
    var.fullName = connectionString;
    //var.tested = true;
    var.available = true;
    if (handle.has_value())
        var.handle = *handle;

    // explizit per insert_or_assign einfügen/ersetzen+

    variableMap.insert_or_assign(name, std::move(var));


    return true;
}

tuple<int16_t, bool, int> SPSDialog::readInt16(const std::string& name) {
    if (variableMap.find(name) == variableMap.end()) {
        ErrorLoggerSingleton::instance().logError("SPSDialog: Variable '" + name + "' nicht gefunden!");
        return {0, false, -1}; // Fehler
    }
    auto var = variableMap[name];
    auto [value, succ, errorIndx] = client->readInt16(var);
    if (succ) {
        return {value, true, 0}; // alles ok
    } else {
        ErrorLoggerSingleton::instance().logError("SPSDialog: Fehler beim Lesen der Variable '" + name + "'!");
        return {-1, false, errorIndx}; // Fehler beim Lesen
    }
}

tuple<int, bool, int> SPSDialog::readInt32(const std::string& name) {
    if (variableMap.find(name) != variableMap.end()) {
        auto var = variableMap[name];
        auto [value, succ, errorIndx] = client->readInt32(var);
        if (!succ) {
            ErrorLoggerSingleton::instance().logError("SPSDialog: Fehler beim Lesen der Variable '" + name + "'!");
            return {0, true, errorIndx}; // Fehler beim Lesen
        } else {
            return {value, false, 0}; // alles ok
        }
    } else {
        ErrorLoggerSingleton::instance().logError("SPSDialog: Variable '" + name + "' nicht gefunden!");
        return {0, false, -1}; // Fehler
    }
}

tuple<float, bool, int> SPSDialog::readFloat(const std::string& name) {
    if (variableMap.find(name) != variableMap.end()) {
        auto var = variableMap[name];
        auto [value, succ, errorIndx] = client->readFloat(var);
        if (!succ) {
            ErrorLoggerSingleton::instance().logError("SPSDialog: Fehler beim Lesen der Variable '" + name + "'!");
            return {0.0f, false, errorIndx}; // Fehler beim Lesen
        } else {
            return {value, true, 0}; // alles ok
        }
    } else {
        ErrorLoggerSingleton::instance().logError("SPSDialog: Variable '" + name + "' nicht gefunden!");
        return {0.0f, false, -1}; // Fehler
    }
}

tuple<double, bool, int> SPSDialog::readDouble(const std::string& name) {
    if (variableMap.find(name) != variableMap.end()) {
        auto var = variableMap[name];
        auto [value, succ, errorIndx] = client->readDouble(var);
        if (!succ) {
            ErrorLoggerSingleton::instance().logError("SPSDialog: Fehler beim Lesen der Variable '" + name + "'!");
            return {0.0, false, errorIndx}; // Fehler beim Lesen
        } else {
            return {value, true, 0}; // alles ok
        }
    } else {
        ErrorLoggerSingleton::instance().logError("SPSDialog: Variable '" + name + "' nicht gefunden!");
        return {0.0, false, -1}; // Fehler
    }
}

tuple<bool, bool, int> SPSDialog::readBool(const std::string& name) {
    if (variableMap.find(name) != variableMap.end()) {
        auto var = variableMap[name];
        auto [value, succ, errorIndx] = client->readBool(var);
        if (!succ) {
            ErrorLoggerSingleton::instance().logError("SPSDialog: Fehler beim Lesen der Variable '" + name + "'!");
            return {false, succ, errorIndx}; // Fehler beim Lesen
        } else {
            return {value, succ, 0}; // alles ok
        }
    } else {
        ErrorLoggerSingleton::instance().logError("SPSDialog: Variable '" + name + "' nicht gefunden!");
        return {false, false, -1}; // Fehler
    }
}

tuple<string, bool, int> SPSDialog::readString(const std::string& name) {
    if (variableMap.find(name) != variableMap.end()) {
        auto var = variableMap[name];
        auto [value, error, errorIndx] = client->readString(var);
        if (!error) {
            ErrorLoggerSingleton::instance().logError("SPSDialog: Fehler beim Lesen der Variable '" + name + "'!");
            return {"", false, errorIndx}; // Fehler beim Lesen
        } else {
            return {value, true, 0}; // alles ok
        }
    } else {
        ErrorLoggerSingleton::instance().logError("SPSDialog: Variable '" + name + "' nicht gefunden!");
        return {"", false, -1}; // Fehler
    }
}

tuple<string, bool, int> SPSDialog::readChars(const std::string& name, int length) {
    if (variableMap.find(name) != variableMap.end()) {
        auto var = variableMap[name];
        auto [value, succ, errorIndx] = client->readChars(var, length);
        if (!succ) {
            ErrorLoggerSingleton::instance().logError("SPSDialog: Fehler beim Lesen der Variable '" + name + "'!");
            return {"", false, errorIndx}; // Fehler beim Lesen
        } else {
            return {value, true, 0}; // alles ok
        }
    } else {
        ErrorLoggerSingleton::instance().logError("SPSDialog: Variable '" + name + "' nicht gefunden!");
        return {"", false, -1}; // Fehler
    }
}

tuple<wstring, bool, int> SPSDialog::readWString(const std::string& name) {
    if (variableMap.find(name) != variableMap.end()) {
        auto var = variableMap[name];
        auto [value, succ, errorIndx] = client->readWString(var);
        if (!succ) {
            ErrorLoggerSingleton::instance().logError("SPSDialog: Fehler beim Lesen der Variable '" + name + "'!");
            return {L"", false, errorIndx}; // Fehler beim Lesen
        } else {
            return {value, true, 0}; // alles ok
        }
    } else {
        ErrorLoggerSingleton::instance().logError("SPSDialog: Variable '" + name + "' nicht gefunden!");
        return {L"", false, -1}; // Fehler
    }
}

tuple<bool, int> SPSDialog::writeInt16(const std::string& name, int16_t valueToWrite) {
    if (variableMap.find(name) != variableMap.end()) {
        auto var = variableMap[name];
        auto [succ, errorCode] = client->writeInt16(var, valueToWrite);
        return {succ, errorCode}; // alles ok
    } else {
        ErrorLoggerSingleton::instance().logError("SPSDialog: Variable '" + name + "' nicht gefunden!");
        return {false, -1}; // Fehler
    }
}

tuple<bool, int> SPSDialog::writeFloat(const std::string& name, float valueToWrite) {
    if (variableMap.find(name) != variableMap.end()) {
        auto var = variableMap[name];
        auto [succ, errorCode] = client->writeFloat(var, valueToWrite);
        return {succ, errorCode}; // alles ok
    } else {
        ErrorLoggerSingleton::instance().logError("SPSDialog: Variable '" + name + "' nicht gefunden!");
        return {false, -1}; // Fehler
    }
}

tuple<bool, int> SPSDialog::writeBool(const std::string& name, bool valueToWrite) {
    if (variableMap.find(name) != variableMap.end()) {
        auto var = variableMap[name];
        auto [succ, errorCode] = client->writeBool(var, valueToWrite);
        return {succ, errorCode}; // alles ok
    } else {
        ErrorLoggerSingleton::instance().logError("SPSDialog: Variable '" + name + "' nicht gefunden!");
        return {false, -1}; // Fehler
    }
}

tuple<bool, int> SPSDialog::writeInt32(const std::string& name, int32_t valueToWrite) {
    if (variableMap.find(name) != variableMap.end()) {
        auto var = variableMap[name];
        auto [succ, errorCode] = client->writeInt32(var, valueToWrite);
        return {succ, errorCode}; // alles ok
    } else {
        ErrorLoggerSingleton::instance().logError("SPSDialog: Variable '" + name + "' nicht gefunden!");
        return {false, -1}; // Fehler
    }
}

tuple<bool, int> SPSDialog::writeDouble(const std::string& name, double valueToWrite) {
    if (variableMap.find(name) != variableMap.end()) {
        auto var = variableMap[name];
        auto [succ, errorCode] = client->writeDouble(var, valueToWrite);
        return {succ, errorCode}; // alles ok
    } else {
        ErrorLoggerSingleton::instance().logError("SPSDialog: Variable '" + name + "' nicht gefunden!");
        return {false, -1}; // Fehler
    }
}

tuple<bool, int> SPSDialog::writeChars(const std::string& name, const std::string& valueToWrite, int length) {
    if (variableMap.find(name) != variableMap.end()) {
        auto var = variableMap[name];
        auto [succ, errorCode] = client->writeChars(var, valueToWrite, length);
        return {succ, errorCode}; // alles ok
    } else {
        ErrorLoggerSingleton::instance().logError("SPSDialog: Variable '" + name + "' nicht gefunden!");
        return {false, -1}; // Fehler
    }
}

tuple<bool, int> SPSDialog::writeWString(const std::string& name, const std::wstring& valueToWrite) {
    if (variableMap.find(name) != variableMap.end()) {
        auto var = variableMap[name];
        auto [succ, errorCode] = client->writeWString(var, valueToWrite);
        return {succ, errorCode}; // alles ok
    } else {
        ErrorLoggerSingleton::instance().logError("SPSDialog: Variable '" + name + "' nicht gefunden!");
        return {false, -1}; // Fehler
    }
}

void SPSDialog::printNamespaceArray() {
    if (!client) {
        ErrorLoggerSingleton::instance().logError("SPSDialog: Client nicht gesetzt!");
    } else {
        client->printNamespaceArray();
    }
}

void SPSDialog::printVariableMap(){
    if (variableMap.empty()) {
        return;
    }
    // Terminal-Ausgaben entfernt. Falls gewünscht, kann hier statt cout auf den ErrorLogger oder einen Info-Logger geschrieben werden.
    for (const auto& [name, var] : variableMap) {
        // keine Terminal-Ausgabe
        (void)name;
        (void)var;
    }
}

// Live-Diagnose aller registrierten Knoten. Nutzt das AccessLevel-Attribut,
// schreibt also nichts auf die SPS.
std::vector<NodeDiagnostic> SPSDialog::nodeDiagnostics()
{
    std::vector<NodeDiagnostic> out;
    out.reserve(variableMap.size());

    auto typeName = [](DataType t) -> std::string {
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
    };

    for (const auto &[name, var] : variableMap)
    {
        NodeDiagnostic d;
        d.name = name;
        d.nodeId = var.fullName;
        d.type = typeName(var.type);
        if (client)
            d.exists = client->readAccessLevel(var.fullName, d.readable, d.writable);
        out.push_back(d);
    }

    std::sort(out.begin(), out.end(),
              [](const NodeDiagnostic &a, const NodeDiagnostic &b) { return a.name < b.name; });
    return out;
}

bool SPSDialog::probeNode(const std::string &nodeId, bool &readable, bool &writable)
{
    readable = false;
    writable = false;
    if (!client)
        return false;
    return client->readAccessLevel(nodeId, readable, writable);
}

bool SPSDialog::readNodeValueAsString(const std::string &nodeId, DataType type, std::string &out)
{
    out.clear();
    if (!client)
        return false;

    // Build the descriptor on the fly instead of looking it up in variableMap:
    // an unregistered node is exactly the case this is meant to diagnose.
    VariableInfo var;
    var.name = nodeId;
    var.fullName = nodeId;
    var.type = type;

    // Floats are shown with a fixed precision and trailing zeros stripped,
    // otherwise a 140 arrives in the UI as "140.000000".
    auto formatReal = [](double v) {
        std::ostringstream os;
        os << std::fixed << std::setprecision(3) << v;
        std::string s = os.str();
        const auto dot = s.find('.');
        if (dot != std::string::npos)
        {
            s.erase(s.find_last_not_of('0') + 1);
            if (!s.empty() && s.back() == '.')
                s.pop_back();
        }
        return s;
    };

    switch (type)
    {
    case DataType::Int16:
    {
        auto [v, ok, code] = client->readInt16(var);
        if (!ok)
            return false;
        out = std::to_string(v);
        return true;
    }
    case DataType::Int32:
    {
        auto [v, ok, code] = client->readInt32(var);
        if (!ok)
            return false;
        out = std::to_string(v);
        return true;
    }
    case DataType::Float:
    {
        auto [v, ok, code] = client->readFloat(var);
        if (!ok)
            return false;
        out = formatReal(static_cast<double>(v));
        return true;
    }
    case DataType::Double:
    {
        auto [v, ok, code] = client->readDouble(var);
        if (!ok)
            return false;
        out = formatReal(v);
        return true;
    }
    case DataType::Bool:
    {
        auto [v, ok, code] = client->readBool(var);
        if (!ok)
            return false;
        out = v ? "true" : "false";
        return true;
    }
    case DataType::String:
    {
        auto [v, ok, code] = client->readString(var);
        if (!ok)
            return false;
        // Info_Text and friends are 255 chars wide; keep the row readable.
        constexpr size_t kMaxLen = 60;
        out = v.size() > kMaxLen ? v.substr(0, kMaxLen) + "..." : v;
        return true;
    }
    default:
        // Arrays: deliberately not read, see the header.
        return false;
    }
}
