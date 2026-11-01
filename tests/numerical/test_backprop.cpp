#include "TestAssert.hpp"
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

    // 1. Assigned initial conditions from prompt specification:
    // x = 1.0, y = 2.0, w = 0.5, b = 0.0, v = 1.0, c = 0.0, eta = 0.1
    const double x = 1.0;
    const double y = 2.0;
    NetworkParameters initParams{0.5, 0.0, 1.0, 0.0}; // {w, b, v, c}
    const double eta = 0.1;

    // Hand-calculated Step 0 values verification
    double h0 = 0.0, z1_0 = 0.0;
    double y_hat0 = BackpropagationSolver::forward(x, initParams, h0, z1_0);
    double loss0 = BackpropagationSolver::computeLoss(y_hat0, y);

    TEST_ASSERT_NEAR(z1_0, 0.5, 1e-12);
    double expectedH0 = std::tanh(0.5);
    TEST_ASSERT_NEAR(h0, expectedH0, 1e-12);
    TEST_ASSERT_NEAR(y_hat0, expectedH0, 1e-12);
    double expectedLoss0 = 0.5 * (expectedH0 - 2.0) * (expectedH0 - 2.0);
    TEST_ASSERT_NEAR(loss0, expectedLoss0, 1e-12);

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
    double expected_dJ_db = err0 * 1.0 * d_tanh * 1.0;
    double expected_dJ_dw = err0 * 1.0 * d_tanh * 1.0; // x = 1.0

    TEST_ASSERT_NEAR(g0.dJ_dv, expected_dJ_dv, 1e-12);
    TEST_ASSERT_NEAR(g0.dJ_dc, expected_dJ_dc, 1e-12);
    TEST_ASSERT_NEAR(g0.dJ_dw, expected_dJ_dw, 1e-12);
    TEST_ASSERT_NEAR(g0.dJ_db, expected_dJ_db, 1e-12);

    std::cout << "[PASS] Step 0 analytical gradients exact:\n"
              << "       dJ/dw = " << g0.dJ_dw << " (expected: -1.20946448)\n"
              << "       dJ/db = " << g0.dJ_db << " (expected: -1.20946448)\n"
              << "       dJ/dv = " << g0.dJ_dv << " (expected: -0.71068205)\n"
              << "       dJ/dc = " << g0.dJ_dc << " (expected: -1.53788284)\n";

    // 3. REGRESSION TEST: Contrast x = 1.0, w = 0.5 vs. x = 0.5, w = 1.0
    // Both configurations yield z1 = 0.5 and the same preactivations,
    // but dJ/dw differs by a factor of 2.0 because dJ/dw = delta1 * x.
    {
        const double x_alt = 0.5;
        NetworkParameters params_alt{1.0, 0.0, 1.0, 0.0}; // w = 1.0
        double h_alt = 0.0, z1_alt = 0.0;
        double y_hat_alt = BackpropagationSolver::forward(x_alt, params_alt, h_alt, z1_alt);
        (void)y_hat_alt;
        TEST_ASSERT_NEAR(z1_alt, 0.5, 1e-12);
        TEST_ASSERT_NEAR(y_hat_alt, y_hat0, 1e-12);

        Gradients g_alt = BackpropagationSolver::computeAnalyticalGradients(x_alt, y, params_alt);
        // Under x = 0.5, dJ/dw must be exactly half of dJ/dw under x = 1.0:
        double expected_dJ_dw_alt = expected_dJ_dw * 0.5;
        TEST_ASSERT_NEAR(g_alt.dJ_dw, expected_dJ_dw_alt, 1e-12);
        TEST_ASSERT(std::abs(g_alt.dJ_dw - g0.dJ_dw) > 0.5); // Must not be equal!
        
        std::cout << "[PASS] Regression test: Confirmed x=1.0, w=0.5 vs x=0.5, w=1.0 gradient divergence:\n"
                  << "       For x=1.0, w=0.5: dJ/dw = " << g0.dJ_dw << "\n"
                  << "       For x=0.5, w=1.0: dJ/dw = " << g_alt.dJ_dw << " (ratio = " 
                  << (g0.dJ_dw / g_alt.dJ_dw) << ")\n";
    }

    // 4. Finite-difference verification (epsilon = 1e-6, tolerance <= 1e-5)
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
        TEST_ASSERT(c.passed);
        TEST_ASSERT(c.absoluteError <= tol);
    }
    std::cout << "[PASS] All 4 parameters pass finite-difference gradient test within tolerance 1e-5.\n\n";

    // 5. Simultaneous parameter update check (Step 1 parameters)
    NetworkParameters expectedP1;
    expectedP1.w = initParams.w - eta * g0.dJ_dw;
    expectedP1.b = initParams.b - eta * g0.dJ_db;
    expectedP1.v = initParams.v - eta * g0.dJ_dv;
    expectedP1.c = initParams.c - eta * g0.dJ_dc;

    auto history = BackpropagationSolver::train(x, y, initParams, eta, 100);
    TEST_ASSERT(history.size() == 101); // 0 to 100

    TEST_ASSERT_NEAR(history[1].params.w, expectedP1.w, 1e-12);
    TEST_ASSERT_NEAR(history[1].params.b, expectedP1.b, 1e-12);
    TEST_ASSERT_NEAR(history[1].params.v, expectedP1.v, 1e-12);
    TEST_ASSERT_NEAR(history[1].params.c, expectedP1.c, 1e-12);
    std::cout << "[PASS] Simultaneous parameter update confirmed without parameter mutation leakage.\n";

    // 6. Training progression (loss reduction)
    TEST_ASSERT(history[0].loss > history[1].loss);
    TEST_ASSERT(history[1].loss > history[10].loss);
    TEST_ASSERT(history[10].loss > history[100].loss);

    std::cout << "[PASS] Loss decreases strictly:\n"
              << "       Step 0 Loss = " << std::scientific << history[0].loss << "\n"
              << "       Step 10 Loss = " << history[10].loss << "\n"
              << "       Step 100 Loss = " << history[100].loss << "\n"
              << "       Final y_hat = " << std::fixed << history[100].y_hat << " (target y = " << y << ")\n";

    // 7. Export real evidence CSV files
    BackpropagationSolver::exportLossHistoryCsv("results/backprop/loss_history.csv", history);
    BackpropagationSolver::exportLossHistoryCsv("../../results/backprop/loss_history.csv", history);
    BackpropagationSolver::exportGradientCheckCsv("results/backprop/gradient_verification.csv", checks);
    BackpropagationSolver::exportGradientCheckCsv("../../results/backprop/gradient_verification.csv", checks);

    std::cout << "[PASS] Exported results/backprop/loss_history.csv and results/backprop/gradient_verification.csv\n";
    std::cout << "========================================================\n";
    std::cout << "ALL BACKPROPAGATION TESTS PASSED!\n";
    std::cout << "========================================================\n";

    return 0;
}
