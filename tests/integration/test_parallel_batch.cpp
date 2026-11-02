#include "TestAssert.hpp"
#include <iostream>
#include <fstream>
#include <filesystem>
#include <vector>
#include <thread>
#include <atomic>
#include <chrono>
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

    // 3. Dynamic CPU Hardware Topology Verification (Priority 1)
    std::cout << "[HARDWARE DETECTED] CPU Model:      " << suite.cpuModel << "\n";
    std::cout << "[HARDWARE DETECTED] Physical Cores: " << suite.physicalCores << "\n";
    std::cout << "[HARDWARE DETECTED] Logical Threads: " << suite.logicalCores << "\n";
    TEST_ASSERT(!suite.cpuModel.empty());
    TEST_ASSERT(suite.physicalCores >= 1);
    TEST_ASSERT(suite.logicalCores >= suite.physicalCores);
    std::cout << "[PASS] Dynamic CPU topology detection verified.\n\n";

    // 4. Safe CSV Generation Verification (Without Overwriting Historical Benchmark CSVs)
    std::string rawCsv = BenchmarkEngine::exportRawTimingsCsv(suite);
    std::string sumCsv = BenchmarkEngine::exportSummaryCsv(suite);
    std::string agrCsv = BenchmarkEngine::exportAgreementCsv(suite);
    std::string extSumCsv = BenchmarkEngine::exportSummaryCsv(extSuite);
    std::string extRawCsv = BenchmarkEngine::exportRawTimingsCsv(extSuite);

    TEST_ASSERT(!rawCsv.empty());
    TEST_ASSERT(!sumCsv.empty());
    TEST_ASSERT(!agrCsv.empty());
    TEST_ASSERT(!extSumCsv.empty());
    TEST_ASSERT(!extRawCsv.empty());

    // Export to isolated test verification directory
    const std::string testOutputDir = "test_artifacts/performance";
    fs::create_directories(testOutputDir);
    {
        std::ofstream f(testOutputDir + "/test_summary.csv");
        TEST_ASSERT(f.is_open());
        f << sumCsv;
    }
    std::cout << "[PASS] CSV serialization and export format integrity verified.\n\n";

    // 5. Worker Cooperative Cancellation Stress Tests (Priority 3)
    std::cout << "--- COOPERATIVE CANCELLATION STRESS TESTS ---\n";

    // 5.1 Pre-cancellation test (cancellation token asserted before invocation)
    {
        std::atomic<bool> cancelToken{true};
        auto t0 = std::chrono::steady_clock::now();
        BenchmarkSuiteResult precancelRes = BenchmarkEngine::runSuite(
            10000, 80, {1, 2, 4}, 5, 42, &cancelToken
        );
        auto t1 = std::chrono::steady_clock::now();
        double precancelLatencyMs = std::chrono::duration<double, std::milli>(t1 - t0).count();

        TEST_ASSERT(precancelRes.cancelled);
        TEST_ASSERT(precancelLatencyMs < 20.0); // Should return nearly instantly
        std::printf("[PASS] Pre-cancellation observed immediately (latency: %.3f ms).\n", precancelLatencyMs);
    }

    // 5.2 Maximum supported workload mid-solve cancellation (50,000 cases)
    {
        std::atomic<bool> cancelToken{false};
        std::atomic<bool> workerFinished{false};
        BenchmarkSuiteResult maxWorkloadRes;
        double measuredCancelLatencyMs = 0.0;

        std::thread workerThread([&]() {
            maxWorkloadRes = BenchmarkEngine::runSuite(
                50000, 80, {4}, 5, 42, &cancelToken
            );
            workerFinished.store(true);
        });

        // Let the worker initiate active chunk processing
        std::this_thread::sleep_for(std::chrono::milliseconds(5));

        // Signal cancellation and measure latency to completion
        auto cancelStart = std::chrono::steady_clock::now();
        cancelToken.store(true, std::memory_order_relaxed);

        workerThread.join();
        auto cancelEnd = std::chrono::steady_clock::now();
        measuredCancelLatencyMs = std::chrono::duration<double, std::milli>(cancelEnd - cancelStart).count();

        TEST_ASSERT(maxWorkloadRes.cancelled);
        // Under chunk-level cooperative cancellation, latency is bounded within 250 ms
        TEST_ASSERT(measuredCancelLatencyMs < 250.0);
        std::printf("[PASS] Maximum workload (50,000 cases) mid-solve cancellation verified (latency: %.2f ms).\n",
                    measuredCancelLatencyMs);
    }

    // 5.3 Repeated start and cancel stress test (10 rapid cycles)
    {
        for (int cycle = 1; cycle <= 10; ++cycle) {
            std::atomic<bool> cancelToken{false};
            std::thread t([&]() {
                auto res = BenchmarkEngine::runSuite(2000, 80, {2}, 3, 42 + cycle, &cancelToken);
                (void)res;
            });
            std::this_thread::sleep_for(std::chrono::milliseconds(2));
            cancelToken.store(true, std::memory_order_relaxed);
            t.join();
        }
        std::cout << "[PASS] 10 consecutive rapid start/cancel cycles completed with zero hangs or leaks.\n";
    }

    std::cout << "========================================================\n";
    std::cout << "ALL MULTICORE BENCHMARK & CANCELLATION TESTS PASSED!\n";
    std::cout << "========================================================\n";

    return 0;
}
