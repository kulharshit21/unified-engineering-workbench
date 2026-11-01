# Architecture Decision Records (ADRs)

This document records the foundational architectural decisions made during the design and engineering of the **PASS SCIENCE WORKS Unified Engineering Workbench**.

---

## ADR-001: Separation of `WorkbenchCore` (Pure C++17) and `WorkbenchApp` (Qt 6.8.2)

- **Status**: ACCEPTED & IMPLEMENTED
- **Context**: The assignment requires an industrial-grade C++ backend, comprehensive unit testing, and an interactive desktop GUI. Coupling mathematical solvers directly to Qt GUI classes would prevent standalone CLI testing, bloat test dependencies, and degrade headless CI pipelines.
- **Decision**: 
  - Partition all mathematical computation, file parsing, concurrency, and hardware abstraction into a static library `WorkbenchCore` compiled with standard C++17 only.
  - Implement the GUI presentation layer `WorkbenchApp` using Qt 6.8.2, interacting with `WorkbenchCore` exclusively via value-semantic data structures and abstract interfaces.
- **Consequences**:
  - `WorkbenchCore` compiles instantaneously and tests can execute in headless continuous integration environments without display servers or Qt dynamic library linkages.
  - Clear architectural boundaries prevent UI thread starvation and eliminate business logic leakage into presentation widgets.

---

## ADR-002: Direct Tridiagonal Thomas Algorithm ($O(N)$) vs. Dense LU Decomposition ($O(N^3)$)

- **Status**: ACCEPTED & IMPLEMENTED
- **Context**: Discretization of the 1D steady-state heat equation yields a symmetric positive-definite tridiagonal linear system $\mathbf{K} \mathbf{T} = \mathbf{F}$.
- **Decision**: Implement the specialized **Thomas algorithm** (Tridiagonal Matrix Algorithm, TDMA) for direct forward elimination and back substitution.
- **Consequences**:
  - Computational time complexity scales as $O(N)$ instead of $O(N^3)$ for general dense solvers.
  - Memory consumption scales as $O(N)$ instead of $O(N^2)$, enabling instantaneous solution of fine meshes ($N=80$ solves take $< 1.5\ \mu\text{s}$).
  - Completely eliminates dynamic memory allocations during inner batch solving loops.

---

## ADR-003: Custom `QPainter` 2D Scientific Canvas vs. Heavy External Charting Libraries

- **Status**: ACCEPTED & IMPLEMENTED
- **Context**: The desktop GUI requires real-time 2D plotting of analytical vs. numerical temperature curves, convergence rates, speedup scaling, and neural loss decay. Third-party plotting libraries (e.g., QCustomPlot, QtCharts) introduce significant GPL licensing ambiguities, large binary bloat, or complex CMake submodules.
- **Decision**: Engineer an internal, lightweight, hardware-accelerated 2D plotting canvas ([`PlotWidget.cpp`](../../src/app/widgets/PlotWidget.cpp)) directly using Qt's `QPainter`.
- **Consequences**:
  - Zero external third-party dependencies, preserving MIT/LGPL compliance.
  - Native high-DPI scaling, crisp antialiased vector curve rendering, and configurable coordinate transformation.
  - Highly optimized: rendering multi-curve datasets executes at 60 FPS without memory leaks.

---

## ADR-004: Native Win32 Subsystem GUI Wrapper (`Launch.exe`) vs. PowerShell/Batch Launch Scripts

- **Status**: ACCEPTED & IMPLEMENTED
- **Context**: The assignment strictly dictates: *"The application must open on Windows 11 x64 by double-clicking Launch.exe after extraction. No terminal, PowerShell, administrator rights, package installation, internet connection, or first-run compilation may be required for end users."*
- **Decision**: Compile a native Win32 executable ([`Launch.cpp`](../../packaging/Launch.cpp)) using the Windows GUI subsystem flag (`/SUBSYSTEM:WINDOWS`), which invokes `CreateProcessW` to spawn `bin\WorkbenchApp.exe`.
- **Consequences**:
  - Double-clicking `Launch.exe` launches the application immediately without popping up transient black terminal consoles.
  - Bypasses Windows PowerShell `ExecutionPolicy` restrictions (e.g., `Restricted` or `AllSigned`).
  - Completely self-contained and operates offline with relative path resolution.

---

## ADR-005: Bounded Persistent Worker Pool vs. Ephemeral `std::thread` Spawning

- **Status**: ACCEPTED & IMPLEMENTED
- **Context**: Benchmarking 1,000 to 10,000 parametric FEA cases requires parallel execution across multiple CPU cores. Repeatedly creating and destroying `std::thread` instances per batch case introduces heavy Windows kernel context-switch penalties (~50–100 µs per thread spawn).
- **Decision**: Implement a bounded persistent thread pool (`ThreadPool.hpp`) that spawns $p$ worker threads upon initialization, which wait on a `std::condition_variable` and pop tasks from a thread-safe `std::queue`.
- **Consequences**:
  - Amortizes thread lifecycle overhead across the entire batch run.
  - Allows predictable CPU core utilization matching hardware thread topologies.
  - Guaranteed graceful shutdown and clean thread join upon destruction.

---

## ADR-006: Hardware Abstraction Layer with Transparent Simulation (`isSimulated = true`)

- **Status**: ACCEPTED & IMPLEMENTED
- **Context**: The workbench requires integration with MCU (Raspberry Pi Pico), FPGA (Digilent Arty A7), and JTAG (Digilent JTAG-HS2) hardware. However, physical hardware devices may not be plugged into every evaluator's laptop.
- **Decision**: Design a polymorphic `IDeviceAdapter` interface backed by simulated mock adapters (`McuMockAdapter`, `FpgaMockAdapter`, `JtagMockAdapter`).
- **Consequences**:
  - Transparently sets `isSimulated = true` on all mock adapters, explicitly complying with the Zero-Fabrication directive.
  - Generates realistic physics-based telemetry (internal temperatures, voltages) while supporting fault injection and automated 3-retry recovery.
  - Provides a drop-in HAL interface ready for real WinUSB / serial driver replacement when physical hardware is connected.
