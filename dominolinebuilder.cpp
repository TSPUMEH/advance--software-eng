#include "Header.h"
#include <iostream>
#include <fstream>
#include <vector>
#include <chrono>
#include <utility>
#include <map>
#include <string>
#include <atomic>
#include <cstdlib>
#include <iomanip>
#include <ctime>

using std::chrono::steady_clock;
using std::chrono::microseconds;
using std::chrono::duration_cast;

namespace {
    // Accumulators for timings (microseconds) and counts.
    std::atomic<long long> Dlinebuilder{0};
    std::atomic<unsigned long> Dlinebuilder_calls{0};

    std::atomic<long long> nextright_us{0};
    std::atomic<unsigned long> nextright_calls{0};

    std::atomic<long long> fdisplayLine{0};
    std::atomic<unsigned long> fdisplayline_calls{0};

    // RAII helper to record elapsed time into provided accumulators.
    struct ScopedRecord {
        steady_clock::time_point start;
        std::atomic<long long>& accum;
        std::atomic<unsigned long>& calls;

        ScopedRecord(std::atomic<long long>& a, std::atomic<unsigned long>& c)
            : start(steady_clock::now()), accum(a), calls(c) {}

        ~ScopedRecord() {
            auto dur = duration_cast<std::chrono::microseconds>(steady_clock::now() - start).count();
            accum.fetch_add(dur, std::memory_order_relaxed);
            calls.fetch_add(1u, std::memory_order_relaxed);
        }
    };

    // Write CSV
    void writeTimingsCsv() {
        // Check if file exists
        bool fileExists = std::ifstream("timings.csv").good();

        std::ofstream csv("timings.csv", std::ios::app); // Changed to append mode
        if (!csv.is_open()) return;

        // Write header
        if (!fileExists) {
            csv << "timestamp,function,total_microseconds,call_count,average_microseconds,data_size\n";
        }

        // Get current timestamp
        auto now = std::time(nullptr);
        auto tm = *std::localtime(&now);
        char timestamp[64];
        std::strftime(timestamp, sizeof(timestamp), "%Y-%m-%d %H:%M:%S", &tm);

        auto print = [&](const char* name, std::atomic<long long>& acc, std::atomic<unsigned long>& calls) {
            unsigned long c = calls.load(std::memory_order_relaxed);
            long long total = acc.load(std::memory_order_relaxed);
            double avg = c ? static_cast<double>(total) / c : 0.0;
            csv << timestamp << ',' << name << ',' << total << ',' << c << ',' << avg << '\n';
        };

        print("DominoLineBuilder::DominoLineBuilder", Dlinebuilder, Dlinebuilder_calls);
        print("DominoLineBuilder::nextRight", nextright_us, nextright_calls);
        print("DominoLineBuilder::displayLine", fdisplayLine, fdisplayline_calls);
        csv.close();
    }

    struct AtExitRegister { AtExitRegister() { std::atexit(writeTimingsCsv); } } atExitRegister;
}





Domino::Domino(std::string theBlueSymbol, std::string theRedSymbol)
{
    blueSymbol = theBlueSymbol;
    redSymbol = theRedSymbol;
}

// Dlinebuilder for time in microseconds, Dlinebuilder_calls for counts
DominoLineBuilder::DominoLineBuilder(unsigned long int totalNumberOfDominoes, std::istream& dominoInputData)
{
    ScopedRecord rec(Dlinebuilder, Dlinebuilder_calls);

    for (unsigned long int i = 0; i < totalNumberOfDominoes; ++i)
    {
        std::string aBlueSymbol, aRedSymbol;
        std::getline(dominoInputData, aBlueSymbol, ':');
        std::getline(dominoInputData, aRedSymbol, '\n');

        disorderedDominoes.emplace(aBlueSymbol, Domino(aBlueSymbol, aRedSymbol));
    }
}

// nextright_us for time in microseconds, nextright_calls for counts
bool DominoLineBuilder::nextRight()
{
    ScopedRecord rec(nextright_us, nextright_calls);

    if (orderedLine.empty())
    {
        if (disorderedDominoes.empty())
        {
            return false;
        }
        orderedLine.push_back(disorderedDominoes.begin()->second);
        disorderedDominoes.erase(disorderedDominoes.begin());
        return true;
    }

    auto range = disorderedDominoes.equal_range(orderedLine.back().redSymbol);
    if (range.first != range.second)
    {
        orderedLine.push_back(range.first->second);
        disorderedDominoes.erase(range.first);
        return true;
    }

    return false;
}


// fdisplayLine for time in microseconds, fdisplayline_calls for counts

void DominoLineBuilder::displayLine(std::ostream& outputStream)
{
    ScopedRecord rec(fdisplayLine, fdisplayline_calls);

    for (Domino eachDomino : orderedLine)
    {
        outputStream << eachDomino.blueSymbol << ':' << eachDomino.redSymbol << ' ' << std::endl;
    }

}


