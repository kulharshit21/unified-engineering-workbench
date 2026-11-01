#pragma once

#include "ThermalFeaSolver.hpp"
#include <vector>
#include <string>

namespace Workbench::Core::Thermal {

struct ConvergenceEntry {
    int numElements{0};
    double elementLength{0.0};
    double maxMidpointError{0.0};
    double theoreticalError{0.0};
    double errorRatio{0.0};       // Ratio E(N_prev) / E(N), expected ~4.0
    double convergenceOrder{0.0};  // log2(E_prev / E_curr), expected ~2.0
};

class ThermalConvergence {
public:
    static std::vector<ConvergenceEntry> evaluateConvergence(
        const ThermalFeaSolver &solver,
        const std::vector<int> &meshSizes = {10, 20, 40, 80}
    );

    static std::string exportConvergenceCsv(const std::vector<ConvergenceEntry> &entries);
    static std::string exportProfilesCsv(const ThermalFeaSolver &solver, const std::vector<int> &meshSizes = {10, 20, 40, 80});
};

} // namespace Workbench::Core::Thermal
