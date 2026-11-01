#include <cassert>
#include <iostream>
#include <cmath>
#include <fstream>
#include <filesystem>
#include "thermal/ThermalFeaSolver.hpp"
#include "thermal/ThermalConvergence.hpp"

namespace fs = std::filesystem;
using namespace Workbench::Core::Thermal;

int main() {
    std::cout << "========================================================\n";
    std::cout << "[TEST SUITE] Running 1D Thermal FEA Numerical Verification\n";
    std::cout << "========================================================\n";

    ThermalParameters params;
    params.length = 1.0;
    params.area = 1e-4;
    params.conductivity = 10.0;
    params.heatGeneration = 10000.0;
    params.tempLeft = 100.0;
    params.tempRight = 20.0;

    ThermalFeaSolver solver(params);

    // 1. Boundary conditions and analytical values check
    assert(std::abs(solver.evaluateExact(0.0) - 100.0) < 1e-12);
    assert(std::abs(solver.evaluateExact(1.0) - 20.0) < 1e-12);
    std::cout << "[PASS] Analytical boundary values verified.\n";

    // 2. Expected values from assignment specification
    const double expectedErrors[4] = {
        1.25000000,   // N = 10
        0.31250000,   // N = 20
        0.07812500,   // N = 40
        0.01953125    // N = 80
    };
    const int meshSizes[4] = {10, 20, 40, 80};

    auto entries = ThermalConvergence::evaluateConvergence(solver, {10, 20, 40, 80});

    std::cout << "\n--- CONVERGENCE TABLE ---\n";
    std::cout << "  N   |  h (m)   | Observed Max Error (C) | Expected (C) | Ratio  | Order (p)\n";
    std::cout << "------+----------+------------------------+--------------+--------+----------\n";

    for (size_t i = 0; i < 4; ++i) {
        int N = meshSizes[i];
        const auto &e = entries[i];
        double expected = expectedErrors[i];
        double delta = std::abs(e.maxMidpointError - expected);
        (void)delta;

        std::printf(" %3d  | %8.4f | %22.8f | %12.8f | %6.2f | %8.4f\n",
                    N, e.elementLength, e.maxMidpointError, expected, e.errorRatio, e.convergenceOrder);

        // Assert midpoint error matches theoretical derivation within 1e-7 deg C
        assert(delta < 1e-7);

        // Assert boundary conditions exactness on solved temperatures
        ThermalSolution sol = solver.solve(N);
        assert(std::abs(sol.nodalTemperatures.front() - 100.0) < 1e-12);
        assert(std::abs(sol.nodalTemperatures.back() - 20.0) < 1e-12);

        // Assert 4-fold error reduction when N doubles
        if (i > 0) {
            assert(std::abs(e.errorRatio - 4.0) < 1e-3);
            assert(std::abs(e.convergenceOrder - 2.0) < 1e-3);
        }
    }
    std::cout << "-------------------------\n";
    std::cout << "[PASS] Quadratic convergence rate (p=2.0, 4x error reduction per mesh doubling) verified.\n";

    // 3. Negative parameter validation tests
    bool caught = false;
    try {
        ThermalParameters badParams = params;
        badParams.conductivity = -5.0;
        solver.setParameters(badParams);
    } catch (const std::invalid_argument &) {
        caught = true;
    }
    assert(caught);
    std::cout << "[PASS] Negative conductivity correctly rejected.\n";

    caught = false;
    try {
        ThermalParameters badParams = params;
        badParams.length = 0.0;
        solver.setParameters(badParams);
    } catch (const std::invalid_argument &) {
        caught = true;
    }
    assert(caught);
    std::cout << "[PASS] Zero rod length correctly rejected.\n";

    // 4. Export real verified evidence CSV files to results/thermal/
    std::string convCsv = ThermalConvergence::exportConvergenceCsv(entries);
    std::string profCsv = ThermalConvergence::exportProfilesCsv(solver, {10, 20, 40, 80});

    const std::vector<std::string> outputDirs = {"results/thermal", "../../results/thermal"};
    for (const auto &dir : outputDirs) {
        std::error_code ec;
        fs::create_directories(dir, ec);
        if (!ec) {
            std::ofstream convFile(dir + "/convergence.csv");
            if (convFile.is_open()) {
                convFile << convCsv;
                convFile.close();
            }

            std::ofstream profFile(dir + "/temperature_profiles.csv");
            if (profFile.is_open()) {
                profFile << profCsv;
                profFile.close();
            }
        }
    }

    std::cout << "[PASS] Real verification results exported to results/thermal/.\n";
    std::cout << "========================================================\n";
    std::cout << "ALL THERMAL FEA VERIFICATION TESTS PASSED!\n";
    std::cout << "========================================================\n";

    return 0;
}
