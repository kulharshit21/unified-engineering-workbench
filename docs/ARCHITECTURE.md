# System Architecture Specification

## 1. High-Level System Architecture

The **Unified Engineering Workbench** is organized as a two-tier desktop architecture: a zero-dependency C++17 core numerical library (`WorkbenchCore`) and an asynchronous Qt 6 presentation shell (`WorkbenchApp`).

```mermaid
graph TD
    subgraph UI ["Desktop Presentation Tier (Qt 6.8.2 Widgets)"]
        MW["MainWindow"]
        GV["GeometryView"]
        TV["ThermalView"]
        PV["PerformanceView"]
        HV["HardwareView"]
        BV["BackpropView"]
        PW["PlotWidget (Custom 2D)"]
        OV["OpenGLViewport (3D Orbit)"]

        MW --> GV
        MW --> TV
        MW --> PV
        MW --> HV
        MW --> BV
        GV --> OV
        TV --> PW
        PV --> PW
        BV --> PW
    end

    subgraph Core ["Numerical Core Tier (C++17 Static Library)"]
        subgraph Geom ["Geometry & Mesh"]
            STL["StlParser"]
            CSV["TemperatureCsvParser"]
            BOX["BoundingBox & Types"]
        end

        subgraph Therm ["Thermal FEA Engine"]
            TFEA["ThermalFeaSolver (Thomas Tridiagonal)"]
            TCON["ThermalConvergence (p=2.0 Verification)"]
            TPAR["ThermalParameters"]
        end

        subgraph Par ["Multicore Concurrency"]
            TP["ThreadPool (Bounded Workers)"]
            BS["BatchSolver"]
            BE["BenchmarkEngine (Sp, Ep Metrics)"]
        end

        subgraph Hw ["Hardware Mock Tier"]
            IDA["IDeviceAdapter"]
            MCU["McuMockAdapter (RP2040)"]
            FPGA["FpgaMockAdapter (Artix-7)"]
            JTAG["JtagMockAdapter (JTAG-HS2)"]
            DM["DeviceManager (Fault & Recovery)"]
        end

        subgraph Neu ["Neural Optimization"]
            BP["BackpropagationSolver"]
            GC["Central Finite-Diff (eps=1e-6)"]
        end
    end

    GV --> Geom
    TV --> Therm
    PV --> Par
    HV --> Hw
    BV --> Neu
```

---

## 2. Key Design Patterns & Structural Principles

### 2.1 Complete Framework Decoupling
- **Zero GUI Leaks in Core**: Files within `src/core/` contain zero `#include <Q...>` references. All math and file logic relies exclusively on standard C++ (`<vector>`, `<cmath>`, `<chrono>`, `<thread>`, `<filesystem>`).
- **Headless & Embedded Portability**: The numerical library compiles cleanly as a static library on any standard C++17 platform (Windows, Linux, macOS, RTOS).

### 2.2 Numerical Invariance & Determinism
- **Parallel Determinism**: In `BatchSolver.cpp`, tasks are chunked deterministically across worker threads and stored into pre-allocated vectors in original input order.
- **Floating-Point Consistency**: Compiled with `/fp:precise` under MSVC to guarantee identical IEEE-754 binary outputs regardless of worker count or thread scheduling.

### 2.3 Hardware Abstraction Layer (HAL)
- The `IDeviceAdapter` interface encapsulates low-level physical transport (CDC-ACM virtual COM, FTDI D2XX, or JTAG TAP controllers).
- Mock implementations (`McuMockAdapter`, `FpgaMockAdapter`, `JtagMockAdapter`) allow 100% test coverage without bench instruments attached, with drop-in production readiness.

---

## 3. Concurrency & Memory Model

1. **Thread Safety**: UI actions trigger non-blocking calls or update visual progress indicators via `QApplication::processEvents()`.
2. **Worker Pool Synchronization**: Tasks in `ThreadPool.hpp` are guarded by `std::mutex` and notified via `std::condition_variable`. Threads are cleanly joined upon destruction using RAII.
3. **Resource Ownership**: Strict usage of `std::shared_ptr` and `std::unique_ptr` throughout the hardware device manager and geometry view ensures zero memory leaks.
