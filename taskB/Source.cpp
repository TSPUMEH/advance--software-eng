#include <fstream>
#include <iostream>
#include <stdexcept>
#include <string>

#include "Header.h"

int main() {
    const unsigned long int datasetSize = 500;

    const std::string filepath =
        R"(C:\Users\umeh uchenna\Documents\school\code\softwareEng\dominoes-test_data (1)\dominoes-test_data\500\500-input.txt)";

    std::ifstream theInputStream(filepath);
    if (!theInputStream.good())
        throw std::runtime_error("Cannot read from specified file.");

    DominoLineBuilder theDominoLine(datasetSize, theInputStream);

    while (theDominoLine.nextRight())
        ;

    theDominoLine.displayLine(std::cout);
}
