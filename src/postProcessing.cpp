/*
Bildverarbeitung.waitUntilAllProcessed();

std::cout << "Alle Bilder wurden verarbeitet. Weiter im Code..." << std::endl;
// =================== raster zusammmenfügen ===================
if (!rasterImages.empty())
{
    cv::Mat combined;
    // Combine  rasters back into a single image

    int vectorSize = LabelIOVector.size();

    std::fill(std::begin(labelIOArray), std::end(labelIOArray), LabelIO{-1, -1, -1, -1});
    for (int i = 0; i < vectorSize; i++)
    {
        labelIOArray[i] = LabelIOVector[i];
    }
    for (int i = 0; i < labelIOArraySize; ++i)
    {
        std::cout << "LabelIOArray[" << i << "]: "
                  << "LabelIO=" << labelIOArray[i].LabelIO << ", "
                  << "DateIO=" << labelIOArray[i].DateIO << ", "
                  << "ProductIdIO=" << labelIOArray[i].ProductIdIO << ", "
                  << "BarcodeIO=" << labelIOArray[i].BarcodeIO << std::endl;
    }
    // Schreibe Ergebnisse mmit ADS

    for (int i = 0; i < labelIOArraySize; i++)
    {
        // SPSContr.writeLabelIO(LabelIOArrayHandle[i], labelIOArray[i]);
    }

    if (displyImg)
    {
        cv::Mat resizedImg;
        cv::resize(combinedImage, resizedImg, cv::Size(300, 600));
        cv::imshow("Combined Rasters", resizedImg);
        cv::waitKey(1);
    }

    auto end = std::chrono::high_resolution_clock::now(); // Endzeit
    std::chrono::duration<double> elapsed = end - start;  // Zeitdifferenz
    std::cout << "Zeit: " << elapsed.count() << "s" << std::endl;
    // =================== Bild speichern ===================

    if (saveImgsForTesting)
    {
        std::string FilePath1 = speicherortBilderForDataAnalysis;
        std::string FilePath2 = speicherortBilderForDataAnalysis;
        bool iO = true;

        for (const auto &label : labelIOArray)
        {
            if (label.LabelIO == 0 || label.DateIO == 0 || label.ProductIdIO == 0 || label.BarcodeIO == 0)
            {

                FilePath1 += "drawings/nIO/";
                FilePath2 += "originals/nIO/";
                iO = false;
                break;
            }
        }
        if (iO)
        {
            FilePath1 += "drawings/IO/";
            FilePath2 += "originals/IO/";
        }
        auto now = std::chrono::system_clock::now();
        std::time_t now_time = std::chrono::system_clock::to_time_t(now);
        std::tm local_tm;
        localtime_s(&local_tm, &now_time);
        std::ostringstream oss;
        oss << std::put_time(&local_tm, "%Y-%m-%d_%H-%M-%S");
        FilePath1 += oss.str() + ".jpg";
        FilePath2 += oss.str() + +".jpg";
        std::filesystem::path dirPath1 = std::filesystem::path(FilePath1).parent_path();
        std::filesystem::path dirPath2 = std::filesystem::path(FilePath2).parent_path();
        if (!std::filesystem::exists(dirPath1))
        {
            try
            {
                std::filesystem::create_directories(dirPath1);
            }
            catch (const std::filesystem::filesystem_error &e)
            {
                std::cerr << "Fehler beim Erstellen des Verzeichnisses: " << e.what() << std::endl;
                SPSContr.writeBool("error", true);
                SPSContr.writeChars("Info:Text", "Fehler : Fehler beim Erstellen des Verzeichnisses!", numChars);
            }
        }
        if (!std::filesystem::exists(dirPath2))
        {
            try
            {
                std::filesystem::create_directories(dirPath2);
            }
            catch (const std::filesystem::filesystem_error &e)
            {
                std::cerr << "Fehler beim Erstellen des Verzeichnisses: " << e.what() << std::endl;
                SPSContr.writeBool("error", true);
                SPSContr.writeChars("Info:Text", "Fehler : Fehler beim Erstellen des Verzeichnisses!", numChars);
            }
        }
        try
        {
            cv::imwrite(FilePath1, combinedImage);
            cv::imwrite(FilePath2, frame);
        }
        catch (const std::exception &e)
        {
            std::cerr << e.what() << '/n';
        }
    }
    try
    {
        cv::imwrite(speicherortBilder, combinedImage);
    }
    catch (const std::exception &e)
    {
        std::cerr << e.what() << '/n';
    }
}
else
{
    std::cerr << "Fehler: Keine Raster gefunden!" << std::endl;
    SPSContr.pushErrorMessage("Fehler: Keine Raster gefunden!", 1);
    ErrorLoggerSingleton::instance().logError("Fehler: Keine Raster gefunden!");
}

// =================== Trigger Beenden  ===================
SPSContr.writeBool("Busy", false);
SPSContr.writeBool("Done", true);
*/