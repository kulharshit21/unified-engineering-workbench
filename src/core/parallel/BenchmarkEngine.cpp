#include "BenchmarkEngine.hpp"
#include <chrono>
#include <algorithm>
#include <numeric>
#include <sstream>
#include <iomanip>

namespace Workbench::Core::Parallel {

static double calculateMedian(std::vector<double> v) {
    if (v.empty()) return 0.0;
    std::sort(v.begin(), v.end());
    size_t n = v.size();
    if (n % 2 == 1) {
        return v[n / 2];
    } else {
        return 0.5 * (v[n / 2 - 1] + v[n / 2]);
    }
}

BenchmarkSuiteResult BenchmarkEngine::runSuite(
    int caseCount,
    int meshElements,
    const std::vector<size_t> &workerCounts,
    int repetitions,
    uint32_t seed,
    std::atomic<bool> *cancelToken) {

    BenchmarkSuiteResult suite;
    suite.cpuModel = "AMD Ryzen AI 9 HX 370 w/ Radeon 890M";
    suite.physicalCores = 12;
    suite.logicalCores = 24;
    suite.caseCount = caseCount;
    suite.meshElements = meshElements;
    suite.seed = seed;
    suite.cancelled = false;

    // 1. Generate pre-allocated deterministic test batch
    std::vector<ThermalCase> cases = BatchGenerator::generateDeterministicBatch(caseCount, meshElements, seed);

    // 2. Warm-up run (prime CPU frequency and thread caches)
    {
        auto warmup = BatchSolver::solveParallel(cases, 2);
        (void)warmup;
    }

    // 3. Baseline serial solve (for agreement and speedup reference)
    std::vector<CaseResult> baselineSerial = BatchSolver::solveSerial(cases);

    double baselineMedianMs = 0.0;

    for (size_t workers : workerCounts) {
        // Cooperative cancellation check: before each worker configuration
        if (cancelToken && cancelToken->load(std::memory_order_relaxed)) {
            suite.cancelled = true;
            return suite;
        }

        WorkerBenchmarkResult res;
        res.workerCount = workers;
        res.repetitionTimesMs.reserve(repetitions);

        std::vector<CaseResult> lastRunResults;

        for (int r = 0; r < repetitions; ++r) {
            // Cooperative cancellation check: between repetitions
            if (cancelToken && cancelToken->load(std::memory_order_relaxed)) {
                suite.cancelled = true;
                return suite;
            }

            auto tStart = std::chrono::steady_clock::now();
            if (workers == 1) {
                lastRunResults = BatchSolver::solveSerial(cases);
            } else {
                lastRunResults = BatchSolver::solveParallel(cases, workers);
            }
            auto tEnd = std::chrono::steady_clock::now();

            double elapsedMs = std::chrono::duration<double, std::milli>(tEnd - tStart).count();
            res.repetitionTimesMs.push_back(elapsedMs);
        }

        res.medianTimeMs = calculateMedian(res.repetitionTimesMs);
        double sum = std::accumulate(res.repetitionTimesMs.begin(), res.repetitionTimesMs.end(), 0.0);
        res.meanTimeMs = sum / static_cast<double>(repetitions);
        res.minTimeMs = *std::min_element(res.repetitionTimesMs.begin(), res.repetitionTimesMs.end());
        res.maxTimeMs = *std::max_element(res.repetitionTimesMs.begin(), res.repetitionTimesMs.end());

        // Max difference vs serial baseline
        res.maxDeltaVsSerial = BatchSolver::computeMaxDifference(baselineSerial, lastRunResults);

        if (workers == 1) {
            baselineMedianMs = res.medianTimeMs;
            res.speedup = 1.0;
            res.efficiency = 1.0;
        } else {
            res.speedup = (res.medianTimeMs > 0.0) ? (baselineMedianMs / res.medianTimeMs) : 1.0;
            res.efficiency = res.speedup / static_cast<double>(workers);
        }

        suite.workerResults.push_back(res);
    }

    return suite;
}

std::string BenchmarkEngine::exportRawTimingsCsv(const BenchmarkSuiteResult &suite) {
    std::ostringstream ss;
    ss << std::fixed << std::setprecision(4);
    ss << "worker_count,repetition_index,wall_clock_time_ms\n";

    for (const auto &w : suite.workerResults) {
        for (size_t r = 0; r < w.repetitionTimesMs.size(); ++r) {
            ss << w.workerCount << ","
               << (r + 1) << ","
               << w.repetitionTimesMs[r] << "\n";
        }
    }
    return ss.str();
}

std::string BenchmarkEngine::exportSummaryCsv(const BenchmarkSuiteResult &suite) {
    std::ostringstream ss;
    ss << std::fixed << std::setprecision(4);
    ss << "workers_p,case_count,mesh_N,median_time_ms,mean_time_ms,min_time_ms,max_time_ms,speedup_T1_over_Tp,efficiency\n";

    for (const auto &w : suite.workerResults) {
        ss << w.workerCount << ","
           << suite.caseCount << ","
           << suite.meshElements << ","
           << w.medianTimeMs << ","
           << w.meanTimeMs << ","
           << w.minTimeMs << ","
           << w.maxTimeMs << ","
           << w.speedup << ","
           << w.efficiency << "\n";
    }
    return ss.str();
}

std::string BenchmarkEngine::exportAgreementCsv(const BenchmarkSuiteResult &suite) {
    std::ostringstream ss;
    ss << std::scientific << std::setprecision(8);
    ss << "workers_p,max_absolute_delta_C,agreement_tolerance_C,status\n";

    for (const auto &w : suite.workerResults) {
        std::string status = (w.maxDeltaVsSerial <= 1e-8) ? "PASS" : "FAIL";
        ss << w.workerCount << ","
           << w.maxDeltaVsSerial << ","
           << 1e-8 << ","
           << status << "\n";
    }
    return ss.str();
}

} // namespace Workbench::Core::Parallel
