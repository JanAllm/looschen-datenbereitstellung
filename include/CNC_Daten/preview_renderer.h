#pragma once

#include "CNC_Daten/gcode_parser.h"
#include "interfaces/SPSController.h"
#include <opencv2/opencv.hpp>
#include <functional>
#include <string>
#include <vector>

/**
 * @struct RenderParams
 * @brief Parameter für Preview-Rendering
 */
struct RenderParams
{
    int imageSizeX{1000};
    int imageSizeY{2000};
    
    cv::Scalar colorG0{128, 128, 128};
    cv::Scalar colorG1{0, 255, 0};
    cv::Scalar colorG2{0, 0, 255};
    cv::Scalar colorG3{255, 0, 0};
    
    int thicknessG0{1};
    int thicknessG1{2};
    int thicknessG2{2};
    int thicknessG3{2};
    
    int horizontalOffset{0};
    int verticalOffset{0};

    bool enableMirroring{true};
    bool drawReferenceAxis{true};

    // Kantenglättung (LINE_AA). Für die schnelle Vorschau abschaltbar (LINE_8).
    bool antialiased{true};

    // Schwellwert-Radius: Bögen (G02/G03) mit kleinerem Radius werden als Linie
    // gezeichnet. 0 = deaktiviert. Wird von der SPS geladen.
    double minArcRadius{0.0};

    // Fortschritts-Einfärbung (SPS-Live UND Preview-Testdurchlauf):
    // currentRecord = aktuell abgearbeiteter Datensatz (LiveStand der SPS bzw.
    // Testdurchlauf-Schritt). -1 = aus (alles in Normalfarbe).
    //   recordIndex <  currentRecord -> doneColor   (erledigt)
    //   recordIndex == currentRecord -> liveColor   (aktuelle Position)
    //   recordIndex >  currentRecord -> Normalfarbe (offen)
    int currentRecord{-1};
    cv::Scalar liveColor{0, 255, 255};   // aktuelle Position (BGR: Gelb)
    cv::Scalar doneColor{90, 90, 90};    // erledigt (BGR: Grau)
    // Strichstärke der Fortschritts-Zustände. 0 = normale (G-Typ-)Stärke behalten.
    int liveThickness{3};                // aktuelle Position (hervorgehoben)
    int doneThickness{0};                // erledigt (0 = wie normal)
    
    /**
     * @brief Lädt Parameter von SPS
     */
    static RenderParams loadFromSPS(SPSController& controller, 
                                    int defaultSizeX, 
                                    int defaultSizeY);
};

/**
 * @class PreviewRenderer
 * @brief Vereinheitlichte Klasse für G-Code Preview-Rendering
 * 
 * Kombiniert Low-Level-Rendering (G-Code → Bild) und 
 * High-Level-Funktionalität (SPS-Integration)
 */
class PreviewRenderer
{
public:
    // ========== CONSTANTS ==========
    static constexpr int FP_SHIFT = 8;
    static constexpr int FP_SCALE = 1 << FP_SHIFT;
    static constexpr double EPSILON = 1e-6;
    static constexpr double FULL_CIRCLE_DEG = 360.0;

    // ========== CONSTRUCTOR ==========
    PreviewRenderer() = default;
    ~PreviewRenderer() = default;

    // ========== HIGH-LEVEL API (mit SPS) ==========
    
    /**
     * @brief Rendert Preview aus Projektdatei (mit SPS-Parametern)
     * @param projectPath Pfad zur .tab/.tap Datei
     * @param params Rendering-Parameter (von SPS geladen)
     * @return {Gerendertes Bild, Statuscode}
     */
    std::tuple<cv::Mat, int> renderPreview(const std::string& projectPath,
                                           const RenderParams& params);
    
    // ========== LOW-LEVEL API (ohne SPS) ==========
    
    /**
     * @brief Rendert geparste G-Code-Befehle direkt
     * @param commands Geparste Befehle
     * @param params Rendering-Parameter
     * @return {Gerendertes Bild, Statuscode}
     */
    std::tuple<cv::Mat, int> render(const std::vector<GCodeParser::Command>& commands,
                                    const RenderParams& params);

    /**
     * @brief Zeichnet Befehle [from, to) in ein BESTEHENDES Bild (inkrementell).
     *
     * Kein Löschen, keine Nachbearbeitung/Spiegelung. Für schnelles schrittweises
     * Rendern (Testdurchlauf): pro Schritt nur der neue Befehl gezeichnet.
     * @param img Ziel-Bild (wird nicht gelöscht).
     * @param startPos Startposition des ersten Befehls in [from, to).
     * @return Endposition nach dem letzten gezeichneten Befehl.
     */
    cv::Point2d renderRange(cv::Mat& img,
                            const std::vector<GCodeParser::Command>& commands,
                            std::size_t from, std::size_t to,
                            const cv::Point2d& startPos,
                            const RenderParams& params) const;

    /**
     * @brief Setzt Post-Processing-Funktion
     * @param func Funktion die das Bild nachbearbeitet
     */
    void setPostProcessor(std::function<cv::Mat(cv::Mat)> func)
    {
        postProcessor_ = std::move(func);
    }

private:
    // ========== PRIVATE TYPES ==========
    enum class ArcDirection { Clockwise, CounterClockwise };
    
    // ========== PRIVATE RENDERING METHODS ==========
    
    /**
     * @brief Zeichnet Referenz-Achsen
     */
    void drawReferenceAxis(cv::Mat& img, const RenderParams& params) const;
    
    /**
     * @brief Rendert einen einzelnen Befehl
     */
    void renderCommand(cv::Mat& img,
                      const GCodeParser::Command& cmd,
                      const cv::Point2d& startPos,
                      const RenderParams& params) const;
    
    /**
     * @brief Rendert Linie (G00/G01)
     */
    void renderLine(cv::Mat& img,
                   const cv::Point2d& start,
                   const cv::Point2d& end,
                   const cv::Scalar& color,
                   int thickness,
                   const RenderParams& params) const;
    
    /**
     * @brief Rendert Kreisbogen (G02/G03)
     */
    void renderArc(cv::Mat& img,
                  const cv::Point2d& start,
                  const cv::Point2d& end,
                  const cv::Point2d& centerOffset,
                  ArcDirection direction,
                  const cv::Scalar& color,
                  int thickness,
                  const RenderParams& params) const;
    
    /**
     * @brief Berechnet Winkel in Grad (OpenCV-Konvention)
     */
    static double angleDeg(double dx, double dy);
    
    /**
     * @brief Konvertiert zu Fixed-Point mit Offset
     */
    static cv::Point toFixedPoint(double x, double y, 
                                  int horizontalOffset, 
                                  int verticalOffset);
    
    /**
     * @brief Konvertiert GCodeParser::Point2D zu cv::Point2d
     */
    static cv::Point2d toOpenCV(const GCodeParser::Point2D& p)
    {
        return cv::Point2d(p.x, p.y);
    }
    
    // ========== MEMBER VARIABLES ==========
    mutable std::function<cv::Mat(cv::Mat)> postProcessor_;
};