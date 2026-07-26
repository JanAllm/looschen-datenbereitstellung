#include "../include/bildverarbeitung/preprocessing/preprocessing.h"
#include <iostream>


// Konstruktor
Preprocessing::Preprocessing() {}

Preprocessing::~Preprocessing() {}

// Setzt das Originalbild
void Preprocessing::setImg(const cv::Mat& newImg) {
    img = newImg.clone();  // Erzeugt eine Kopie des übergebenen Bildes
}

// Gibt das verarbeitete Bild zurück
cv::Mat Preprocessing::getImg() const {
    return img;
}

// Führt eine zylindrische Transformation durch
void Preprocessing::cylindricalTransform() {
    if (img.empty()) {
        std::cerr << "Kein Bild gesetzt für die Transformation!" << std::endl;
        return;
    }
    // Hier sollte der eigentliche Transformationscode stehen
    // Zylindertransformation auf `img` anwenden
}
/**
 * @brief Resizes the image to the specified width and height.
 * 
 * This function changes the dimensions of the image to the given width and height.
 * If the image is empty, an error message is printed and the function returns without
 * performing any resizing.
 * 
 * @param width The desired width of the resized image.
 * @param height The desired height of the resized image.
 */
void Preprocessing::resizeImage(int width, int height) {
    if (img.empty()) {
        std::cerr << "Kein Bild für die Größenänderung verfügbar!" << std::endl;
        return;
    }
    cv::resize(img, img, cv::Size(width, height));  // Größenänderung des Bildes
}
/**
 * @brief Converts the image to grayscale.
 * 
 * This function checks if the image is empty. If the image is not empty,
 * it converts the image from BGR color space to grayscale using OpenCV's
 * cvtColor function.
 * 
 * @note If the image is empty, an error message is printed to the standard error stream.
 */
void Preprocessing::convertToGray() {
    if (img.empty()) {
        std::cerr << "Kein Bild für die Konvertierung in Graustufen verfügbar!" << std::endl;
        return;
    }
    if (img.channels() == 1) {
        std::cerr << "Das Bild ist bereits in Graustufen!" << std::endl;
        return;
    }
    cv::cvtColor(img, img, cv::COLOR_BGR2GRAY);  // Konvertierung in Graustufen
}

/**
 * @brief Applies a Region of Interest (ROI) mask to an image using two concentric circles.
 * 
 * This function creates two circular masks: an inner circle and an outer circle. 
 * It then performs a bitwise XOR operation to create a ring-shaped mask between 
 * the two circles. The resulting mask is applied to the image, setting all pixels 
 * outside the ring to black.
 * 
 * @param center The center point of the circles (cv::Point).
 * @param innerRadius The radius of the inner circle.
 * @param outRadious The radius of the outer circle.
 * 
 * The function assumes that the image (`img`) is a member of the `Preprocessing` class 
 * and is already initialized. The resulting image will have all pixels outside the 
 * ring-shaped ROI set to black.
 */
void Preprocessing::ROIWithInnerAndOutCircles (cv::Point center, int innerRadius, int outRadious) {
    
    cv::Mat mask1 = cv::Mat::zeros(img.size(), CV_8UC1);  // Erste Maske (schwarz)
    cv::Mat mask2 = cv::Mat::zeros(img.size(), CV_8UC1);  // Zweite Maske (schwarz)

    // Zentrierkoordinaten und Durchmesser
    
    int diameter1 = innerRadius;    // Durchmesser des inneren Kreises
    int diameter2 = outRadious;    // Durchmesser des äußeren Kreises

    // Zeichnen der Kreise
    circle(mask1, center, diameter1 / 2, cv::Scalar(255), -1); // Fülle den inneren Kreis
    circle(mask2, center, diameter2 / 2, cv::Scalar(255), -1); // Fülle den äußeren Kreis

    // Bitweise XOR-Operation der beiden Masken
    cv::Mat mask;
    bitwise_xor(mask2, mask1, mask);

    // Wende die Maske auf das Bild an (Setze Pixel auf 0, wo mask = 0)
    img.setTo(cv::Scalar(0, 0, 0), ~mask);
}



/**
 * @brief Führt eine Morphologie-Öffnungsoperation auf dem Bild durch.
 * 
 * Die Öffnungsoperation wird verwendet, um kleine Objekte im Bild zu entfernen
 * oder schmale Verbindungen zwischen Objekten zu trennen. Sie besteht aus einer
 * Erosion, gefolgt von einer Dilatation, unter Verwendung eines strukturellen
 * Elements (Kernel).
 * 
 * @param kernelSize Die Größe des quadratischen Kernels, der für die Operation
 * verwendet wird. Die Kernelgröße muss eine ungerade Zahl sein.
 * 
 * @note Wenn das Bild leer ist, wird eine Fehlermeldung ausgegeben und die
 * Funktion beendet. Ebenso wird eine Fehlermeldung ausgegeben, wenn die
 * Kernelgröße keine ungerade Zahl ist.
 * 
 * @warning Stellen Sie sicher, dass das Bild geladen ist und die Kernelgröße
 * korrekt angegeben wird, um unerwartete Ergebnisse zu vermeiden.
 */
void Preprocessing::opening(int kernelSize) {
    if (img.empty()) {
        std::cerr << "Kein Bild für die Morphologie-Operation verfügbar!" << std::endl;
        return;
    }
    if (kernelSize % 2 == 0) {
        std::cerr << "Kernelgröße für die Morphologie-Operation muss ungerade sein!" << std::endl;
        return;
    }
    cv::Mat kernel = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(kernelSize, kernelSize));
    cv::morphologyEx(img, img, cv::MORPH_OPEN, kernel);
}
/**
 * @brief Führt eine Closing-Operation (Morphologische Operation) auf dem Bild durch.
 * 
 * Die Closing-Operation besteht aus einer Dilatation, gefolgt von einer Erosion.
 * Sie wird verwendet, um kleine Löcher oder Lücken in Objekten zu schließen.
 * 
 * @param kernelSize Die Größe des quadratischen Kernels, der für die Operation verwendet wird.
 *                   Die Kernelgröße muss eine ungerade Zahl sein.
 * 
 * @note Wenn kein Bild geladen ist (img ist leer), wird eine Fehlermeldung ausgegeben
 *       und die Funktion beendet.
 * @note Wenn die Kernelgröße gerade ist, wird eine Fehlermeldung ausgegeben
 *       und die Funktion beendet.
 * 
 * @warning Stellen Sie sicher, dass ein gültiges Bild geladen ist, bevor Sie diese Funktion aufrufen.
 * 
 * @example
 * Preprocessing preprocessing;
 * preprocessing.closing(5); // Führt eine Closing-Operation mit einem 5x5-Kernel aus.
 */
void Preprocessing::applySobel(int AusrichtungX, int AusrichtungY, int KernelGroesse) {
    if (img.empty()) {
        std::cerr << "Kein Bild für Sobel-Filter verfügbar!" << std::endl;
        return;
    }
    if (KernelGroesse % 2 == 0) {
        std::cerr << "Kernelgröße für Sobel-Filter muss ungerade sein!" << std::endl;
        return;
    }
    cv::Mat sobel_x, sobel_y;
    if (AusrichtungY != 0) {
        cv::Sobel(img, sobel_y, CV_64F, 0, AusrichtungY, KernelGroesse);
        cv::Mat abs_sobely;
        cv::convertScaleAbs(sobel_y, sobel_y);

    }
    if (AusrichtungX != 0){
        cv::Sobel(img, sobel_x, CV_64F, AusrichtungX, 0, KernelGroesse);
        cv::Mat abs_sobelx;
        cv::convertScaleAbs(sobel_x, sobel_x);

    }

    if (AusrichtungX != 0 && AusrichtungY != 0) {
        
        img = sobel_x + sobel_y;
    } else if (AusrichtungX != 0) {
        
        img = sobel_x;
    } else if (AusrichtungY != 0) {
        img = sobel_y;
    }
    
}

void Preprocessing::splitImageInto1Channel(int channel) {
    if (img.empty()) {
        std::cerr << "Kein Bild für die Kanaltrennung verfügbar!" << std::endl;
        return;
    }
    if (img.channels() != 3) {
        std::cerr << "Das Bild muss 3 Kanäle haben, um in einzelne Kanäle aufgeteilt zu werden!" << std::endl;
        return;
    }
    if (channel < 0 || channel > 2) {
        std::cerr << "Ungültiger Kanalindex für die Kanaltrennung!" << std::endl;
        return;
    }
    cv::Mat channels[3];
    cv::split(img, channels);
    img = channels[channel];
}
/**
 * @brief Calculates the mean grayscale value of the image.
 * 
 * This function computes the average grayscale intensity of the image.
 * If the image is in color, it is first converted to grayscale.
 * 
 * @return double The mean grayscale value of the image.
 *                Returns -1 if the image is empty.
 */
double Preprocessing::calculateMeanGrayValue() {
    if (img.empty()) {
        std::cerr << "Kein Bild für die Berechnung des mittleren Graustufenwerts verfügbar!" << std::endl;
        return -1;
    }

    cv::Mat grayImg;
    if (img.channels() == 3) {
        cv::cvtColor(img, grayImg, cv::COLOR_BGR2GRAY);
    } else {
        grayImg = img;
    }

    cv::Scalar meanValue = cv::mean(grayImg);
    return meanValue[0];
}

// Wendet Median-Blur mit variabler Kernelgröße an
/**
 * @brief Applies a median blur filter to the image.
 *
 * This function applies a median blur filter to the image stored in the class.
 * The median blur filter is useful for reducing noise in the image. Keeps Edges sharp.
 *
 * @param KernelGroesse The size of the kernel to be used for the median blur.
 *                      The kernel size must be an odd number.
 *
 * @note If the image is empty, an error message will be printed and the function will return.
 * @note If the kernel size is even, an error message will be printed and the function will return.
 */
void Preprocessing::applyMedianBlur(int KernelGroesse) {
    if (img.empty()) {
        std::cerr << "Kein Bild für Median-Blur verfügbar!" << std::endl;
        return;
    }
    if (KernelGroesse % 2 == 0) {
        std::cerr << "Kernelgröße für Median-Blur muss ungerade sein!" << std::endl;
        return;
    }
    cv::medianBlur(img, img, KernelGroesse);  // Anwenden von Median-Blur
}

// Wendet Gaussian-Blur mit variabler Kernelgröße an
/**
 * @brief Applies Gaussian Blur to the image.
 *
 * This function applies a Gaussian Blur filter to the image stored in the class.
 * The Gaussian Blur is used to reduce image noise and detail.
 *
 * @param KernelGroesse The size of the kernel to be used for the Gaussian Blur.
 *                      The kernel size must be an odd number greater than 1.
 *                      If the kernel size is less than or equal to 1, or if it is an even number,
 *                      an error message will be printed and the function will return without applying the blur.
 *
 * @note If the image is empty, an error message will be printed and the function will return without applying the blur.
 */
void Preprocessing::applyGaussianBlur(int KernelGroesse) {
    if (img.empty()) {
        std::cerr << "Kein Bild für Gaussian-Blur verfügbar!" << std::endl;
        return;
    }
    if (KernelGroesse <= 1 || KernelGroesse % 2 == 0) {
        std::cerr << "Kernelgröße für Gaussian-Blur muss ungerade und größer als 1 sein!" << std::endl;
        return;
    }
    cv::GaussianBlur(img, img, cv::Size(KernelGroesse, KernelGroesse), 0);  // Anwenden von Gaussian-Blur
}

/**
 * @brief Applies a binary threshold to the image.
 * 
 * This function applies a binary threshold to the image stored in the `img` member variable.
 * If the image is empty, an error message is printed and the function returns without processing.
 * 
 * @param threshold The threshold value to be used for the binary thresholding.
 *                  All pixel values greater than this threshold will be set to 255, 
 *                  and all pixel values less than or equal to this threshold will be set to 0.
 */
void Preprocessing::apyplyThreshold(int threshold) {
    if (img.empty()) {
        std::cerr << "Kein Bild für Threshold verfügbar!" << std::endl;
        return;
    }
    cv::threshold(img, img, threshold, 255, cv::THRESH_BINARY);  // Anwenden von Threshold
}
void Preprocessing::applyThresholdOtsu() {
    if (img.empty()) {
        std::cerr << "Kein Bild für Otsu-Threshold verfügbar!" << std::endl;
        return;
    }
    cv::threshold(img, img, 0, 255, cv::THRESH_BINARY | cv::THRESH_OTSU);  // Anwenden von Otsu-Threshold
}
void Preprocessing::applyThresholdUpAndLow(int lower, int upper) {
    if (img.empty()) {
        std::cerr << "Kein Bild für Up-And-Low-Threshold verfügbar!" << std::endl;
        return;
    }
    cv::threshold(img, img, lower, 255, cv::THRESH_TOZERO);  // Anwenden von Up-And-Low-Threshold
    cv::threshold(img, img, upper, 255, cv::THRESH_TOZERO_INV);

}

void Preprocessing::applyThresholdMax(int threshold) {
    if (img.empty()) {
        std::cerr << "Kein Bild für Max-Threshold verfügbar!" << std::endl;
        return;
    }
    cv::threshold(img, img, threshold, 255, cv::THRESH_BINARY_INV);  // Anwenden von Max-Threshold
}
/**
 * @brief Applies adaptive thresholding to the image.
 *
 * This function applies an adaptive threshold to the image stored in the class.
 * Adaptive thresholding is useful for images with varying lighting conditions.
 *
 * @param maxValue The maximum value to use with the THRESH_BINARY and THRESH_BINARY_INV thresholding types.
 * @param adaptiveMethod The adaptive thresholding algorithm to use, either cv::ADAPTIVE_THRESH_MEAN_C or cv::ADAPTIVE_THRESH_GAUSSIAN_C.
 * @param thresholdType The type of thresholding to apply, either cv::THRESH_BINARY or cv::THRESH_BINARY_INV.
 * @param blockSize The size of the pixel neighborhood used to calculate the threshold value. It must be an odd number.
 * @param C A constant subtracted from the mean or weighted mean. It may be positive or negative.
 *
 * @note If the image is empty, an error message will be printed and the function will return without applying the threshold.
 */
void Preprocessing::appplyAdaptiveThreshold(int maxValue, int adaptiveMethod, int thresholdType, int blockSize, int C) {
    if (img.empty()) {
        std::cerr << "Kein Bild für Adaptive Threshold verfügbar!" << std::endl;
        return;
    }
    cv::adaptiveThreshold(img, img, maxValue, adaptiveMethod, thresholdType, blockSize, C);  // Anwenden von Adaptive Threshold
}
/**
 * @brief Applies the Canny edge detection algorithm to the image.
 * 
 * This method performs Canny edge detection on the image stored in the class.
 * If no image is available, an error message is printed.
 * 
 * @param threshold1 The first threshold for the hysteresis procedure.
 * @param threshold2 The second threshold for the hysteresis procedure.
 */

void Preprocessing::applyCanny(int threshold1, int threshold2) {
    if (img.empty()) {
        std::cerr << "Kein Bild für Canny-Edge-Detection verfügbar!" << std::endl;
        return;
    }
    cv::Canny(img, img, threshold1, threshold2);  // Anwenden von Canny-Edge-Detection
}



/**
 * @brief Defines a Region of Interest (ROI) in the image using normalized coordinates.
 * 
 * This function sets a Region of Interest (ROI) in the image using normalized coordinates.
 * The coordinates and dimensions are specified as values between 0 and 1, representing
 * the relative position and size of the ROI within the image.
 * 
 * @param x_norm The normalized x-coordinate of the top-left corner of the ROI (0 to 1).
 * @param y_norm The normalized y-coordinate of the top-left corner of the ROI (0 to 1).
 * @param width_norm The normalized width of the ROI (0 to 1).
 * @param height_norm The normalized height of the ROI (0 to 1).
 */
void Preprocessing::defineROI(float x_norm, float y_norm, float width_norm, float height_norm) {
    if (img.empty()) {
        std::cerr << "Kein Bild für ROI-Definition verfügbar!" << std::endl;
        return;
    }
    if (x_norm < 0 || y_norm < 0 || width_norm <= 0 || height_norm <= 0 || 
        x_norm + width_norm > 1 || y_norm + height_norm > 1) {
        std::cerr << "Ungültige normalisierte ROI-Parameter!" << std::endl;
        return;
    }

    int x = static_cast<int>(x_norm * img.cols);
    int y = static_cast<int>(y_norm * img.rows);
    int width = static_cast<int>(width_norm * img.cols);
    int height = static_cast<int>(height_norm * img.rows);

    try {
        cv::Rect roi(x, y, width, height);
        if (x + width > img.cols || y + height > img.rows) {
            std::cerr << "ROI überschreitet die Bildgrenzen!" << std::endl;
            return;
        }
        img = img(roi).clone();  // Extrahieren des ROI-Bereichs
    } catch (const cv::Exception& e) {
        std::cerr << "Fehler bei der ROI-Definition: " << e.what() << std::endl;
    }
}

/**
 * @brief Splits the image into a grid of smaller cells.
 * 
 * This function divides the image into a specified number of rows and columns,
 * creating a grid of smaller cells. Each cell is extracted and stored in a 
 * vector of cv::Mat objects.
 * 
 * @param rows The number of rows to divide the image into. Must be greater than 0.
 * @param cols The number of columns to divide the image into. Must be greater than 0.
 * @return std::vector<cv::Mat> A vector containing the smaller cells of the image.
 * 
 * @throws std::runtime_error If the image is empty.
 * @throws std::invalid_argument If the number of rows or columns is less than or equal to 0.
 */
std::vector<cv::Mat> Preprocessing::rasterize(int rows, int cols) {
    if (img.empty()) {
        throw std::runtime_error("Das Bild ist leer. Bitte ein gültiges Bild setzen.");
    }

    if (rows <= 0 || cols <= 0) {
        throw std::invalid_argument("Zeilen und Spalten müssen größer als 0 sein.");
    }
    

    // Größe der Zellen berechnen
    int cellWidth = img.cols / cols;
    int cellHeight = img.rows / rows;

    // Container für die Zellen
    std::vector<cv::Mat> cells;

    // Raster erstellen
    for (int r = 0; r < rows; ++r) {
        for (int c = 0; c < cols; ++c) {
            // Rechteck für die aktuelle Zelle definieren
            cv::Rect roi(c * cellWidth, r * cellHeight, cellWidth, cellHeight);

            // Zelle extrahieren und hinzufügen
            cells.push_back(img(roi).clone());
        }
    }

    return cells;
}

void Preprocessing::convertToHSV() {
    if (img.empty()) {
        std::cerr << "Kein Bild für die Konvertierung in HSV verfügbar!" << std::endl;
        return;
    }
    if (img.channels() != 3) {
        std::cerr << "Das Bild muss 3 Kanäle (BGR) haben, um in HSV konvertiert zu werden!" << std::endl;
        return;
    }
    cv::cvtColor(img, img, cv::COLOR_BGR2HSV);  // Konvertierung in HSV
}   

void Preprocessing::covertToBGR() {
    if (img.empty()) {
        std::cerr << "Kein Bild für die Konvertierung in GBR verfügbar!" << std::endl;
        return;
    }
    if (img.channels() != 1) {
        std::cerr << "Das Bild muss 1 Kanäle (BGR) haben, um in GBR konvertiert zu werden!" << std::endl;
        return;
    }
    cv::cvtColor(img, img, cv::COLOR_GRAY2BGR);  // Konvertierung in GBR
}

void Preprocessing::enlargeImage(int factor) {
    if (img.empty()) {
        std::cerr << "Kein Bild für die Vergrößerung verfügbar!" << std::endl;
        return;
    }
    if (factor <= 0) {
        std::cerr << "Der Vergrößerungsfaktor muss größer als 0 sein!" << std::endl;
        return;
    }
    cv::resize(img, img, cv::Size(img.cols * factor, img.rows * factor), 1);  // Vergrößerung des Bildes
}


int Preprocessing::getLeastUsedColorChannel( const cv::Mat& mask) {
    // Sicherstellen, dass das Bild im BGR-Format und die Maske binär ist
    CV_Assert(img.channels() == 3 && mask.type() == CV_8UC1);

    // Initialisiere die Summe für jeden Kanal
    std::vector<double> channelSums(3, 0.0);
    cv::Mat image = img.clone();  // Kopie des Bildes, um es nicht zu verändern
    // Iteriere durch jedes Pixel innerhalb der Maske
    for (int y = 0; y < image.rows; y++) {
        for (int x = 0; x < image.cols; x++) {
            if (mask.at<uchar>(y, x) > 0) {  // Nur Pixel innerhalb der Maske berücksichtigen
                const cv::Vec3b& pixel = image.at<cv::Vec3b>(y, x);
                channelSums[0] += pixel[0]; // Blau-Kanal
                channelSums[1] += pixel[1]; // Grün-Kanal
                channelSums[2] += pixel[2]; // Rot-Kanal
            }
        }
    }

    // Finde den Index des minimalen Werts
    int leastUsedChannelIndex = std::min_element(channelSums.begin(), channelSums.end()) - channelSums.begin();

    // Debugging: Gib die Summen und den Index aus
    std::cout << "Kanal-Summen (B, G, R): " 
              << channelSums[0] << ", " 
              << channelSums[1] << ", " 
              << channelSums[2] << std::endl;
    std::cout << "Am wenigsten genutzter Kanal: " << leastUsedChannelIndex << std::endl;

    return leastUsedChannelIndex;
}

cv::Mat Preprocessing::Histogram(cv::Mat mask) {
     cv::Mat imgGray;
    if (img.channels() == 3) {
        cv::cvtColor(img, imgGray, cv::COLOR_BGR2GRAY);
    } else {
        imgGray = img;
    }

    if (!mask.empty()) {
        imgGray = imgGray.clone();
        imgGray.setTo(0, mask == 0); // Bereiche außerhalb der Maske auf 0 setzen
    }

    // Histogramm-Parameter
    int histSize = 256; // Anzahl der Graustufen
    float range[] = {0, 256};
    const float* histRange = {range};
    bool uniform = true, accumulate = false;

    cv::Mat hist;
    cv::calcHist(&imgGray, 1, 0, mask, hist, 1, &histSize, &histRange, uniform, accumulate);

    // Dynamische Höhe und Breite festlegen
    int histHeight = 800; // Höhe des Histogramms
    int histWidth = 1000; // Breite des Histogramms
    int binWidth = cvRound((double)histWidth / histSize);

    // Maximale Häufigkeit aus dem Histogramm ermitteln
    double maxVal = 0;
    cv::minMaxLoc(hist, nullptr, &maxVal);

    // Skalierungsfaktor für die Y-Achse
    float yScale = (float)(histHeight) / (float)maxVal;
    yScale *= 20; // 90% der Höhe verwenden

    cv::Mat histImage(histHeight + 50, histWidth + 50, CV_8UC3, cv::Scalar(255, 255, 255));

    // Balken zeichnen (Y-Werte skalieren)
    for (int i = 1; i < histSize; i++) {
        int binHeight = cvRound(hist.at<float>(i) * yScale); // Skalierung anwenden
        cv::rectangle(histImage,
                      cv::Point(binWidth * (i - 1) + 25, histHeight + 25 - binHeight),
                      cv::Point(binWidth * i + 25, histHeight + 25),
                      cv::Scalar(0, 0, 255), // Farbe: Rot
                      cv::FILLED);
    }

    // Gitternetz und Achsenbeschriftung
    for (int i = 32; i <= 255; i += 32) { // x-Achse
        int x = cvRound((double)i / 256 * histWidth) + 25;
        cv::line(histImage, cv::Point(x, 25), cv::Point(x, histHeight + 25), cv::Scalar(200, 200, 200), 1);
        cv::putText(histImage, std::to_string(i), cv::Point(x - 10, histHeight + 45),
                    cv::FONT_HERSHEY_SIMPLEX, 0.4, cv::Scalar(0, 0, 0), 1);
    }

    for (int i = 0; i <= histHeight; i += 100) { // y-Achse
        int y = histHeight + 25 - i;
        cv::line(histImage, cv::Point(25, y), cv::Point(histWidth + 25, y), cv::Scalar(200, 200, 200), 1);
        cv::putText(histImage, std::to_string((int)(i / yScale)), cv::Point(5, y + 5),
                    cv::FONT_HERSHEY_SIMPLEX, 0.4, cv::Scalar(0, 0, 0), 1);
    }

    // Titel hinzufügen
    cv::putText(histImage, "Histogram", cv::Point(histWidth / 2 - 50, 20),
                cv::FONT_HERSHEY_SIMPLEX, 0.7, cv::Scalar(0, 0, 0), 2);

    return histImage;
}

void Preprocessing::SplitImgWithbetterContrastGBR(int splitChannel, float greenWeight, float blueWeight, float redWeight){

    if (img.empty()) {
        std::cerr << "Kein Bild für die Kanaltrennung verfügbar!" << std::endl;
        return;
    }
    if (img.channels() != 3) {
        std::cerr << "Das Bild muss 3 Kanäle haben, um in einzelne Kanäle aufgeteilt zu werden!" << std::endl;
        return;
    }

    std::vector<cv::Mat> channels(3);
    cv::split(img, channels);

    cv::Mat mainChhanels = channels[splitChannel];
    img = mainChhanels.clone();
    // Berechnung der Kanalmischung
    if (splitChannel == 0){ // Blau
        mainChhanels = cv::abs(mainChhanels * blueWeight - greenWeight * channels[1] - redWeight * channels[2]);
    } else if (splitChannel == 1){ // Grün
        mainChhanels = cv::abs(mainChhanels * greenWeight - blueWeight * channels[0] - redWeight * channels[2]);
    } else if (splitChannel == 2){ // Rot
        mainChhanels = cv::abs(mainChhanels * redWeight - blueWeight * channels[0] - greenWeight * channels[1]);
    } else {
        std::cerr << "Ungültiger Kanalindex für die Kanaltrennung!" << std::endl;
        return;
    }
    
    // Umwandlung des Kanals in Graustufen (CV_8UC1)
    mainChhanels.convertTo(mainChhanels, CV_8UC1); 

    // Normiere das Bild auf den Bereich [0, 255]
    cv::Mat result;
    cv::normalize(mainChhanels, result, 0, 255, cv::NORM_MINMAX);
    
    
    cv::subtract(img, result, img);
}

void Preprocessing::ROIWithBox(cv::Rect box) {
    if (img.empty()) {
        std::cerr << "Kein Bild für die ROI-Definition verfügbar!" << std::endl;
        return;
    }
    if (box.x < 0 || box.y < 0 || box.width <= 0 || box.height <= 0 || 
        box.x + box.width > img.cols || box.y + box.height > img.rows) {
        std::cerr << "Ungültige ROI-Parameter!" << std::endl;
        return;
    }
    if (box.width > img.cols || box.height > img.rows) {
        std::cerr << "Die Box ist größer als das Bild!" << std::endl;
        return;
    }

    try {
        img = img(box).clone();  // Extrahieren des ROI-Bereichs
    } catch (const cv::Exception& e) {
        std::cerr << "Fehler bei der ROI-Definition: " << e.what() << std::endl;
    }
}

/**
 * @brief Rotates the image by a specified angle.
 * 
 * This function rotates the image stored in the `img` member variable by the given angle.
 * If the image is empty, an error message is printed and the function returns immediately.
 * For an angle of 180 degrees, a fast flip operation is performed. For other angles, 
 * the image is rotated using an affine transformation.
 * 
 * @param angle The angle by which to rotate the image, in degrees.
 *              A positive value rotates the image counterclockwise.
 * 
 * @note If the image is empty, the function will print an error message and return.
 */
void Preprocessing::rotateImg(int angle) {
    if (img.empty()) {
        std::cerr << "Kein Bild für die Rotation verfügbar!" << std::endl;
        return;
    }
    if (angle == 180.0) {
        cv::flip(img, img, -1);  // Schnelle 180-Grad-Drehung
    } else {
        cv::Point2f center(img.cols / 2.0, img.rows / 2.0);
        cv::Mat rotationMatrix = cv::getRotationMatrix2D(center, angle, 1.0);
        cv::warpAffine(img, img, rotationMatrix, img.size());
    }
}
/**
 * @brief Mirrors the image along the specified axis.
 * 
 * This function mirrors the image stored in the `img` member variable
 * along the specified axis. If the image is empty, an error message
 * is printed and the function returns without performing any operation.
 * 
 * @param axis The axis along which to mirror the image.
 *             0 for x-axis, 1 for y-axis, and -1 for both axes.
 */
void Preprocessing::mirrorImg(int axis) {
    if (img.empty()) {
        std::cerr << "Kein Bild für die Spiegelung verfügbar!" << std::endl;
        return;
    }
    cv::flip(img, img, axis);
}