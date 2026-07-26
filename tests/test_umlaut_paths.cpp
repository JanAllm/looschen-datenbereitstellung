#include <gtest/gtest.h>

#include <filesystem>
#include <fstream>

#include "CNC_Daten/project_file_reader.h"

namespace fs = std::filesystem;

// Verifiziert, dass Ordner- UND Dateipfade mit Umlauten (ä, ö, ü, ß) korrekt
// verarbeitet werden - das gesamte ProjectFileReader-Pipeline (Verzeichnis
// scannen, Datei finden, Datensätze zählen). Setzt das UTF-8-Manifest voraus,
// das die aktive Codepage des Prozesses auf UTF-8 stellt.
TEST(UmlautPaths, ListsAndReadsTapFileWithUmlautsInPathAndName)
{
    // Ordnername mit Umlauten (Quelltext ist /utf-8 -> Bytes sind UTF-8).
    fs::path base = fs::temp_directory_path() / "looschen_ümlaut_prüfördner";
    fs::remove_all(base);
    ASSERT_TRUE(fs::create_directories(base));

    // Dateiname mit Umlauten und ß.
    fs::path tap = base / "größö_tästdatäi.tap";
    {
        std::ofstream f(tap);
        ASSERT_TRUE(f.is_open()) << "Konnte Umlaut-Datei nicht anlegen";
        f << "(Kommentar-Kopfzeile)\n";
        f << "G01 X10 Y20\n";
        f << "G03 X5 Y5 I0.1 J0.0\n";
    }

    ProjectFileReader reader(base.string());

    // 1) Verzeichnis scannen -> Datei mit Umlaut-Namen wird gefunden.
    auto projects = reader.listProjects(10);
    ASSERT_EQ(projects.size(), 1u);
    EXPECT_EQ(projects[0], "größö_tästdatäi.tap");

    // 2) Projektinfo -> existiert, Datensätze zählbar (Kopfzeile wird übersprungen).
    auto info = reader.getProjectInfo(projects[0]);
    ASSERT_TRUE(info.has_value());
    EXPECT_TRUE(info->exists);
    EXPECT_GT(info->recordCount, 0u);

    // 3) Datenblock aus Umlaut-Pfad lesen.
    auto block = reader.readDataBlock(info->filePath, 0, 100);
    EXPECT_FALSE(block.empty());

    fs::remove_all(base);
}
