#ifndef IMAGEPROCESSOR_H
#define IMAGEPROCESSOR_H
#include <iostream>
#include <opencv2/opencv.hpp>


class Preprocessing {

    public:
        
        // Konstruktor
        Preprocessing();
        ~Preprocessing();
        // Setter-Methode, um das Originalbild zu setzen
        void setImg(const cv::Mat& newImg);

        // Getter-Methode, um das verarbeitete Bild zu holen
        cv::Mat getImg() const;

        // Methoden zur Bildverarbeitung
        
        void cylindricalTransform();
        void applySobel(int AusrichtungX, int AusrichtungY, int KernelGroesse);
        void applyMedianBlur(int KernelGroesse);
        void applyGaussianBlur(int KernelGroesse);
        void apyplyThreshold(int threshold);
        void applyThresholdMax(int threshold);
        void applyThresholdOtsu();
        void applyThresholdUpAndLow(int lowThreshold, int upThreshold);
        void applyCanny(int threshold1, int threshold2);
        void appplyAdaptiveThreshold(int maxValue, int adaptiveMethod, int thresholdType, int blockSize, int C);
        void resizeImage(int width, int height);
        void convertToGray();
        void convertToHSV();
        void covertToBGR();
        void defineROI(float x, float y, float width, float height);
        void opening(int kernelSize);
        void closing(int kernelSize);
        void enlargeImage(int factor);
        void ROIWithInnerAndOutCircles(cv::Point center, int innerRadius, int outRadious);
        void splitImageInto1Channel(int channel);
        int findDominantColorInMask(cv::Mat mask);
        int getLeastUsedColorChannel(const cv::Mat& mask);
        void SplitImgWithbetterContrastGBR(int splitChannel, float greenWeight, float blueWeight, float redWeight);
        void rotateImg(int angle);
        void mirrorImg(int axis);
        cv::Mat Histogram(cv::Mat mask = cv::Mat());
        std::vector<cv::Mat> rasterize( int rows, int cols);
        void ROIWithBox(cv::Rect box);
        double calculateMeanGrayValue();

    private:
        
        cv::Mat img;      // Originalbild
        
        
};



#endif // IMAGEPROCESSOR_H