#include "JtagMockAdapter.hpp"
#include <cmath>

namespace Workbench::Core::Hardware {

JtagMockAdapter::JtagMockAdapter() {
    m_info.deviceId = "JTAG-HS2-003";
    m_info.displayName = "Digilent JTAG-HS2 High-Speed Programming Adapter";
    m_info.type = DeviceType::JtagAdapter;
    m_info.manufacturer = "Digilent Inc.";
    m_info.serialNumber = "210249B891A0";
    m_info.firmwareVersion = "fw-rev-C-v1.08";
    m_info.physicalInterface = "High-Speed USB 2.0 (FT232H VID:0403 PID:6014)";
    m_info.nominalVoltage = 3.3; // Senses target VREF (1.8V - 5.0V)
    m_info.isSimulated = true;

    m_caps.supportedProtocols = {"JTAG (IEEE 1149.1)", "SPI", "I2C"};
    m_caps.maxClockSpeedHz = 30000000; // 30 MHz max TCK
    m_caps.bufferSizeBytes = 4096;      // 4 KB FTDI FIFO
    m_caps.supportsVoltageSensing = true; // VREF sense pin
    m_caps.supportsBreakpoints = true;
    m_caps.supportsTelemetryStream = true;
}

DeviceInfo JtagMockAdapter::getInfo() const {
    return m_info;
}

DeviceCapabilities JtagMockAdapter::getCapabilities() const {
    return m_caps;
}

bool JtagMockAdapter::connect() {
    if (m_faultInjected.load()) {
        m_status = ConnectionStatus::Error;
        return false;
    }
    m_status = ConnectionStatus::Connected;
    m_packetCounter = 0;
    return true;
}

void JtagMockAdapter::disconnect() {
    m_status = ConnectionStatus::Disconnected;
}

ConnectionStatus JtagMockAdapter::getStatus() const {
    return m_status;
}

TelemetryData JtagMockAdapter::readTelemetry(int timeoutMs) {
    TelemetryData data;
    data.timestamp = std::chrono::system_clock::now();

    if (m_status != ConnectionStatus::Connected) {
        data.valid = false;
        data.statusMessage = "ERR_NOT_CONNECTED: JTAG adapter is not connected";
        return data;
    }

    if (m_faultInjected.load()) {
        m_status = ConnectionStatus::TimedOut;
        data.valid = false;
        data.statusMessage = "ERR_TIMEOUT: JTAG TAP scan-chain response timed out after " + std::to_string(timeoutMs) + " ms";
        return data;
    }

    m_packetCounter++;
    double step = static_cast<double>(m_packetCounter);

    // Realistic JTAG adapter telemetry
    data.internalTemperatureC = 28.5 + 0.3 * std::sin(step * 0.05);
    data.supplyVoltageV = 3.305 + 0.005 * std::cos(step * 0.03); // Target VREF sensed
    data.coreVoltageV = 3.300; // Adapter internal LDO
    data.packetsProcessed = m_packetCounter;
    data.statusMessage = "OK: JTAG TAP Controller in RUN-TEST/IDLE state, VREF locked";
    data.valid = true;

    return data;
}

void JtagMockAdapter::injectFault(bool enableTimeout) {
    m_faultInjected.store(enableTimeout);
    if (enableTimeout && m_status == ConnectionStatus::Connected) {
        m_status = ConnectionStatus::TimedOut;
    }
}

void JtagMockAdapter::clearFault() {
    m_faultInjected.store(false);
    if (m_status == ConnectionStatus::TimedOut || m_status == ConnectionStatus::Error) {
        m_status = ConnectionStatus::Connected;
    }
}

} // namespace Workbench::Core::Hardware
