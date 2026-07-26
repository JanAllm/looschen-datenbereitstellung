# pragma once

#include <string>
#include <optional>
#include <tuple>
#include "DataType.h"

struct VariableInfo {
    std::string name;
    std::string fullName;
    DataType type;
    bool tested = false;
    bool available = false;
    std::optional<long> handle = std::nullopt; // Nur für ADS-relevante Clients
};

/// Live-Diagnose eines OPC-Knotens fuer die Weboberflaeche.
/// exists=false -> Knoten existiert nicht (falscher Namespace/Index).
/// readable/writable stammen aus dem AccessLevel-Attribut des Servers -
/// es wird NICHT testweise geschrieben.
struct NodeDiagnostic {
    std::string name;     ///< logischer Name, z.B. "Error"
    std::string nodeId;   ///< z.B. "ns=1;i=120"
    std::string type;     ///< erwarteter Datentyp
    bool exists = false;
    bool readable = false;
    bool writable = false;
};
