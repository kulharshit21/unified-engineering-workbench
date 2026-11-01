#include "ThermalFeaSolver.hpp"
#include <cmath>
#include <algorithm>
#include <stdexcept>

namespace Workbench::Core::Thermal {

ThermalFeaSolver::ThermalFeaSolver(const ThermalParameters &params)
    : m_params(params) {
    m_params.validate();
}

void ThermalFeaSolver::setParameters(const ThermalParameters &params) {
    params.validate();
    m_params = params;
}

double ThermalFeaSolver::evaluateExact(double x) const {
    const double L = m_params.length;
    const double k = m_params.conductivity;
    const double q = m_params.heatGeneration;
    const double T0 = m_params.tempLeft;
    const double TL = m_params.tempRight;

    // Governing analytical solution:
    // T_exact(x) = T0 - (T0 - TL)*(x/L) + (q / (2*k)) * x * (L - x)
    return T0 - (T0 - TL) * (x / L) + (q / (2.0 * k)) * x * (L - x);
}

ThermalSolution ThermalFeaSolver::solve(int numElements) const {
    if (numElements < 1) {
        throw std::invalid_argument("Number of finite elements must be >= 1.");
    }

    ThermalSolution sol;
    sol.numElements = numElements;
    sol.numNodes = numElements + 1;
    sol.elementLength = m_params.length / static_cast<double>(numElements);

    const int N = numElements;
    const double h = sol.elementLength;
    const double k = m_params.conductivity;
    const double q = m_params.heatGeneration;
    const double T0 = m_params.tempLeft;
    const double TL = m_params.tempRight;

    sol.nodeCoordinates.resize(sol.numNodes);
    for (int i = 0; i <= N; ++i) {
        sol.nodeCoordinates[i] = i * h;
    }

    sol.nodalTemperatures.resize(sol.numNodes, 0.0);
    sol.nodalTemperatures[0] = T0;
    sol.nodalTemperatures[N] = TL;

    // If only 1 element, only boundary nodes exist
    if (N > 1) {
        // Interior nodes: 1 to N-1 (size M = N - 1)
        const int M = N - 1;

        // Tridiagonal matrix:
        // Main diagonal d_i = 2.0
        // Sub-diagonal a_i = -1.0
        // Super-diagonal c_i = -1.0
        // Normalized RHS b_i = q*h^2/k (+ boundary terms at ends)
        std::vector<double> a(M, -1.0);
        std::vector<double> d(M, 2.0);
        std::vector<double> c(M, -1.0);
        std::vector<double> rhs(M, (q * h * h) / k);

        // Apply boundary contributions
        rhs[0] += T0;
        rhs[M - 1] += TL;

        // Thomas Algorithm (TDMA) for tridiagonal system
        // Forward elimination
        for (int i = 1; i < M; ++i) {
            double m = a[i] / d[i - 1];
            d[i] -= m * c[i - 1];
            rhs[i] -= m * rhs[i - 1];
        }

        // Back substitution
        std::vector<double> interiorT(M, 0.0);
        interiorT[M - 1] = rhs[M - 1] / d[M - 1];
        for (int i = M - 2; i >= 0; --i) {
            interiorT[i] = (rhs[i] - c[i] * interiorT[i + 1]) / d[i];
        }

        for (int i = 0; i < M; ++i) {
            sol.nodalTemperatures[i + 1] = interiorT[i];
        }
    }

    // Evaluate FE interpolation at element midpoints and compare with exact
    sol.midpointCoordinates.resize(N);
    sol.midpointFemTemperatures.resize(N);
    sol.midpointExactTemperatures.resize(N);
    sol.midpointAbsoluteErrors.resize(N);
    sol.maxMidpointError = 0.0;

    for (int e = 0; e < N; ++e) {
        const double xMid = (e + 0.5) * h;
        // 1D Linear element interpolation at midpoint: N1 = 0.5, N2 = 0.5
        const double tFemMid = 0.5 * sol.nodalTemperatures[e] + 0.5 * sol.nodalTemperatures[e + 1];
        const double tExactMid = evaluateExact(xMid);
        const double err = std::abs(tFemMid - tExactMid);

        sol.midpointCoordinates[e] = xMid;
        sol.midpointFemTemperatures[e] = tFemMid;
        sol.midpointExactTemperatures[e] = tExactMid;
        sol.midpointAbsoluteErrors[e] = err;

        if (err > sol.maxMidpointError) {
            sol.maxMidpointError = err;
        }
    }

    return sol;
}

} // namespace Workbench::Core::Thermal
