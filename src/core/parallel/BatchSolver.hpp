#pragma once

#include "BatchCase.hpp"
#include <vector>
#include <atomic>

namespace Workbench::Core::Parallel {

class BatchSolver {
public:
    static std::vector<CaseResult> solveSerial(
        const std::vector<ThermalCase> &cases,
        std::atomic<bool> *cancelToken = nullptr
    );

    static std::vector<CaseResult> solveParallel(
        const std::vector<ThermalCase> &cases,
        size_t numWorkers,
        std::atomic<bool> *cancelToken = nullptr
    );

    static double computeMaxDifference(
        const std::vector<CaseResult> &serialResults,
        const std::vector<CaseResult> &parallelResults
    );

private:
    static CaseResult solveSingleCase(const ThermalCase &c);
};

} // namespace Workbench::Core::Parallel
