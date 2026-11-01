#pragma once

#include "ThermalParameters.hpp"
#include <vector>

namespace Workbench::Core::Thermal {

struct ThermalSolution {
    int numElements{0};
    int numNodes{0};
    double elementLength{0.0};

    std::vector<double> nodeCoordinates;
    std::vector<double> nodalTemperatures;

    std::vector<double> midpointCoordinates;
    std::vector<double> midpointFemTemperatures;
    std::vector<double> midpointExactTemperatures;
    std::vector<double> midpointAbsoluteErrors;

    double maxMidpointError{0.0};
};

class ThermalFeaSolver {
public:
    explicit ThermalFeaSolver(const ThermalParameters &params = ThermalParameters{});

    void setParameters(const ThermalParameters &params);
    const ThermalParameters& getParameters() const { return m_params; }

    // Evaluates the exact analytical temperature at coordinate x:
    // T_exact(x) = T0 - (T0 - TL)*(x/L) + (q / (2*k)) * x * (L - x)
    double evaluateExact(double x) const;

    // Assembles and solves the 1D FEA problem for N uniform elements
    ThermalSolution solve(int numElements) const;

private:
    ThermalParameters m_params;
};

} // namespace Workbench::Core::Thermal
