#include "BackpropagationSolver.hpp"
#include <cmath>
#include <fstream>
#include <iomanip>
#include <filesystem>
#include <stdexcept>

namespace Workbench::Core::Neural {

double BackpropagationSolver::forward(double x, const NetworkParameters& p, double& h, double& z1) {
    z1 = p.w * x + p.b;
    h = std::tanh(z1);
    double y_hat = p.v * h + p.c;
    return y_hat;
}

double BackpropagationSolver::forward(double x, const NetworkParameters& p) {
    double h = 0.0, z1 = 0.0;
    return forward(x, p, h, z1);
}

double BackpropagationSolver::computeLoss(double y_hat, double y) {
    double diff = y_hat - y;
    return 0.5 * diff * diff;
}

Gradients BackpropagationSolver::computeAnalyticalGradients(double x, double y, const NetworkParameters& p) {
    double h = 0.0, z1 = 0.0;
    double y_hat = forward(x, p, h, z1);
    double err = y_hat - y;

    Gradients g;
    // Output layer gradients
    g.dJ_dv = err * h;
    g.dJ_dc = err * 1.0;

    // Hidden layer gradients (chain rule through tanh: d(tanh(z))/dz = 1 - tanh^2(z) = 1 - h^2)
    double dJ_dz1 = err * p.v * (1.0 - h * h);
    g.dJ_dw = dJ_dz1 * x;
    g.dJ_db = dJ_dz1 * 1.0;

    return g;
}

Gradients BackpropagationSolver::computeNumericalGradients(double x, double y, const NetworkParameters& p, double epsilon) {
    Gradients numG;

    // Partial with respect to w
    {
        NetworkParameters pPlus = p;  pPlus.w += epsilon;
        NetworkParameters pMinus = p; pMinus.w -= epsilon;
        double J_plus = computeLoss(forward(x, pPlus), y);
        double J_minus = computeLoss(forward(x, pMinus), y);
        numG.dJ_dw = (J_plus - J_minus) / (2.0 * epsilon);
    }

    // Partial with respect to b
    {
        NetworkParameters pPlus = p;  pPlus.b += epsilon;
        NetworkParameters pMinus = p; pMinus.b -= epsilon;
        double J_plus = computeLoss(forward(x, pPlus), y);
        double J_minus = computeLoss(forward(x, pMinus), y);
        numG.dJ_db = (J_plus - J_minus) / (2.0 * epsilon);
    }

    // Partial with respect to v
    {
        NetworkParameters pPlus = p;  pPlus.v += epsilon;
        NetworkParameters pMinus = p; pMinus.v -= epsilon;
        double J_plus = computeLoss(forward(x, pPlus), y);
        double J_minus = computeLoss(forward(x, pMinus), y);
        numG.dJ_dv = (J_plus - J_minus) / (2.0 * epsilon);
    }

    // Partial with respect to c
    {
        NetworkParameters pPlus = p;  pPlus.c += epsilon;
        NetworkParameters pMinus = p; pMinus.c -= epsilon;
        double J_plus = computeLoss(forward(x, pPlus), y);
        double J_minus = computeLoss(forward(x, pMinus), y);
        numG.dJ_dc = (J_plus - J_minus) / (2.0 * epsilon);
    }

    return numG;
}

std::vector<GradientCheckResult> BackpropagationSolver::verifyGradients(
    double x, double y, const NetworkParameters& p, double epsilon, double tolerance) 
{
    Gradients ana = computeAnalyticalGradients(x, y, p);
    Gradients num = computeNumericalGradients(x, y, p, epsilon);

    std::vector<GradientCheckResult> results;

    auto checkParam = [&](const std::string& name, double a, double n) {
        GradientCheckResult r;
        r.paramName = name;
        r.analyticalGrad = a;
        r.numericalGrad = n;
        r.absoluteError = std::abs(a - n);
        r.passed = (r.absoluteError <= tolerance);
        results.push_back(r);
    };

    checkParam("w", ana.dJ_dw, num.dJ_dw);
    checkParam("b", ana.dJ_db, num.dJ_db);
    checkParam("v", ana.dJ_dv, num.dJ_dv);
    checkParam("c", ana.dJ_dc, num.dJ_dc);

    return results;
}

std::vector<TrainingStepRecord> BackpropagationSolver::train(
    double x, double y, NetworkParameters initialParams, double learningRate, int steps) 
{
    std::vector<TrainingStepRecord> history;
    history.reserve(steps + 1);

    NetworkParameters currentParams = initialParams;

    for (int step = 0; step < steps; ++step) {
        TrainingStepRecord rec;
        rec.step = step;
        rec.params = currentParams;

        double h = 0.0, z1 = 0.0;
        rec.y_hat = forward(x, currentParams, h, z1);
        rec.h = h;
        rec.loss = computeLoss(rec.y_hat, y);

        // Compute gradients using current (un-updated) parameters
        rec.grads = computeAnalyticalGradients(x, y, currentParams);
        history.push_back(rec);

        // Simultaneous gradient descent update using evaluated gradients
        currentParams.w -= learningRate * rec.grads.dJ_dw;
        currentParams.b -= learningRate * rec.grads.dJ_db;
        currentParams.v -= learningRate * rec.grads.dJ_dv;
        currentParams.c -= learningRate * rec.grads.dJ_dc;
    }

    // Record final converged state
    {
        TrainingStepRecord finalRec;
        finalRec.step = steps;
        finalRec.params = currentParams;
        double h = 0.0, z1 = 0.0;
        finalRec.y_hat = forward(x, currentParams, h, z1);
        finalRec.h = h;
        finalRec.loss = computeLoss(finalRec.y_hat, y);
        finalRec.grads = computeAnalyticalGradients(x, y, currentParams);
        history.push_back(finalRec);
    }

    return history;
}

bool BackpropagationSolver::exportLossHistoryCsv(const std::string& filePath, const std::vector<TrainingStepRecord>& history) {
    std::filesystem::path p(filePath);
    if (p.has_parent_path()) {
        std::filesystem::create_directories(p.parent_path());
    }

    std::ofstream out(filePath);
    if (!out.is_open()) return false;

    out << "step,loss,w,b,v,c,y_hat,h,dJ_dw,dJ_db,dJ_dv,dJ_dc\n";
    out << std::fixed << std::setprecision(8);

    for (const auto& r : history) {
        out << r.step << ","
            << r.loss << ","
            << r.params.w << ","
            << r.params.b << ","
            << r.params.v << ","
            << r.params.c << ","
            << r.y_hat << ","
            << r.h << ","
            << r.grads.dJ_dw << ","
            << r.grads.dJ_db << ","
            << r.grads.dJ_dv << ","
            << r.grads.dJ_dc << "\n";
    }

    return true;
}

bool BackpropagationSolver::exportGradientCheckCsv(const std::string& filePath, const std::vector<GradientCheckResult>& checks) {
    std::filesystem::path p(filePath);
    if (p.has_parent_path()) {
        std::filesystem::create_directories(p.parent_path());
    }

    std::ofstream out(filePath);
    if (!out.is_open()) return false;

    out << "parameter,analytical_grad,numerical_grad,absolute_error,status\n";
    out << std::scientific << std::setprecision(8);

    for (const auto& c : checks) {
        out << c.paramName << ","
            << c.analyticalGrad << ","
            << c.numericalGrad << ","
            << c.absoluteError << ","
            << (c.passed ? "PASS" : "FAIL") << "\n";
    }

    return true;
}

} // namespace Workbench::Core::Neural
