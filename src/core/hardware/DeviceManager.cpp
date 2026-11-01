#include "DeviceManager.hpp"
#include "McuMockAdapter.hpp"
#include "FpgaMockAdapter.hpp"
#include "JtagMockAdapter.hpp"
#include <sstream>
#include <iomanip>

namespace Workbench::Core::Hardware {

DeviceManager::DeviceManager() = default;

void DeviceManager::registerDevice(std::shared_ptr<IDeviceAdapter> device) {
    if (device) {
        m_devices.push_back(device);
    }
}

void DeviceManager::registerDefaultMockDevices() {
    m_devices.clear();
    m_devices.push_back(std::make_shared<McuMockAdapter>());
    m_devices.push_back(std::make_shared<FpgaMockAdapter>());
    m_devices.push_back(std::make_shared<JtagMockAdapter>());
}

const std::vector<std::shared_ptr<IDeviceAdapter>>& DeviceManager::getDevices() const {
    return m_devices;
}

std::shared_ptr<IDeviceAdapter> DeviceManager::getDeviceById(const std::string& deviceId) const {
    for (const auto& dev : m_devices) {
        if (dev && dev->getInfo().deviceId == deviceId) {
            return dev;
        }
    }
    return nullptr;
}

bool DeviceManager::connectAll() {
    bool allConnected = true;
    for (auto& dev : m_devices) {
        if (dev && !dev->connect()) {
            allConnected = false;
        }
    }
    return allConnected;
}

void DeviceManager::disconnectAll() {
    for (auto& dev : m_devices) {
        if (dev) {
            dev->disconnect();
        }
    }
}

std::unordered_map<std::string, TelemetryData> DeviceManager::queryAllTelemetry(int timeoutMs) {
    std::unordered_map<std::string, TelemetryData> results;
    for (auto& dev : m_devices) {
        if (dev) {
            results[dev->getInfo().deviceId] = dev->readTelemetry(timeoutMs);
        }
    }
    return results;
}

bool DeviceManager::attemptRecovery(const std::string& deviceId, int maxRetries) {
    auto dev = getDeviceById(deviceId);
    if (!dev) {
        return false;
    }

    for (int attempt = 1; attempt <= maxRetries; ++attempt) {
        // Attempt recovery sequence: disconnect, clear faults, reconnect
        dev->disconnect();
        dev->clearFault();
        if (dev->connect()) {
            // Verify telemetry read succeeds
            auto testTelemetry = dev->readTelemetry(200);
            if (testTelemetry.valid) {
                return true;
            }
        }
    }
    return false;
}

std::string DeviceManager::generateDiagnosticReport() const {
    std::ostringstream oss;
    oss << "=== EMBEDDED SYSTEM DIAGNOSTIC REPORT ===\n";
    oss << "Registered Devices: " << m_devices.size() << "\n\n";

    for (const auto& dev : m_devices) {
        if (!dev) continue;
        auto info = dev->getInfo();
        auto caps = dev->getCapabilities();
        auto status = dev->getStatus();

        oss << "Device: " << info.displayName << " [" << info.deviceId << "]\n";
        oss << "  Manufacturer: " << info.manufacturer << "\n";
        oss << "  Serial Number: " << info.serialNumber << "\n";
        oss << "  Interface: " << info.physicalInterface << "\n";
        oss << "  Nominal Voltage: " << std::fixed << std::setprecision(2) << info.nominalVoltage << " V\n";
        oss << "  Simulated Mode: " << (info.isSimulated ? "TRUE (MOCK ADAPTER)" : "FALSE (PHYSICAL HARDWARE)") << "\n";
        oss << "  Status: ";
        switch (status) {
            case ConnectionStatus::Connected: oss << "CONNECTED\n"; break;
            case ConnectionStatus::Disconnected: oss << "DISCONNECTED\n"; break;
            case ConnectionStatus::Connecting: oss << "CONNECTING\n"; break;
            case ConnectionStatus::TimedOut: oss << "TIMED OUT (RECOVERABLE FAULT)\n"; break;
            case ConnectionStatus::Error: oss << "ERROR\n"; break;
        }
        oss << "  Max Clock: " << (caps.maxClockSpeedHz / 1000000.0) << " MHz\n";
        oss << "  Buffer Size: " << caps.bufferSizeBytes << " bytes\n";
        oss << "  Supported Protocols: ";
        for (size_t i = 0; i < caps.supportedProtocols.size(); ++i) {
            oss << caps.supportedProtocols[i] << (i + 1 < caps.supportedProtocols.size() ? ", " : "\n");
        }
        oss << "\n";
    }

    return oss.str();
}

} // namespace Workbench::Core::Hardware
