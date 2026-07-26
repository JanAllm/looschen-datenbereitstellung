#include "../include/bildverarbeitung/drawing/drawingsOpenCV.h"
#include <iostream>

using namespace std;

drawOpenCV::drawOpenCV() {}

/**
 * @brief Setzt das interne Bild.
 * 1
 * @param newImg Das neue Bild, das verwendet werden soll.
 */
void drawOpenCV::setImg(const cv::Mat& newImg) {
    if (newImg.channels() == 1) {  // Graustufenbild erkannt
        
        cv::cvtColor(newImg, img, cv::COLOR_GRAY2BGR);  // Graustufenbild in BGR umwandeln
    } else if  (newImg.channels() == 3) {  // Farbbild erkannt
        img = newImg.clone();  // Falls es bereits ein Farbbild ist, direkt setzen
    } else {
        std::cerr << "Ungültiges Bildformat! Bitte ein Bild mit 1 oder 3 Kanälen verwenden." << std::endl;
    }
}
/**
 * @brief Gibt das interne Bild zurück.
 * 
 * @return cv::Mat Das interne Bild.
 */
cv::Mat drawOpenCV::getImg() const {
    return img;  // Sicherstellen, dass keine Referenzprobleme auftreten
}

/**
 * @brief Zeichnet ein Rechteck (ROI) auf das Bild.
 * 
 * @param box Das Rechteck, das gezeichnet werden soll.
 * @param color Die Farbe des Rechtecks.
 * @param thickness Die Linienbreite des Rechtecks.
 */
void drawOpenCV::drawROI(cv::Rect box, cv::Scalar color, int thickness) {
    if (img.empty()) {
        std::cerr << "Kein Bild für die Rechteckzeichnung verfügbar!" << std::endl;
        return;
    }
    cv::rectangle(img, box, color, thickness);
}
/**
 * @brief Draws a circle on the image.
 * 
 * This function draws a circle on the image at the specified center point with the given radius, color, and thickness.
 * 
 * @param center The center point of the circle.
 * @param radius The radius of the circle.
 * @param color The color of the circle in BGR format.
 * @param thickness The thickness of the circle outline. If it is negative, the circle will be filled.
 * 
 * @note If the image is empty, an error message will be printed and the function will return without drawing.
 */
void drawOpenCV::drawCircle(cv::Point center, int radius, cv::Scalar color, int thickness) {
    if (img.empty()) {
        std::cerr << "Kein Bild für die Kreiszeichnung verfügbar!" << std::endl;
        return;
    }
    cv::circle(img, center, radius, color, thickness);
}
/**
 * @brief Zeichnet Linien auf das Bild.
 * 
 * @param lines Eine Liste von Linien, die gezeichnet werden sollen.
 * @param color Die Farbe der Linien.
 */
void drawOpenCV::drawLines(cv::Point point1, cv::Point point2, cv::Scalar color, int thickness) {
    if (img.empty()) {
        std::cerr << "Kein Bild für die Linienzeichnung verfügbar!" << std::endl;
        return;
    }
    cv::line(img, point1, point2, color, thickness);
}



void drawOpenCV::drawLinesVec(std::vector<cv::Vec4i> lines, cv::Scalar color, int thickness) {
    if (img.empty()) {
        std::cerr << "Kein Bild für die Linienzeichnung verfügbar!" << std::endl;
        return;
    }
    for (size_t i = 0; i < lines.size(); i++) {
        cv::line(img, cv::Point(lines[i][0], lines[i][1]), cv::Point(lines[i][2], lines[i][3]), color, thickness);
    }
}
/**
 * @brief Zeichnet einen Punkt auf das Bild.
 * 
 * @param point Der Punkt, der gezeichnet werden soll.
 * @param color Die Farbe des Punktes.
 */
void drawOpenCV::drawPoint(cv::Point2f point, cv::Scalar color, int thickness) {
    if (img.empty()) {
        std::cerr << "Kein Bild für die Punktzeichnung verfügbar!" << std::endl;
        return;
    }
    cv::circle(img, point, thickness, color, -1);
}

/**
 * @brief Zeichnet ein Rechteck auf ein Bild.
 * 
 * @param img Das Bild, auf dem das Rechteck gezeichnet wird.
 * @param box Das Rechteck, das gezeichnet werden soll.
 * @param color Die Farbe des Rechtecks.
 * @param thickness Die Linienbreite des Rechtecks.
 */
void drawOpenCV::drawBox(cv::Rect box, cv::Scalar color, int thickness) {
    if (img.empty()) {
        std::cerr << "Kein Bild für die Rechteckzeichnung verfügbar!" << std::endl;
        return;
    }
    cv::rectangle(img, box, color, thickness);
}

/**
 * @brief Zeichnet ein RotatedRect auf das interne Bild.
 * 
 * @param box Das RotatedRect, das gezeichnet werden soll.
 * @param color Die Farbe des Rechtecks.
 * @param thickness Die Linienbreite des Rechtecks.
 */
void drawOpenCV::drawRotatedBoundingBox(cv::RotatedRect box, cv::Scalar color, int thickness) {
    if (img.empty()) {
        std::cerr << "Kein Bild für die RotatedRect-Zeichnung verfügbar!" << std::endl;
        return;
    }
    cv::Point2f vertices[4];
    box.points(vertices);  // Punkte des RotatedRect holen
    for (int i = 0; i < 4; i++) {
        cv::line(img, vertices[i], vertices[(i + 1) % 4], color, thickness);  // Verbinde die Punkte
    }
}
void drawOpenCV::writeText(cv::Point point, std::string text, cv::Scalar color, int thickness, float size) {
    if (img.empty()) {
        std::cerr << "Kein Bild für die Textzeichnung verfügbar!" << std::endl;
        return;
    }
    cv::putText(img, text, point, cv::FONT_HERSHEY_SIMPLEX, size, color, thickness);
}

void drawOpenCV::drawContours(std::vector<std::vector<cv::Point>> contours, cv::Scalar color, int thickness) {
    if (img.empty()) {
        std::cerr << "Kein Bild für die Konturzeichnung verfügbar!" << std::endl;
        return;
    }
    cv::drawContours(img, contours, -1, color, thickness);
}
void drawOpenCV::drawContour(std::vector<cv::Point> contour, cv::Scalar color, int thickness) {
    if (img.empty()) {
        std::cerr << "Kein Bild für die Konturzeichnung verfügbar!" << std::endl;
        return;
    }
    std::vector<std::vector<cv::Point>> contours = {contour};
    cv::drawContours(img, contours, -1, color, thickness);
}
void drawOpenCV::drawFilledContours(std::vector<std::vector<cv::Point>> contours, cv::Scalar color, float transparency) {
    if (img.empty()) {
        std::cerr << "Kein Bild für die gefüllte Konturzeichnung verfügbar!" << std::endl;
        return;
    }
    cv::Mat overlay = img.clone();
    cv::drawContours(overlay, contours, -1, color, cv::FILLED);
    cv::addWeighted(overlay, transparency, img, 1 - transparency, 0, img);
}