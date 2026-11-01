#include "McuMockAdapter.hpp"
#include <cmath>
#include <thread>

namespace Workbench::Core::Hardware {

McuMockAdapter::McuMockAdapter() {
    m_info.deviceId = "MCU-RP2040-001";
    m_info.displayName = "Raspberry Pi Pico (RP2040 Dual Cortex-M0+)";
    m_info.type = DeviceType::Microcontroller;
    m_info.manufacturer = "Raspberry Pi Ltd";
    m_info.serialNumber = "E660583883713028";
    m_info.firmwareVersion = "v1.5.1-pico-telemetry";
    m_info.physicalInterface = "USB 1.1 CDC-ACM (COM3)";
    m_info.nominalVoltage = 3.3;
    m_info.isSimulated = true;

    m_caps.supportedProtocols = {"UART", "SPI", "I2C", "PWM", "USB CDC-ACM"};
    m_caps.maxClockSpeedHz = 133000000; // 133 MHz
    m_caps.bufferSizeBytes = 264 * 1024; // 264 KB SRAM
    m_caps.supportsVoltageSensing = true;
    m_caps.supportsBreakpoints = true;
    m_caps.supportsTelemetryStream = true;
}

DeviceInfo McuMockAdapter::getInfo() const {
    return m_info;
}

DeviceCapabilities McuMockAdapter::getCapabilities() const {
    return m_caps;
}

bool McuMockAdapter::connect() {
    if (m_faultInjected.load()) {
        m_status = ConnectionStatus::Error;
        return false;
    }
    m_status = ConnectionStatus::Connected;
    m_packetCounter = 0;
    return true;
}

void McuMockAdapter::disconnect() {
    m_status = ConnectionStatus::Disconnected;
}

ConnectionStatus McuMockAdapter::getStatus() const {
    return m_status;
}

TelemetryData McuMockAdapter::readTelemetry(int timeoutMs) {
    TelemetryData data;
    data.timestamp = std::chrono::system_clock::now();

    if (m_status != ConnectionStatus::Connected) {
        data.valid = false;
        data.statusMessage = "ERR_NOT_CONNECTED: Device is not connected";
        return data;
    }

    if (m_faultInjected.load()) {
        m_status = ConnectionStatus::TimedOut;
        data.valid = false;
        data.statusMessage = "ERR_TIMEOUT: Response timed out after " + std::to_string(timeoutMs) + " ms";
        return data;
    }

    m_packetCounter++;
    double step = static_cast<double>(m_packetCounter);
    
    // Plausible simulated temperature and voltage fluctuations
    data.internalTemperatureC = 32.4 + 0.6 * std::sin(step * 0.1);
    data.supplyVoltageV = 3.31 + 0.015 * std::cos(step * 0.05);
    data.coreVoltageV = 1.100 + 0.005 * std::sin(step * 0.07);
    data.packetsProcessed = m_packetCounter;
    data.statusMessage = "OK: RP2040 internal temperature sensor & ADC healthy";
    data.valid = true;

    return data;
}

void McuMockAdapter::injectFault(bool enableTimeout) {
    m_faultInjected.store(enableTimeout);
    if (enableTimeout && m_status == ConnectionStatus::Connected) {
        m_status = ConnectionStatus::TimedOut;
    }
}

void McuMockAdapter::clearFault() {
    m_faultInjected.store(false);
    if (m_status == ConnectionStatus::TimedOut || m_status == ConnectionStatus::Error) {
        m_status = ConnectionStatus::Connected;
    }
}

} // namespace Workbench::Core::Hardware
