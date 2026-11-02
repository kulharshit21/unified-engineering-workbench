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
    std::string cpuModel;
    int physicalCores{12};
    int logicalCores{24};
    int caseCount{500};
    int meshElements{80};
    uint32_t seed{42};
    bool cancelled{false}; // true if runSuite() was interrupted early
    std::vector<WorkerBenchmarkResult> workerResults;
};

class BenchmarkEngine {
public:
    // cancelToken: if non-null and set to true, runSuite() stops between
    // repetitions and returns a partial result with result.cancelled == true.
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
