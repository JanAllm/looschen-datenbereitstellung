# include  <interfaces/SPSDialog.h>



std::tuple<std::vector<std::vector<int16_t>>, bool, int>
SPSDialog::readInt16Array(const std::string &name, size_t startRow, size_t rowCount, size_t startCol, size_t colCount) {
    if (variableMap.find(name) == variableMap.end()) {
        std::cerr << "SPSDialog: Variable '" << name << "' nicht gefunden!" << std::endl;
        ErrorLoggerSingleton::instance().logError("SPSDialog: Variable '" + name + "' nicht gefunden!");
        return {{}, false, -1}; // Fehler
    }
    auto var = variableMap[name];
    auto [values, succ, errorIndx] = client->readInt16Array(var, startRow, rowCount, startCol, colCount);
    if (!succ) {
        std::cerr << "SPSDialog: Fehler beim Lesen des Arrays '" << name << "'!" << std::endl;
        ErrorLoggerSingleton::instance().logError("SPSDialog: Fehler beim Lesen des Arrays '" + name + "'!");
        return {{}, false, errorIndx}; // Fehler beim Lesen
    }
    return {values, true, 0}; // alles ok
}


std::tuple<std::vector<std::vector<int32_t>>, bool, int>
SPSDialog::readInt32Array(const std::string &name, size_t startRow, size_t rowCount, size_t startCol, size_t colCount) {
    if (variableMap.find(name) == variableMap.end()) {
        std::cerr << "SPSDialog: Variable '" << name << "' nicht gefunden!" << std::endl;
        ErrorLoggerSingleton::instance().logError("SPSDialog: Variable '" + name + "' nicht gefunden!");
        return {{}, false, -1};
    }
    auto var = variableMap[name];
    auto [values, succ, errorIndx] = client->readInt32Array(var, startRow, rowCount, startCol, colCount);
    if (!succ) {
        std::cerr << "SPSDialog: Fehler beim Lesen des Arrays '" << name << "'!" << std::endl;
        ErrorLoggerSingleton::instance().logError("SPSDialog: Fehler beim Lesen des Arrays '" + name + "'!");
        return {{}, false, errorIndx};
    }
    return {values, true, 0};
}



std::tuple<std::vector<std::vector<float>>, bool, int>
SPSDialog::readFloatArray(const std::string &name, size_t startRow, size_t rowCount, size_t startCol, size_t colCount) {
    if (variableMap.find(name) == variableMap.end()) {
        std::cerr << "SPSDialog: Variable '" << name << "' nicht gefunden!" << std::endl;
        ErrorLoggerSingleton::instance().logError("SPSDialog: Variable '" + name + "' nicht gefunden!");
        return {{}, false, -1};
    }
    auto var = variableMap[name];
    auto [values, succ, errorIndx] = client->readFloatArray(var, startRow, rowCount, startCol, colCount);
    if (!succ) {
        std::cerr << "SPSDialog: Fehler beim Lesen des Arrays '" << name << "'!" << std::endl;
        ErrorLoggerSingleton::instance().logError("SPSDialog: Fehler beim Lesen des Arrays '" + name + "'!");
        return {{}, false, errorIndx};
    }
    return {values, true, 0};
}

std::tuple<std::vector<std::vector<double>>, bool, int>
SPSDialog::readDoubleArray(const std::string &name, size_t startRow, size_t rowCount, size_t startCol, size_t colCount) {
    if (variableMap.find(name) == variableMap.end()) {
        std::cerr << "SPSDialog: Variable '" << name << "' nicht gefunden!" << std::endl;
        ErrorLoggerSingleton::instance().logError("SPSDialog: Variable '" + name + "' nicht gefunden!");
        return {{}, false, -1};
    }
    auto var = variableMap[name];
    auto [values, succ, errorIndx] = client->readDoubleArray(var, startRow, rowCount, startCol, colCount);
    if (!succ) {
        std::cerr << "SPSDialog: Fehler beim Lesen des Arrays '" << name << "'!" << std::endl;
        ErrorLoggerSingleton::instance().logError("SPSDialog: Fehler beim Lesen des Arrays '" + name + "'!");
        return {{}, false, errorIndx};
    }
    return {values, true, 0};
}

std::tuple<std::vector<std::vector<std::string>>, bool, int>
SPSDialog::readStringArray(const std::string &name, size_t startRow, size_t rowCount, size_t startCol, size_t colCount) {
    if (variableMap.find(name) == variableMap.end()) {
        std::cerr << "SPSDialog: Variable '" << name << "' nicht gefunden!" << std::endl;
        ErrorLoggerSingleton::instance().logError("SPSDialog: Variable '" + name + "' nicht gefunden!");
        return {{}, false, -1};
    }
    auto var = variableMap[name];
    auto [values, succ, errorIndx] = client->readStringArray(var, startRow, rowCount, startCol, colCount);
    if (!succ) {
        std::cerr << "SPSDialog: Fehler beim Lesen des Arrays '" << name << "'!" << std::endl;
        ErrorLoggerSingleton::instance().logError("SPSDialog: Fehler beim Lesen des Arrays '" + name + "'!");
        return {{}, false, errorIndx};
    }
    return {values, true, 0};
}

std::tuple<std::vector<std::vector<std::wstring>>, bool, int>
SPSDialog::readWStringArray(const std::string &name, size_t startRow, size_t rowCount, size_t startCol, size_t colCount) {
    if (variableMap.find(name) == variableMap.end()) {
        std::cerr << "SPSDialog: Variable '" << name << "' nicht gefunden!" << std::endl;
        ErrorLoggerSingleton::instance().logError("SPSDialog: Variable '" + name + "' nicht gefunden!");
        return {{}, false, -1};
    }
    auto var = variableMap[name];
    auto [values, succ, errorIndx] = client->readWStringArray(var, startRow, rowCount, startCol, colCount);
    if (!succ) {
        std::cerr << "SPSDialog: Fehler beim Lesen des Arrays '" << name << "'!" << std::endl;
        ErrorLoggerSingleton::instance().logError("SPSDialog: Fehler beim Lesen des Arrays '" + name + "'!");
        return {{}, false, errorIndx};
    }
    return {values, true, 0};
}

std::tuple<std::vector<std::vector<bool>>, bool, int>
SPSDialog::readBoolArray(const std::string &name, size_t startRow, size_t rowCount, size_t startCol, size_t colCount) {
    if (variableMap.find(name) == variableMap.end()) {
        std::cerr << "SPSDialog: Variable '" << name << "' nicht gefunden!" << std::endl;
        ErrorLoggerSingleton::instance().logError("SPSDialog: Variable '" + name + "' nicht gefunden!");
        return {{}, false, -1};
    }
    auto var = variableMap[name];
    auto [values, succ, errorIndx] = client->readBoolArray(var, startRow, rowCount, startCol, colCount);
    if (!succ) {
        std::cerr << "SPSDialog: Fehler beim Lesen des Arrays '" << name << "'!" << std::endl;
        ErrorLoggerSingleton::instance().logError("SPSDialog: Fehler beim Lesen des Arrays '" + name + "'!");
        return {{}, false, errorIndx};
    }
    return {values, true, 0};
}

std::tuple<bool, int>
SPSDialog::writeInt16Array(const std::string &name, const std::vector<std::vector<int16_t>> &valueToWrite, size_t startRow, size_t startCol) {
    if (variableMap.find(name) == variableMap.end()) {
        std::cerr << "SPSDialog: Variable '" << name << "' nicht gefunden!" << std::endl;
        ErrorLoggerSingleton::instance().logError("SPSDialog: Variable '" + name + "' nicht gefunden!");
        return {false, -1}; // Fehler
    }
    auto var = variableMap[name];
    auto [succ, errorCode] = client->writeInt16Array(var, valueToWrite, startRow, startCol);
    if (!succ) {
        std::cerr << "SPSDialog: Fehler beim Schreiben des Arrays '" << name << "'!" << std::endl;
        ErrorLoggerSingleton::instance().logError("SPSDialog: Fehler beim Schreiben des Arrays '" + name + "'!");
        return {false, errorCode}; // Fehler beim Schreiben
    }
    return {true, 0}; // alles ok
}

std::tuple<bool, int>
SPSDialog::writeInt32Array(const std::string &name, const std::vector<std::vector<int32_t>> &valueToWrite, size_t startRow, size_t startCol) {
    if (variableMap.find(name) == variableMap.end()) {
        std::cerr << "SPSDialog: Variable '" << name << "' nicht gefunden!" << std::endl;
        ErrorLoggerSingleton::instance().logError("SPSDialog: Variable '" + name + "' nicht gefunden!");
        return {false, -1};
    }
    auto var = variableMap[name];
    auto [succ, errorCode] = client->writeInt32Array(var, valueToWrite, startRow, startCol);
    if (!succ) {
        std::cerr << "SPSDialog: Fehler beim Schreiben des Arrays '" << name << "'!" << std::endl;
        ErrorLoggerSingleton::instance().logError("SPSDialog: Fehler beim Schreiben des Arrays '" + name + "'!");
        return {false, errorCode};
    }
    return {true, 0};
}

std::tuple<bool, int>
SPSDialog::writeFloatArray(const std::string &name, const std::vector<std::vector<float>> &valueToWrite, size_t startRow, size_t startCol) {
    if (variableMap.find(name) == variableMap.end()) {
        std::cerr << "SPSDialog: Variable '" << name << "' nicht gefunden!" << std::endl;
        ErrorLoggerSingleton::instance().logError("SPSDialog: Variable '" + name + "' nicht gefunden!");
        return {false, -1};
    }
    auto var = variableMap[name];
    auto [succ, errorCode] = client->writeFloatArray(var, valueToWrite, startRow, startCol);
    if (!succ) {
        std::cerr << "SPSDialog: Fehler beim Schreiben des Arrays '" << name << "'!" << std::endl;
        ErrorLoggerSingleton::instance().logError("SPSDialog: Fehler beim Schreiben des Arrays '" + name + "'!");
        return {false, errorCode};
    }
    return {true, 0};
}

std::tuple<bool, int>
SPSDialog::writeDoubleArray(const std::string &name, const std::vector<std::vector<double>> &valueToWrite, size_t startRow, size_t startCol) {
    if (variableMap.find(name) == variableMap.end()) {
        std::cerr << "SPSDialog: Variable '" << name << "' nicht gefunden!" << std::endl;
        ErrorLoggerSingleton::instance().logError("SPSDialog: Variable '" + name + "' nicht gefunden!");
        return {false, -1};
    }
    auto var = variableMap[name];
    auto [succ, errorCode] = client->writeDoubleArray(var, valueToWrite, startRow, startCol);
    if (!succ) {
        std::cerr << "SPSDialog: Fehler beim Schreiben des Arrays '" << name << "'!" << std::endl;
        ErrorLoggerSingleton::instance().logError("SPSDialog: Fehler beim Schreiben des Arrays '" + name + "'!");
        return {false, errorCode};
    }
    return {true, 0};
}

std::tuple<bool, int>
SPSDialog::writeStringArray(const std::string &name, const std::vector<std::vector<std::string>> &valueToWrite, size_t startRow, size_t startCol) {
    if (variableMap.find(name) == variableMap.end()) {
        std::cerr << "SPSDialog: Variable '" << name << "' nicht gefunden!" << std::endl;
        ErrorLoggerSingleton::instance().logError("SPSDialog: Variable '" + name + "' nicht gefunden!");
        return {false, -1};
    }
    auto var = variableMap[name];
    auto [succ, errorCode] = client->writeStringArray(var, valueToWrite, startRow, startCol);
    if (!succ) {
        std::cerr << "SPSDialog: Fehler beim Schreiben des Arrays '" << name << "'!" << std::endl;
        ErrorLoggerSingleton::instance().logError("SPSDialog: Fehler beim Schreiben des Arrays '" + name + "'!");
        return {false, errorCode};
    }
    return {true, 0};
}
//ByteStringArray !
std::tuple<bool, int>
SPSDialog::writeWStringArray(const std::string &name, const std::vector<std::vector<std::wstring>> &valueToWrite, size_t startRow, size_t startCol) {
    if (variableMap.find(name) == variableMap.end()) {
        std::cerr << "SPSDialog: Variable '" << name << "' nicht gefunden!" << std::endl;
        ErrorLoggerSingleton::instance().logError("SPSDialog: Variable '" + name + "' nicht gefunden!");
        return {false, -1};
    }
    auto var = variableMap[name];
    auto [succ, errorCode] = client->writeWStringArray(var, valueToWrite, startRow, startCol);
    if (!succ) {
        std::cerr << "SPSDialog: Fehler beim Schreiben des Arrays '" << name << "'!" << std::endl;
        ErrorLoggerSingleton::instance().logError("SPSDialog: Fehler beim Schreiben des Arrays '" + name + "'!");
        return {false, errorCode};
    }
    return {true, 0};
}

std::tuple<bool, int>
SPSDialog::writeBoolArray(const std::string &name, const std::vector<std::vector<bool>> &valueToWrite, size_t startRow, size_t startCol) {
    if (variableMap.find(name) == variableMap.end()) {
        std::cerr << "SPSDialog: Variable '" << name << "' nicht gefunden!" << std::endl;
        ErrorLoggerSingleton::instance().logError("SPSDialog: Variable '" + name + "' nicht gefunden!");
        return {false, -1};
    }
    auto var = variableMap[name];
    auto [succ, errorCode] = client->writeBoolArray(var, valueToWrite, startRow, startCol);
    if (!succ) {
        std::cerr << "SPSDialog: Fehler beim Schreiben des Arrays '" << name << "'!" << std::endl;
        ErrorLoggerSingleton::instance().logError("SPSDialog: Fehler beim Schreiben des Arrays '" + name + "'!");
        return {false, errorCode};
    }
    return {true, 0};
}


