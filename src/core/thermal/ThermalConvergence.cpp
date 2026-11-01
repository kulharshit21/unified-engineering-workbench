#include "ThermalConvergence.hpp"
#include <sstream>
#include <iomanip>
#include <cmath>

namespace Workbench::Core::Thermal {

std::vector<ConvergenceEntry> ThermalConvergence::evaluateConvergence(
    const ThermalFeaSolver &solver,
    const std::vector<int> &meshSizes) {

    std::vector<ConvergenceEntry> entries;
    entries.reserve(meshSizes.size());

    const double q = solver.getParameters().heatGeneration;
    const double k = solver.getParameters().conductivity;

    double prevError = 0.0;

    for (int N : meshSizes) {
        ThermalSolution sol = solver.solve(N);
        const double h = sol.elementLength;
        const double eTheory = (q / (8.0 * k)) * (h * h);

        ConvergenceEntry entry;
        entry.numElements = N;
        entry.elementLength = h;
        entry.maxMidpointError = sol.maxMidpointError;
        entry.theoreticalError = eTheory;

        if (prevError > 0.0) {
            entry.errorRatio = prevError / sol.maxMidpointError;
            entry.convergenceOrder = std::log2(entry.errorRatio);
        } else {
            entry.errorRatio = 0.0;
            entry.convergenceOrder = 0.0;
        }

        prevError = sol.maxMidpointError;
        entries.push_back(entry);
    }

    return entries;
}

std::string ThermalConvergence::exportConvergenceCsv(const std::vector<ConvergenceEntry> &entries) {
    std::ostringstream ss;
    ss << std::fixed << std::setprecision(8);
    ss << "elements_N,element_length_h_m,max_midpoint_error_C,theoretical_error_C,reduction_ratio,convergence_order\n";

    for (const auto &e : entries) {
        ss << e.numElements << ","
           << e.elementLength << ","
           << e.maxMidpointError << ","
           << e.theoreticalError << ","
           << e.errorRatio << ","
           << e.convergenceOrder << "\n";
    }

    return ss.str();
}

std::string ThermalConvergence::exportProfilesCsv(const ThermalFeaSolver &solver, const std::vector<int> &meshSizes) {
    std::ostringstream ss;
    ss << std::fixed << std::setprecision(8);
    ss << "mesh_N,node_index,x_m,T_numerical_C,T_exact_C,absolute_difference_C\n";

    for (int N : meshSizes) {
        ThermalSolution sol = solver.solve(N);
        for (int i = 0; i < sol.numNodes; ++i) {
            double x = sol.nodeCoordinates[i];
            double tNum = sol.nodalTemperatures[i];
            double tExact = solver.evaluateExact(x);
            double diff = std::abs(tNum - tExact);

            ss << N << ","
               << i << ","
               << x << ","
               << tNum << ","
               << tExact << ","
               << diff << "\n";
        }
    }

    return ss.str();
}

} // namespace Workbench::Core::Thermal
