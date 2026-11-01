#include "TestAssert.hpp"
#include <iostream>
#include "hardware/IDeviceAdapter.hpp"
#include "hardware/McuMockAdapter.hpp"
#include "hardware/FpgaMockAdapter.hpp"
#include "hardware/JtagMockAdapter.hpp"
#include "hardware/DeviceManager.hpp"

using namespace Workbench::Core::Hardware;

int main() {
    std::cout << "========================================================\n";
    std::cout << "[TEST SUITE] Running Embedded Mock Hardware Tests\n";
    std::cout << "========================================================\n";

    // 1. DeviceManager and Adapter Initialization
    DeviceManager manager;
    manager.registerDefaultMockDevices();
    TEST_ASSERT(manager.getDevices().size() == 3);
    std::cout << "[PASS] Registered 3 default mock adapters.\n";

    auto mcu = manager.getDeviceById("MCU-RP2040-001");
    auto fpga = manager.getDeviceById("FPGA-ARTY-A7-002");
    auto jtag = manager.getDeviceById("JTAG-HS2-003");

    TEST_ASSERT(mcu != nullptr);
    TEST_ASSERT(fpga != nullptr);
    TEST_ASSERT(jtag != nullptr);
    std::cout << "[PASS] Retrieved all 3 adapters by deviceId.\n";

    // 2. Truthful simulation disclosure verification
    TEST_ASSERT(mcu->getInfo().isSimulated == true);
    TEST_ASSERT(fpga->getInfo().isSimulated == true);
    TEST_ASSERT(jtag->getInfo().isSimulated == true);
    std::cout << "[PASS] All mock devices explicitly disclose isSimulated == true.\n";

    // 3. Hardware specifications and capabilities verification
    TEST_ASSERT(mcu->getInfo().nominalVoltage == 3.3);
    TEST_ASSERT(mcu->getCapabilities().maxClockSpeedHz == 133000000);
    TEST_ASSERT(fpga->getInfo().nominalVoltage == 3.3);
    TEST_ASSERT(fpga->getCapabilities().maxClockSpeedHz == 100000000);
    TEST_ASSERT(jtag->getInfo().nominalVoltage == 3.3);
    TEST_ASSERT(jtag->getCapabilities().maxClockSpeedHz == 30000000);
    std::cout << "[PASS] Hardware specs match RP2040, Arty A7, and JTAG-HS2 hardware datasheets.\n";

    // 4. Connection Lifecycle
    TEST_ASSERT(mcu->getStatus() == ConnectionStatus::Disconnected);
    bool connOk = manager.connectAll();
    TEST_ASSERT(connOk);
    TEST_ASSERT(mcu->getStatus() == ConnectionStatus::Connected);
    TEST_ASSERT(fpga->getStatus() == ConnectionStatus::Connected);
    TEST_ASSERT(jtag->getStatus() == ConnectionStatus::Connected);
    std::cout << "[PASS] Connect lifecycle succeeded for all devices.\n";

    // 5. Telemetry queries within physical sanity bounds
    auto telemetryMap = manager.queryAllTelemetry(500);
    TEST_ASSERT(telemetryMap.size() == 3);

    auto mcuTelem = telemetryMap["MCU-RP2040-001"];
    TEST_ASSERT(mcuTelem.valid);
    TEST_ASSERT(mcuTelem.internalTemperatureC >= 25.0 && mcuTelem.internalTemperatureC <= 45.0);
    TEST_ASSERT(mcuTelem.supplyVoltageV >= 3.2 && mcuTelem.supplyVoltageV <= 3.4);
    TEST_ASSERT(mcuTelem.coreVoltageV >= 1.05 && mcuTelem.coreVoltageV <= 1.15);
    TEST_ASSERT(mcuTelem.packetsProcessed == 1);
    std::cout << "[PASS] RP2040 telemetry in valid range (T=" << mcuTelem.internalTemperatureC 
              << " C, Vdd=" << mcuTelem.supplyVoltageV << " V, Vcore=" << mcuTelem.coreVoltageV << " V).\n";

    auto fpgaTelem = telemetryMap["FPGA-ARTY-A7-002"];
    TEST_ASSERT(fpgaTelem.valid);
    TEST_ASSERT(fpgaTelem.internalTemperatureC >= 35.0 && fpgaTelem.internalTemperatureC <= 55.0);
    TEST_ASSERT(fpgaTelem.coreVoltageV >= 0.95 && fpgaTelem.coreVoltageV <= 1.05); // VCCINT 1.0V nominal
    std::cout << "[PASS] Arty A7 telemetry in valid range (T=" << fpgaTelem.internalTemperatureC 
              << " C, Vccint=" << fpgaTelem.coreVoltageV << " V).\n";

    auto jtagTelem = telemetryMap["JTAG-HS2-003"];
    TEST_ASSERT(jtagTelem.valid);
    TEST_ASSERT(jtagTelem.supplyVoltageV >= 3.2 && jtagTelem.supplyVoltageV <= 3.4); // VREF sensed
    std::cout << "[PASS] JTAG-HS2 telemetry in valid range (Vref=" << jtagTelem.supplyVoltageV << " V).\n";

    // 6. Fault Injection and Timeout Handling
    std::cout << "[INFO] Testing fault injection and timeout...\n";
    fpga->injectFault(true);
    auto timedOutTelem = fpga->readTelemetry(250);
    TEST_ASSERT(!timedOutTelem.valid);
    TEST_ASSERT(fpga->getStatus() == ConnectionStatus::TimedOut);
    std::cout << "[PASS] Fault injection correctly triggered ConnectionStatus::TimedOut.\n";

    // 7. Automatic Recovery Mechanism
    std::cout << "[INFO] Testing automatic recovery sequence...\n";
    fpga->clearFault();
    bool recovered = manager.attemptRecovery("FPGA-ARTY-A7-002", 3);
    TEST_ASSERT(recovered);
    TEST_ASSERT(fpga->getStatus() == ConnectionStatus::Connected);
    auto recoveredTelem = fpga->readTelemetry(250);
    TEST_ASSERT(recoveredTelem.valid);
    std::cout << "[PASS] DeviceManager successfully recovered faulted device.\n";

    // 8. Diagnostic Report Generation
    std::string report = manager.generateDiagnosticReport();
    TEST_ASSERT(!report.empty());
    TEST_ASSERT(report.find("MCU-RP2040-001") != std::string::npos);
    TEST_ASSERT(report.find("FPGA-ARTY-A7-002") != std::string::npos);
    TEST_ASSERT(report.find("JTAG-HS2-003") != std::string::npos);
    TEST_ASSERT(report.find("TRUE (MOCK ADAPTER)") != std::string::npos);
    std::cout << "[PASS] Diagnostic report generated and contains all required disclosures.\n";

    // 9. Clean Disconnect
    manager.disconnectAll();
    TEST_ASSERT(mcu->getStatus() == ConnectionStatus::Disconnected);
    TEST_ASSERT(fpga->getStatus() == ConnectionStatus::Disconnected);
    TEST_ASSERT(jtag->getStatus() == ConnectionStatus::Disconnected);
    std::cout << "[PASS] DisconnectAll succeeded cleanly.\n";

    std::cout << "========================================================\n";
    std::cout << "All Embedded Mock tests passed successfully.\n";
    std::cout << "========================================================\n";
    return 0;
}
