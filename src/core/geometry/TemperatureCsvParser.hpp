#pragma once

#include "GeometryTypes.hpp"
#include <string>

namespace Workbench::Core::Geometry {

class TemperatureCsvParser {
public:
    static TemperatureDataset parseFile(const std::string &filePath);
    static TemperatureDataset parseString(const std::string &csvContent);

    // Documented schema constants
    static constexpr const char* EXPECTED_HEADER = "node_id,x_m,y_m,z_m,temperature_C";
    static constexpr size_t MAX_RECOMMENDED_NODES = 1000000;
};

} // namespace Workbench::Core::Geometry
