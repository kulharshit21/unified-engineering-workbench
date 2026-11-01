#include "TemperatureCsvParser.hpp"
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <unordered_set>
#include <cctype>
#include <algorithm>
#include <numeric>

namespace Workbench::Core::Geometry {

static inline std::string trim(const std::string &s) {
    auto start = s.begin();
    while (start != s.end() && std::isspace(static_cast<unsigned char>(*start))) start++;
    auto end = s.end();
    do { end--; } while (std::distance(start, end) > 0 && std::isspace(static_cast<unsigned char>(*end)));
    return std::string(start, end + 1);
}

static inline std::string toLower(std::string s) {
    std::transform(s.begin(), s.end(), s.begin(), [](unsigned char c) -> char { return static_cast<char>(std::tolower(c)); });
    return s;
}

static std::vector<std::string> splitCsvRow(const std::string &line) {
    std::vector<std::string> fields;
    std::string current;
    bool inQuotes = false;

    for (size_t i = 0; i < line.size(); ++i) {
        char c = line[i];
        if (c == '"') {
            inQuotes = !inQuotes;
        } else if (c == ',' && !inQuotes) {
            fields.push_back(trim(current));
            current.clear();
        } else {
            current += c;
        }
    }
    fields.push_back(trim(current));
    return fields;
}

TemperatureDataset TemperatureCsvParser::parseFile(const std::string &filePath) {
    std::ifstream file(filePath);
    if (!file.is_open()) {
        throw std::runtime_error("Cannot open CSV file: " + filePath);
    }
    std::stringstream buffer;
    buffer << file.rdbuf();
    return parseString(buffer.str());
}

TemperatureDataset TemperatureCsvParser::parseString(const std::string &csvContent) {
    if (csvContent.empty()) {
        throw std::runtime_error("CSV content is completely empty.");
    }

    std::istringstream stream(csvContent);
    std::string line;
    TemperatureDataset dataset;

    // 1. Validate Header Row
    if (!std::getline(stream, line)) {
        throw std::runtime_error("Empty CSV file: no header found.");
    }

    auto headerFields = splitCsvRow(line);
    if (headerFields.size() != 5) {
        throw std::runtime_error("Invalid CSV schema: expected 5 columns (node_id,x_m,y_m,z_m,temperature_C), found " +
            std::to_string(headerFields.size()));
    }

    // Verify column names case-insensitively
    if (toLower(headerFields[0]) != "node_id" ||
        toLower(headerFields[1]) != "x_m" ||
        toLower(headerFields[2]) != "y_m" ||
        toLower(headerFields[3]) != "z_m" ||
        toLower(headerFields[4]) != "temperature_c") {
        throw std::runtime_error("Malformed CSV header. Expected exactly: 'node_id,x_m,y_m,z_m,temperature_C'. Got: '" + line + "'");
    }

    std::unordered_set<int> seenNodeIds;
    size_t lineNum = 1;
    double tempSum = 0.0;

    // 2. Parse Data Rows
    while (std::getline(stream, line)) {
        lineNum++;
        std::string trimmed = trim(line);
        if (trimmed.empty()) {
            continue;
        }

        auto fields = splitCsvRow(trimmed);
        if (fields.size() != 5) {
            throw std::runtime_error("CSV row " + std::to_string(lineNum) + 
                " has " + std::to_string(fields.size()) + " columns (expected 5).");
        }

        TemperatureNode node;
        try {
            node.nodeId = std::stoi(fields[0]);
            node.x = std::stod(fields[1]);
            node.y = std::stod(fields[2]);
            node.z = std::stod(fields[3]);
            node.temperatureC = std::stod(fields[4]);
        } catch (const std::exception &e) {
            throw std::runtime_error("Numeric parsing failure on CSV line " + std::to_string(lineNum) + ": " + e.what());
        }

        if (!node.isFinite()) {
            throw std::runtime_error("Non-finite coordinates or temperature (NaN/Inf) on CSV line " + std::to_string(lineNum));
        }

        // Duplicate node ID handling policy:
        // Retain first occurrence, record duplicate count
        if (seenNodeIds.find(node.nodeId) != seenNodeIds.end()) {
            dataset.duplicateCount++;
            continue;
        }
        seenNodeIds.insert(node.nodeId);

        // Expand bounds and statistics
        Vector3D pt{ static_cast<float>(node.x), static_cast<float>(node.y), static_cast<float>(node.z) };
        dataset.bounds.expand(pt);

        dataset.minTemperature = std::min(dataset.minTemperature, node.temperatureC);
        dataset.maxTemperature = std::max(dataset.maxTemperature, node.temperatureC);
        tempSum += node.temperatureC;

        dataset.nodes.push_back(node);
    }

    if (dataset.nodes.empty()) {
        throw std::runtime_error("CSV dataset contains zero valid data rows.");
    }

    dataset.meanTemperature = tempSum / static_cast<double>(dataset.nodes.size());
    return dataset;
}

} // namespace Workbench::Core::Geometry
