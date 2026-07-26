# pragma once

#include <string>
#include <optional>
#include <tuple>
#include "DataType.h"

struct setupVarInfos {
    // ------------------Algemeien Variablen Knoten (Dialog etc)------------------
    //
    int ns = 1; // Namespace Index der SPS Variablen
    int iError  = 0; // Index der Fehlervariable (write, bool)
    int iInfo   = 0; // Index der Informationsvariable (write, string)
    int iHeartbeat = 0; // Index der Herzschlagvariable  (write, bool)
    int iStatusCode = 0; // Index des Statuscode  (write, int16_t)
    //
    // ------------------Arrays------------------
    //
   
    int iDataArry = 0; // Index des Datenarrays (write, float array)
    int iProjektArray = 0; // Index des Projektarrays (write, string array)
    // 
    // ------------------Dialog Variablen------------------
    //
    int iUpdateProjektList = 0; // Index der Aktualisierungsliste des Projekts (read, bool)
    int iProjektlistUpdated = 0; // Index der Aktualisierungsliste des Projekts (write, bool)

    int iProjektName = 0; // Ausgewählte Projekt (read, string)
    int iReadProjekt = 0; // Ausgewähltes Projekt laden (read, bool)

    int ishowProjektInfo = 0; // Zeige Projektinfo (read, bool)
    int iProjektInfoWrote = 0; // Schreibe Projektinfo (write, bool)
    int iProjektVorhanden = 0; // Projekt vorhanden (read, bool)
    
    
    int iLenProjekt = 0; // Länge des ausgewählten Datensatzensatzes (read, int16_t)
    int iAnzDatenblocke = 0; // Anzahl der Datenblöcke (write, int16_t)
    int iWriteData = 0; //  schreibe Datenblock (write, bool)
    int iIndexData = 0; // Index des Datenblocks (write, int16_t)
    int iReadData = 0; // Datenblock gelesen (read, bool)
    int iabbruchUbertragen = 0; // Abbruch der Übertragung (write, int16_t)
    int iUebertgarungBeendet = 0; // Übertragung beendet (write, int16_t)

    // ------------------Konfiguration------------------
    int iUebertragungslaege = 0; //Übertragungsgröße (read, int16_t)

    // ------------------Drawings------------------

    int icolorG0 = 0; // Farbe G00 (read, array of int)
    int istreghtsG0 = 0; // Stärke G00 (read, array of int)
    int icolorG1 = 0; // Farbe G01 (read, array of int)
    int istreghtsG1 = 0; // Stärke G01 (read, array of int)
    int icolorG2 = 0; // Farbe G02 (read, array of int)
    int istreghtsG2 = 0; // Stärke G02 (read, array of int)
    int icolorG3 = 0; // Farbe G03 (read, array of int)
    int istreghtsG3 = 0; // Stärke G03 (read, array of int)
    int iFarbeLive = 0; // Farbe aktuelle Position/Livebild (read, array of int)
    int iFarbeErledigt = 0; // Farbe bereits abgearbeiteter Abschnitte (read, array of int); OPTIONAL
    int iStaerkeLive = 0; // Stärke aktuelle Position/Livebild (read, int)
    int iStaerkeErledigt = 0; // Stärke bereits abgearbeiteter Abschnitte (read, int); OPTIONAL (0 = wie normal)
    // ------------------Canvas ------------------
    int objektgroesseX = 0; // Größe des Objekts in X-Richtung (read, int)
    int objektgroesseY = 0; // Größe des Objekts in Y-Richtung (read, int)

    int iLiveStand = 0; // Livebild Stand (read, int)
    int iLiveAbbruch = 0; // Livebild Abbruch (write, int)

    // ------------------Bogenglättung------------------
    int iMinArcRadius = 0; // Schwellwert Radius: kleinere Bögen werden zu Linien (read, float); 0 = deaktiviert

};