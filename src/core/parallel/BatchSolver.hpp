#pragma once

#include "BatchCase.hpp"
#include <vector>

namespace Workbench::Core::Parallel {

class BatchSolver {
public:
    static std::vector<CaseResult> solveSerial(const std::vector<ThermalCase> &cases);
    static std::vector<CaseResult> solveParallel(const std::vector<ThermalCase> &cases, size_t numWorkers);

    static double computeMaxDifference(
        const std::vector<CaseResult> &serialResults,
        const std::vector<CaseResult> &parallelResults
    );

private:
    static CaseResult solveSingleCase(const ThermalCase &c);
};

} // namespace Workbench::Core::Parallel
