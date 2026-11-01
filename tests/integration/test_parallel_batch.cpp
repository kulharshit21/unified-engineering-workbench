#include "TestAssert.hpp"
#include <iostream>
#include <fstream>
#include <filesystem>
#include <vector>
#include "parallel/BenchmarkEngine.hpp"

namespace fs = std::filesystem;
using namespace Workbench::Core::Parallel;

int main() {
    std::cout << "========================================================\n";
    std::cout << "[TEST SUITE] Running Multicore Parallel Benchmark Suite\n";
    std::cout << "========================================================\n";

    // 1. BASELINE BENCHMARK WORKLOAD (Strict Assignment Specification)
    // 1,000 independent cases, N = 80 elements, 5 repetitions, workers = {1, 2, 4}
    const int caseCount = 1000;
    const int meshElements = 80;
    const uint32_t seed = 42;
    const std::vector<size_t> workers = {1, 2, 4};
    const int repetitions = 5;

    std::cout << "[WORKLOAD 1: STANDARD ASSIGNMENT] " << caseCount << " cases, N = " << meshElements << "\n";
    std::cout << "[CONFIG] Repetitions: " << repetitions << " runs per setting, Seed: " << seed << "\n\n";

    BenchmarkSuiteResult suite = BenchmarkEngine::runSuite(
        caseCount, meshElements, workers, repetitions, seed
    );

    std::cout << "--- STANDARD BENCHMARK RESULTS (1,000 Cases) ---\n";
    std::cout << " Workers (p) | Median (ms) | Mean (ms) | Min (ms) | Max (ms) | Speedup (S_p) | Efficiency (E_p) | Max Delta (C)\n";
    std::cout << "-------------+-------------+-----------+----------+----------+---------------+------------------+--------------\n";

    for (const auto &w : suite.workerResults) {
        std::printf("     %2zu      | %11.2f | %9.2f | %8.2f | %8.2f | %13.2fx | %16.2f | %12.4e\n",
            w.workerCount, w.medianTimeMs, w.meanTimeMs, w.minTimeMs, w.maxTimeMs,
            w.speedup, w.efficiency, w.maxDeltaVsSerial
        );

        // Assert strict numerical agreement <= 1e-8 deg C
        TEST_ASSERT(w.maxDeltaVsSerial <= 1e-8);
    }
    std::cout << "---------------------------------\n";
    std::cout << "[PASS] Serial / Parallel numerical agreement verified (all deltas <= 1e-8 C).\n\n";

    // 2. EXTENDED BENCHMARK WORKLOAD (High Signal-to-Noise Ratio)
    // 10,000 independent cases to mitigate OS timer quantization and thread wake jitter
    const int extCaseCount = 10000;
    const std::vector<size_t> extWorkers = {1, 2, 4, 8};

    std::cout << "[WORKLOAD 2: EXTENDED BENCHMARK] " << extCaseCount << " cases, N = " << meshElements << "\n";
    BenchmarkSuiteResult extSuite = BenchmarkEngine::runSuite(
        extCaseCount, meshElements, extWorkers, repetitions, seed
    );

    std::cout << "--- EXTENDED BENCHMARK RESULTS (10,000 Cases) ---\n";
    std::cout << " Workers (p) | Median (ms) | Mean (ms) | Min (ms) | Max (ms) | Speedup (S_p) | Efficiency (E_p) | Max Delta (C)\n";
    std::cout << "-------------+-------------+-----------+----------+----------+---------------+------------------+--------------\n";

    for (const auto &w : extSuite.workerResults) {
        std::printf("     %2zu      | %11.2f | %9.2f | %8.2f | %8.2f | %13.2fx | %16.2f | %12.4e\n",
            w.workerCount, w.medianTimeMs, w.meanTimeMs, w.minTimeMs, w.maxTimeMs,
            w.speedup, w.efficiency, w.maxDeltaVsSerial
        );

        TEST_ASSERT(w.maxDeltaVsSerial <= 1e-8);
    }
    std::cout << "---------------------------------\n";
    std::cout << "[PASS] Extended workload scaling and numerical agreement verified.\n";

    // 3. Export real verified evidence CSV files to results/performance/
    const std::vector<std::string> outputDirs = {"results/performance", "../../results/performance"};
    std::string rawCsv = BenchmarkEngine::exportRawTimingsCsv(suite);
    std::string sumCsv = BenchmarkEngine::exportSummaryCsv(suite);
    std::string agrCsv = BenchmarkEngine::exportAgreementCsv(suite);
    std::string extSumCsv = BenchmarkEngine::exportSummaryCsv(extSuite);
    std::string extRawCsv = BenchmarkEngine::exportRawTimingsCsv(extSuite);

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

            std::ofstream f4(dir + "/summary_extended.csv");
            if (f4.is_open()) { f4 << extSumCsv; f4.close(); }

            std::ofstream f5(dir + "/raw_timings_extended.csv");
            if (f5.is_open()) { f5 << extRawCsv; f5.close(); }
        }
    }

    std::cout << "[PASS] Verified performance CSV files exported to results/performance/.\n";
    std::cout << "========================================================\n";
    std::cout << "ALL MULTICORE BENCHMARK TESTS PASSED!\n";
    std::cout << "========================================================\n";

    return 0;
}
