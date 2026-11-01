#pragma once

#include "GeometryTypes.hpp"
#include <string>

namespace Workbench::Core::Geometry {

class StlParser {
public:
    static StlModel parseFile(const std::string &filePath);
    static StlModel parseString(const std::string &asciiContent);
    static bool isBinaryStl(const std::string &filePath);

    // Documented limitation constants
    static constexpr size_t MAX_RECOMMENDED_TRIANGLES = 500000;
    static constexpr size_t MAX_FILE_SIZE_BYTES = 100 * 1024 * 1024; // 100 MB
};

} // namespace Workbench::Core::Geometry
