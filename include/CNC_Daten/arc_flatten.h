#pragma once

#include <algorithm>
#include <cmath>
#include <sstream>
#include <string>
#include <vector>

/**
 * @file arc_flatten.h
 * @brief Gemeinsame Logik, um Kreisbögen (G02/G03) mit kleinem Radius in
 *        Linien (G01) umzuwandeln.
 *
 * Dieselbe Entscheidungslogik wird von zwei Stellen genutzt, damit sich die
 * Visualisierung und die zur SPS gesendeten Daten garantiert identisch verhalten:
 *   - PreviewRenderer::renderArc  (Visualisierung)
 *   - ProjectManager::transferProjectData  (gesendete Datenzeilen)
 *
 * Der Radius eines Bogens ergibt sich aus den I/J-Offsets (Vektor vom
 * Startpunkt zum Kreismittelpunkt): r = hypot(I, J).
 *
 * Der Schwellwert kommt aus einer OPC-UA-Variable. Ein Wert <= 0 deaktiviert
 * die Funktion (Bögen bleiben Bögen).
 */
namespace arc_flatten
{

/// Radius eines Bogens aus den I/J-Offsets (Start -> Mittelpunkt).
inline double radiusFromIJ(double I, double J)
{
    return std::hypot(I, J);
}

/**
 * @brief Entscheidet, ob ein Bogen zu einer Linie geglättet werden soll.
 * @param radius    Bogenradius (mm).
 * @param threshold Schwellwert (mm). <= 0 => deaktiviert.
 * @return true, wenn 0 < radius < threshold.
 */
inline bool shouldFlatten(double radius, double threshold)
{
    return threshold > 0.0 && radius > 0.0 && radius < threshold;
}

/// Numerischen Wert nach dem Adressbuchstaben lesen (Dezimalkomma -> Punkt).
inline double parseValue(std::string s)
{
    std::replace(s.begin(), s.end(), ',', '.');
    try
    {
        return std::stod(s);
    }
    catch (...)
    {
        return 0.0;
    }
}

/// Normalisierte G-Nummer eines Tokens (z.B. "G02" -> "2"); "" wenn kein G-Wort.
inline std::string gNumber(const std::string& token)
{
    if (token.size() < 2 || (token[0] != 'G' && token[0] != 'g'))
        return "";
    std::string num = token.substr(1);
    size_t i = 0;
    while (i < num.size() && num[i] == '0')
        ++i;
    std::string n = num.substr(i);
    return n.empty() ? "0" : n;
}

/**
 * @brief Wandelt eine G02/G03-Zeile mit kleinem Radius in eine G01-Linie um.
 *
 * Arbeitet rein auf Token-Ebene: Der G-Token wird zu "G01", die I/J-Token
 * werden entfernt, alle übrigen Token (insbesondere X/Y) bleiben wörtlich
 * erhalten (bewahrt Dezimalkomma und Werte exakt). Nicht-Bogen-Zeilen und
 * Bögen mit Radius >= threshold werden unverändert zurückgegeben.
 *
 * @param line      Original-G-Code-Zeile.
 * @param threshold Schwellwert (mm). <= 0 => Zeile unverändert.
 * @return Ggf. umgeschriebene Zeile.
 */
inline std::string flattenArcLine(const std::string& line, double threshold)
{
    if (threshold <= 0.0)
        return line;

    // Tokenisieren (an Whitespace).
    std::istringstream ss(line);
    std::vector<std::string> tokens;
    std::string tok;
    while (ss >> tok)
        tokens.push_back(tok);
    if (tokens.empty())
        return line;

    // Befehl bestimmen und I/J einsammeln.
    int gIdx = -1;
    bool isArc = false;
    double I = 0.0;
    double J = 0.0;

    for (size_t k = 0; k < tokens.size(); ++k)
    {
        const std::string& t = tokens[k];
        if (t.empty())
            continue;
        const char c0 = t[0];

        std::string gnum = gNumber(t);
        if (!gnum.empty())
        {
            if (gnum == "2" || gnum == "3")
            {
                gIdx = static_cast<int>(k);
                isArc = true;
            }
        }
        else if (c0 == 'I' || c0 == 'i')
        {
            std::string v = t.substr(1);
            if (v.empty() && k + 1 < tokens.size())
                v = tokens[k + 1]; // Form "I 1.5"
            I = parseValue(v);
        }
        else if (c0 == 'J' || c0 == 'j')
        {
            std::string v = t.substr(1);
            if (v.empty() && k + 1 < tokens.size())
                v = tokens[k + 1];
            J = parseValue(v);
        }
    }

    if (!isArc)
        return line;

    if (!shouldFlatten(radiusFromIJ(I, J), threshold))
        return line;

    // Neue Zeile bauen: G-Token -> "G01", I/J entfernen, Rest wörtlich behalten.
    std::string out;
    for (size_t k = 0; k < tokens.size(); ++k)
    {
        const std::string& t = tokens[k];
        const char c0 = t.empty() ? '\0' : t[0];

        if (static_cast<int>(k) == gIdx)
        {
            if (!out.empty())
                out += ' ';
            out += "G01";
            continue;
        }
        if (c0 == 'I' || c0 == 'i' || c0 == 'J' || c0 == 'j')
        {
            // Getrennte Form ("I 1.5"): das Folge-Token (Wert) mit überspringen.
            if (t.size() == 1 && k + 1 < tokens.size())
                ++k;
            continue;
        }
        if (!out.empty())
            out += ' ';
        out += t;
    }
    return out;
}

} // namespace arc_flatten
