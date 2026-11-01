#pragma once

#include <vector>
#include <string>

namespace Workbench::Core::Neural {

struct NetworkParameters {
    double w{0.5}; // Input-to-hidden weight
    double b{0.0}; // Hidden bias
    double v{1.0}; // Hidden-to-output weight
    double c{0.0}; // Output bias
};

struct Gradients {
    double dJ_dw{0.0};
    double dJ_db{0.0};
    double dJ_dv{0.0};
    double dJ_dc{0.0};
};

struct TrainingStepRecord {
    int step{0};
    double loss{0.0};
    NetworkParameters params;
    double y_hat{0.0};
    double h{0.0};
    Gradients grads;
};

struct GradientCheckResult {
    std::string paramName;
    double analyticalGrad{0.0};
    double numericalGrad{0.0};
    double absoluteError{0.0};
    bool passed{false};
};

class BackpropagationSolver {
public:
    // Forward pass: returns y_hat, outputs intermediate h and z1
    static double forward(double x, const NetworkParameters& p, double& h, double& z1);
    static double forward(double x, const NetworkParameters& p);

    // Squared error loss: J = 0.5 * (y_hat - y)^2
    static double computeLoss(double y_hat, double y);

    // Exact analytical backprop gradients (simultaneous evaluation on original parameters)
    static Gradients computeAnalyticalGradients(double x, double y, const NetworkParameters& p);

    // Central finite differences: (J(theta + eps) - J(theta - eps)) / (2 * eps)
    static Gradients computeNumericalGradients(double x, double y, const NetworkParameters& p, double epsilon = 1e-6);

    // Gradient verification comparing analytical vs numerical within tolerance (default <= 1e-5)
    static std::vector<GradientCheckResult> verifyGradients(double x, double y, const NetworkParameters& p, 
                                                           double epsilon = 1e-6, double tolerance = 1e-5);

    // Train for N steps with fixed learning rate eta, recording full progression
    static std::vector<TrainingStepRecord> train(double x, double y, 
                                                 NetworkParameters initialParams = {0.5, 0.0, 1.0, 0.0}, 
                                                 double learningRate = 0.1, 
                                                 int steps = 100);

    // Export utilities
    static bool exportLossHistoryCsv(const std::string& filePath, const std::vector<TrainingStepRecord>& history);
    static bool exportGradientCheckCsv(const std::string& filePath, const std::vector<GradientCheckResult>& checks);
};

} // namespace Workbench::Core::Neural
