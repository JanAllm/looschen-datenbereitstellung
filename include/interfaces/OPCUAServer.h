#pragma once

#include <open62541.h>
#include <string>
#include <thread>
#include <atomic>
#include <mutex>
#include <unordered_map>
#include <memory>
#include <functional>
#include <vector>

/**
 * @brief Cross-platform OPC UA Server basierend auf open62541
 *
 * Erlaubt das dynamische Hinzufügen von Variablen und deren Manipulation.
 * Thread-safe und geeignet für automatisierte Tests sowie Produktivbetrieb.
 */
class OPCUAServer
{
public:
    /**
     * @brief Konstruktor
     * @param port Server-Port (Standard: 4840)
     * @param serverName Server-Name (erscheint in Clients)
     */
    explicit OPCUAServer(uint16_t port = 4840, const std::string &serverName = "TestOPCUAServer");

    /**
     * @brief Destruktor - Stoppt Server automatisch
     */
    ~OPCUAServer();

    // ========== Server Lifecycle ==========

    /**
     * @brief Startet den OPC UA Server in separatem Thread
     * @return true bei Erfolg, false bei Fehler
     */
    bool start();

    /**
     * @brief Stoppt den OPC UA Server
     */
    void stop();

    /**
     * @brief Prüft ob Server läuft
     * @return true wenn Server aktiv ist
     */
    bool isRunning() const { return running_.load(); }

    /**
     * @brief Gibt die Server-URL zurück
     * @return opc.tcp://localhost:PORT
     */
    std::string getEndpointUrl() const;

    // ========== Variablen erstellen (String-basierte NodeIDs) ==========

    /**
     * @brief Erstellt eine Int16 Variable
     * @param name Variablenname (z.B. "Heartbeat")
     * @param initialValue Startwert
     * @return true bei Erfolg
     */
    bool addInt16Variable(const std::string &name, int16_t initialValue = 0);

    /**
     * @brief Erstellt eine Int32 Variable
     */
    bool addInt32Variable(const std::string &name, int32_t initialValue = 0);

    /**
     * @brief Erstellt eine Bool Variable
     */
    bool addBoolVariable(const std::string &name, bool initialValue = false);

    /**
     * @brief Erstellt eine Float Variable
     */
    bool addFloatVariable(const std::string &name, float initialValue = 0.0f);

    /**
     * @brief Erstellt eine Double Variable
     */
    bool addDoubleVariable(const std::string &name, double initialValue = 0.0);

    /**
     * @brief Erstellt eine String Variable
     * @param maxLength Maximale String-Länge
     */
    bool addStringVariable(const std::string &name, const std::string &initialValue = "", size_t maxLength = 255);

    // ========== Variablen mit numerischen NodeIDs erstellen ==========

    /**
     * @brief Erstellt Variable mit numerischer NodeId im Format ns=nsIndex;i=numericId
     * @param nsIndex Namespace-Index (z.B. 4)
     * @param numericId Numerische ID (z.B. 123 für iHeartbeat)
     * @param name Variablenname (z.B. "Heartbeat")
     * @param initialValue Startwert
     */
    bool addInt16Variable(uint16_t nsIndex, uint32_t numericId, const std::string &name, int16_t initialValue = 0);
    bool addInt32Variable(uint16_t nsIndex, uint32_t numericId, const std::string &name, int32_t initialValue = 0);
    bool addBoolVariable(uint16_t nsIndex, uint32_t numericId, const std::string &name, bool initialValue = false);
    bool addFloatVariable(uint16_t nsIndex, uint32_t numericId, const std::string &name, float initialValue = 0.0f);
    bool addDoubleVariable(uint16_t nsIndex, uint32_t numericId, const std::string &name, double initialValue = 0.0);
    bool addStringVariable(uint16_t nsIndex, uint32_t numericId, const std::string &name, const std::string &initialValue = "", size_t maxLength = 255);
    
    /**
     * @brief Erstellt String-Array Variable
     */
    bool addStringArrayVariable(uint16_t nsIndex, uint32_t numericId, const std::string &name, const std::vector<std::string> &initialValue = {});
    
    /**
     * @brief Erstellt Int16-Array Variable
     */
    bool addInt16ArrayVariable(uint16_t nsIndex, uint32_t numericId, const std::string &name, const std::vector<int16_t> &initialValue = {});

    // ========== Variablen lesen ==========

    bool readInt16(const std::string &name, int16_t &value);
    bool readInt32(const std::string &name, int32_t &value);
    bool readBool(const std::string &name, bool &value);
    bool readFloat(const std::string &name, float &value);
    bool readDouble(const std::string &name, double &value);
    bool readString(const std::string &name, std::string &value);
    bool readStringArray(const std::string &name, std::vector<std::string> &value);

    // ========== Variablen schreiben ==========

    bool writeInt16(const std::string &name, int16_t value);
    bool writeInt32(const std::string &name, int32_t value);
    bool writeBool(const std::string &name, bool value);
    bool writeFloat(const std::string &name, float value);
    bool writeDouble(const std::string &name, double value);
    bool writeString(const std::string &name, const std::string &value);

    // ========== Utility ==========

    /**
     * @brief Listet alle Variablen auf (für Debugging)
     */
    void printVariables() const;

    /**
     * @brief Löscht eine Variable
     */
    bool removeVariable(const std::string &name);

private:
    // Server-Instanz
    UA_Server *server_;
    uint16_t port_;
    std::string serverName_;

    // Threading
    std::thread serverThread_;
    std::atomic<bool> running_;
    std::atomic<bool> stopRequested_;

    // Variable Registry (Name -> NodeId Mapping)
    struct VariableInfo
    {
        UA_NodeId nodeId;
        std::string typeName;
    };
    std::unordered_map<std::string, VariableInfo> variables_;
    mutable std::mutex variablesMutex_;

    // Serializes every UA_Server_* call against the server loop. open62541 is
    // built with UA_MULTITHREADING=0 on Linux, so concurrent server access
    // from test threads would be a data race without this.
    mutable std::mutex serverMutex_;

    // Server Loop (läuft in separatem Thread)
    void serverLoop();

    // Helper: NodeId für Variable holen
    bool getNodeId(const std::string &name, UA_NodeId &nodeId) const;

    // Helper: Variable zum Namespace hinzufügen (String-basierte NodeID)
    bool addVariableNode(const std::string &name,
                         UA_NodeId typeId,
                         const UA_Variant &initialValue,
                         const std::string &typeName);

    // Helper: Variable zum Namespace hinzufügen (Numerische NodeID)
    bool addVariableNodeNumeric(uint16_t nsIndex,
                                uint32_t numericId,
                                const std::string &name,
                                UA_NodeId typeId,
                                const UA_Variant &initialValue,
                                const std::string &typeName);
};