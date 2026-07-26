#pragma once

#include <vector>

#include <opencv2/opencv.hpp>
// OpenCV 5: contour/shape functions moved to the geometry module (not in opencv.hpp)
#if CV_VERSION_MAJOR >= 5
#include <opencv2/geometry.hpp>
#endif

#include "web/app_state.h"

/**
 * @file image_sink.h
 * @brief Bild an die Weboberfläche übergeben (ersetzt WebServer::loadImg).
 *
 * Früher ging das Bild über die Python-Brücke (JPEG + Verkleinerung + GIL).
 * Jetzt wird es verlustfrei als PNG in nativer Auflösung im AppState abgelegt
 * und von GET /image direkt ausgeliefert.
 */
inline void publishImage(AppState& state, const cv::Mat& img)
{
    if (img.empty())
        return;
    std::vector<unsigned char> buf;
    if (cv::imencode(".png", img, buf))
        state.setImage(std::move(buf));
}

/// Wie publishImage, aber vorher auf die halbe Kantenlänge verkleinert.
/// Für Animationsframes: ~4x billiger zu kodieren/übertragen.
inline void publishImageHalf(AppState& state, const cv::Mat& img)
{
    if (img.empty())
        return;
    cv::Mat small;
    cv::resize(img, small, cv::Size(img.cols / 2, img.rows / 2), 0, 0, cv::INTER_AREA);
    publishImage(state, small);
}
