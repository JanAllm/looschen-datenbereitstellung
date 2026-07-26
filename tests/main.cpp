#include <gtest/gtest.h>
#include <chrono>
#include <iostream>
#include <ctime>
#include <clocale>

int main(int argc, char **argv) {
    // UTF-8 für die CRT (ergänzt das UTF-8-Manifest) - Umlaut-Pfade in Tests.
    // Nur LC_CTYPE, damit LC_NUMERIC (Dezimalpunkt für std::stod) unberührt bleibt.
    std::setlocale(LC_CTYPE, ".UTF-8");

    ::testing::InitGoogleTest(&argc, argv);

    // Aktuelle Uhrzeit ausgeben
    auto now = std::chrono::system_clock::now();
    std::time_t currentTime = std::chrono::system_clock::to_time_t(now);
    std::cout << "========== Teststart: " << std::ctime(&currentTime) << "==========" << std::endl;

    // Tests ausführen
    int result = RUN_ALL_TESTS();

    std::cout << "========== Testende ==========" << std::endl;
    

    return result;
}
