#include <cassert>
#include <iostream>
#include <cmath>
#include <iomanip>
#include <filesystem>
#include "neural/BackpropagationSolver.hpp"

using namespace Workbench::Core::Neural;

int main() {
    std::cout << "========================================================\n";
    std::cout << "[TEST SUITE] Running Backpropagation Numerical Tests\n";
    std::cout << "========================================================\n";

    const double x = 1.0;
    const double y = 2.0;
    NetworkParameters initParams{0.5, 0.0, 1.0, 0.0};
    const double eta = 0.1;

    // 1. Hand-calculated Step 0 values verification
    double h0 = 0.0, z1_0 = 0.0;
    double y_hat0 = BackpropagationSolver::forward(x, initParams, h0, z1_0);
    double loss0 = BackpropagationSolver::computeLoss(y_hat0, y);

    assert(std::abs(z1_0 - 0.5) < 1e-12);
    double expectedH0 = std::tanh(0.5);
    assert(std::abs(h0 - expectedH0) < 1e-12);
    assert(std::abs(y_hat0 - expectedH0) < 1e-12);
    double expectedLoss0 = 0.5 * (expectedH0 - 2.0) * (expectedH0 - 2.0);
    assert(std::abs(loss0 - expectedLoss0) < 1e-12);

    std::cout << "[PASS] Step 0 forward values exact:\n"
              << "       z1 = " << z1_0 << "\n"
              << "       h = " << std::setprecision(8) << h0 << "\n"
              << "       y_hat = " << y_hat0 << "\n"
              << "       loss = " << loss0 << "\n";

    // 2. Analytical gradients at step 0
    Gradients g0 = BackpropagationSolver::computeAnalyticalGradients(x, y, initParams);
    double err0 = expectedH0 - 2.0;
    double expected_dJ_dv = err0 * expectedH0;
    double expected_dJ_dc = err0;
    double d_tanh = 1.0 - expectedH0 * expectedH0;
    double expected_dJ_dw = err0 * 1.0 * d_tanh * 1.0;
    double expected_dJ_db = err0 * 1.0 * d_tanh * 1.0;

    assert(std::abs(g0.dJ_dv - expected_dJ_dv) < 1e-12);
    assert(std::abs(g0.dJ_dc - expected_dJ_dc) < 1e-12);
    assert(std::abs(g0.dJ_dw - expected_dJ_dw) < 1e-12);
    assert(std::abs(g0.dJ_db - expected_dJ_db) < 1e-12);

    std::cout << "[PASS] Step 0 analytical gradients exact:\n"
              << "       dJ/dw = " << g0.dJ_dw << "\n"
              << "       dJ/db = " << g0.dJ_db << "\n"
              << "       dJ/dv = " << g0.dJ_dv << "\n"
              << "       dJ/dc = " << g0.dJ_dc << "\n";

    // 3. Finite-difference verification (epsilon = 1e-6, tolerance <= 1e-5)
    const double eps = 1e-6;
    const double tol = 1e-5;
    auto checks = BackpropagationSolver::verifyGradients(x, y, initParams, eps, tol);

    std::cout << "\n--- FINITE-DIFFERENCE GRADIENT VERIFICATION (eps = 1e-6) ---\n";
    std::cout << " Param | Analytical   | Numerical    | Absolute Error | Status\n";
    std::cout << "-------+--------------+--------------+----------------+--------\n";
    for (const auto& c : checks) {
        std::cout << "   " << c.paramName << "   | "
                  << std::showpos << std::scientific << std::setprecision(6)
                  << c.analyticalGrad << " | "
                  << c.numericalGrad << " | "
                  << std::noshowpos
                  << c.absoluteError << "   | "
                  << (c.passed ? "PASS" : "FAIL") << "\n";
        assert(c.passed);
        assert(c.absoluteError <= tol);
    }
    std::cout << "[PASS] All 4 parameters pass finite-difference gradient test within tolerance 1e-5.\n\n";

    // 4. Simultaneous parameter update check (Step 1 parameters)
    NetworkParameters expectedP1;
    expectedP1.w = initParams.w - eta * g0.dJ_dw;
    expectedP1.b = initParams.b - eta * g0.dJ_db;
    expectedP1.v = initParams.v - eta * g0.dJ_dv;
    expectedP1.c = initParams.c - eta * g0.dJ_dc;

    auto history = BackpropagationSolver::train(x, y, initParams, eta, 100);
    assert(history.size() == 101); // 0 to 100

    assert(std::abs(history[1].params.w - expectedP1.w) < 1e-12);
    assert(std::abs(history[1].params.b - expectedP1.b) < 1e-12);
    assert(std::abs(history[1].params.v - expectedP1.v) < 1e-12);
    assert(std::abs(history[1].params.c - expectedP1.c) < 1e-12);
    std::cout << "[PASS] Simultaneous parameter update confirmed without parameter mutation leakage.\n";

    // 5. Training progression (loss reduction)
    assert(history[0].loss > history[1].loss);
    assert(history[1].loss > history[10].loss);
    assert(history[10].loss > history[100].loss);
    std::cout << "[PASS] Loss decreases strictly: Step 0 Loss = " << history[0].loss 
              << " -> Step 10 Loss = " << history[10].loss 
              << " -> Step 100 Loss = " << history[100].loss << "\n";
    std::cout << "       Final y_hat = " << history[100].y_hat << " (target y = 2.0)\n";

    // 6. CSV exports to results/backprop/
    std::filesystem::create_directories("results/backprop");
    bool expHist = BackpropagationSolver::exportLossHistoryCsv("results/backprop/loss_history.csv", history);
    bool expGrad = BackpropagationSolver::exportGradientCheckCsv("results/backprop/gradient_verification.csv", checks);
    assert(expHist);
    assert(expGrad);
    std::cout << "[PASS] Exported results/backprop/loss_history.csv and results/backprop/gradient_verification.csv\n";

    std::cout << "========================================================\n";
    std::cout << "ALL BACKPROPAGATION TESTS PASSED!\n";
    std::cout << "========================================================\n";
    return 0;
}
