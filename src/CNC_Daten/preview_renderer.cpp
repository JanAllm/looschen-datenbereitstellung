#include "CNC_Daten/preview_renderer.h"
#include "CNC_Daten/arc_flatten.h"
#include "bildverarbeitung/preprocessing/preprocessing.h"
#include "ErrorLoggerSingleton.h"
#include <iostream>

// ========== RenderParams - Laden von SPS ==========

RenderParams RenderParams::loadFromSPS(
    SPSController& controller,
    int defaultSizeX,
    int defaultSizeY)
{
    RenderParams params;
    params.imageSizeX = defaultSizeX;
    params.imageSizeY = defaultSizeY;
    
    // Farben laden
    try
    {
        auto [g0, ok0, code0] = controller.readInt16Array("colorG0");
        auto [g1, ok1, code1] = controller.readInt16Array("colorG1");
        auto [g2, ok2, code2] = controller.readInt16Array("colorG2");
        auto [g3, ok3, code3] = controller.readInt16Array("colorG3");
        
        params.colorG0 = (ok0 && g0.size() == 1 && g0[0].size() == 3) 
            ? cv::Scalar(static_cast<double>(g0[0][2]), 
                        static_cast<double>(g0[0][1]), 
                        static_cast<double>(g0[0][0])) 
            : cv::Scalar(255, 255, 255);
            
        params.colorG1 = (ok1 && g1.size() == 1 && g1[0].size() == 3) 
            ? cv::Scalar(static_cast<double>(g1[0][2]), 
                        static_cast<double>(g1[0][1]), 
                        static_cast<double>(g1[0][0])) 
            : cv::Scalar(0, 255, 0);
            
        params.colorG2 = (ok2 && g2.size() == 1 && g2[0].size() == 3) 
            ? cv::Scalar(static_cast<double>(g2[0][2]), 
                        static_cast<double>(g2[0][1]), 
                        static_cast<double>(g2[0][0])) 
            : cv::Scalar(0, 0, 255);
            
        params.colorG3 = (ok3 && g3.size() == 1 && g3[0].size() == 3)
            ? cv::Scalar(static_cast<double>(g3[0][2]),
                        static_cast<double>(g3[0][1]),
                        static_cast<double>(g3[0][0]))
            : cv::Scalar(255, 0, 0);

        // Fortschritts-Farben. FarbeLive existiert bereits; FarbeErledigt ist neu
        // und OPTIONAL (fehlt der Knoten, greift der Default - kein Setup-Fehler).
        auto [live, okLive, codeLive] = controller.readInt16Array("FarbeLive");
        params.liveColor = (okLive && live.size() == 1 && live[0].size() == 3)
            ? cv::Scalar(static_cast<double>(live[0][2]),
                        static_cast<double>(live[0][1]),
                        static_cast<double>(live[0][0]))
            : cv::Scalar(0, 255, 255);  // Default: Gelb (BGR)

        auto [done, okDone, codeDone] = controller.readInt16Array("FarbeErledigt");
        params.doneColor = (okDone && done.size() == 1 && done[0].size() == 3)
            ? cv::Scalar(static_cast<double>(done[0][2]),
                        static_cast<double>(done[0][1]),
                        static_cast<double>(done[0][0]))
            : cv::Scalar(90, 90, 90);   // Default: Grau (BGR)

        // Strichstärke der Fortschritts-Zustände. StaerkeLive existiert bereits;
        // StaerkeErledigt ist neu und OPTIONAL (0 = normale Stärke behalten).
        auto [stLive, okStLive, codeStLive] = controller.readInt16("StaerkeLive");
        params.liveThickness = okStLive ? stLive : 3;
        auto [stDone, okStDone, codeStDone] = controller.readInt16("StaerkeErledigt");
        params.doneThickness = okStDone ? stDone : 0;
    }
    catch (const std::exception& e)
    {
        ErrorLoggerSingleton::instance().logError(
            "Fehler beim Konvertieren der Farben: " + std::string(e.what())
        );
    }
    
    // Dicken laden
    auto [thicknessG0, okT0, codeT0] = controller.readInt16("streghtsG0");
    auto [thicknessG1, okT1, codeT1] = controller.readInt16("streghtsG1");
    auto [thicknessG2, okT2, codeT2] = controller.readInt16("streghtsG2");
    auto [thicknessG3, okT3, codeT3] = controller.readInt16("streghtsG3");

    params.thicknessG0 = okT0 ? thicknessG0 : 1;
    params.thicknessG1 = okT1 ? thicknessG1 : 2;
    params.thicknessG2 = okT2 ? thicknessG2 : 3;
    params.thicknessG3 = okT3 ? thicknessG3 : 4;
    
    // Offsets laden
    auto [offsetH, okH, codeH] = controller.readInt16("objektgroesseX");
    auto [offsetV, okV, codeV] = controller.readInt16("objektgroesseY");

    params.horizontalOffset = okH ? offsetH : 0;
    params.verticalOffset = okV ? offsetV : 0;

    // Schwellwert-Radius für die Bogenglättung laden (0 = deaktiviert).
    auto [minArcRadius, okArc, codeArc] = controller.readFloat("minArcRadius");
    params.minArcRadius = okArc ? static_cast<double>(minArcRadius) : 0.0;

    return params;
}

// ========== HIGH-LEVEL API ==========

std::tuple<cv::Mat, int> PreviewRenderer::renderPreview(
    const std::string& projectPath,
    const RenderParams& params)
{
    // 1. Datei parsen
    GCodeParser parser;
    auto parseResult = parser.parseFile(projectPath);
    
    if (!parseResult.success)
    {
        std::cerr << "Parsing failed for: " << projectPath << std::endl;
        for (const auto& msg : parseResult.errorMessages)
        {
            std::cerr << "  " << msg << std::endl;
        }
        
        // Leeres schwarzes Bild zurückgeben
        cv::Mat emptyImg(cv::Size(params.imageSizeX, params.imageSizeY), 
                        CV_8UC3, cv::Scalar(0, 0, 0));
        return {emptyImg, 1}; // Statuscode 1 = Fehler
    }
    
    // 2. Rendern
    return render(parseResult.commands, params);
}

// ========== LOW-LEVEL API ==========

std::tuple<cv::Mat, int> PreviewRenderer::render(
    const std::vector<GCodeParser::Command>& commands,
    const RenderParams& params)
{
    // Schwarzes Bild erstellen
    cv::Mat img(cv::Size(params.imageSizeX, params.imageSizeY), 
                CV_8UC3, cv::Scalar(0, 0, 0));
    
    // Referenz-Achsen zeichnen
    if (params.drawReferenceAxis)
    {
        drawReferenceAxis(img, params);
    }
    
    // Befehle rendern
    cv::Point2d currentPos(0.0, 0.0);
    
    for (const auto& cmd : commands)
    {
        try
        {
            renderCommand(img, cmd, currentPos, params);
            currentPos = toOpenCV(cmd.endPosition);
        }
        catch (const std::exception& e)
        {
            std::cerr << "Rendering error at line " << cmd.lineNumber 
                     << ": " << e.what() << std::endl;
        }
    }
    
    // Post-Processing
    if (postProcessor_)
    {
        img = postProcessor_(img);
    }
    else if (params.enableMirroring)
    {
        Preprocessing prep;
        prep.setImg(img);
        prep.mirrorImg(0);
        img = prep.getImg();
    }
    
    return {img, 0}; // Statuscode 0 = Erfolg
}

cv::Point2d PreviewRenderer::renderRange(
    cv::Mat& img,
    const std::vector<GCodeParser::Command>& commands,
    std::size_t from, std::size_t to,
    const cv::Point2d& startPos,
    const RenderParams& params) const
{
    cv::Point2d cur = startPos;
    const std::size_t end = std::min(to, commands.size());
    for (std::size_t i = from; i < end; ++i)
    {
        try
        {
            renderCommand(img, commands[i], cur, params);
        }
        catch (const std::exception&)
        {
        }
        cur = toOpenCV(commands[i].endPosition);
    }
    return cur;
}

// ========== PRIVATE RENDERING METHODS ==========

void PreviewRenderer::drawReferenceAxis(cv::Mat& img, const RenderParams& params) const
{
    const int hOffset = params.horizontalOffset;
    const int vOffset = params.verticalOffset;
    const int width = params.imageSizeX;
    const int height = params.imageSizeY;
    
    // Vertikale Linie (Y-Achse)
    cv::line(img,
             cv::Point(static_cast<int>(std::llround(hOffset * 1.0 * FP_SCALE)), 0),
             cv::Point(static_cast<int>(std::llround(hOffset * 1.0 * FP_SCALE)),
                      height * FP_SCALE),
             params.colorG0,
             params.thicknessG0,
             (params.antialiased ? cv::LINE_AA : cv::LINE_8),
             FP_SHIFT);
    
    // Horizontale Linie (X-Achse)
    cv::line(img,
             cv::Point(0, static_cast<int>(std::llround(vOffset * 1.0 * FP_SCALE))),
             cv::Point(width * FP_SCALE,
                      static_cast<int>(std::llround(vOffset * 1.0 * FP_SCALE))),
             params.colorG0,
             params.thicknessG0,
             (params.antialiased ? cv::LINE_AA : cv::LINE_8),
             FP_SHIFT);
}

void PreviewRenderer::renderCommand(
    cv::Mat& img,
    const GCodeParser::Command& cmd,
    const cv::Point2d& startPos,
    const RenderParams& params) const
{
    using CT = GCodeParser::CommandType;

    if (cmd.type == CT::None)
        return;

    cv::Point2d endPos = toOpenCV(cmd.endPosition);
    cv::Point2d arcCtr = toOpenCV(cmd.arcCenter);

    const bool isArc = (cmd.type == CT::G02 || cmd.type == CT::G03);

    // Wird dieser Bogen zu einer Linie geglättet? Dann als Linie (G1) behandeln.
    bool flattenToLine = false;
    if (isArc)
    {
        const double r = std::hypot(arcCtr.x, arcCtr.y);
        flattenToLine = arc_flatten::shouldFlatten(r, params.minArcRadius);
    }

    // Basisfarbe/-stärke nach G-Typ (ein geglätteter Bogen ist eine G01-Linie).
    cv::Scalar color;
    int thickness;
    switch (cmd.type)
    {
    case CT::G00: color = params.colorG0; thickness = params.thicknessG0; break;
    case CT::G01: color = params.colorG1; thickness = params.thicknessG1; break;
    case CT::G02: color = params.colorG2; thickness = params.thicknessG2; break;
    case CT::G03: color = params.colorG3; thickness = params.thicknessG3; break;
    default: return;
    }
    if (flattenToLine)
    {
        color = params.colorG1;
        thickness = params.thicknessG1;
    }

    // Fortschritts-Einfärbung (überschreibt nur die Farbe): erledigt/aktuell.
    // Offene Befehle bleiben in Normalfarbe. currentRecord < 0 => Feature aus.
    if (params.currentRecord >= 0 && cmd.recordIndex >= 0)
    {
        if (cmd.recordIndex < params.currentRecord)
        {
            color = params.doneColor;
            if (params.doneThickness > 0) thickness = params.doneThickness;
        }
        else if (cmd.recordIndex == params.currentRecord)
        {
            color = params.liveColor;
            if (params.liveThickness > 0) thickness = params.liveThickness;
        }
    }

    if (isArc && !flattenToLine)
    {
        const ArcDirection dir = (cmd.type == CT::G02)
            ? ArcDirection::Clockwise
            : ArcDirection::CounterClockwise;
        renderArc(img, startPos, endPos, arcCtr, dir, color, thickness, params);
    }
    else
    {
        renderLine(img, startPos, endPos, color, thickness, params);
    }
}

void PreviewRenderer::renderLine(
    cv::Mat& img,
    const cv::Point2d& start,
    const cv::Point2d& end,
    const cv::Scalar& color,
    int thickness,
    const RenderParams& params) const
{
    cv::line(img,
             toFixedPoint(start.x, start.y, 
                         params.horizontalOffset, params.verticalOffset),
             toFixedPoint(end.x, end.y,
                         params.horizontalOffset, params.verticalOffset),
             color,
             thickness,
             (params.antialiased ? cv::LINE_AA : cv::LINE_8),
             FP_SHIFT);
}

void PreviewRenderer::renderArc(
    cv::Mat& img,
    const cv::Point2d& start,
    const cv::Point2d& end,
    const cv::Point2d& centerOffset,
    ArcDirection direction,
    const cv::Scalar& color,
    int thickness,
    const RenderParams& params) const
{
    // Kreismittelpunkt berechnen
    const double cx = start.x + centerOffset.x;
    const double cy = start.y + centerOffset.y;
    
    // Radius berechnen
    const double r = std::hypot(start.x - cx, start.y - cy);

    // Degenerierter Kreis -> als Linie zeichnen
    if (r <= EPSILON)
    {
        renderLine(img, start, end, color, thickness, params);
        return;
    }

    // Kleiner Radius -> als Linie zeichnen (identische Logik wie bei den
    // gesendeten Daten, siehe arc_flatten.h). minArcRadius == 0 => deaktiviert.
    // Ein geglätteter Bogen wird zur G01-Linie und daher auch in der Linien-
    // Farbe/-Stärke (G01) gezeichnet - konsistent zu den an die SPS gesendeten
    // G01-Daten. So ist "keine Kurve mehr" auch farblich eindeutig sichtbar.
    if (arc_flatten::shouldFlatten(r, params.minArcRadius))
    {
        renderLine(img, start, end, params.colorG1, params.thicknessG1, params);
        return;
    }
    
    // Bogen als Polylinie abtasten statt via cv::ellipse zu zeichnen.
    // Grund: cv::ellipse verlangt Start-/Endwinkel in der Bild-Konvention
    // (Y nach unten), die frühere angleDeg()-Berechnung (mit -dy) lieferte
    // aber Y-oben-Winkel -> falsche Sweeps und dadurch zerrissene Bögen.
    // Die Polylinie trifft Start-/Endpunkt exakt (durchgehend) und ist
    // unabhängig von der Winkelkonvention.
    const double thetaS = std::atan2(start.y - cy, start.x - cx);
    const double thetaE = std::atan2(end.y - cy, end.x - cx);

    // Sweep in Zeichenrichtung. In der Mat-Frame (Y nach unten) erscheint eine
    // maschinen-Uhrzeigersinn-Bewegung (G02) als steigender Winkel; der finale
    // vertikale Flip der Canvas dreht die Ansicht wieder korrekt.
    double sweep = thetaE - thetaS;
    if (direction == ArcDirection::Clockwise)  // G02
    {
        while (sweep >= 0.0)
            sweep -= 2.0 * CV_PI;
    }
    else  // G03 (CounterClockwise)
    {
        while (sweep <= 0.0)
            sweep += 2.0 * CV_PI;
    }
    if (std::abs(sweep) < 1e-9)
        sweep = 2.0 * CV_PI;  // Start==Ende -> Vollkreis

    // Segmentanzahl an der Bogenlänge (in Pixeln) ausrichten: ~3 px pro Segment.
    int segs = static_cast<int>(std::llround(std::abs(sweep) * r / 3.0));
    segs = std::max(2, std::min(segs, 4000));

    std::vector<cv::Point> pts;
    pts.reserve(static_cast<std::size_t>(segs) + 1);
    for (int i = 0; i <= segs; ++i)
    {
        const double th = thetaS + sweep * (static_cast<double>(i) / segs);
        const double px = cx + r * std::cos(th);
        const double py = cy + r * std::sin(th);
        pts.push_back(toFixedPoint(px, py,
                                   params.horizontalOffset, params.verticalOffset));
    }

    const cv::Point *ppt = pts.data();
    const int npt = static_cast<int>(pts.size());
    cv::polylines(img, &ppt, &npt, /*ncontours=*/1, /*isClosed=*/false,
                  color, thickness,
                  (params.antialiased ? cv::LINE_AA : cv::LINE_8), FP_SHIFT);
}

double PreviewRenderer::angleDeg(double dx, double dy)
{
    return std::fmod(FULL_CIRCLE_DEG + std::atan2(-dy, dx) * 180.0 / CV_PI, 
                     FULL_CIRCLE_DEG);
}

cv::Point PreviewRenderer::toFixedPoint(
    double x, double y,
    int horizontalOffset,
    int verticalOffset)
{
    const double xx = x + horizontalOffset;
    const double yy = y + verticalOffset;
    return cv::Point(
        static_cast<int>(std::llround(xx * FP_SCALE)),
        static_cast<int>(std::llround(yy * FP_SCALE))
    );
}
