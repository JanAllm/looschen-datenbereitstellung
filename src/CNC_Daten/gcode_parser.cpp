#include "CNC_Daten/gcode_parser.h"

#include <fstream>
#include <sstream>
#include <filesystem>
#include <iostream>
#include <algorithm>
#include <cctype>

// ========== PUBLIC METHODS ==========

GCodeParser::ParseResult GCodeParser::parseFile(const std::string& filename)
{
    ParseResult result;
    
    // Validierung
    if (!isValidFilePath(filename))
    {
        result.success = false;
        result.errorMessages.push_back("Invalid file path: " + filename);
        return result;
    }
    
    // Datei öffnen
    std::ifstream file(filename);
    if (!file.is_open())
    {
        result.success = false;
        result.errorMessages.push_back("Cannot open file: " + filename);
        return result;
    }
    
    // Leer-Check
    if (file.peek() == std::ifstream::traits_type::eof())
    {
        result.success = true;
        result.errorMessages.push_back("Warning: File is empty");
        return result;
    }
    
    // Parsing
    std::string line;
    int lineNumber = 0;
    int recordIndex = 0;  // zählt NICHT-leere Zeilen (identisch zu readDataBlock
                          // -> passt zum LiveStand-Zähler der SPS)
    Point2D currentPosition(0.0, 0.0);  // ← Geändert

    while (std::getline(file, line))
    {
        ++lineNumber;

        // CR/Leerzeichen am Zeilenende entfernen (CRLF-Dateien), damit der
        // Parser dieselben Zeilen sieht wie die zur SPS gesendeten Daten.
        while (!line.empty() &&
               (line.back() == '\r' || line.back() == '\n' ||
                line.back() == ' ' || line.back() == '\t'))
        {
            line.pop_back();
        }

        // Reine Leerzeilen zählen NICHT als Datensatz (wie readDataBlock).
        bool onlySpace = true;
        for (char ch : line)
        {
            if (!std::isspace(static_cast<unsigned char>(ch)))
            {
                onlySpace = false;
                break;
            }
        }
        if (onlySpace)
            continue;

        const int thisRecord = recordIndex;  // Datensatz-Index dieser Zeile
        ++recordIndex;

        try
        {
            auto cmd = parseLine(line, lineNumber, currentPosition);
            if (cmd.has_value())
            {
                cmd.value().recordIndex = thisRecord;
                result.commands.push_back(cmd.value());
                currentPosition = cmd.value().endPosition;
            }
        }
        catch (const std::exception& e)
        {
            result.errorLines.push_back(lineNumber);
            result.errorMessages.push_back(
                "Line " + std::to_string(lineNumber) + ": " + e.what()
            );
        }
    }
    
    result.success = result.errorLines.empty();
    return result;
}

GCodeParser::ParseResult GCodeParser::parseString(const std::string& content)
{
    ParseResult result;
    
    std::istringstream stream(content);
    std::string line;
    int lineNumber = 0;
    Point2D currentPosition(0.0, 0.0);  // ← Geändert
    
    while (std::getline(stream, line))
    {
        ++lineNumber;
        
        try
        {
            auto cmd = parseLine(line, lineNumber, currentPosition);
            if (cmd.has_value())
            {
                result.commands.push_back(cmd.value());
                currentPosition = cmd.value().endPosition;
            }
        }
        catch (const std::exception& e)
        {
            result.errorLines.push_back(lineNumber);
            result.errorMessages.push_back(
                "Line " + std::to_string(lineNumber) + ": " + e.what()
            );
        }
    }
    
    result.success = result.errorLines.empty();
    return result;
}

GCodeParser::CommandType GCodeParser::detectCommandType(const std::string& line)
{
    std::istringstream ss(line);
    std::string token;
    
    while (ss >> token)
    {
        if (token.size() < 2) continue;
        if (token[0] != 'G' && token[0] != 'g') continue;
        
        std::string num = token.substr(1);
        
        // Führende Nullen entfernen
        size_t i = 0;
        while (i < num.size() && num[i] == '0') ++i;
        std::string n = num.substr(i);
        if (n.empty()) n = "0";
        
        if (n == "0") return CommandType::G00;
        if (n == "1") return CommandType::G01;
        if (n == "2") return CommandType::G02;
        if (n == "3") return CommandType::G03;
    }
    
    return CommandType::None;
}

bool GCodeParser::isValidFilePath(const std::string& path)
{
    namespace fs = std::filesystem;
    fs::path p(path);
    return fs::exists(p) && fs::is_regular_file(p);
}

// ========== PRIVATE METHODS ==========

std::optional<GCodeParser::Command> GCodeParser::parseLine(
    const std::string& line,
    int lineNumber,
    Point2D& currentPos)  // ← Geändert
{
    // Leere Zeilen überspringen
    if (line.empty())
        return std::nullopt;
    
    // Kommentare überspringen
    if (line[0] == ';' || line[0] == '(')
        return std::nullopt;
    
    // Command-Typ erkennen
    CommandType type = detectCommandType(line);
    if (type == CommandType::None)
        return std::nullopt;
    
    // Command erstellen
    Command cmd;
    cmd.type = type;
    cmd.lineNumber = lineNumber;
    cmd.originalLine = line;
    
    // Koordinaten parsen
    double newX = currentPos.x;
    double newY = currentPos.y;
    double I = 0.0;
    double J = 0.0;
    
    parseCoordinates(line, currentPos, newX, newY, I, J);
    
    cmd.endPosition = Point2D(newX, newY);  // ← Geändert
    cmd.arcCenter = Point2D(I, J);          // ← Geändert
    
    return cmd;
}

void GCodeParser::parseCoordinates(
    const std::string& line,
    const Point2D& currentPos,  // ← Geändert
    double& newX, double& newY,
    double& I, double& J)
{
    std::istringstream ss(line);
    std::string word;
    
    while (ss >> word)
    {
        if (word.empty()) continue;
        
        char c = word[0];
        
        try
        {
            if (c == 'X' || c == 'x')
                newX = readValueAfterLetter(ss, word);
            else if (c == 'Y' || c == 'y')
                newY = readValueAfterLetter(ss, word);
            else if (c == 'I' || c == 'i')
                I = readValueAfterLetter(ss, word);
            else if (c == 'J' || c == 'j')
                J = readValueAfterLetter(ss, word);
        }
        catch (const std::exception& e)
        {
            throw std::runtime_error(
                "Failed to parse coordinate '" + word + "': " + e.what()
            );
        }
    }
}

double GCodeParser::readValueAfterLetter(
    std::istringstream& ss,
    const std::string& word)
{
    std::string value;
    
    if (word.size() > 1)
    {
        value = word.substr(1);
    }
    else
    {
        if (!(ss >> value))
            throw std::invalid_argument("Missing value after letter");
    }
    
    std::replace(value.begin(), value.end(), ',', '.');
    
    try
    {
        return std::stod(value);
    }
    catch (const std::exception&)
    {
        throw std::invalid_argument("Invalid numeric value: " + value);
    }
}