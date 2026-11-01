#pragma once
#include <string>
#include <stdexcept>

namespace Workbench::Core::Thermal {

struct ThermalParameters {
    double length{1.0};           // Rod length L (m)
    double area{1e-4};            // Cross-sectional area A (m^2)
    double conductivity{10.0};    // Thermal conductivity k (W/(m K))
    double heatGeneration{10000.0}; // Volumetric heat generation q (W/m^3)
    double tempLeft{100.0};       // Prescribed T(0) (deg C)
    double tempRight{20.0};       // Prescribed T(L) (deg C)

    void validate() const {
        if (length <= 0.0) {
            throw std::invalid_argument("Rod length L must be strictly positive (> 0).");
        }
        if (area <= 0.0) {
            throw std::invalid_argument("Cross-sectional area A must be strictly positive (> 0).");
        }
        if (conductivity <= 0.0) {
            throw std::invalid_argument("Thermal conductivity k must be strictly positive (> 0).");
        }
        if (heatGeneration < 0.0) {
            throw std::invalid_argument("Volumetric heat generation q cannot be negative.");
        }
    }
};

} // namespace Workbench::Core::Thermal
