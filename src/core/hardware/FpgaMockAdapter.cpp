#include "FpgaMockAdapter.hpp"
#include <cmath>

namespace Workbench::Core::Hardware {

FpgaMockAdapter::FpgaMockAdapter() {
    m_info.deviceId = "FPGA-ARTY-A7-002";
    m_info.displayName = "Digilent Arty A7-35T (Xilinx Artix-7 XC7A35T-1CPG236C)";
    m_info.type = DeviceType::Fpga;
    m_info.manufacturer = "Digilent / AMD Xilinx";
    m_info.serialNumber = "210319A2B44C";
    m_info.firmwareVersion = "bitstream-v2.3-accelerator";
    m_info.physicalInterface = "USB UART / FT2232HQ Channel B (COM5)";
    m_info.nominalVoltage = 3.3;
    m_info.isSimulated = true;

    m_caps.supportedProtocols = {"SPI", "UART", "AXI4-Lite", "AXI-Stream", "JTAG"};
    m_caps.maxClockSpeedHz = 100000000; // 100 MHz system clock
    m_caps.bufferSizeBytes = 256 * 1024 * 1024; // 256 MB DDR3L
    m_caps.supportsVoltageSensing = true; // XADC internal sensors
    m_caps.supportsBreakpoints = false;
    m_caps.supportsTelemetryStream = true;
}

DeviceInfo FpgaMockAdapter::getInfo() const {
    return m_info;
}

DeviceCapabilities FpgaMockAdapter::getCapabilities() const {
    return m_caps;
}

bool FpgaMockAdapter::connect() {
    if (m_faultInjected.load()) {
        m_status = ConnectionStatus::Error;
        return false;
    }
    m_status = ConnectionStatus::Connected;
    m_packetCounter = 0;
    return true;
}

void FpgaMockAdapter::disconnect() {
    m_status = ConnectionStatus::Disconnected;
}

ConnectionStatus FpgaMockAdapter::getStatus() const {
    return m_status;
}

TelemetryData FpgaMockAdapter::readTelemetry(int timeoutMs) {
    TelemetryData data;
    data.timestamp = std::chrono::system_clock::now();

    if (m_status != ConnectionStatus::Connected) {
        data.valid = false;
        data.statusMessage = "ERR_NOT_CONNECTED: FPGA device is not connected";
        return data;
    }

    if (m_faultInjected.load()) {
        m_status = ConnectionStatus::TimedOut;
        data.valid = false;
        data.statusMessage = "ERR_TIMEOUT: FPGA XADC AXI-Stream interface timed out after " + std::to_string(timeoutMs) + " ms";
        return data;
    }

    m_packetCounter++;
    double step = static_cast<double>(m_packetCounter);

    // Realistic FPGA die temperature under moderate logic load
    data.internalTemperatureC = 41.8 + 0.9 * std::sin(step * 0.08);
    data.supplyVoltageV = 3.295 + 0.008 * std::cos(step * 0.04);
    data.coreVoltageV = 1.002 + 0.003 * std::sin(step * 0.06); // VCCINT is 1.0V nominal
    data.packetsProcessed = m_packetCounter;
    data.statusMessage = "OK: Artix-7 XADC die temperature and internal rails nominal";
    data.valid = true;

    return data;
}

void FpgaMockAdapter::injectFault(bool enableTimeout) {
    m_faultInjected.store(enableTimeout);
    if (enableTimeout && m_status == ConnectionStatus::Connected) {
        m_status = ConnectionStatus::TimedOut;
    }
}

void FpgaMockAdapter::clearFault() {
    m_faultInjected.store(false);
    if (m_status == ConnectionStatus::TimedOut || m_status == ConnectionStatus::Error) {
        m_status = ConnectionStatus::Connected;
    }
}

} // namespace Workbench::Core::Hardware
