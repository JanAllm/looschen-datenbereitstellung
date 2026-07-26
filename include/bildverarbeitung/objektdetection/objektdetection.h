
# include <iostream>
# include <opencv2/opencv.hpp>
// OpenCV 5: contour/shape functions moved to the geometry module (not in opencv.hpp)
#if CV_VERSION_MAJOR >= 5
# include <opencv2/geometry.hpp>
#endif


using namespace std;

class objectdetectionOpenCV {

    public:
        
        // Konstruktor
        objectdetectionOpenCV();
        ~objectdetectionOpenCV();

        // Setter-Methode, um das Originalbild zu setzen
        void setImg(const cv::Mat& newImg);
        void setmidPoint(cv::Point2f point);
        // Getter-Methode, um das verarbeitete Bild zu holen
        cv::Mat getImg() const;

        
        std::vector<std::vector<cv::Point>> findContours(bool sortedToSize);
        void FindLines( int thresholdAkk, int minLineLength, int maxLineGap, int distaceThreshold, int angleThreshold);
        void AverageLines (int distaceThreshold, int angleThreshold);
        vector<cv::Vec4i> getLines() const;
        vector<cv::Vec4i> getAveragedLines() const;
        float meanDistLinesToMidPointX();
        float rotationToMidX(float PixInDegree);
        std::tuple<cv::Rect, cv::RotatedRect, std::vector<std::vector<cv::Point>>> findObjekt(int maxSize, int minSize, int type);
        double calculateObjectPercentageInBoundingBox(cv::Rect box, const std::vector<cv::Point>& contour);
        std::pair<std::vector<cv::Vec4i>, std::vector<int>> ConvexityDefects(const std::vector<cv::Point>& contour);
        vector<int>ConvexHull(const std::vector<cv::Point>& contour);

        
        // Methoden zur Bildverarbeitung




    private:

        cv::Mat img;      // Originalbild
        vector<cv::Vec4i> lines;
        std::vector<cv::Vec4i> filtered_lines;
        std::vector<cv::Vec4i> averaged_lines;

        // mittelpunk
        cv::Point2f midPoint;

        
};
