#include "BenchmarkEngine.hpp"
#include <chrono>
#include <algorithm>
#include <numeric>
#include <sstream>
#include <iomanip>
#include <thread>
#include <cstring>

#ifdef _WIN32
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>
#pragma comment(lib, "advapi32.lib")
#endif

namespace Workbench::Core::Parallel {

void BenchmarkEngine::detectHostCpu(std::string &model, int &physicalCores, int &logicalCores) {
    logicalCores = static_cast<int>(std::thread::hardware_concurrency());
    if (logicalCores <= 0) logicalCores = 1;
    physicalCores = logicalCores; // safe baseline fallback
    model = "Generic Multicore CPU";

#ifdef _WIN32
    // 1. Query CPU brand string from Windows Registry
    HKEY hKey = nullptr;
    if (RegOpenKeyExA(HKEY_LOCAL_MACHINE,
                      "HARDWARE\\DESCRIPTION\\System\\CentralProcessor\\0",
                      0, KEY_READ, &hKey) == ERROR_SUCCESS) {
        char buffer[256] = {0};
        DWORD bufferSize = sizeof(buffer) - 1;
        DWORD type = REG_SZ;
        if (RegQueryValueExA(hKey, "ProcessorNameString", nullptr, &type,
                             reinterpret_cast<LPBYTE>(buffer), &bufferSize) == ERROR_SUCCESS) {
            std::string name(buffer);
            size_t first = name.find_first_not_of(" \t\r\n");
            size_t last = name.find_last_not_of(" \t\r\n");
            if (first != std::string::npos && last != std::string::npos) {
                model = name.substr(first, (last - first + 1));
            } else if (!name.empty()) {
                model = name;
            }
        }
        RegCloseKey(hKey);
    } else {
        const char *envModel = std::getenv("PROCESSOR_IDENTIFIER");
        if (envModel && std::strlen(envModel) > 0) {
            model = envModel;
        }
    }

    // 2. Query physical core count via GetLogicalProcessorInformationEx
    DWORD length = 0;
    if (!GetLogicalProcessorInformationEx(RelationProcessorCore, nullptr, &length) &&
        GetLastError() == ERROR_INSUFFICIENT_BUFFER && length > 0) {
        std::vector<char> infoBuffer(length);
        auto *pInfo = reinterpret_cast<PSYSTEM_LOGICAL_PROCESSOR_INFORMATION_EX>(infoBuffer.data());
        if (GetLogicalProcessorInformationEx(RelationProcessorCore, pInfo, &length)) {
            int cores = 0;
            DWORD offset = 0;
            while (offset < length) {
                auto *item = reinterpret_cast<PSYSTEM_LOGICAL_PROCESSOR_INFORMATION_EX>(infoBuffer.data() + offset);
                if (item->Relationship == RelationProcessorCore) {
                    cores++;
                }
                offset += item->Size;
            }
            if (cores > 0) {
                physicalCores = cores;
            }
        }
    }
#endif
}

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
    detectHostCpu(suite.cpuModel, suite.physicalCores, suite.logicalCores);
    suite.caseCount = caseCount;
    suite.meshElements = meshElements;
    suite.seed = seed;
    suite.cancelled = false;

    // Early cooperative cancellation check
    if (cancelToken && cancelToken->load(std::memory_order_relaxed)) {
        suite.cancelled = true;
        return suite;
    }

    // 1. Generate pre-allocated deterministic test batch
    std::vector<ThermalCase> cases = BatchGenerator::generateDeterministicBatch(caseCount, meshElements, seed);

    if (cancelToken && cancelToken->load(std::memory_order_relaxed)) {
        suite.cancelled = true;
        return suite;
    }

    // 2. Warm-up run (prime CPU frequency and thread caches)
    {
        auto warmup = BatchSolver::solveParallel(cases, 2, cancelToken);
        (void)warmup;
    }

    if (cancelToken && cancelToken->load(std::memory_order_relaxed)) {
        suite.cancelled = true;
        return suite;
    }

    // 3. Baseline serial solve (for agreement and speedup reference)
    std::vector<CaseResult> baselineSerial = BatchSolver::solveSerial(cases, cancelToken);

    if (cancelToken && cancelToken->load(std::memory_order_relaxed)) {
        suite.cancelled = true;
        return suite;
    }

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
                lastRunResults = BatchSolver::solveSerial(cases, cancelToken);
            } else {
                lastRunResults = BatchSolver::solveParallel(cases, workers, cancelToken);
            }
            auto tEnd = std::chrono::steady_clock::now();

            if (cancelToken && cancelToken->load(std::memory_order_relaxed)) {
                suite.cancelled = true;
                return suite;
            }

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
