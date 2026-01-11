#pragma once
#ifndef DOMINOLINEBUILDER_H
#define DOMINOLINEBUILDER_H

#include <iostream>
#include <vector>
#include <string>
#include <unordered_map>

struct Domino
{
    std::string blueSymbol;
    std::string redSymbol;

    Domino(std::string theBlueSymbol, std::string theRedSymbol);
};

class DominoLineBuilder
{
public:
    DominoLineBuilder(unsigned long int totalNumberOfDominoes, std::istream& dominoInputData);
    bool nextRight();
    void displayLine(std::ostream& theOutputStream);

private:
    std::unordered_multimap<std::string, Domino> disorderedDominoes;
    std::vector<Domino> orderedLine;
};

#endif
 