#pragma once

#include "IDeviceAdapter.hpp"
#include <atomic>

namespace Workbench::Core::Hardware {

class McuMockAdapter : public IDeviceAdapter {
public:
    McuMockAdapter();
    ~McuMockAdapter() override = default;

    DeviceInfo getInfo() const override;
    DeviceCapabilities getCapabilities() const override;

    bool connect() override;
    void disconnect() override;
    ConnectionStatus getStatus() const override;

    TelemetryData readTelemetry(int timeoutMs = 500) override;

    void injectFault(bool enableTimeout) override;
    void clearFault() override;

private:
    DeviceInfo m_info;
    DeviceCapabilities m_caps;
    ConnectionStatus m_status{ConnectionStatus::Disconnected};
    std::atomic<bool> m_faultInjected{false};
    uint32_t m_packetCounter{0};
};

} // namespace Workbench::Core::Hardware
