#pragma once
#ifndef DOMINOLINEBUILDER_H
#define DOMINOLINEBUILDER_H

#include <iostream>
#include <vector>
#include <string>
#include <unordered_map>
#include <deque>

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
    bool nextLeft();
    void displayLine(std::ostream& theOutputStream);

private:
    void removeDominoFromMaps(const Domino& domino);
    std::unordered_multimap<std::string, Domino> disorderedDominoesByBlue;
    std::unordered_multimap<std::string, Domino> disorderedDominoesByRed;
    std::deque<Domino> orderedLine;
};

#endif
 