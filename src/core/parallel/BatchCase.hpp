#pragma once

#include "thermal/ThermalParameters.hpp"
#include "thermal/ThermalFeaSolver.hpp"
#include <vector>
#include <random>

namespace Workbench::Core::Parallel {

struct ThermalCase {
    int caseId{0};
    Thermal::ThermalParameters params;
    int numElements{80};
};

struct CaseResult {
    int caseId{0};
    double maxMidpointError{0.0};
    double midpointTemperatureMid{0.0};
    double boundaryErrorLeft{0.0};
    double boundaryErrorRight{0.0};
    std::vector<double> nodalTemperatures;
};

class BatchGenerator {
public:
    static std::vector<ThermalCase> generateDeterministicBatch(
        int caseCount = 500,
        int numElements = 80,
        uint32_t seed = 42) {

        std::vector<ThermalCase> cases;
        cases.reserve(caseCount);

        std::mt19937 rng(seed);
        std::uniform_real_distribution<double> distK(5.0, 50.0);        // Conductivity W/(m K)
        std::uniform_real_distribution<double> distQ(1000.0, 50000.0);   // Heat generation W/m^3
        std::uniform_real_distribution<double> distT0(80.0, 150.0);      // Left Boundary deg C
        std::uniform_real_distribution<double> distTL(10.0, 40.0);       // Right Boundary deg C
        std::uniform_real_distribution<double> distL(0.5, 2.0);          // Length m

        for (int i = 0; i < caseCount; ++i) {
            ThermalCase c;
            c.caseId = i;
            c.numElements = numElements;
            c.params.length = distL(rng);
            c.params.area = 1e-4;
            c.params.conductivity = distK(rng);
            c.params.heatGeneration = distQ(rng);
            c.params.tempLeft = distT0(rng);
            c.params.tempRight = distTL(rng);

            cases.push_back(c);
        }

        return cases;
    }
};

} // namespace Workbench::Core::Parallel
