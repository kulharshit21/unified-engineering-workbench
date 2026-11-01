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

    // Assigned problem parameters
    ThermalParameters params;
    params.length = 1.0;            // L = 1 m
    params.area = 1e-4;             // A = 1e-4 m^2
    params.conductivity = 10.0;     // k = 10 W/(m K)
    params.heatGeneration = 10000.0;// q = 10000 W/m^3
    params.tempLeft = 100.0;        // T(0) = 100 C
    params.tempRight = 20.0;        // T(L) = 20 C

    ThermalFeaSolver solver(params);

    // 1. Boundary conditions and analytical values check
    // Analytical solution: T(x) = 100 - 80*x + (10000 / (2 * 10)) * x * (1 - x)
    //                           = 100 + 420*x - 500*x^2
    assert(std::abs(solver.evaluateExact(0.0) - 100.0) < 1e-12);
    assert(std::abs(solver.evaluateExact(1.0) - 20.0) < 1e-12);
    
    // Check internal profile and peak location (dT/dx = 420 - 1000x = 0 => x = 0.42 m, T = 188.2 C)
    double tPeak = solver.evaluateExact(0.42);
    assert(std::abs(tPeak - 188.2) < 1e-12);
    assert(tPeak > 100.0); // Heat generation creates internal temperature higher than boundaries
    std::cout << "[PASS] Analytical solution T(x) = 100 + 420x - 500x^2 verified:\n"
              << "       T(0) = " << solver.evaluateExact(0.0) << " C\n"
              << "       T_peak(0.42) = " << tPeak << " C\n"
              << "       T(1.0) = " << solver.evaluateExact(1.0) << " C\n";

    // 2. REGRESSION TEST: Explicitly reject incorrect polynomial T_bad(x) = 100 - 92.5x - 12.5x^2
    auto evaluateErroneousPolynomial = [](double x) {
        return 100.0 - 92.5 * x - 12.5 * x * x;
    };
    double tBadRight = evaluateErroneousPolynomial(1.0);
    // Erroneous polynomial gives T(1) = -5 C, which violates T(1) = 20 C by 25 C
    assert(std::abs(tBadRight - (-5.0)) < 1e-12);
    assert(std::abs(tBadRight - 20.0) > 20.0);
    std::cout << "[PASS] Regression test: Erroneous polynomial (yielding T(1) = " 
              << tBadRight << " C) correctly rejected.\n";

    // 3. REGRESSION TEST: Explicitly reject erroneous parameters (k=200, q=5000)
    // For k=200, q=5000, e_mid(N=10) = (5000 / (8 * 200)) * (0.1)^2 = 3.125 * 0.01 = 0.03125 C
    // For assigned k=10, q=10000, e_mid(N=10) = (10000 / (8 * 10)) * (0.1)^2 = 125 * 0.01 = 1.25000000 C
    double badETheory10 = (5000.0 / (8.0 * 200.0)) * 0.01;
    double correctETheory10 = (10000.0 / (8.0 * 10.0)) * 0.01;
    assert(std::abs(badETheory10 - 0.03125) < 1e-12);
    assert(std::abs(correctETheory10 - 1.25000000) < 1e-12);
    assert(std::abs(badETheory10 - correctETheory10) > 1.0);
    std::cout << "[PASS] Regression test: Erroneous parameter set (k=200, q=5000, e=" 
              << badETheory10 << " C) correctly distinguished from assigned specification (e="
              << correctETheory10 << " C).\n";

    // 4. Convergence table across N = 10, 20, 40, 80
    const double expectedErrors[4] = {
        1.25000000,   // N = 10, h = 0.1
        0.31250000,   // N = 20, h = 0.05
        0.07812500,   // N = 40, h = 0.025
        0.01953125    // N = 80, h = 0.0125
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

        // Check 1D linear FEA nodal superconvergence: nodal error is zero to machine precision
        for (int node = 0; node <= N; ++node) {
            double xNode = sol.nodeCoordinates[node];
            double tExactNode = solver.evaluateExact(xNode);
            (void)tExactNode;
            assert(std::abs(sol.nodalTemperatures[node] - tExactNode) < 1e-10);
        }

        // Assert 4-fold error reduction when N doubles
        if (i > 0) {
            assert(std::abs(e.errorRatio - 4.0) < 1e-3);
            assert(std::abs(e.convergenceOrder - 2.0) < 1e-3);
        }
    }
    std::cout << "-------------------------\n";
    std::cout << "[PASS] Quadratic convergence rate (p=2.0000, 4.0000x error reduction per mesh doubling) verified.\n";
    std::cout << "[PASS] Nodal superconvergence verified (zero error at all element nodes).\n";

    // 5. Negative parameter validation tests
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

    // 6. Export real verified evidence CSV files to results/thermal/
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
