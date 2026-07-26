#include "interfaces/OPCUAServer.h"
#include <iostream>
#include <chrono>

OPCUAServer::OPCUAServer(uint16_t port, const std::string &serverName)
    : server_(nullptr), port_(port), serverName_(serverName), running_(false), stopRequested_(false)
{
    // Server SOFORT erstellen!
    server_ = UA_Server_new();
    if (!server_)
    {
        throw std::runtime_error("UA_Server_new() fehlgeschlagen!");
    }

    // Konfiguration
    UA_ServerConfig *config = UA_Server_getConfig(server_);
    UA_ServerConfig_setMinimal(config, port_, nullptr);

    config->applicationDescription.applicationName =
        UA_LOCALIZEDTEXT_ALLOC("en-US", serverName_.c_str());

    std::cout << "✅ OPCUAServer erstellt (Port: " << port_ << ")" << std::endl;
}
OPCUAServer::~OPCUAServer()
{
    stop();

    if (server_)
    {
        UA_Server_delete(server_);
        server_ = nullptr;
    }
}
// ========== Server Lifecycle ==========

bool OPCUAServer::start()
{
    if (running_.load())
    {
        std::cerr << "⚠️  Server läuft bereits!" << std::endl;
        return false;
    }

    if (!server_)
    {
        std::cerr << "❌ Server nicht initialisiert!" << std::endl;
        return false;
    }

    // Server STARTEN (öffnet Socket!)
    UA_StatusCode retval = UA_Server_run_startup(server_);
    if (retval != UA_STATUSCODE_GOOD)
    {
        std::cerr << "❌ Server-Startup fehlgeschlagen: " << retval << std::endl;
        return false;
    }

    // Server in separatem Thread starten
    stopRequested_.store(false);
    running_.store(true);

    serverThread_ = std::thread(&OPCUAServer::serverLoop, this);

    // Kurz warten
    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    std::cout << "✅ OPC UA Server gestartet: " << getEndpointUrl() << std::endl;
    return true;
}

void OPCUAServer::stop()
{
    if (!running_.load())
    {
        return;
    }

    std::cout << "🛑 Stoppe OPC UA Server..." << std::endl;

    stopRequested_.store(true);
    running_.store(false);

    if (serverThread_.joinable())
    {
        serverThread_.join();
    }

    // Server herunterfahren
    if (server_)
    {
        UA_Server_run_shutdown(server_);
    }

    std::cout << "✅ OPC UA Server gestoppt" << std::endl;
}
std::string OPCUAServer::getEndpointUrl() const
{
    return "opc.tcp://localhost:" + std::to_string(port_);
}

void OPCUAServer::serverLoop()
{
    std::cout << "🔄 Server-Loop gestartet..." << std::endl;

    while (!stopRequested_.load())
    {
        // Server-Iteration (16ms = ~60 FPS)
        UA_Server_run_iterate(server_, true);
        std::this_thread::sleep_for(std::chrono::milliseconds(16));
    }

    std::cout << "🔄 Server-Loop beendet" << std::endl;
}

// ========== Variablen erstellen ==========

bool OPCUAServer::addInt16Variable(const std::string &name, int16_t initialValue)
{
    UA_Variant variant;
    UA_Variant_init(&variant);
    UA_Variant_setScalarCopy(&variant, &initialValue, &UA_TYPES[UA_TYPES_INT16]);

    // KEIN UA_Variant_clear! Ownership geht an attr!
    return addVariableNode(name, UA_TYPES[UA_TYPES_INT16].typeId, variant, "Int16");
}

bool OPCUAServer::addInt32Variable(const std::string &name, int32_t initialValue)
{
    UA_Variant variant;
    UA_Variant_init(&variant);
    UA_Variant_setScalarCopy(&variant, &initialValue, &UA_TYPES[UA_TYPES_INT32]);

    return addVariableNode(name, UA_TYPES[UA_TYPES_INT32].typeId, variant, "Int32");
}

bool OPCUAServer::addBoolVariable(const std::string &name, bool initialValue)
{
    UA_Boolean value = initialValue;
    UA_Variant variant;
    UA_Variant_init(&variant);
    UA_Variant_setScalarCopy(&variant, &value, &UA_TYPES[UA_TYPES_BOOLEAN]);

    return addVariableNode(name, UA_TYPES[UA_TYPES_BOOLEAN].typeId, variant, "Boolean");
}

bool OPCUAServer::addFloatVariable(const std::string &name, float initialValue)
{
    UA_Variant variant;
    UA_Variant_init(&variant);
    UA_Variant_setScalarCopy(&variant, &initialValue, &UA_TYPES[UA_TYPES_FLOAT]);

    return addVariableNode(name, UA_TYPES[UA_TYPES_FLOAT].typeId, variant, "Float");
}

bool OPCUAServer::addDoubleVariable(const std::string &name, double initialValue)
{
    UA_Variant variant;
    UA_Variant_init(&variant);
    UA_Variant_setScalarCopy(&variant, &initialValue, &UA_TYPES[UA_TYPES_DOUBLE]);

    return addVariableNode(name, UA_TYPES[UA_TYPES_DOUBLE].typeId, variant, "Double");
}
bool OPCUAServer::addStringVariable(const std::string &name, const std::string &initialValue, size_t maxLength)
{
    UA_String uaString = UA_STRING_ALLOC(initialValue.c_str());

    UA_Variant variant;
    UA_Variant_init(&variant);
    UA_Variant_setScalarCopy(&variant, &uaString, &UA_TYPES[UA_TYPES_STRING]);

    // uaString clearen (unsere Kopie)
    UA_String_clear(&uaString);

    // KEIN UA_Variant_clear! Ownership an attr!
    return addVariableNode(name, UA_TYPES[UA_TYPES_STRING].typeId, variant, "String");
}

// ========== Variablen mit numerischem Index erstellen ==========

// ========== Variablen mit numerischem Index erstellen ==========

bool OPCUAServer::addInt16Variable(uint16_t nsIndex, uint32_t numericId, const std::string &name, int16_t initialValue)
{
    UA_Variant variant;
    UA_Variant_init(&variant);
    UA_Variant_setScalarCopy(&variant, &initialValue, &UA_TYPES[UA_TYPES_INT16]);

    return addVariableNodeNumeric(nsIndex, numericId, name, UA_TYPES[UA_TYPES_INT16].typeId, variant, "Int16");
}

bool OPCUAServer::addInt32Variable(uint16_t nsIndex, uint32_t numericId, const std::string &name, int32_t initialValue)
{
    UA_Variant variant;
    UA_Variant_init(&variant);
    UA_Variant_setScalarCopy(&variant, &initialValue, &UA_TYPES[UA_TYPES_INT32]);

    return addVariableNodeNumeric(nsIndex, numericId, name, UA_TYPES[UA_TYPES_INT32].typeId, variant, "Int32");
}

bool OPCUAServer::addBoolVariable(uint16_t nsIndex, uint32_t numericId, const std::string &name, bool initialValue)
{
    UA_Boolean value = initialValue;
    UA_Variant variant;
    UA_Variant_init(&variant);
    UA_Variant_setScalarCopy(&variant, &value, &UA_TYPES[UA_TYPES_BOOLEAN]);

    return addVariableNodeNumeric(nsIndex, numericId, name, UA_TYPES[UA_TYPES_BOOLEAN].typeId, variant, "Boolean");
}

bool OPCUAServer::addFloatVariable(uint16_t nsIndex, uint32_t numericId, const std::string &name, float initialValue)
{
    UA_Variant variant;
    UA_Variant_init(&variant);
    UA_Variant_setScalarCopy(&variant, &initialValue, &UA_TYPES[UA_TYPES_FLOAT]);

    return addVariableNodeNumeric(nsIndex, numericId, name, UA_TYPES[UA_TYPES_FLOAT].typeId, variant, "Float");
}

bool OPCUAServer::addDoubleVariable(uint16_t nsIndex, uint32_t numericId, const std::string &name, double initialValue)
{
    UA_Variant variant;
    UA_Variant_init(&variant);
    UA_Variant_setScalarCopy(&variant, &initialValue, &UA_TYPES[UA_TYPES_DOUBLE]);

    return addVariableNodeNumeric(nsIndex, numericId, name, UA_TYPES[UA_TYPES_DOUBLE].typeId, variant, "Double");
}

bool OPCUAServer::addStringVariable(uint16_t nsIndex, uint32_t numericId, const std::string &name, const std::string &initialValue, size_t maxLength)
{
    UA_String uaString = UA_STRING_ALLOC(initialValue.c_str());

    UA_Variant variant;
    UA_Variant_init(&variant);
    UA_Variant_setScalarCopy(&variant, &uaString, &UA_TYPES[UA_TYPES_STRING]);

    UA_String_clear(&uaString);

    return addVariableNodeNumeric(nsIndex, numericId, name, UA_TYPES[UA_TYPES_STRING].typeId, variant, "String");
}

bool OPCUAServer::addStringArrayVariable(uint16_t nsIndex, uint32_t numericId, const std::string &name, const std::vector<std::string> &initialValue)
{
    // Konvertiere std::vector<std::string> zu UA_String Array
    size_t arraySize = initialValue.size();
    UA_String *uaArray = nullptr;
    
    if (arraySize > 0) {
        uaArray = (UA_String *)UA_Array_new(arraySize, &UA_TYPES[UA_TYPES_STRING]);
        for (size_t i = 0; i < arraySize; i++) {
            uaArray[i] = UA_STRING_ALLOC(initialValue[i].c_str());
        }
    }

    UA_Variant variant;
    UA_Variant_init(&variant);
    UA_Variant_setArray(&variant, uaArray, arraySize, &UA_TYPES[UA_TYPES_STRING]);

    bool result = addVariableNodeNumeric(nsIndex, numericId, name, UA_TYPES[UA_TYPES_STRING].typeId, variant, "StringArray");

    // Cleanup
    if (uaArray) {
        UA_Array_delete(uaArray, arraySize, &UA_TYPES[UA_TYPES_STRING]);
    }

    return result;
}

bool OPCUAServer::addInt16ArrayVariable(uint16_t nsIndex, uint32_t numericId, const std::string &name, const std::vector<int16_t> &initialValue)
{
    size_t arraySize = initialValue.size();
    UA_Int16 *uaArray = nullptr;
    
    if (arraySize > 0) {
        uaArray = (UA_Int16 *)UA_Array_new(arraySize, &UA_TYPES[UA_TYPES_INT16]);
        for (size_t i = 0; i < arraySize; i++) {
            uaArray[i] = initialValue[i];
        }
    }

    UA_Variant variant;
    UA_Variant_init(&variant);
    UA_Variant_setArray(&variant, uaArray, arraySize, &UA_TYPES[UA_TYPES_INT16]);

    bool result = addVariableNodeNumeric(nsIndex, numericId, name, UA_TYPES[UA_TYPES_INT16].typeId, variant, "Int16Array");

    // Cleanup
    if (uaArray) {
        UA_Array_delete(uaArray, arraySize, &UA_TYPES[UA_TYPES_INT16]);
    }

    return result;
}

// ========== Private Helper mit numerischer NodeId ==========

bool OPCUAServer::addVariableNodeNumeric(uint16_t nsIndex,
                                         uint32_t numericId,
                                         const std::string &name,
                                         UA_NodeId typeId,
                                         const UA_Variant &initialValue,
                                         const std::string &typeName)
{
    std::lock_guard<std::mutex> lock(variablesMutex_);

    // Prüfe ob Variable schon existiert
    if (variables_.find(name) != variables_.end())
    {
        std::cerr << "⚠️  Variable existiert bereits: " << name << std::endl;
        return false;
    }

    // Attribute setzen
    UA_VariableAttributes attr = UA_VariableAttributes_default;
    attr.value = initialValue;
    attr.displayName = UA_LOCALIZEDTEXT_ALLOC("en-US", name.c_str());
    attr.description = UA_LOCALIZEDTEXT_ALLOC("en-US", typeName.c_str());
    attr.accessLevel = UA_ACCESSLEVELMASK_READ | UA_ACCESSLEVELMASK_WRITE;

    // KORRIGIERT: Verwende den übergebenen nsIndex und numericId!
    UA_NodeId variableNodeId = UA_NODEID_NUMERIC(nsIndex, numericId);

    // Variable zum Server hinzufügen
    UA_StatusCode retval = UA_Server_addVariableNode(
        server_,
        variableNodeId,
        UA_NODEID_NUMERIC(0, UA_NS0ID_OBJECTSFOLDER),
        UA_NODEID_NUMERIC(0, UA_NS0ID_ORGANIZES),
        UA_QUALIFIEDNAME_ALLOC(nsIndex, name.c_str()),  // ← Auch hier nsIndex verwenden!
        UA_NODEID_NUMERIC(0, UA_NS0ID_BASEDATAVARIABLETYPE),
        attr,
        nullptr,
        nullptr);

    // Cleanup
    UA_VariableAttributes_clear(&attr);

    if (retval != UA_STATUSCODE_GOOD)
    {
        std::cerr << "❌ Fehler beim Erstellen der Variable: " << name 
                  << " (ns=" << nsIndex << ";i=" << numericId << ") - Status: " << retval << std::endl;
        return false;
    }

    // Variable registrieren
    variables_[name] = {variableNodeId, typeName};

    std::cout << "✅ Variable erstellt: ns=" << nsIndex << ";i=" << numericId 
              << " - " << name << " (" << typeName << ")" << std::endl;
    return true;
}
// ========== Variablen lesen ==========

bool OPCUAServer::readInt16(const std::string &name, int16_t &value)
{
    UA_NodeId nodeId;
    if (!getNodeId(name, nodeId))
        return false;

    UA_Variant variant;
    UA_StatusCode retval = UA_Server_readValue(server_, nodeId, &variant);

    if (retval == UA_STATUSCODE_GOOD && variant.type == &UA_TYPES[UA_TYPES_INT16])
    {
        value = *(int16_t *)variant.data;
        UA_Variant_clear(&variant);
        return true;
    }

    UA_Variant_clear(&variant);
    return false;
}

bool OPCUAServer::readInt32(const std::string &name, int32_t &value)
{
    UA_NodeId nodeId;
    if (!getNodeId(name, nodeId))
        return false;

    UA_Variant variant;
    UA_StatusCode retval = UA_Server_readValue(server_, nodeId, &variant);

    if (retval == UA_STATUSCODE_GOOD && variant.type == &UA_TYPES[UA_TYPES_INT32])
    {
        value = *(int32_t *)variant.data;
        UA_Variant_clear(&variant);
        return true;
    }

    UA_Variant_clear(&variant);
    return false;
}

bool OPCUAServer::readBool(const std::string &name, bool &value)
{
    UA_NodeId nodeId;
    if (!getNodeId(name, nodeId))
        return false;

    UA_Variant variant;
    UA_StatusCode retval = UA_Server_readValue(server_, nodeId, &variant);

    if (retval == UA_STATUSCODE_GOOD && variant.type == &UA_TYPES[UA_TYPES_BOOLEAN])
    {
        value = *(UA_Boolean *)variant.data;
        UA_Variant_clear(&variant);
        return true;
    }

    UA_Variant_clear(&variant);
    return false;
}

bool OPCUAServer::readFloat(const std::string &name, float &value)
{
    UA_NodeId nodeId;
    if (!getNodeId(name, nodeId))
        return false;

    UA_Variant variant;
    UA_StatusCode retval = UA_Server_readValue(server_, nodeId, &variant);

    if (retval == UA_STATUSCODE_GOOD && variant.type == &UA_TYPES[UA_TYPES_FLOAT])
    {
        value = *(float *)variant.data;
        UA_Variant_clear(&variant);
        return true;
    }

    UA_Variant_clear(&variant);
    return false;
}

bool OPCUAServer::readDouble(const std::string &name, double &value)
{
    UA_NodeId nodeId;
    if (!getNodeId(name, nodeId))
        return false;

    UA_Variant variant;
    UA_StatusCode retval = UA_Server_readValue(server_, nodeId, &variant);

    if (retval == UA_STATUSCODE_GOOD && variant.type == &UA_TYPES[UA_TYPES_DOUBLE])
    {
        value = *(double *)variant.data;
        UA_Variant_clear(&variant);
        return true;
    }

    UA_Variant_clear(&variant);
    return false;
}

bool OPCUAServer::readString(const std::string &name, std::string &value)
{
    UA_NodeId nodeId;
    if (!getNodeId(name, nodeId))
        return false;

    UA_Variant variant;
    UA_StatusCode retval = UA_Server_readValue(server_, nodeId, &variant);

    if (retval == UA_STATUSCODE_GOOD && variant.type == &UA_TYPES[UA_TYPES_STRING])
    {
        UA_String *uaString = (UA_String *)variant.data;
        value = std::string((char *)uaString->data, uaString->length);
        UA_Variant_clear(&variant);
        return true;
    }

    UA_Variant_clear(&variant);
    return false;
}

// ========== Variablen schreiben ==========

bool OPCUAServer::writeInt16(const std::string &name, int16_t value)
{
    UA_NodeId nodeId;
    if (!getNodeId(name, nodeId))
        return false;

    UA_Variant variant;
    UA_Variant_init(&variant);
    UA_Variant_setScalar(&variant, &value, &UA_TYPES[UA_TYPES_INT16]);

    UA_StatusCode retval = UA_Server_writeValue(server_, nodeId, variant);
    return retval == UA_STATUSCODE_GOOD;
}

bool OPCUAServer::writeInt32(const std::string &name, int32_t value)
{
    UA_NodeId nodeId;
    if (!getNodeId(name, nodeId))
        return false;

    UA_Variant variant;
    UA_Variant_init(&variant);
    UA_Variant_setScalar(&variant, &value, &UA_TYPES[UA_TYPES_INT32]);

    UA_StatusCode retval = UA_Server_writeValue(server_, nodeId, variant);
    return retval == UA_STATUSCODE_GOOD;
}

bool OPCUAServer::writeBool(const std::string &name, bool value)
{
    UA_NodeId nodeId;
    if (!getNodeId(name, nodeId))
        return false;

    UA_Boolean uaValue = value;
    UA_Variant variant;
    UA_Variant_init(&variant);
    UA_Variant_setScalar(&variant, &uaValue, &UA_TYPES[UA_TYPES_BOOLEAN]);

    UA_StatusCode retval = UA_Server_writeValue(server_, nodeId, variant);
    return retval == UA_STATUSCODE_GOOD;
}

bool OPCUAServer::writeFloat(const std::string &name, float value)
{
    UA_NodeId nodeId;
    if (!getNodeId(name, nodeId))
        return false;

    UA_Variant variant;
    UA_Variant_init(&variant);
    UA_Variant_setScalar(&variant, &value, &UA_TYPES[UA_TYPES_FLOAT]);

    UA_StatusCode retval = UA_Server_writeValue(server_, nodeId, variant);
    return retval == UA_STATUSCODE_GOOD;
}

bool OPCUAServer::writeDouble(const std::string &name, double value)
{
    UA_NodeId nodeId;
    if (!getNodeId(name, nodeId))
        return false;

    UA_Variant variant;
    UA_Variant_init(&variant);
    UA_Variant_setScalar(&variant, &value, &UA_TYPES[UA_TYPES_DOUBLE]);

    UA_StatusCode retval = UA_Server_writeValue(server_, nodeId, variant);
    return retval == UA_STATUSCODE_GOOD;
}

bool OPCUAServer::writeString(const std::string &name, const std::string &value)
{
    UA_NodeId nodeId;
    if (!getNodeId(name, nodeId))
        return false;

    UA_String uaString = UA_STRING_ALLOC(value.c_str());

    UA_Variant variant;
    UA_Variant_init(&variant);
    UA_Variant_setScalar(&variant, &uaString, &UA_TYPES[UA_TYPES_STRING]);

    UA_StatusCode retval = UA_Server_writeValue(server_, nodeId, variant);

    UA_String_clear(&uaString);
    return retval == UA_STATUSCODE_GOOD;
}

// ========== Utility ==========

void OPCUAServer::printVariables() const
{
    std::lock_guard<std::mutex> lock(variablesMutex_);

    std::cout << "\n========== OPC UA Variablen ==========" << std::endl;
    std::cout << "Server: " << getEndpointUrl() << std::endl;
    std::cout << "Anzahl: " << variables_.size() << std::endl;
    std::cout << "---------------------------------------" << std::endl;

    for (const auto &[name, info] : variables_)
    {
        std::cout << "  • " << name << " (" << info.typeName << ")" << std::endl;
    }

    std::cout << "======================================\n"
              << std::endl;
}

bool OPCUAServer::removeVariable(const std::string &name)
{
    std::lock_guard<std::mutex> lock(variablesMutex_);

    auto it = variables_.find(name);
    if (it == variables_.end())
    {
        return false;
    }

    UA_Server_deleteNode(server_, it->second.nodeId, true);
    variables_.erase(it);

    return true;
}

// ========== Private Helpers ==========

bool OPCUAServer::getNodeId(const std::string &name, UA_NodeId &nodeId) const
{
    std::lock_guard<std::mutex> lock(variablesMutex_);

    auto it = variables_.find(name);
    if (it == variables_.end())
    {
        std::cerr << "❌ Variable nicht gefunden: " << name << std::endl;
        return false;
    }

    nodeId = it->second.nodeId;
    return true;
}

bool OPCUAServer::addVariableNode(const std::string &name,
                                  UA_NodeId typeId,
                                  const UA_Variant &initialValue,
                                  const std::string &typeName)
{
    std::lock_guard<std::mutex> lock(variablesMutex_);

    // Prüfe ob Variable schon existiert
    if (variables_.find(name) != variables_.end())
    {
        std::cerr << "⚠️  Variable existiert bereits: " << name << std::endl;
        return false;
    }

    // Attribute setzen
    UA_VariableAttributes attr = UA_VariableAttributes_default;
    attr.value = initialValue;
    attr.displayName = UA_LOCALIZEDTEXT_ALLOC("en-US", name.c_str());
    attr.description = UA_LOCALIZEDTEXT_ALLOC("en-US", typeName.c_str());
    attr.accessLevel = UA_ACCESSLEVELMASK_READ | UA_ACCESSLEVELMASK_WRITE;

    // NodeId generieren
    UA_NodeId variableNodeId = UA_NODEID_STRING_ALLOC(1, name.c_str());

    // Variable zum Server hinzufügen
    UA_StatusCode retval = UA_Server_addVariableNode(
        server_,
        variableNodeId,
        UA_NODEID_NUMERIC(0, UA_NS0ID_OBJECTSFOLDER),
        UA_NODEID_NUMERIC(0, UA_NS0ID_ORGANIZES),
        UA_QUALIFIEDNAME_ALLOC(1, name.c_str()),
        UA_NODEID_NUMERIC(0, UA_NS0ID_BASEDATAVARIABLETYPE),
        attr,
        nullptr,
        nullptr);

    // Cleanup
    UA_VariableAttributes_clear(&attr);

    if (retval != UA_STATUSCODE_GOOD)
    {
        std::cerr << "❌ Fehler beim Erstellen der Variable: " << name << std::endl;
        UA_NodeId_clear(&variableNodeId);
        return false;
    }

    // Variable registrieren
    variables_[name] = {variableNodeId, typeName};

    std::cout << "✅ Variable erstellt: " << name << " (" << typeName << ")" << std::endl;
    return true;
}