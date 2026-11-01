#include <cassert>
#include <iostream>
#include <stdexcept>
#include "geometry/StlParser.hpp"

using namespace Workbench::Core::Geometry;

int main() {
    std::cout << "========================================================\n";
    std::cout << "[TEST SUITE] Running ASCII STL Parser Tests\n";
    std::cout << "========================================================\n";

    // 1. Valid Minimal ASCII STL string
    const std::string validStl = 
        "solid SimpleTetra\n"
        "  facet normal 0.0 0.0 1.0\n"
        "    outer loop\n"
        "      vertex 0.0 0.0 0.0\n"
        "      vertex 1.0 0.0 0.0\n"
        "      vertex 0.0 1.0 0.0\n"
        "    endloop\n"
        "  endfacet\n"
        "endsolid SimpleTetra\n";

    StlModel model = StlParser::parseString(validStl);
    assert(model.solidName == "SimpleTetra");
    assert(model.triangleCount() == 1);
    assert(model.vertexCount() == 3);
    assert(model.bounds.isValid());
    assert(model.bounds.min.x == 0.0f && model.bounds.max.x == 1.0f);
    assert(model.bounds.min.y == 0.0f && model.bounds.max.y == 1.0f);
    assert(model.bounds.min.z == 0.0f && model.bounds.max.z == 0.0f);
    std::cout << "[PASS] Valid ASCII STL parsed with correct bounds.\n";

    // 2. Empty string rejection
    bool caught = false;
    try {
        StlParser::parseString("");
    } catch (const std::runtime_error &e) {
        caught = true;
        std::cout << "[PASS] Empty STL correctly rejected: " << e.what() << "\n";
    }
    assert(caught);

    // 3. Malformed keyword (missing 'solid')
    caught = false;
    try {
        StlParser::parseString("facet normal 0 0 1\n");
    } catch (const std::runtime_error &e) {
        caught = true;
        std::cout << "[PASS] Missing solid keyword correctly rejected: " << e.what() << "\n";
    }
    assert(caught);

    // 4. Incomplete facet (only 2 vertices)
    const std::string malformedFacet = 
        "solid Incomplete\n"
        "  facet normal 0 0 1\n"
        "    outer loop\n"
        "      vertex 0 0 0\n"
        "      vertex 1 0 0\n"
        "    endloop\n"
        "  endfacet\n"
        "endsolid Incomplete\n";
    caught = false;
    try {
        StlParser::parseString(malformedFacet);
    } catch (const std::runtime_error &e) {
        caught = true;
        std::cout << "[PASS] Incomplete facet (2 vertices) rejected: " << e.what() << "\n";
    }
    assert(caught);

    // 5. Non-finite values (NaN / Inf)
    const std::string nanFacet = 
        "solid NanSolid\n"
        "  facet normal 0 0 1\n"
        "    outer loop\n"
        "      vertex 0.0 0.0 0.0\n"
        "      vertex NaN 0.0 0.0\n"
        "      vertex 0.0 1.0 0.0\n"
        "    endloop\n"
        "  endfacet\n"
        "endsolid NanSolid\n";
    caught = false;
    try {
        StlParser::parseString(nanFacet);
    } catch (const std::runtime_error &e) {
        caught = true;
        std::cout << "[PASS] NaN vertex correctly rejected: " << e.what() << "\n";
    }
    assert(caught);

    // 6. Load sample_bracket.stl from disk
    const std::vector<std::string> paths = {
        "data/geometry/sample_bracket.stl",
        "../../data/geometry/sample_bracket.stl"
    };
    bool loaded = false;
    for (const auto &p : paths) {
        try {
            StlModel bracket = StlParser::parseFile(p);
            assert(bracket.triangleCount() == 12);
            assert(bracket.vertexCount() == 36);
            assert(bracket.bounds.isValid());
            std::cout << "[PASS] Loaded " << p << " successfully: " 
                      << bracket.triangleCount() << " triangles, max extent: " 
                      << bracket.bounds.maxDimension() << " m\n";
            loaded = true;
            break;
        } catch (...) {
            // try next path
        }
    }
    assert(loaded);

    std::cout << "========================================================\n";
    std::cout << "ALL ASCII STL PARSER TESTS PASSED!\n";
    std::cout << "========================================================\n";
    return 0;
}
