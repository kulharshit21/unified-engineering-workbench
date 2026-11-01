#include "BatchSolver.hpp"
#include "ThreadPool.hpp"
#include <cmath>
#include <algorithm>
#include <stdexcept>

namespace Workbench::Core::Parallel {

CaseResult BatchSolver::solveSingleCase(const ThermalCase &c) {
    Thermal::ThermalFeaSolver solver(c.params);
    Thermal::ThermalSolution sol = solver.solve(c.numElements);

    CaseResult res;
    res.caseId = c.caseId;
    res.maxMidpointError = sol.maxMidpointError;
    res.nodalTemperatures = sol.nodalTemperatures;

    if (!sol.midpointFemTemperatures.empty()) {
        res.midpointTemperatureMid = sol.midpointFemTemperatures[sol.midpointFemTemperatures.size() / 2];
    }
    res.boundaryErrorLeft = std::abs(sol.nodalTemperatures.front() - c.params.tempLeft);
    res.boundaryErrorRight = std::abs(sol.nodalTemperatures.back() - c.params.tempRight);

    return res;
}

std::vector<CaseResult> BatchSolver::solveSerial(const std::vector<ThermalCase> &cases) {
    std::vector<CaseResult> results(cases.size());
    for (size_t i = 0; i < cases.size(); ++i) {
        results[i] = solveSingleCase(cases[i]);
    }
    return results;
}

std::vector<CaseResult> BatchSolver::solveParallel(const std::vector<ThermalCase> &cases, size_t numWorkers) {
    if (numWorkers == 0) {
        throw std::invalid_argument("Worker count must be >= 1.");
    }
    if (numWorkers == 1) {
        return solveSerial(cases);
    }

    std::vector<CaseResult> results(cases.size());
    ThreadPool pool(numWorkers);

    const size_t totalCases = cases.size();
    // Chunking to minimize task scheduling overhead
    const size_t chunkSize = std::max<size_t>(1, totalCases / (numWorkers * 8));

    for (size_t startIdx = 0; startIdx < totalCases; startIdx += chunkSize) {
        size_t endIdx = std::min(startIdx + chunkSize, totalCases);
        pool.enqueue([&cases, &results, startIdx, endIdx]() {
            for (size_t i = startIdx; i < endIdx; ++i) {
                results[i] = solveSingleCase(cases[i]);
            }
        });
    }

    pool.waitAll();
    return results;
}

double BatchSolver::computeMaxDifference(
    const std::vector<CaseResult> &serialResults,
    const std::vector<CaseResult> &parallelResults) {

    if (serialResults.size() != parallelResults.size()) {
        throw std::runtime_error("Result size mismatch between serial and parallel runs.");
    }

    double maxDiff = 0.0;
    for (size_t i = 0; i < serialResults.size(); ++i) {
        const auto &s = serialResults[i];
        const auto &p = parallelResults[i];

        if (s.caseId != p.caseId) {
            throw std::runtime_error("Case ID order mismatch at index " + std::to_string(i));
        }
        if (s.nodalTemperatures.size() != p.nodalTemperatures.size()) {
            throw std::runtime_error("Nodal temperature vector size mismatch at case " + std::to_string(s.caseId));
        }

        for (size_t j = 0; j < s.nodalTemperatures.size(); ++j) {
            double diff = std::abs(s.nodalTemperatures[j] - p.nodalTemperatures[j]);
            if (diff > maxDiff) {
                maxDiff = diff;
            }
        }
    }

    return maxDiff;
}

} // namespace Workbench::Core::Parallel
