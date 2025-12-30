#include "header.h"
#include <iostream>
#include <fstream>
#include <vector>
#include <chrono>
#include <utility>
#include <map>
#include <string>
#include <atomic>
#include <cstdlib>

using std::chrono::steady_clock;
using std::chrono::microseconds;
using std::chrono::duration_cast;

namespace {
    // Accumulators for timings (microseconds) and counts.
    std::atomic<long long> g_ctor_us{0};
    std::atomic<unsigned long> g_ctor_calls{0};

    std::atomic<long long> g_next_us{0};
    std::atomic<unsigned long> g_next_calls{0};

    std::atomic<long long> g_display_us{0};
    std::atomic<unsigned long> g_display_calls{0};

    // RAII helper to record elapsed time into provided accumulators.
    struct ScopedRecord {
        steady_clock::time_point start;
        std::atomic<long long>& accum;
        std::atomic<unsigned long>& calls;

        ScopedRecord(std::atomic<long long>& a, std::atomic<unsigned long>& c)
            : start(steady_clock::now()), accum(a), calls(c) {}

        ~ScopedRecord() {
            auto dur = duration_cast<microseconds>(steady_clock::now() - start).count();
            accum.fetch_add(dur, std::memory_order_relaxed);
            calls.fetch_add(1u, std::memory_order_relaxed);
        }
    };

    // Write CSV 
    void writeTimingsCsv() {
        std::ofstream csv("timings.csv");
        if (!csv.is_open()) return;
        csv << "function,total_microseconds,call_count,average_microseconds\n";
        auto print = [&](const char* name, std::atomic<long long>& acc, std::atomic<unsigned long>& calls) {
            unsigned long c = calls.load(std::memory_order_relaxed);
            long long total = acc.load(std::memory_order_relaxed);
            double avg = c ? static_cast<double>(total) / c : 0.0;
            csv << name << ',' << total << ',' << c << ',' << avg << '\n';
        };
        print("DominoLineBuilder::DominoLineBuilder", g_ctor_us, g_ctor_calls);
        print("DominoLineBuilder::nextRight", g_next_us, g_next_calls);
        print("DominoLineBuilder::displayLine", g_display_us, g_display_calls);
        csv.close();
    }

    
    struct AtExitRegister { AtExitRegister() { std::atexit(writeTimingsCsv); } } atExitRegister;
} 

Domino::Domino(std::string theBlueSymbol, std::string theRedSymbol)
{
    blueSymbol = theBlueSymbol;
    redSymbol = theRedSymbol;
}

DominoLineBuilder::DominoLineBuilder(unsigned long int totalNumberOfDominoes, std::istream& dominoInputData)
{
    ScopedRecord rec(g_ctor_us, g_ctor_calls);

    for (unsigned long int i = 0; i < totalNumberOfDominoes; ++i)
    {
        std::string aBlueSymbol, aRedSymbol;
        std::getline(dominoInputData, aBlueSymbol, ':');
        std::getline(dominoInputData, aRedSymbol, '\n');

        disorderedDominoes.push_back(Domino(aBlueSymbol, aRedSymbol));
    }
    
}

bool DominoLineBuilder::nextRight()
{
    ScopedRecord rec(g_next_us, g_next_calls);

    if (orderedLine.empty())
    {
        orderedLine.push_back(disorderedDominoes.back());
        disorderedDominoes.pop_back();
        return true;
    }

    for (Domino currentDomino : disorderedDominoes)
    {
        if (currentDomino.blueSymbol == orderedLine.back().redSymbol)
        {
            orderedLine.push_back(currentDomino);
            return true;
        }
    }

    return false;
}

void DominoLineBuilder::displayLine(std::ostream& outputStream)
{
    ScopedRecord rec(g_display_us, g_display_calls);

    for (Domino eachDomino : orderedLine)
    {
        outputStream << eachDomino.blueSymbol << ':' << eachDomino.redSymbol << ' ' << std::endl;
    }
   
}


