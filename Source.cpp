#include <fstream>
#include <iostream>
#include <stdexcept>
#include <string>

#include "Header.h"

int main() {
    const unsigned long int datasetSize = 5000;

    const std::string filepath =
        R"(/Users/sentineli/Documents/code projects/advance-software-coursework/dominoes-test_data/5000/5000-input.txt)";

    std::ifstream theInputStream(filepath);
    if (!theInputStream.good())
        throw std::runtime_error("Cannot read from specified file.");

    DominoLineBuilder theDominoLine(datasetSize, theInputStream);

    while (theDominoLine.nextRight())
        ;

    theDominoLine.displayLine(std::cout);
}
