#include <cstring>  // für std::memcpy
#include "interfaces/OPC_UA.h"
// iostream entfernt, keine Terminal-Ausgaben mehr
// #include <open62541/client.h>

// Konstruktor: Initialisiert den Client mit der Default-Konfiguration ohne Sicherheitsmaßnahmen
OPCClient::OPCClient(const std::string &serverURL)
    : serverURL(serverURL), client(nullptr)
{
    client = UA_Client_new();
    // Hier wird die Standardkonfiguration gesetzt, welche keine Verschlüsselung oder Signierung nutzt.
    UA_ClientConfig *cfg = UA_Client_getConfig(client);
    UA_ClientConfig_setDefault(cfg);

    // Timeouts begrenzen: Ohne das blockiert UA_Client_connect() bei einem
    // nicht erreichbaren Endpunkt sehr lange (interner Retry). Der Dienst soll
    // aber zügig weiterlaufen und auf Einstellungsänderungen reagieren können.
    cfg->timeout = 3000;                    // ms je Anfrage
    cfg->connectivityCheckInterval = 2000;  // ms
}

// Destruktor: Trennt die Verbindung und löscht den Client
OPCClient::~OPCClient()
{
    disconnect();
    if (client)
    {
        UA_Client_delete(client);
        client = nullptr;
    }
}

// Stellt die Verbindung zum OPC UA Server her
bool OPCClient::connect()
{
    UA_StatusCode status = UA_Client_connect(client, serverURL.c_str());
    if (status != UA_STATUSCODE_GOOD)
    {
        return false;
    }
    return true;
}

// Trennt die Verbindung zum OPC UA Server
bool OPCClient::disconnect()
{
    UA_Client_disconnect(client);
    return true;
}
// Hilfsfunktion zum Parsen eines NodeId-Strings (z.B. "ns=4;s=MAIN.nMyCounter")
/// Robustes parseNodeId mit Unterstützung für Numeric und String
UA_NodeId OPCClient::parseNodeId(const std::string &nodeIdStr)
{
    UA_NodeId node;

    // Variante A: Nutzung der Bibliotheksfunktion (open62541 ≥ 1.3)
#if UA_VERSION_MAJOR > 1 || (UA_VERSION_MAJOR == 1 && UA_VERSION_MINOR >= 3)
    if (UA_NodeId_parse(&node, UA_STRING((char *)nodeIdStr.c_str())) == UA_STATUSCODE_GOOD)
        return node;
#else
    // Variante B: Eigenes Parsen für ältere Versionen
    uint16_t nsIndex = 0;
    // z.B. ns=2;i=4005
    uint32_t numericId = 0;
    if (sscanf(nodeIdStr.c_str(), "ns=%hu;i=%u", &nsIndex, &numericId) == 2)
    {
        return UA_NODEID_NUMERIC(nsIndex, numericId);
    }
    // z.B. ns=2;s=MeineVariable
    char identifier[256] = {0};
    if (sscanf(nodeIdStr.c_str(), "ns=%hu;s=%255[^//n]", &nsIndex, identifier) == 2)
    {
        return UA_NODEID_STRING_ALLOC(nsIndex, identifier);
    }
#endif

    // Wenn alles fehlschlägt, zurück zu NULL-NodeId
    UA_NodeId_init(&node); // entspricht UA_NODEID_NULL
    return node;
}

size_t getTypeIndex(const UA_DataType *dataType)
{
    // Berechne die Anzahl der Elemente im UA_TYPES-Array:
    size_t numTypes = sizeof(UA_TYPES) / sizeof(UA_DataType);
    for (size_t i = 0; i < numTypes; ++i)
    {
        if (dataType == &UA_TYPES[i])
        {
            return i;
        }
    }
    return static_cast<size_t>(-1); // Falls nicht gefunden
}
std::tuple<bool, std::optional<long>> OPCClient::registerVariable(const std::string &name,
                                                                  const DataType type,
                                                                  const std::string connectionString)
{
    // Infos zusammenstellen
    VariableInfo var;
    var.fullName = connectionString;
    var.name = name;
    var.type = type;

    UA_Variant value;
    UA_Variant_init(&value);

    UA_NodeId nodeId = parseNodeId(var.fullName);
    UA_StatusCode rc = UA_Client_readValueAttribute(client, nodeId, &value);
    if (rc != UA_STATUSCODE_GOOD)
    {
        UA_Variant_clear(&value);
        return {false, std::nullopt};
    }

    // 1) Den passenden Datentyp-String ermitteln
    bool typeOk = false;
    switch (type)
    {
    case DataType::Int16:
        typeOk = UA_Variant_hasScalarType(&value, &UA_TYPES[UA_TYPES_INT16]);
        break;
    case DataType::Int32:
        typeOk = UA_Variant_hasScalarType(&value, &UA_TYPES[UA_TYPES_INT32]);
        break;
    case DataType::Float:
        typeOk = UA_Variant_hasScalarType(&value, &UA_TYPES[UA_TYPES_FLOAT]);
        break;
    case DataType::Double:
        typeOk = UA_Variant_hasScalarType(&value, &UA_TYPES[UA_TYPES_DOUBLE]);
        break;
    case DataType::Bool:
        typeOk = UA_Variant_hasScalarType(&value, &UA_TYPES[UA_TYPES_BOOLEAN]);
        break;
    case DataType::String:
        typeOk = UA_Variant_hasScalarType(&value, &UA_TYPES[UA_TYPES_STRING]);
        break;

    case DataType::Int16Array:
        typeOk = UA_Variant_hasArrayType(&value, &UA_TYPES[UA_TYPES_INT16]);
        break;
    case DataType::Int32Array:
        typeOk = UA_Variant_hasArrayType(&value, &UA_TYPES[UA_TYPES_INT32]);
        break;
    case DataType::FloatArray:
        typeOk = UA_Variant_hasArrayType(&value, &UA_TYPES[UA_TYPES_FLOAT]);
        break;
    case DataType::DoubleArray:
        typeOk = UA_Variant_hasArrayType(&value, &UA_TYPES[UA_TYPES_DOUBLE]);
        break;
    case DataType::BoolArray:
        typeOk = UA_Variant_hasArrayType(&value, &UA_TYPES[UA_TYPES_BOOLEAN]);
        break;
    case DataType::StringArray:
        typeOk = UA_Variant_hasArrayType(&value, &UA_TYPES[UA_TYPES_STRING]);
        break;
    case DataType::WStringArray: // Wide String Array
        typeOk = UA_Variant_hasArrayType(&value, &UA_TYPES[UA_TYPES_BYTESTRING]);
        break;

    default:
        break;
    }
    if (!typeOk)
    {
        UA_Variant_clear(&value);
        return {false, std::nullopt}; // Fehler beim Typ
    }

    return {true, std::nullopt};
}
std::tuple<int16_t, bool, int> OPCClient::readInt16(const VariableInfo &var)
{
    if (!client)
    {
        return {0, false, 1}; // Fehlercode 1 = nicht verbunden
    }
    // Knoten referenzieren
    UA_NodeId nodeId = parseNodeId(var.fullName);
    if (UA_NodeId_equal(&nodeId, &UA_NODEID_NULL))
    {
        return {0, false, 2}; // Fehlercode 2 = ungültige NodeId
    }
    UA_Variant value;
    UA_Variant_init(&value);
    UA_StatusCode retval = UA_Client_readValueAttribute(client, nodeId, &value);
    if (retval != UA_STATUSCODE_GOOD)
    {
        UA_Variant_clear(&value);
        return {0, false, 3}; // Fehlercode 3 = Read failed
    }
    // Prüfen und umwandeln auf Int16
    if (value.type == &UA_TYPES[UA_TYPES_INT16] && value.arrayLength == 0)
    {
        int16_t result = *static_cast<int16_t *>(value.data);
        UA_Variant_clear(&value);
        return {result, true, 0};
    }

    UA_Variant_clear(&value);
    return {0, false, 5}; // Fehlercode 5 = unerwarteter Typ
}

std::tuple<float, bool, int> OPCClient::readFloat(const VariableInfo &var)
{
    if (!client)
    {
        return {0.0f, false, 1}; // Fehlercode 1 = nicht verbunden
    }
    UA_NodeId nodeId = parseNodeId(var.fullName);
    if (UA_NodeId_equal(&nodeId, &UA_NODEID_NULL))
    {
        return {0.0f, false, 2}; // Fehlercode 2 = ungültige NodeId
    }

    UA_Variant value;
    UA_Variant_init(&value);
    UA_StatusCode retval = UA_Client_readValueAttribute(client, nodeId, &value);
    if (retval != UA_STATUSCODE_GOOD)
    {
        UA_Variant_clear(&value);
        return {0.0f, false, 3}; // Fehlercode 3 = Read failed
    }

    if (value.type == &UA_TYPES[UA_TYPES_FLOAT] && value.arrayLength == 0)
    {
        float result = *static_cast<float *>(value.data);
        UA_Variant_clear(&value);
        return {result, true, 0};
    }

    UA_Variant_clear(&value);
    return {0.0f, false, 5}; // Fehlercode 5 = unerwarteter Typ
}

std::tuple<bool, bool, int> OPCClient::readBool(const VariableInfo &var)
{
    if (!client)
    {
        return {false, false, 1};
    }
    UA_NodeId nodeId = parseNodeId(var.fullName);
    if (UA_NodeId_equal(&nodeId, &UA_NODEID_NULL))
    {
        return {false, false, 2};
    }

    UA_Variant value;
    UA_Variant_init(&value);
    UA_StatusCode retval = UA_Client_readValueAttribute(client, nodeId, &value);
    if (retval != UA_STATUSCODE_GOOD)
    {
        UA_Variant_clear(&value);
        return {false, false, 3};
    }

    if (value.type == &UA_TYPES[UA_TYPES_BOOLEAN] && value.arrayLength == 0)
    {
        bool result = *static_cast<bool *>(value.data);
        UA_Variant_clear(&value);
        return {result, true, 0};
    }

    UA_Variant_clear(&value);
    return {false, false, 5};
}

std::tuple<double, bool, int> OPCClient::readDouble(const VariableInfo &var)
{
    if (!client)
    {
        return {0.0, false, 1};
    }
    UA_NodeId nodeId = parseNodeId(var.fullName);
    if (UA_NodeId_equal(&nodeId, &UA_NODEID_NULL))
    {
        return {0.0, false, 2};
    }

    UA_Variant value;
    UA_Variant_init(&value);
    UA_StatusCode retval = UA_Client_readValueAttribute(client, nodeId, &value);
    if (retval != UA_STATUSCODE_GOOD)
    {
        UA_Variant_clear(&value);
        return {0.0, false, 3};
    }

    if (value.type == &UA_TYPES[UA_TYPES_DOUBLE] && value.arrayLength == 0)
    {
        double result = *static_cast<double *>(value.data);
        UA_Variant_clear(&value);
        return {result, true, 0};
    }

    UA_Variant_clear(&value);
    return {0.0, false, 5};
}

std::tuple<int32_t, bool, int> OPCClient::readInt32(const VariableInfo &var)
{
    if (!client)
    {
        return {0, false, 1};
    }
    UA_NodeId nodeId = parseNodeId(var.fullName);
    if (UA_NodeId_equal(&nodeId, &UA_NODEID_NULL))
    {
        return {0, false, 2};
    }

    UA_Variant value;
    UA_Variant_init(&value);
    UA_StatusCode retval = UA_Client_readValueAttribute(client, nodeId, &value);
    if (retval != UA_STATUSCODE_GOOD)
    {
        UA_Variant_clear(&value);
        return {0, false, 3};
    }

    if (value.type == &UA_TYPES[UA_TYPES_INT32] && value.arrayLength == 0)
    {
        int32_t result = *static_cast<int32_t *>(value.data);
        UA_Variant_clear(&value);
        return {result, true, 0};
    }

    UA_Variant_clear(&value);
    return {0, false, 5};
}

std::tuple<std::string, bool, int> OPCClient::readString(const VariableInfo &var)
{
    if (!client)
    {
        return {"", false, 1};
    }
    UA_NodeId nodeId = parseNodeId(var.fullName);
    if (UA_NodeId_equal(&nodeId, &UA_NODEID_NULL))
    {
        return {"", false, 2};
    }

    UA_Variant value;
    UA_Variant_init(&value);
    UA_StatusCode retval = UA_Client_readValueAttribute(client, nodeId, &value);
    if (retval != UA_STATUSCODE_GOOD)
    {
        UA_Variant_clear(&value);
        return {"", false, 3};
    }

    if (value.type == &UA_TYPES[UA_TYPES_STRING] && value.arrayLength == 0)
    {
        UA_String *strVal = static_cast<UA_String *>(value.data);
        std::string result(reinterpret_cast<char *>(strVal->data), strVal->length);
        UA_Variant_clear(&value);
        return {result, true, 0};
    }

    UA_Variant_clear(&value);
    return {"", false, 5};
}

std::tuple<std::wstring, bool, int> OPCClient::readWString(const VariableInfo &var)
{
    if (!client)
    {
        return {L"", false, 1};
    }

    UA_NodeId nodeId = parseNodeId(var.fullName);
    if (UA_NodeId_equal(&nodeId, &UA_NODEID_NULL))
    {
        return {L"", false, 2};
    }

    UA_Variant value;
    UA_Variant_init(&value);
    UA_StatusCode retval =
        UA_Client_readValueAttribute(client, nodeId, &value);
    if (retval != UA_STATUSCODE_GOOD)
    {
        UA_Variant_clear(&value);
        return {L"", false, 3};
    }

    // 1) Prüfen, ob es ein Scalar ByteString (WSTRING) ist
    if (value.type == &UA_TYPES[UA_TYPES_BYTESTRING] && value.arrayLength == 0)
    {
        // scalar ByteString (also WSTRING)
        UA_ByteString *byteStr = static_cast<UA_ByteString *>(value.data);
        std::wstring result;
        if (byteStr->data && byteStr->length > 0)
        {
            size_t wcharCount = byteStr->length / sizeof(wchar_t);
            const wchar_t *wptr = reinterpret_cast<const wchar_t *>(byteStr->data);
            result.assign(wptr, wptr + wcharCount);
        }
        UA_Variant_clear(&value);
        return {result, true, 0};
    }

    UA_Variant_clear(&value);
    return {L"", false, 5};
}

std::tuple<std::string, bool, int> OPCClient::readChars(const VariableInfo &var, int length)
{
    if (!client)
    {
        return {"", false, 1};
    }
    UA_NodeId nodeId = parseNodeId(var.fullName);
    if (UA_NodeId_equal(&nodeId, &UA_NODEID_NULL))
    {
        return {"", false, 2};
    }

    UA_Variant value;
    UA_Variant_init(&value);
    UA_StatusCode retval = UA_Client_readValueAttribute(client, nodeId, &value);
    if (retval != UA_STATUSCODE_GOOD)
    {
        UA_Variant_clear(&value);
        return {"", false, 3};
    }

    if (value.type == &UA_TYPES[UA_TYPES_STRING] && value.arrayLength == 0)
    {
        UA_ByteString *byteStr = static_cast<UA_ByteString *>(value.data);
        if (byteStr->length > 0)
        {
            int copyLen = (std::min)(length, static_cast<int>(byteStr->length));
            std::string result(reinterpret_cast<char *>(byteStr->data), copyLen);
            UA_Variant_clear(&value);
            return {result, true, 0};
        }
    }

    UA_Variant_clear(&value);
    return {"", false, 5};
}

/**
 * @brief Writes an Int16 value to a specified node.
 */
std::tuple<bool, int> OPCClient::writeInt16(const VariableInfo &var, int16_t valueToWrite)
{

    UA_Variant variant;
    UA_Variant_init(&variant);
    UA_Variant_setScalar(&variant, &valueToWrite, &UA_TYPES[UA_TYPES_INT16]);

    UA_NodeId node = parseNodeId(var.fullName);
    if (UA_NodeId_equal(&node, &UA_NODEID_NULL))
    {
        return {false, 1};
    }

    UA_StatusCode status = UA_Client_writeValueAttribute(client, node, &variant);

    if (status == UA_STATUSCODE_GOOD)
    {
        return {true, 0};
    }
    else
    {
        return {false, 2};
    }
}
std::tuple<bool, int> OPCClient::writeFloat(const VariableInfo &var, float valueToWrite)
{

    UA_Variant variant;
    UA_Variant_init(&variant);
    UA_Variant_setScalar(&variant, &valueToWrite, &UA_TYPES[UA_TYPES_FLOAT]);

    UA_NodeId node = parseNodeId(var.fullName);
    if (UA_NodeId_equal(&node, &UA_NODEID_NULL))
    {
        return {false, 1};
    }

    UA_StatusCode status = UA_Client_writeValueAttribute(client, node, &variant);

    if (status == UA_STATUSCODE_GOOD)
    {
        return {true, 0};
    }
    else
    {
        return {false, 2};
    }
}

std::tuple<bool, int> OPCClient::writeInt32(const VariableInfo &var, int32_t valueToWrite)
{
    UA_Variant variant;
    UA_Variant_init(&variant);
    UA_Variant_setScalar(&variant, &valueToWrite, &UA_TYPES[UA_TYPES_INT32]);

    UA_NodeId node = parseNodeId(var.fullName);
    if (UA_NodeId_equal(&node, &UA_NODEID_NULL))
    {
        return {false, 1};
    }

    UA_StatusCode status = UA_Client_writeValueAttribute(client, node, &variant);

    if (status == UA_STATUSCODE_GOOD)
    {
        return {true, 0};
    }
    else
    {
        return {false, 2};
    }
}

std::tuple<bool, int> OPCClient::writeDouble(const VariableInfo &var, double valueToWrite)
{

    UA_Variant variant;
    UA_Variant_init(&variant);
    UA_Variant_setScalar(&variant, &valueToWrite, &UA_TYPES[UA_TYPES_DOUBLE]);

    UA_NodeId node = parseNodeId(var.fullName);
    if (UA_NodeId_equal(&node, &UA_NODEID_NULL))
    {
        return {false, 1};
    }

    UA_StatusCode status = UA_Client_writeValueAttribute(client, node, &variant);

    if (status == UA_STATUSCODE_GOOD)
    {
        return {true, 0};
    }
    else
    {
        return {false, 2};
    }
}

std::tuple<bool, int> OPCClient::writeBool(const VariableInfo &var, bool valueToWrite)
{

    UA_Variant variant;
    UA_Variant_init(&variant);
    UA_Variant_setScalar(&variant, &valueToWrite, &UA_TYPES[UA_TYPES_BOOLEAN]);

    UA_NodeId node = parseNodeId(var.fullName);
    if (UA_NodeId_equal(&node, &UA_NODEID_NULL))
    {
        return {false, 1};
    }

    UA_StatusCode status = UA_Client_writeValueAttribute(client, node, &variant);

    if (status == UA_STATUSCODE_GOOD)
    {
        return {true, 0};
    }
    else
    {
        return {false, 2};
    }
}

std::tuple<bool, int> OPCClient::writeChars(const VariableInfo &var, const std::string &valueToWrite, const int length)
{
    UA_Variant variant;
    UA_Variant_init(&variant);

    // Statt UA_ByteString jetzt UA_String verwenden
    UA_String uaStr;
    uaStr.length = static_cast<UA_UInt32>(length);
    uaStr.data = (UA_Byte *)UA_malloc(length * sizeof(UA_Byte));
    if (uaStr.data == nullptr)
    {
        return {false, 1};
    }
    memcpy(uaStr.data, valueToWrite.c_str(), length);

    // Als STRING ins Variant packen
    UA_Variant_setScalar(&variant, &uaStr, &UA_TYPES[UA_TYPES_STRING]);

    UA_NodeId node = parseNodeId(var.fullName);

    if (UA_NodeId_equal(&node, &UA_NODEID_NULL))
    {
        UA_String_clear(&uaStr);
        return {false, 1};
    }

    UA_StatusCode status = UA_Client_writeValueAttribute(client, node, &variant);

    UA_String_clear(&uaStr);

    return {status == UA_STATUSCODE_GOOD, status == UA_STATUSCODE_GOOD ? 0 : 2};
}

std::tuple<bool, int> OPCClient::writeWString(const VariableInfo &var,
                                              const std::wstring &valueToWrite)
{
    // 1) NodeId parsen
    UA_NodeId nodeId = parseNodeId(var.fullName);
    if (UA_NodeId_equal(&nodeId, &UA_NODEID_NULL))
    {
        return {false, 1};
    }

    // 2) ByteString aus std::wstring anlegen
    UA_ByteString bs;
    bs.length = static_cast<UA_UInt32>(valueToWrite.size() * sizeof(wchar_t));
    bs.data = static_cast<UA_Byte *>(UA_malloc(bs.length));
    if (!bs.data && bs.length > 0)
    {
        return {false, 2};
    }
    std::memcpy(bs.data,
                valueToWrite.data(),
                static_cast<size_t>(bs.length));

    // 3) Variant initialisieren und scalar ByteString setzen
    UA_Variant variant;
    UA_Variant_init(&variant);
    UA_StatusCode status = UA_Variant_setScalarCopy(
        &variant, &bs, &UA_TYPES[UA_TYPES_BYTESTRING]);
    // Die Kopie liegt jetzt im Variant, den temporären bs löschen
    UA_ByteString_clear(&bs);

    if (status != UA_STATUSCODE_GOOD)
    {
        UA_Variant_clear(&variant);
        return {false, 3};
    }

    // 4) Schreiben
    status = UA_Client_writeValueAttribute(client, nodeId, &variant);
    UA_Variant_clear(&variant);

    if (status != UA_STATUSCODE_GOOD)
    {
        return {false, 4};
    }

    return {true, 0};
}

/**
 * @brief Browse the OPC UA server's address space starting from the Objects folder.
 *
 * This function initiates the browsing of the address space from the Objects folder
 * (Namespace 0, NodeId: UA_NS0ID_OBJECTSFOLDER) and traverses the address space recursively.
 */
void OPCClient::browseAddressSpace()
{
    // Startpunkt: Objects folder (Namespace 0, NodeId: UA_NS0ID_OBJECTSFOLDER)
    UA_NodeId objectsNode = UA_NODEID_NUMERIC(0, UA_NS0ID_OBJECTSFOLDER);
    browseRecursive(objectsNode, 0);
}

// Private Hilfsfunktion für rekursives Browsen.
void OPCClient::browseRecursive(const UA_NodeId &nodeId, int indent)
{
    UA_BrowseRequest bReq;
    UA_BrowseRequest_init(&bReq);
    bReq.requestHeader.timestamp = UA_DateTime_now();
    bReq.nodesToBrowse = static_cast<UA_BrowseDescription *>(UA_Array_new(1, &UA_TYPES[UA_TYPES_BROWSEDESCRIPTION]));
    bReq.nodesToBrowseSize = 1;
    UA_BrowseDescription_init(&bReq.nodesToBrowse[0]);
    UA_NodeId_copy(&nodeId, &bReq.nodesToBrowse[0].nodeId);
    bReq.nodesToBrowse[0].resultMask = UA_BROWSERESULTMASK_ALL;

    UA_BrowseResponse bResp = UA_Client_Service_browse(client, bReq);
    if (bResp.responseHeader.serviceResult == UA_STATUSCODE_GOOD)
    {
        for (size_t i = 0; i < bResp.resultsSize; i++)
        {
            for (size_t j = 0; j < bResp.results[i].referencesSize; j++)
            {
                UA_ReferenceDescription *ref = &(bResp.results[i].references[j]);

                // Rekursives Browsen, wenn der Knoten eine gültige Referenz hat
                if (!UA_NodeId_equal(&ref->nodeId.nodeId, &UA_NODEID_NULL))
                {
                    UA_NodeId childId;
                    UA_NodeId_init(&childId);
                    UA_NodeId_copy(&ref->nodeId.nodeId, &childId);
                    browseRecursive(childId, indent + 2);
                    UA_NodeId_clear(&childId);
                }
            }
        }
    }
    UA_BrowseRequest_clear(&bReq);
    UA_BrowseResponse_clear(&bResp);
}

bool OPCClient::isNamespaceUriPresent(const std::string &namespaceUri)
{
    UA_String uri = UA_STRING((char *)namespaceUri.c_str());
    UA_UInt16 nsIndex = 0;
    UA_StatusCode status = UA_Client_NamespaceGetIndex(client, &uri, &nsIndex);
    if (status != UA_STATUSCODE_GOOD)
    {
        return false;
    }
    return true;
}

void OPCClient::printNamespaceArray()
{
    if (!client)
    {
        return;
    }
    UA_Variant value;
    UA_Variant_init(&value);
    UA_StatusCode retval = UA_Client_readValueAttribute(
        client, UA_NODEID_NUMERIC(0, UA_NS0ID_SERVER_NAMESPACEARRAY), &value);
    if (retval != UA_STATUSCODE_GOOD)
    {
        UA_Variant_clear(&value);
        return;
    }
    // Prüfen, ob es sich um ein String-Array handelt
    if (value.type != &UA_TYPES[UA_TYPES_STRING] || !value.arrayLength)
    {
        UA_Variant_clear(&value);
        return;
    }
    size_t count = value.arrayLength;
    UA_String *namespaces = static_cast<UA_String *>(value.data);
    const size_t maxPrint = 10;
    for (size_t i = 0; i < count && i < maxPrint; ++i)
    {
        (void)namespaces; // keine Ausgabe gewünscht
    }
    UA_Variant_clear(&value);
}

/*
void OPCClient::browseNamespaceNodes( const UA_NodeId& nodeId, uint16_t nsIndex, int indent = 0) {
    // auskommentierter Code belassen
}
*/

// Liest das AccessLevel-Attribut eines Knotens. Es wird NICHT geschrieben -
// die Schreibbarkeit ergibt sich aus der Angabe des Servers.
bool OPCClient::readAccessLevel(const std::string &nodeIdStr, bool &readable, bool &writable)
{
    readable = false;
    writable = false;
    if (!client)
        return false;

    UA_NodeId node = parseNodeId(nodeIdStr);
    UA_Byte accessLevel = 0;
    UA_StatusCode st = UA_Client_readAccessLevelAttribute(client, node, &accessLevel);
    UA_NodeId_clear(&node);

    if (st != UA_STATUSCODE_GOOD)
        return false;  // Knoten existiert nicht / nicht lesbar

    readable = (accessLevel & UA_ACCESSLEVELMASK_READ) != 0;
    writable = (accessLevel & UA_ACCESSLEVELMASK_WRITE) != 0;
    return true;
}

// Echter Sitzungsstatus des Clients. Verlaesslicher als der Rueckgabewert eines
// Schreibvorgangs: Writes koennen "erfolgreich" melden, obwohl der Server weg ist.
bool OPCClient::isConnected()
{
    if (!client)
        return false;
    UA_SecureChannelState channelState;
    UA_SessionState sessionState;
    UA_StatusCode connectStatus;
    UA_Client_getState(client, &channelState, &sessionState, &connectStatus);
    return sessionState == UA_SESSIONSTATE_ACTIVATED &&
           channelState == UA_SECURECHANNELSTATE_OPEN;
}
