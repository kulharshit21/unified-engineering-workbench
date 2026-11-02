#pragma once

#include "BatchSolver.hpp"
#include <vector>
#include <string>
#include <atomic>

namespace Workbench::Core::Parallel {

struct WorkerBenchmarkResult {
    size_t workerCount{1};
    std::vector<double> repetitionTimesMs; // 5 repetitions
    double medianTimeMs{0.0};
    double meanTimeMs{0.0};
    double minTimeMs{0.0};
    double maxTimeMs{0.0};
    double speedup{1.0};      // T1 / Tp
    double efficiency{1.0};   // (T1 / Tp) / p
    double maxDeltaVsSerial{0.0};
};

struct BenchmarkSuiteResult {
    std::string cpuModel{"Generic Multicore CPU"};
    int physicalCores{1};
    int logicalCores{1};
    int caseCount{500};
    int meshElements{80};
    uint32_t seed{42};
    bool cancelled{false}; // true if runSuite() was interrupted early
    std::vector<WorkerBenchmarkResult> workerResults;
};

class BenchmarkEngine {
public:
    // Detect host CPU brand and topology using native platform APIs
    static void detectHostCpu(std::string &model, int &physicalCores, int &logicalCores);

    // cancelToken: if non-null and set to true, runSuite() stops cooperatively
    // and returns a partial result with result.cancelled == true.
    static BenchmarkSuiteResult runSuite(
        int caseCount = 1000,
        int meshElements = 80,
        const std::vector<size_t> &workerCounts = {1, 2, 4},
        int repetitions = 5,
        uint32_t seed = 42,
        std::atomic<bool> *cancelToken = nullptr
    );

    static std::string exportRawTimingsCsv(const BenchmarkSuiteResult &suite);
    static std::string exportSummaryCsv(const BenchmarkSuiteResult &suite);
    static std::string exportAgreementCsv(const BenchmarkSuiteResult &suite);
};

} // namespace Workbench::Core::Parallel
