

# include "../include/bildverarbeitung/objektdetection/objektdetection.h"
#include "../include/bildverarbeitung/preprocessing/preprocessing.h"
#include <iostream>
#include <filesystem>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui.hpp>
#include <string>
#include <vector>
#include <bitset>
#include <algorithm>

 

objectdetectionOpenCV::objectdetectionOpenCV() {}

objectdetectionOpenCV::~objectdetectionOpenCV() {}
void objectdetectionOpenCV::setImg(const cv::Mat& newImg) {
    img = newImg;  // Erzeugt eine Kopie des übergebenen Bildes
}
void objectdetectionOpenCV::setmidPoint(cv::Point2f point) {
    midPoint = point;
}

cv::Mat objectdetectionOpenCV::getImg() const {
    return img;
}

vector<cv::Vec4i> objectdetectionOpenCV::getLines() const {
    return lines;
}

vector<cv::Vec4i> objectdetectionOpenCV::getAveragedLines() const {
    return averaged_lines;
}

/**
 * @brief Finds contours in the image using OpenCV's findContours function.
 * 
 * This function detects the contours in the image stored in the class instance
 * and returns them as a vector of points. It uses the RETR_EXTERNAL mode to 
 * retrieve only the extreme outer contours and the CHAIN_APPROX_SIMPLE method 
 * to compress horizontal, vertical, and diagonal segments and leave only their 
 * end points.
 * 
 * @return std::vector<std::vector<cv::Point>> A vector of contours, where each 
 * contour is represented as a vector of points.
 */
std::vector<std::vector<cv::Point>> objectdetectionOpenCV::findContours(bool sortedToSize = false) {
    std::vector<std::vector<cv::Point>> contours;
    cv::findContours(img, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);
    if (sortedToSize) {
        std::sort(contours.begin(), contours.end(), [](const std::vector<cv::Point>& a, const std::vector<cv::Point>& b) {
            return cv::contourArea(a) > cv::contourArea(b);
        });
    }
    return contours;
}

/**
 * @brief Detects lines in an image using the Hough Line Transform and filters out similar lines.
 * 
 * This function uses the probabilistic Hough Line Transform to detect lines in the provided image.
 * It then filters out lines that are considered duplicates based on the specified distance threshold.
 * 
 * @param thresholdAkk The accumulator threshold parameter for the Hough Line Transform. Only those lines are returned that get enough votes.
 * @param minLineLength The minimum line length. Line segments shorter than this are rejected.
 * @param maxLineGap The maximum allowed gap between points on the same line to link them.
 * @param distaceThreshold The distance threshold for filtering out similar lines. Lines with endpoints closer than this distance are considered duplicates.
 * @param angleThreshold The angle threshold for filtering out similar lines. (Currently not used in the function)
 * 
 * @note The function assumes that the image is already loaded into the `img` member variable.
 * If the image is empty, an error message is printed and the function returns without processing.
 */
std::pair<std::vector<cv::Vec4i>, std::vector<int>> objectdetectionOpenCV::ConvexityDefects(const std::vector<cv::Point>& contour) {
    std::vector<cv::Vec4i> defects;
    std::vector<int> hull;
    if (contour.size() < 5) {
        return std::make_pair(defects, hull);
    }
    
    cv::convexHull(contour, hull);
    if (hull.size() < 3) {
        return std::make_pair(defects, hull);
    }
    std::vector<cv::Vec4i> convexityDefects;
    cv::convexityDefects(contour, hull, convexityDefects);
    for (const auto& defect : convexityDefects) {
        if (defect[3] > 20 * 256) {
            defects.push_back(defect);
        }
    }
    return std::make_pair(defects, hull);
}
void objectdetectionOpenCV::FindLines(int thresholdAkk, int minLineLength, int maxLineGap, int distaceThreshold, int angleThreshold) {
    if (img.empty()) {
        std::cerr << "Kein Bild für die Linienfindung verfügbar!" << std::endl;
        return;
    }
    cv::HoughLinesP(img, lines, 1, CV_PI / 180, thresholdAkk, minLineLength, maxLineGap);

    //filterSimilarLines(distaceThreshold);
    
    filtered_lines.clear();
            for (const auto& line : lines) {
                bool is_duplicate = false;
                for (const auto& filtered_line : filtered_lines) {
                    double dist1 = cv::norm(cv::Point(line[0], line[1]) - cv::Point(filtered_line[0], filtered_line[1]));
                    double dist2 = cv::norm(cv::Point(line[2], line[3]) - cv::Point(filtered_line[2], filtered_line[3]));
                    if (dist1 < distaceThreshold && dist2 < distaceThreshold) {
                        is_duplicate = true;
                        break;
                    }
                }
                if (!is_duplicate) {
                    filtered_lines.push_back(line);
                }
            }
            
        }

/**
 * @brief Averages similar lines based on distance and angle thresholds.
 * 
 * This function takes a set of lines and averages those that are close to each other
 * based on the provided distance and angle thresholds. The result is a set of averaged lines.
 * 
 * @param distaceThreshold The maximum distance between the midpoints of two lines for them to be considered similar.
 * @param angleThreshold The maximum angle difference between two lines for them to be considered similar.
 * 
 * The function iterates through the filtered lines, grouping lines that are close to each other
 * and averaging their coordinates to produce a single representative line for each group.
 * 
 * @note The function assumes that the input lines are stored in the member variable `filtered_lines`
 * and the averaged lines are stored in the member variable `averaged_lines`.
 */
void objectdetectionOpenCV::AverageLines(int distaceThreshold, int angleThreshold) {

    
        std::vector<bool> used(lines.size(), false);

        for (size_t i = 0; i < filtered_lines.size(); ++i) {
            if (used[i]) continue;
            used[i] = true;

            std::vector<cv::Vec4i> similar_lines = { filtered_lines[i] };
            for (size_t j = i + 1; j < filtered_lines.size(); ++j) {
                if (used[j]) continue;

                double mid_x1 = (filtered_lines[i][0] + filtered_lines[i][2]) / 2.0;
                double mid_x2 = (filtered_lines[j][0] + filtered_lines[j][2]) / 2.0;
                if (std::abs(mid_x1 - mid_x2) < distaceThreshold) {
                    similar_lines.push_back(filtered_lines[j]);
                    used[j] = true;
                }
            }

            cv::Vec4i avg_line(0, 0, 0, 0);
            for (const auto& line : similar_lines) {
                avg_line[0] += line[0];
                avg_line[1] += line[1];
                avg_line[2] += line[2];
                avg_line[3] += line[3];
            }
            
            int n = static_cast<int>(similar_lines.size());
            avg_line[0] /= n; avg_line[1] /= n;
            avg_line[2] /= n; avg_line[3] /= n;
            averaged_lines.push_back(avg_line);
        }

    }

/**
 * @brief Calculates the mean distance of the midpoints of lines to a given midpoint on the x-axis.
 *
 * This function iterates over the `averaged_lines` and computes the x-coordinate of the midpoint
 * for each line. It then calculates the absolute distance between this midpoint and a given 
 * midpoint (`midPoint.x`). The mean of these distances is returned.
 *
 * @return The mean distance of the midpoints of lines to the given midpoint on the x-axis.
 */
float objectdetectionOpenCV::meanDistLinesToMidPointX() {
    float meanDist = 0;
    for (const auto& line : averaged_lines) {
        float midLineX = static_cast<float>((line[0] + line[2]) / 2.0);
        meanDist += std::abs(midLineX - midPoint.x);
    }
    return meanDist / averaged_lines.size();
    }

/**
 * @brief Finds the largest object within a specified size range in the image.
 *
 * This function detects contours in the image and identifies the largest object
 * whose area falls within the specified size range. It returns the bounding box
 * of the largest object and the contours found in the image.
 *
 * @param maxSize The maximum allowed size of the object.
 * @param minSize The minimum allowed size of the object.
 * @param type The type of search to perform (1 for largest area, 2 for other criteria).
 * @return A pair containing the bounding box of the largest object and the contours found.
 */
std::tuple<cv::Rect, cv::RotatedRect , std::vector<std::vector<cv::Point>>> objectdetectionOpenCV::findObjekt(int maxSize, int minSize, int type) {
    // Standardmäßige leere Box
    cv::Rect box;
    cv::RotatedRect rotatedBox;
    std::vector<std::vector<cv::Point>> contours;

    // Überprüfen, ob ein Bild vorhanden ist
    if (img.empty()) {
        std::cout << "Kein Bild für die Objekterkennung verfügbar!" << std::endl;
        return {box,rotatedBox, contours};
    }
    
    // Konturen finden
    cv::findContours(img.clone(), contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);
    if (contours.empty()) {
        return {box, rotatedBox, contours};
    }

    // Größte Fläche finden
    double maxArea = 0;
    std::vector<std::vector<cv::Point>> largestContour;

    for (const auto& contour : contours) {
        double area = cv::contourArea(contour);

        // Überprüfen, ob die Fläche in den erlaubten Bereich fällt
        if (area > minSize && area < maxSize) {
            if (type == 1) {  // Nach größter Fläche suchen
                if (area > maxArea) {
                    maxArea = area;
                    box = cv::boundingRect(contour);
                    rotatedBox = cv::minAreaRect(contour);
                    largestContour = {contour};
                }
            } else if (type == 2) {
                std::cout << "coming soon" << std::endl;
            }
        }
    }

    return {box, rotatedBox, largestContour};
}

double objectdetectionOpenCV::calculateObjectPercentageInBoundingBox(cv::Rect box, const std::vector<cv::Point>& contour) {
    double percent = 0;
    double area = cv::contourArea(contour);
    double boxArea = box.width * box.height;
    percent = (area / boxArea) ; 
    return percent;
}

/**
 * @brief Calculates the average rotation angle to the midpoint in the X direction.
 *
 * This function iterates through a collection of averaged lines and calculates the rotation angle
 * required to align each line's midpoint with a given midpoint in the X direction. The rotation angle
 * is calculated based on the pixel-to-degree conversion factor provided by the PixInDegree parameter.
 *
 * @param PixInDegree The conversion factor from pixels to degrees.
 * @return The average rotation angle required to align the midpoints of the lines with the given midpoint.
 */
float objectdetectionOpenCV::rotationToMidX(float  PixInDegree) {
        float rotation = 0;
        float rot;
        for (const auto& line : averaged_lines) {
            if (line[0] > midPoint.x && line[2] > midPoint.x) {
                float midLineX = static_cast<float>((line[0] + line[2]) / 2.0);
                rot = 90 - std::abs(midLineX - midPoint.x) * PixInDegree;
            }
            else if (line[0] <  midPoint.x && line[2] <  midPoint.x ){
                float midLineX = static_cast<float>((line[0] + line[2]) / 2.0);
                rot = std::abs(midLineX - midPoint.x) * PixInDegree;
            }
            else {
                float midLineX = static_cast<float>((line[0] + line[2]) / 2.0);
                rot = std::abs(midLineX - midPoint.x) * PixInDegree;
            }
            rotation += rot;
        }
        return rotation / averaged_lines.size();

    }





    




    


    
