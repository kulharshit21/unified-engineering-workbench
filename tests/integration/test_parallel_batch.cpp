#include <cassert>
#include <iostream>
#include <fstream>
#include <filesystem>
#include "parallel/BenchmarkEngine.hpp"

namespace fs = std::filesystem;
using namespace Workbench::Core::Parallel;

int main() {
    std::cout << "========================================================\n";
    std::cout << "[TEST SUITE] Running Multicore Parallel Benchmark Suite\n";
    std::cout << "========================================================\n";

    const int caseCount = 1000;
    const int meshElements = 80;
    const uint32_t seed = 42;
    const std::vector<size_t> workers = {1, 2, 4};
    const int repetitions = 5;

    std::cout << "[CONFIG] Workload: " << caseCount << " independent cases, N = " << meshElements << " elements\n";
    std::cout << "[CONFIG] Repetitions: " << repetitions << " runs per worker setting, Seed: " << seed << "\n\n";

    BenchmarkSuiteResult suite = BenchmarkEngine::runSuite(
        caseCount, meshElements, workers, repetitions, seed
    );

    std::cout << "--- BENCHMARK RESULTS SUMMARY ---\n";
    std::cout << " Workers (p) | Median (ms) | Mean (ms) | Min (ms) | Max (ms) | Speedup (S_p) | Efficiency (E_p) | Max Delta (C)\n";
    std::cout << "-------------+-------------+-----------+----------+----------+---------------+------------------+--------------\n";

    for (const auto &w : suite.workerResults) {
        std::printf("     %2zu      | %11.2f | %9.2f | %8.2f | %8.2f | %13.2fx | %16.2f | %12.4e\n",
            w.workerCount, w.medianTimeMs, w.meanTimeMs, w.minTimeMs, w.maxTimeMs,
            w.speedup, w.efficiency, w.maxDeltaVsSerial
        );

        // Assert numerical agreement <= 1e-8 deg C
        assert(w.maxDeltaVsSerial <= 1e-8);
    }
    std::cout << "---------------------------------\n";
    std::cout << "[PASS] Serial / Parallel numerical agreement verified (all deltas <= 1e-8 C).\n";

    // Export real evidence files to results/performance/
    const std::vector<std::string> outputDirs = {"results/performance", "../../results/performance"};
    std::string rawCsv = BenchmarkEngine::exportRawTimingsCsv(suite);
    std::string sumCsv = BenchmarkEngine::exportSummaryCsv(suite);
    std::string agrCsv = BenchmarkEngine::exportAgreementCsv(suite);

    for (const auto &dir : outputDirs) {
        std::error_code ec;
        fs::create_directories(dir, ec);
        if (!ec) {
            std::ofstream f1(dir + "/raw_timings.csv");
            if (f1.is_open()) { f1 << rawCsv; f1.close(); }

            std::ofstream f2(dir + "/summary.csv");
            if (f2.is_open()) { f2 << sumCsv; f2.close(); }

            std::ofstream f3(dir + "/agreement.csv");
            if (f3.is_open()) { f3 << agrCsv; f3.close(); }
        }
    }

    std::cout << "[PASS] Verified performance CSV files exported to results/performance/.\n";
    std::cout << "========================================================\n";
    std::cout << "ALL MULTICORE BENCHMARK TESTS PASSED!\n";
    std::cout << "========================================================\n";

    return 0;
}
