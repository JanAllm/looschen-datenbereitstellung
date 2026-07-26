
#pragma once
# include <iostream>
# include <opencv2/opencv.hpp>
// OpenCV 5: contour/shape functions moved to the geometry module (not in opencv.hpp)
#if CV_VERSION_MAJOR >= 5
# include <opencv2/geometry.hpp>
#endif

using namespace std;


class drawOpenCV {
    public:
        
        // Konstruktor
        drawOpenCV();

        // Setter-Methode, um das Originalbild zu setzen
        void setImg(const cv::Mat& newImg);

        // Getter-Methode, um das verarbeitete Bild zu holen
        cv::Mat getImg() const;

         void drawLines(cv::Point point1 , cv::Point point2 , cv::Scalar color, int thickness = 3);
         void drawPoint(cv::Point2f point, cv::Scalar color, int thickness = 3);
         void drawBox( cv::Rect box, cv::Scalar color, int thickness);
         void drawROI(cv::Rect box, cv::Scalar color, int thickness);
         void drawRotatedBoundingBox( cv::RotatedRect box, cv::Scalar color, int thickness);
         void writeText(cv::Point point, std::string text, cv::Scalar color, int thickness = 3, float size = 1);
         void drawContours(std::vector<std::vector<cv::Point>> contours, cv::Scalar color, int thickness);
         void drawContour(std::vector<cv::Point> contour, cv::Scalar color, int thickness);
         void drawFilledContours(std::vector<std::vector<cv::Point>> contours, cv::Scalar color, float transparency);
         void drawLinesVec(std::vector<cv::Vec4i> lines, cv::Scalar color, int thickness);
         void drawCircle(cv::Point center, int radius, cv::Scalar color, int thickness);

        

        // Methoden zur Bildverarbeitung
    private:

        cv::Mat img;      // Originalbild
        
};
        