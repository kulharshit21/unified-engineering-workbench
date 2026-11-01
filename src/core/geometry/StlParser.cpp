#include "StlParser.hpp"
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <cctype>
#include <algorithm>
#include <cstdint>

namespace Workbench::Core::Geometry {

static inline std::string trim(const std::string &s) {
    auto start = s.begin();
    while (start != s.end() && std::isspace(static_cast<unsigned char>(*start))) {
        start++;
    }
    auto end = s.end();
    do {
        end--;
    } while (std::distance(start, end) > 0 && std::isspace(static_cast<unsigned char>(*end)));
    return std::string(start, end + 1);
}

static inline std::string toLower(std::string s) {
    std::transform(s.begin(), s.end(), s.begin(), [](unsigned char c) -> char { return static_cast<char>(std::tolower(c)); });
    return s;
}

bool StlParser::isBinaryStl(const std::string &filePath) {
    std::ifstream file(filePath, std::ios::binary | std::ios::ate);
    if (!file.is_open()) {
        return false;
    }
    std::streamsize fileSize = file.tellg();
    if (fileSize < 84) {
        return false; // Too small for binary STL
    }

    file.seekg(0, std::ios::beg);
    char header[80];
    file.read(header, 80);

    uint32_t numTriangles = 0;
    file.read(reinterpret_cast<char*>(&numTriangles), sizeof(numTriangles));

    // A binary STL file size is exactly 80 + 4 + (numTriangles * 50) bytes
    uint64_t expectedSize = 84ULL + static_cast<uint64_t>(numTriangles) * 50ULL;
    if (static_cast<uint64_t>(fileSize) == expectedSize) {
        return true;
    }

    // Inspect first 512 bytes for null bytes or control characters
    file.seekg(0, std::ios::beg);
    std::streamsize checkLen = std::min<std::streamsize>(fileSize, 512);
    std::vector<char> buf(checkLen);
    file.read(buf.data(), checkLen);

    for (char c : buf) {
        if (c == '\0') {
            return true; // Null byte indicates binary format
        }
    }

    return false;
}

StlModel StlParser::parseFile(const std::string &filePath) {
    if (isBinaryStl(filePath)) {
        throw std::runtime_error("Binary STL format detected in '" + filePath + 
            "'. The project specification strictly requires standard ASCII STL format.");
    }

    std::ifstream file(filePath);
    if (!file.is_open()) {
        throw std::runtime_error("Cannot open STL file: " + filePath);
    }

    std::stringstream buffer;
    buffer << file.rdbuf();
    return parseString(buffer.str());
}

StlModel StlParser::parseString(const std::string &asciiContent) {
    if (asciiContent.empty()) {
        throw std::runtime_error("STL content is completely empty.");
    }

    std::istringstream stream(asciiContent);
    std::string line;
    StlModel model;

    // Check for initial 'solid' token
    if (!std::getline(stream, line)) {
        throw std::runtime_error("Empty STL stream.");
    }

    std::string trimmedLine = trim(line);
    if (toLower(trimmedLine).rfind("solid", 0) != 0) {
        throw std::runtime_error("Malformed ASCII STL: missing initial 'solid' keyword.");
    }

    if (trimmedLine.length() > 6) {
        model.solidName = trim(trimmedLine.substr(5));
    } else {
        model.solidName = "UnnamedSolid";
    }

    std::string token;
    Triangle currentTriangle;
    int vertexIndex = 0;
    bool inFacet = false;
    bool inLoop = false;
    size_t lineNum = 1;

    while (std::getline(stream, line)) {
        lineNum++;
        std::string trimmed = trim(line);
        if (trimmed.empty()) {
            continue;
        }

        std::istringstream lineStream(trimmed);
        lineStream >> token;
        std::string lowerToken = toLower(token);

        if (lowerToken == "facet") {
            if (inFacet) {
                throw std::runtime_error("Malformed STL: nested 'facet' at line " + std::to_string(lineNum));
            }
            inFacet = true;
            vertexIndex = 0;

            std::string normalKw;
            lineStream >> normalKw;
            if (toLower(normalKw) != "normal") {
                throw std::runtime_error("Malformed STL: expected 'normal' after 'facet' at line " + std::to_string(lineNum));
            }

            if (!(lineStream >> currentTriangle.normal.x >> currentTriangle.normal.y >> currentTriangle.normal.z)) {
                throw std::runtime_error("Malformed STL: invalid normal coordinates at line " + std::to_string(lineNum));
            }
            if (!currentTriangle.normal.isFinite()) {
                throw std::runtime_error("Non-finite normal coordinates (NaN/Inf) at line " + std::to_string(lineNum));
            }
        }
        else if (lowerToken == "outer") {
            std::string loopKw;
            lineStream >> loopKw;
            if (toLower(loopKw) != "loop") {
                throw std::runtime_error("Malformed STL: expected 'outer loop' at line " + std::to_string(lineNum));
            }
            inLoop = true;
        }
        else if (lowerToken == "vertex") {
            if (!inLoop) {
                throw std::runtime_error("Malformed STL: vertex declared outside 'outer loop' at line " + std::to_string(lineNum));
            }
            Vector3D v;
            if (!(lineStream >> v.x >> v.y >> v.z)) {
                throw std::runtime_error("Malformed STL: invalid vertex coordinates at line " + std::to_string(lineNum));
            }
            if (!v.isFinite()) {
                throw std::runtime_error("Non-finite vertex coordinates (NaN/Inf) at line " + std::to_string(lineNum));
            }

            if (vertexIndex == 0) currentTriangle.v1 = v;
            else if (vertexIndex == 1) currentTriangle.v2 = v;
            else if (vertexIndex == 2) currentTriangle.v3 = v;
            else {
                throw std::runtime_error("Malformed STL: more than 3 vertices in triangle facet at line " + std::to_string(lineNum));
            }
            vertexIndex++;
        }
        else if (lowerToken == "endloop") {
            inLoop = false;
            if (vertexIndex != 3) {
                throw std::runtime_error("Malformed STL: facet ended with " + std::to_string(vertexIndex) + 
                    " vertices (expected 3) at line " + std::to_string(lineNum));
            }
        }
        else if (lowerToken == "endfacet") {
            inFacet = false;
            if (!currentTriangle.isFinite()) {
                throw std::runtime_error("Non-finite coordinates in triangle facet at line " + std::to_string(lineNum));
            }

            model.bounds.expand(currentTriangle.v1);
            model.bounds.expand(currentTriangle.v2);
            model.bounds.expand(currentTriangle.v3);
            model.triangles.push_back(currentTriangle);
        }
        else if (lowerToken == "endsolid") {
            break;
        }
    }

    if (model.triangles.empty()) {
        throw std::runtime_error("Empty STL geometry: parsed zero valid triangular facets.");
    }

    return model;
}

} // namespace Workbench::Core::Geometry
