#ifndef GCODE_PARSER_H
#define GCODE_PARSER_H

#include <string>
#include <vector>
#include <optional>
#include <stdexcept>

/**
 * @class GCodeParser
 * @brief Parst G-Code-Dateien in strukturierte Befehle
 */
class GCodeParser
{
public:
    // ========== ENUMS ==========
    enum class CommandType
    {
        None,
        G00,  // Rapid positioning
        G01,  // Linear interpolation
        G02,  // Circular interpolation CW
        G03   // Circular interpolation CCW
    };

    // ========== STRUCTS ==========
    
    /**
     * @struct Point2D
     * @brief Einfache 2D-Punkt-Struktur (OpenCV-unabhängig)
     */
    struct Point2D
    {
        double x;
        double y;
        
        Point2D() : x(0.0), y(0.0) {}
        Point2D(double x_, double y_) : x(x_), y(y_) {}
        
        bool operator==(const Point2D& other) const
        {
            return x == other.x && y == other.y;
        }
        
        bool operator!=(const Point2D& other) const
        {
            return !(*this == other);
        }
    };
    
    /**
     * @struct Command
     * @brief Repräsentiert einen geparsten G-Code-Befehl
     */
    struct Command
    {
        CommandType type;
        Point2D endPosition;     // Zielposition (X, Y)
        Point2D arcCenter;       // Kreismittelpunkt-Offset (I, J) für G02/G03
        int lineNumber;          // Zeilennummer in der Datei
        // Datensatz-Index (zählt NICHT-leere Zeilen, wie readDataBlock/LiveStand
        // der SPS). Damit lässt sich der Fortschritt (erledigt/aktuell/offen)
        // exakt zur SPS-Position einfärben. -1 = nicht gesetzt.
        int recordIndex{-1};
        std::string originalLine; // Original-Zeile für Debugging

        Command()
            : type(CommandType::None)
            , endPosition(0.0, 0.0)
            , arcCenter(0.0, 0.0)
            , lineNumber(0)
        {}
        
        bool isArc() const 
        { 
            return type == CommandType::G02 || type == CommandType::G03; 
        }
        
        bool isLinear() const 
        { 
            return type == CommandType::G01; 
        }
        
        bool isRapid() const 
        { 
            return type == CommandType::G00; 
        }
    };
    
    /**
     * @struct ParseResult
     * @brief Ergebnis des Parsing-Vorgangs
     */
    struct ParseResult
    {
        std::vector<Command> commands;
        std::vector<int> errorLines;
        std::vector<std::string> errorMessages;
        bool success;
        
        ParseResult() : success(true) {}
        
        bool hasErrors() const { return !errorLines.empty(); }
        size_t commandCount() const { return commands.size(); }
    };

    // ========== CONSTRUCTOR ==========
    GCodeParser() = default;
    ~GCodeParser() = default;

    // ========== PUBLIC METHODS ==========
    
    /**
     * @brief Parst eine G-Code-Datei
     * @param filename Pfad zur Datei
     * @return ParseResult mit Befehlen und Fehlern
     */
    ParseResult parseFile(const std::string& filename);
    
    /**
     * @brief Parst einen String mit G-Code-Zeilen
     * @param content G-Code-Inhalt
     * @return ParseResult mit Befehlen und Fehlern
     */
    ParseResult parseString(const std::string& content);
    
    /**
     * @brief Erkennt G-Code-Typ aus einer Zeile
     * @param line Einzelne G-Code-Zeile
     * @return CommandType
     */
    static CommandType detectCommandType(const std::string& line);
    
    /**
     * @brief Prüft ob Dateipfad gültig ist
     * @param path Dateipfad
     * @return true wenn Datei existiert und lesbar ist
     */
    static bool isValidFilePath(const std::string& path);

private:
    // ========== PRIVATE HELPER METHODS ==========
    
    /**
     * @brief Parst eine einzelne Zeile
     * @param line Zeile
     * @param lineNumber Zeilennummer
     * @param currentPos Aktuelle Position (wird aktualisiert)
     * @return Optional Command (leer bei Kommentaren/leeren Zeilen)
     */
    std::optional<Command> parseLine(const std::string& line, 
                                     int lineNumber,
                                     Point2D& currentPos);
    
    /**
     * @brief Extrahiert X, Y, I, J Werte aus Zeile
     * @param line Zeile
     * @param currentPos Aktuelle Position (für relative Werte)
     * @param newX Output: neuer X-Wert
     * @param newY Output: neuer Y-Wert
     * @param I Output: I-Wert (Arc center offset X)
     * @param J Output: J-Wert (Arc center offset Y)
     */
    static void parseCoordinates(const std::string& line,
                                const Point2D& currentPos,
                                double& newX, double& newY,
                                double& I, double& J);
    
    /**
     * @brief Liest numerischen Wert nach einem Buchstaben
     * @param ss String-Stream
     * @param word Wort mit Buchstaben + Wert (z.B. "X123.45")
     * @return Geparster Wert
     */
    static double readValueAfterLetter(std::istringstream& ss, 
                                      const std::string& word);
};

#endif // GCODE_PARSER_H