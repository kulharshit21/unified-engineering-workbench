#pragma once

#include "IDeviceAdapter.hpp"
#include <memory>
#include <vector>
#include <string>
#include <unordered_map>

namespace Workbench::Core::Hardware {

class DeviceManager {
public:
    DeviceManager();
    ~DeviceManager() = default;

    void registerDevice(std::shared_ptr<IDeviceAdapter> device);
    void registerDefaultMockDevices();

    const std::vector<std::shared_ptr<IDeviceAdapter>>& getDevices() const;
    std::shared_ptr<IDeviceAdapter> getDeviceById(const std::string& deviceId) const;

    bool connectAll();
    void disconnectAll();

    std::unordered_map<std::string, TelemetryData> queryAllTelemetry(int timeoutMs = 500);

    // Fault recovery: attempts reconnection and clears faults if recoverable
    bool attemptRecovery(const std::string& deviceId, int maxRetries = 3);

    // Logging & Diagnostic reporting
    std::string generateDiagnosticReport() const;

private:
    std::vector<std::shared_ptr<IDeviceAdapter>> m_devices;
};

} // namespace Workbench::Core::Hardware
