#pragma once

#include <string>
#include <vector>
#include <chrono>

namespace Workbench::Core::Hardware {

enum class DeviceType {
    Microcontroller,
    Fpga,
    JtagAdapter
};

enum class ConnectionStatus {
    Disconnected,
    Connecting,
    Connected,
    Error,
    TimedOut
};

struct DeviceInfo {
    std::string deviceId;
    std::string displayName;
    DeviceType type;
    std::string manufacturer;
    std::string serialNumber;
    std::string firmwareVersion;
    std::string physicalInterface; // e.g. "USB CDC-ACM (COM4)", "FTDI JTAG (USB VID:0403 PID:6014)"
    double nominalVoltage{3.3};     // I/O voltage domain (V)
    bool isSimulated{true};          // Mandatory explicit disclosure
};

struct DeviceCapabilities {
    std::vector<std::string> supportedProtocols;
    uint32_t maxClockSpeedHz{0};
    uint32_t bufferSizeBytes{0};
    bool supportsVoltageSensing{false};
    bool supportsBreakpoints{false};
    bool supportsTelemetryStream{true};
};

struct TelemetryData {
    std::chrono::system_clock::time_point timestamp;
    double internalTemperatureC{0.0};
    double supplyVoltageV{0.0};
    double coreVoltageV{0.0};
    uint32_t packetsProcessed{0};
    std::string statusMessage;
    bool valid{false};
};

class IDeviceAdapter {
public:
    virtual ~IDeviceAdapter() = default;

    virtual DeviceInfo getInfo() const = 0;
    virtual DeviceCapabilities getCapabilities() const = 0;

    virtual bool connect() = 0;
    virtual void disconnect() = 0;
    virtual ConnectionStatus getStatus() const = 0;

    // Reads telemetry. Can simulate configurable latency and recoverable timeouts.
    virtual TelemetryData readTelemetry(int timeoutMs = 500) = 0;

    // Recoverable failure simulation controls
    virtual void injectFault(bool enableTimeout) = 0;
    virtual void clearFault() = 0;
};

} // namespace Workbench::Core::Hardware
