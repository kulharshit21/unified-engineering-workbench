#include <cassert>
#include <iostream>
#include <stdexcept>
#include "geometry/TemperatureCsvParser.hpp"

using namespace Workbench::Core::Geometry;

int main() {
    std::cout << "========================================================\n";
    std::cout << "[TEST SUITE] Running Temperature CSV Parser Tests\n";
    std::cout << "========================================================\n";

    // 1. Valid minimal CSV
    const std::string validCsv =
        "node_id,x_m,y_m,z_m,temperature_C\n"
        "1,0.0,0.0,0.0,100.0\n"
        "2,1.0,0.0,0.0,50.0\n"
        "3,1.0,1.0,0.0,20.0\n";

    TemperatureDataset ds = TemperatureCsvParser::parseString(validCsv);
    assert(ds.nodeCount() == 3);
    assert(ds.minTemperature == 20.0);
    assert(ds.maxTemperature == 100.0);
    assert(std::abs(ds.meanTemperature - 56.6666667) < 1e-4);
    assert(ds.duplicateCount == 0);
    std::cout << "[PASS] Valid CSV parsed with correct statistics.\n";

    // 2. Empty CSV rejection
    bool caught = false;
    try {
        TemperatureCsvParser::parseString("");
    } catch (const std::runtime_error &e) {
        caught = true;
        std::cout << "[PASS] Empty CSV rejected: " << e.what() << "\n";
    }
    assert(caught);

    // 3. Invalid header (wrong column count)
    caught = false;
    try {
        TemperatureCsvParser::parseString("node_id,x_m,y_m\n1,0,0\n");
    } catch (const std::runtime_error &e) {
        caught = true;
        std::cout << "[PASS] Incomplete header rejected: " << e.what() << "\n";
    }
    assert(caught);

    // 4. Invalid header column names
    caught = false;
    try {
        TemperatureCsvParser::parseString("id,pos_x,pos_y,pos_z,temp\n1,0,0,0,100\n");
    } catch (const std::runtime_error &e) {
        caught = true;
        std::cout << "[PASS] Incorrect column names rejected: " << e.what() << "\n";
    }
    assert(caught);

    // 5. Non-numeric data
    caught = false;
    try {
        TemperatureCsvParser::parseString("node_id,x_m,y_m,z_m,temperature_C\n1,invalid,0,0,100\n");
    } catch (const std::runtime_error &e) {
        caught = true;
        std::cout << "[PASS] Non-numeric entry rejected: " << e.what() << "\n";
    }
    assert(caught);

    // 6. Non-finite values (NaN / Inf)
    caught = false;
    try {
        TemperatureCsvParser::parseString("node_id,x_m,y_m,z_m,temperature_C\n1,0,0,0,NaN\n");
    } catch (const std::runtime_error &e) {
        caught = true;
        std::cout << "[PASS] NaN temperature rejected: " << e.what() << "\n";
    }
    assert(caught);

    // 7. Duplicate Node ID Handling Policy
    const std::string duplicateIdCsv =
        "node_id,x_m,y_m,z_m,temperature_C\n"
        "1,0.0,0.0,0.0,100.0\n"
        "1,0.5,0.0,0.0,80.0\n" // duplicate ID
        "2,1.0,0.0,0.0,20.0\n";

    TemperatureDataset dupDs = TemperatureCsvParser::parseString(duplicateIdCsv);
    assert(dupDs.nodeCount() == 2); // only unique nodes 1 and 2
    assert(dupDs.duplicateCount == 1);
    assert(dupDs.nodes[0].temperatureC == 100.0); // first occurrence retained
    std::cout << "[PASS] Duplicate node ID policy (retain first, track duplicate count) verified.\n";

    // 8. Degenerate range handling (all temperatures equal)
    const std::string uniformTempCsv =
        "node_id,x_m,y_m,z_m,temperature_C\n"
        "1,0.0,0.0,0.0,25.0\n"
        "2,1.0,0.0,0.0,25.0\n";
    TemperatureDataset uniDs = TemperatureCsvParser::parseString(uniformTempCsv);
    assert(uniDs.minTemperature == 25.0);
    assert(uniDs.maxTemperature == 25.0);
    std::cout << "[PASS] Degenerate temperature range (T_min == T_max) handled without divide-by-zero.\n";

    // 9. Load synthetic_ansys_temperatures.csv from disk
    const std::vector<std::string> paths = {
        "data/thermal/synthetic_ansys_temperatures.csv",
        "../../data/thermal/synthetic_ansys_temperatures.csv"
    };
    bool loaded = false;
    for (const auto &p : paths) {
        try {
            TemperatureDataset fileDs = TemperatureCsvParser::parseFile(p);
            assert(fileDs.nodeCount() == 27);
            assert(fileDs.minTemperature == 20.0);
            assert(fileDs.maxTemperature == 100.0);
            assert(fileDs.bounds.isValid());
            std::cout << "[PASS] Loaded " << p << " successfully: "
                      << fileDs.nodeCount() << " nodes, Range: ["
                      << fileDs.minTemperature << " C, "
                      << fileDs.maxTemperature << " C], Mean: "
                      << fileDs.meanTemperature << " C\n";
            loaded = true;
            break;
        } catch (...) {
            // try next path
        }
    }
    assert(loaded);

    std::cout << "========================================================\n";
    std::cout << "ALL TEMPERATURE CSV PARSER TESTS PASSED!\n";
    std::cout << "========================================================\n";
    return 0;
}
