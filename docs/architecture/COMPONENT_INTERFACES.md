# Component Interfaces & UI Interaction Specification

## 1. Architectural Interface Principles

The Workbench enforces strict interface boundaries between the presentation layer (`WorkbenchApp`) and the numerical engine (`WorkbenchCore`):
- **Value Semantics**: Data passed across module boundaries is encapsulated in lightweight, self-validating plain-old-data (POD) structs (`ThermalParameters`, `ThermalSolution`, `Gradients`, `DeviceInfo`).
- **Abstract Adapter Interfaces**: Hardware peripherals implement the abstract base class `IDeviceAdapter` to decouple physical USB/JTAG protocols from UI monitoring code.
- **Asynchronous Decoupling**: Compute-intensive tasks communicate completion to the Qt main thread via signal-slot connections or Qt concurrent futures, preventing UI lockups.

---

## 2. UI-to-Core Interaction Diagram (Diagram B)

```mermaid
sequenceDiagram
    autonumber
    actor User as Engineer / User
    participant View as Qt View (e.g. ThermalView)
    participant Core as WorkbenchCore Solver
    participant Worker as ThreadPool / Background Worker
    participant Plot as PlotWidget (QPainter)

    User->>View: Enters parameters (k=10, q=10000, T0=100, TL=20)
    User->>View: Clicks "Run Convergence Study"
    View->>Core: ThermalParameters::validate()
    Note over View,Core: Validation passes (positive k, L, A)
    View->>Core: ThermalConvergence::evaluateConvergence(solver, {10,20,40,80})
    activate Core
    loop Each Mesh Size N
        Core->>Core: ThermalFeaSolver::solve(N)
        Note over Core: Assembles K, F; Solves TDMA O(N)
        Core->>Core: Evaluates midpoint error e = |T_fem - T_exact|
    end
    Core-->>View: std::vector<ConvergenceEntry>
    deactivate Core
    View->>View: Populates QTableWidget (h, error, ratio, order)
    View->>Plot: setData(x_points, T_numerical, T_exact)
    activate Plot
    Plot->>Plot: Computes coordinate transforms & draws axes
    Plot->>User: Renders 60 FPS curve with legend & labels
    deactivate Plot
```

---

## 3. Core Class Interfaces

### 3.1 Hardware Abstraction Layer: `IDeviceAdapter`
```cpp
namespace Workbench::Core::Hardware {

enum class DeviceType { Microcontroller, Fpga, JtagAdapter };
enum class ConnectionStatus { Disconnected, Connected, Connecting, Error, TimedOut };

struct DeviceInfo {
    std::string deviceId;
    std::string displayName;
    DeviceType type;
    std::string manufacturer;
    std::string serialNumber;
    std::string firmwareVersion;
    std::string physicalInterface;
    double nominalVoltage{3.3};
    bool isSimulated{false};
};

struct TelemetryData {
    std::chrono::system_clock::time_point timestamp;
    double internalTemperatureC{0.0};
    double supplyVoltageV{0.0};
    double coreVoltageV{0.0};
    uint64_t packetsProcessed{0};
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
    virtual TelemetryData readTelemetry(int timeoutMs = 500) = 0;
    virtual void injectFault(bool enableTimeout) = 0;
    virtual void clearFault() = 0;
};

}
```

### 3.2 Thermal FEA Solver: `ThermalFeaSolver`
```cpp
namespace Workbench::Core::Thermal {

struct ThermalParameters {
    double length{1.0};           // Rod length L (m)
    double area{1e-4};            // Cross-sectional area A (m^2)
    double conductivity{10.0};    // Thermal conductivity k (W/(m K))
    double heatGeneration{10000.0}; // Volumetric heat generation q (W/m^3)
    double tempLeft{100.0};       // T(0) (deg C)
    double tempRight{20.0};       // T(L) (deg C)

    void validate() const;        // Throws std::invalid_argument if invalid
};

struct ThermalSolution {
    int numElements{0};
    int numNodes{0};
    double elementLength{0.0};
    std::vector<double> nodeCoordinates;
    std::vector<double> nodalTemperatures;
    std::vector<double> midpointCoordinates;
    std::vector<double> midpointFemTemperatures;
    std::vector<double> midpointExactTemperatures;
    std::vector<double> midpointAbsoluteErrors;
    double maxMidpointError{0.0};
};

class ThermalFeaSolver {
public:
    explicit ThermalFeaSolver(const ThermalParameters &params);
    void setParameters(const ThermalParameters &params);
    const ThermalParameters& getParameters() const;
    double evaluateExact(double x) const;
    ThermalSolution solve(int numElements) const;
};

}
```

### 3.3 Neural Backpropagation Engine: `BackpropagationSolver`
```cpp
namespace Workbench::Core::Neural {

struct NetworkParameters {
    double w{0.5}; // Hidden weight
    double b{0.0}; // Hidden bias
    double v{1.0}; // Output weight
    double c{0.0}; // Output bias
};

struct Gradients {
    double dJ_dw{0.0};
    double dJ_db{0.0};
    double dJ_dv{0.0};
    double dJ_dc{0.0};
};

class BackpropagationSolver {
public:
    static double forward(double x, const NetworkParameters& p, double& h, double& z1);
    static double computeLoss(double y_hat, double y);
    static Gradients computeAnalyticalGradients(double x, double y, const NetworkParameters& p);
    static Gradients computeNumericalGradients(double x, double y, const NetworkParameters& p, double epsilon = 1e-6);
    static std::vector<GradientCheckResult> verifyGradients(double x, double y, const NetworkParameters& p, double epsilon = 1e-6, double tolerance = 1e-5);
    static std::vector<TrainingStepRecord> train(double x, double y, NetworkParameters initialParams, double learningRate = 0.1, int steps = 100);
};

}
```
