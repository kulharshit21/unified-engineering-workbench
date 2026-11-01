# Engineering Development Log (Continuous DevLog)

**Project**: PASS Science Works — Unified Engineering Workbench  
**Format**: Structured chronological milestone records adhering to the `engineering-evidence` schema.

---

## Milestone 0: Environment Reconnaissance, Toolchain Provisioning & Requirements Matrix
* **Timestamp**: 2026-11-01 19:40 IST
* **Phase**: Phase 0 — Reconnaissance & Setup
* **Objective**: Ingest assignment PDF, audit laptop hardware, provision C++17 compiler and Qt 6.8.2 toolchains, establish git repository, and map all 100-point rubric requirements.
* **Files Created / Modified**:
  * `.gitignore`, `.gitattributes`, `LICENSE`
  * `docs/HARDWARE_AUDIT.md`, `docs/TOOLCHAIN.md`
  * `docs/REQUIREMENTS_TRACEABILITY.md`, `docs/RISK_REGISTER.md`
  * `docs/ARCHITECTURE_DECISIONS.md`, `docs/DEPENDENCIES_AND_LICENSES.md`
  * `docs/AI_USE_LOG.md`, `docs/FAILURES_AND_FIXES.md`, `docs/devlog/DEVLOG.md`
* **Architecture Decisions**:
  * C++17 core library decoupled from Qt GUI (`WorkbenchCore` vs `WorkbenchApp`).
  * Pure Thomas algorithm tridiagonal solver for 1D thermal FEA.
  * OpenGL 3.3 Core for 3D STL & CSV point clouds.
* **Execution Commands**:
  * `git init -b main`
  * `python -m pip install aqtinstall`
  * `aqt install-qt windows desktop 6.8.2 win64_msvc2022_64 -O C:\Qt`
* **Real Test Results**:
  * Git initialized: clean working tree.
  * Qt 6.8.2 verified: `QMake version 3.1, Using Qt version 6.8.2 in C:/Qt/6.8.2/msvc2022_64/lib`.
  * Visual Studio 2026 / MSVC 19.51 Developer Command Prompt verified.
* **Successes**: Complete toolchain ready in < 5 minutes without administrative escalation.
* **Failures & Fixes**: Sourced `vcvars64.bat` to resolve missing `cl.exe` in standard PowerShell session (see `DEF-001`).
* **Next Step**: Create Phase 1 minimal C++/Qt application, root `Launch.exe` wrapper, and build scripts.

---

## Milestone 1: Minimal Application, Build Automation & Standalone Packaging Pipeline
* **Timestamp**: 2026-11-01 19:55 IST
* **Phase**: Phase 1 — Minimal Application + Release Skeleton
* **Objective**: Establish CMake architecture, pure C++ core library target, Qt 6 Widgets GUI target, CTest runner, native Win32 `Launch.exe` wrapper, build/test/package scripts, and verify standalone execution.
* **Files Created / Modified**:
  * `CMakeLists.txt`, `CMakePresets.json`
  * `src/core/CoreVersion.hpp`
  * `src/app/main.cpp`, `src/app/widgets/MainWindow.hpp`, `src/app/widgets/MainWindow.cpp`
  * `packaging/Launch.cpp`
  * `tests/unit/test_smoke.cpp`
  * `scripts/build_msvc.cmd`, `scripts/test_msvc.cmd`, `scripts/package_msvc.cmd`
  * `.vscode/launch.json`, `.vscode/tasks.json`
* **Architecture Decisions**:
  * Native Win32 `Launch.exe` with zero runtime dependencies delegates to `bin/WorkbenchApp.exe` without spawning terminal window.
  * Native CMD scripts for rock-solid batch execution across developer command prompts.
* **Execution Commands**:
  * `cmd.exe /c ".\scripts\build_msvc.cmd Release"`
  * `cmd.exe /c ".\scripts\test_msvc.cmd Release"`
  * `cmd.exe /c ".\scripts\package_msvc.cmd CandidateID"`
* **Real Test Results**:
  * Build succeeded in 9.2s using MSVC 19.51 and Ninja (16 compilation units).
  * Smoke test passed via CTest: 100% tests passed.
  * Packaging created `dist/UnifiedEngineeringWorkbench` (28.98 MB compressed ZIP with SHA256 checksum).
  * Standalone launch smoke test passed: `Launch.exe` spawned `WorkbenchApp.exe` (verified PID 19164) from outside build tree.
* **Next Step**: Implement Task C — Verified 1D Steady-State Thermal Finite-Element Solver with Thomas algorithm, exact analytical benchmark, and midpoint mesh convergence.

---

## Milestone 2: Verified 1D Steady-State Thermal Finite-Element Solver & Convergence
* **Timestamp**: 2026-11-01 20:00 IST
* **Phase**: Phase 2 — Numerical Core (Task C)
* **Objective**: Implement steady-state 1D thermal FEA solver for uniform rod using 2-node linear elements, Thomas algorithm tridiagonal solver, Dirichlet boundary imposition, analytical comparison, and midpoint convergence verification ($N=10, 20, 40, 80$).
* **Files Created / Modified**:
  * `src/core/thermal/ThermalParameters.hpp`
  * `src/core/thermal/ThermalFeaSolver.hpp`, `src/core/thermal/ThermalFeaSolver.cpp`
  * `src/core/thermal/ThermalConvergence.hpp`, `src/core/thermal/ThermalConvergence.cpp`
  * `tests/numerical/test_thermal_fea.cpp`
  * `docs/THERMAL_FEA_DERIVATION.md`, `docs/THERMAL_VERIFICATION.md`
  * `results/thermal/convergence.csv`, `results/thermal/temperature_profiles.csv`
* **Architecture Decisions**:
  * Decoupled pure C++17 thermal solver from GUI.
  * $O(N)$ Thomas algorithm directly solves the assembled tridiagonal system after Dirichlet elimination.
  * Midpoint interpolation evaluates true quadratic discretization error ($e = \frac{q}{8k} h^2$) rather than superficial nodal zero error.
* **Execution Commands**:
  * `cmd.exe /c ".\scripts\build_msvc.cmd Release"`
  * `cmd.exe /c ".\scripts\test_msvc.cmd Release"`
* **Real Test Results**:
  * $N=10 \implies e_\text{mid} = 1.25000000^\circ\text{C}$ (Expected: $1.25000000^\circ\text{C}$)
  * $N=20 \implies e_\text{mid} = 0.31250000^\circ\text{C}$ (Expected: $0.31250000^\circ\text{C}$)
  * $N=40 \implies e_\text{mid} = 0.07812500^\circ\text{C}$ (Expected: $0.07812500^\circ\text{C}$)
  * $N=80 \implies e_\text{mid} = 0.01953125^\circ\text{C}$ (Expected: $0.01953125^\circ\text{C}$)
  * Observed error reduction ratio: exactly $4.0000$, convergence order $p = 2.0000$.
  * Boundary values preserved exactly: $T(0) = 100.0^\circ\text{C}, T(L) = 20.0^\circ\text{C}$.
  * Both automated CTest targets (`test_smoke`, `test_thermal_fea`) passed 100%.
* **Next Step**: Implement Task B — ASCII STL geometry parser and ANSYS nodal temperature CSV parser with unit tests and synthetic test datasets.

---

## Milestone 3: ASCII STL & ANSYS Temperature CSV Parsers with Negative Test Suite
* **Timestamp**: 2026-11-01 20:05 IST
* **Phase**: Phase 3 — File Import Core (Task B)
* **Objective**: Implement robust C++17 parsers for ASCII STL geometry and ANSYS CSV nodal temperatures (`node_id,x_m,y_m,z_m,temperature_C`), including bounding box computation, non-finite checks (NaN/Inf), empty file handling, binary STL detection, duplicate node-ID resolution, and synthetic sample data generation.
* **Files Created / Modified**:
  * `src/core/geometry/GeometryTypes.hpp`
  * `src/core/geometry/StlParser.hpp`, `src/core/geometry/StlParser.cpp`
  * `src/core/geometry/TemperatureCsvParser.hpp`, `src/core/geometry/TemperatureCsvParser.cpp`
  * `data/geometry/sample_bracket.stl`, `data/thermal/synthetic_ansys_temperatures.csv`
  * `data/README.md`, `docs/DATA_PROVENANCE.md`
  * `tests/unit/test_stl_parser.cpp`, `tests/unit/test_csv_parser.cpp`
* **Architecture Decisions**:
  * Pure standard library string parsing (`std::stod`, `std::stoi`, `std::isfinite`).
  * Explicit duplicate node-ID policy: Retain first occurrence, record duplicate count, log warning.
  * Degenerate range protection: $T_\text{min} == T_\text{max}$ handled without zero division.
* **Execution Commands**:
  * `cmd.exe /c ".\scripts\build_msvc.cmd Release"`
  * `cmd.exe /c ".\scripts\test_msvc.cmd Release"`
* **Real Test Results**:
  * Valid ASCII STL parsed: 12 triangles, max extent 0.1 m.
  * Negative STL tests passed: empty STL, missing 'solid', incomplete facet, and NaN vertex correctly rejected with descriptive exceptions.
  * Valid CSV parsed: 27 nodes, range $[20^\circ\text{C}, 100^\circ\text{C}]$, mean $57.6852^\circ\text{C}$.
  * Negative CSV tests passed: empty CSV, malformed header, non-numeric values, NaN values, duplicate IDs, and degenerate ranges correctly handled.
  * 4/4 CTest suites passed 100%.
* **Next Step**: Implement Task D — Multicore Parallel Batch Thermal Solves, Thread Pool, and Benchmarking Engine.

---

## Milestone 4: Multicore Parallel Batch Solves, ThreadPool & Benchmark Suite
* **Timestamp**: 2026-11-01 20:07 IST
* **Phase**: Phase 4 — Multicore Performance (Task D)
* **Objective**: Implement bounded `ThreadPool`, reproducible batch generator with fixed RNG seed ($k, q$ randomized), serial baseline solve, CPU-parallel batch solve, 5 repetitions, median timing, speedup $S_p$, efficiency $E_p$, and serial/parallel agreement verification ($\le 10^{-8\circ}\text{C}$).
* **Files Created / Modified**:
  * `src/core/parallel/ThreadPool.hpp`
  * `src/core/parallel/BatchCase.hpp`
  * `src/core/parallel/BatchSolver.hpp`, `src/core/parallel/BatchSolver.cpp`
  * `src/core/parallel/BenchmarkEngine.hpp`, `src/core/parallel/BenchmarkEngine.cpp`
  * `tests/integration/test_parallel_batch.cpp`
  * `docs/PERFORMANCE_METHODOLOGY.md`, `docs/PERFORMANCE_RESULTS.md`
  * `results/performance/raw_timings.csv`, `results/performance/summary.csv`, `results/performance/agreement.csv`
* **Architecture Decisions**:
  * Bounded worker pool using standard C++17 `std::thread`, mutex, condition variables.
  * Deterministic pre-allocated chunking to minimize lock contention.
  * Deterministic case output indexing strictly ordered by `caseId`.
* **Execution Commands**:
  * `cmd.exe /c ".\scripts\build_msvc.cmd Release"`
  * `cmd.exe /c ".\scripts\test_msvc.cmd Release"`
* **Real Test Results**:
  * Workload: 1,000 independent cases, $N=80$ elements each, Seed = 42.
  * Measured Medians on Ryzen AI 9 HX 370:
    * 1 Worker: $1.8989\text{ ms}$ (Speedup $1.00\times$, Efficiency $1.00$)
    * 2 Workers: $1.5447\text{ ms}$ (Speedup $1.23\times$, Efficiency $0.61$)
    * 4 Workers: $0.8940\text{ ms}$ (Speedup $2.12\times$, Efficiency $0.53$)
  * Numerical Agreement: Max absolute delta between serial and 4-worker parallel is $0.0000\times 10^{-15\circ}\text{C}$ (well below $10^{-8\circ}\text{C}$ threshold).
  * 5/5 automated CTest targets passed 100%.
* **Next Step**: Implement Task E — Host-Connected Embedded System Integration Design and C++ Mock Device Adapters.


---

## Milestone 5: Embedded Integration Mocks, Architecture & Automatic Fault Recovery
* **Timestamp**: 2026-11-01 20:12 IST
* **Phase**: Phase 5 — Embedded Integration (Task E)
* **Objective**: Design complete host-connected embedded architecture for Raspberry Pi Pico (RP2040), Digilent Arty A7-35T (Artix-7), and Digilent JTAG-HS2 adapter; specify 13-pin voltage and interface table; implement C++ `IDeviceAdapter` and mock adapters with configurable telemetry, timeout fault injection, and automatic 3-retry recovery.
* **Files Created / Modified**:
  * `src/core/hardware/IDeviceAdapter.hpp`
  * `src/core/hardware/McuMockAdapter.hpp`, `src/core/hardware/McuMockAdapter.cpp`
  * `src/core/hardware/FpgaMockAdapter.hpp`, `src/core/hardware/FpgaMockAdapter.cpp`
  * `src/core/hardware/JtagMockAdapter.hpp`, `src/core/hardware/JtagMockAdapter.cpp`
  * `src/core/hardware/DeviceManager.hpp`, `src/core/hardware/DeviceManager.cpp`
  * `tests/unit/test_embedded_mocks.cpp`
  * `docs/EMBEDDED_ARCHITECTURE.md`, `docs/INTERFACE_AND_VOLTAGE_TABLE.md`, `docs/HARDWARE_SAFETY_AND_LIMITATIONS.md`
* **Architecture Decisions**:
  * `IDeviceAdapter` abstract base class isolates physical transport from user interface.
  * Explicit `isSimulated = true` flag prevents any false claims of physical hardware presence.
  * Structured 3-retry recovery sequence: flush FIFOs $\to$ pulse reset $\to$ re-synchronize baud framing $\to$ verify telemetry handshake.
* **Execution Commands**:
  * `cmd.exe /c ".\scripts\build_msvc.cmd Debug"`
  * `cmd.exe /c ".\scripts\test_msvc.cmd Debug"`
* **Real Test Results**:
  * All 3 mock adapters registered and retrieved by `deviceId`.
  * Simulated telemetry validated within physical bounds (Pico temp $32.4^\circ\text{C}$, supply $3.31\text{V}$; Arty A7 die temp $41.8^\circ\text{C}$, VCCINT $1.00\text{V}$; JTAG-HS2 VREF $3.31\text{V}$).
  * Fault injection triggered `ConnectionStatus::TimedOut` with invalid packet flag.
  * Recovery sequence cleanly re-synchronized communication, restoring `ConnectionStatus::Connected`.
  * 6/6 automated CTest targets passed 100%.
* **Next Step**: Implement Task F — Handwritten Backpropagation Engine, Analytical Gradients, and Central Finite-Difference Verification.

---

## Milestone 6: Neural Backpropagation Engine, Simultaneous Updates & Finite Differences
* **Timestamp**: 2026-11-01 20:15 IST
* **Phase**: Phase 6 — Neural Optimization (Task F)
* **Objective**: Implement single-neuron artificial neural network ($z_1 = wx+b$, $h=\tanh(z_1)$, $\hat{y}=vh+c$, $J=0.5(\hat{y}-y)^2$) in pure C++ without autodiff; evaluate exact analytical gradients via multivariate chain rule; enforce simultaneous parameter updates using evaluated original parameter values; verify via central finite differences ($\epsilon = 10^{-6}$, $|\delta| \le 10^{-5}$); run 100-step training; provide derivation guide and paper scan checklist.
* **Files Created / Modified**:
  * `src/core/neural/BackpropagationSolver.hpp`, `src/core/neural/BackpropagationSolver.cpp`
  * `tests/numerical/test_backprop.cpp`
  * `docs/BACKPROP_DERIVATION_GUIDE.md`, `docs/HANDWRITTEN_APPENDIX_CHECKLIST.md`
  * `results/backprop/loss_history.csv`, `results/backprop/gradient_verification.csv`
* **Architecture Decisions**:
  * Complete analytical derivation directly in pure C++ standard library.
  * All 4 parameter gradients ($w, b, v, c$) computed before updating any weight or bias.
  * Central difference formula with $O(\epsilon^2)$ truncation error used for rigorous assertion checking.
* **Execution Commands**:
  * `cmd.exe /c ".\scripts\build_msvc.cmd Debug"`
  * `.\build\Debug\bin\test_backprop.exe`
* **Real Test Results**:
  * Step 0 hand-calculated values verified: $z_1 = 0.5, h = 0.46211716, \hat{y} = 0.46211716, J = 1.1825418$.
  * Analytical gradients verified: $dJ/dw = -1.2094645, dJ/db = -1.2094645, dJ/dv = -0.7106820, dJ/dc = -1.5378828$.
  * Central finite differences: Max error across all 4 parameters is $1.13 \times 10^{-10}$ (exceeding $\le 10^{-5}$ threshold).
  * Monotonic loss decay: $J^{(0)} = 1.182542 \to J^{(10)} = 1.0211 \times 10^{-2} \to J^{(100)} = 1.7026 \times 10^{-20}$.
  * Final predicted output: $\hat{y} = 2.00000000$ (target $y = 2.0$).
* **Next Step**: Implement Task A & 3D Viewport — Integrated Qt 6 desktop interface with 5 dedicated views.

---

## Milestone 7: Integrated Qt 6 Desktop Interface, 3D Viewport & Custom Plotting Engine
* **Timestamp**: 2026-11-01 20:25 IST
* **Phase**: Phase 7 — Desktop UI & Viewport (Tasks A–F)
* **Objective**: Build unified desktop workstation in Qt 6 Widgets; implement custom hardware-accelerated 2D scientific plotting widget (`PlotWidget`); implement 3D orbital viewport with Lambertian lighting, Turbo/Jet scalar colormap, Cartesian tripod, temperature legend in °C, and screen-space click-to-inspect nearest-node picking (`OpenGLViewport`); build 5 dedicated views (`GeometryView`, `ThermalView`, `PerformanceView`, `HardwareView`, `BackpropView`); integrate into `MainWindow`.
* **Files Created / Modified**:
  * `src/app/widgets/PlotWidget.hpp`, `src/app/widgets/PlotWidget.cpp`
  * `src/app/visualization/OpenGLViewport.hpp`, `src/app/visualization/OpenGLViewport.cpp`
  * `src/app/widgets/GeometryView.hpp`, `src/app/widgets/GeometryView.cpp`
  * `src/app/widgets/ThermalView.hpp`, `src/app/widgets/ThermalView.cpp`
  * `src/app/widgets/PerformanceView.hpp`, `src/app/widgets/PerformanceView.cpp`
  * `src/app/widgets/HardwareView.hpp`, `src/app/widgets/HardwareView.cpp`
  * `src/app/widgets/BackpropView.hpp`, `src/app/widgets/BackpropView.cpp`
  * `src/app/widgets/MainWindow.hpp`, `src/app/widgets/MainWindow.cpp`
* **Architecture Decisions**:
  * Value-semantic data transfers between UI views and core solvers preserve complete separation.
  * Custom `PlotWidget` using `QPainter` with antialiasing avoids GPL charting dependencies.
  * Orbital camera with pitch clamping ($-89^\circ$ to $+89^\circ$) prevents gimbal lock.
* **Execution Commands**:
  * `cmd.exe /c ".\scripts\build_msvc.cmd Release"`
  * `cmd.exe /c ".\scripts\test_msvc.cmd Release"`
* **Real Test Results**:
  * Release build succeeded with zero warnings and zero errors under `/W4`.
  * All 7 automated CTest targets passed 100% in 6.97s.
  * Process alive test confirmed: `WorkbenchApp.exe` initialized, created GUI window, and ran smoothly.
* **Next Step**: Implement Phase 8 & 9 — Standalone Release Packaging, Comprehensive 6-page Technical Report, and Acceptance Audits.

---

## Milestone 8: One-Click Standalone Windows 11 Packaging & Deliverable Distribution
* **Timestamp**: 2026-11-01 20:30 IST
* **Phase**: Phase 8 — Packaging & Release Pipeline
* **Objective**: Bundle the Release binaries, Qt 6 runtime libraries (via `windeployqt`), MSVC CRT DLLs, native Win32 `Launch.exe` starter, synthetic CAD/ANSYS datasets, empirical verification results, complete documentation, and offline Git commit history bundle into `CandidateID_Workbench.zip`; generate SHA256 checksum; execute out-of-tree acceptance test.
* **Files Created / Modified**:
  * `scripts/package_msvc.cmd`
  * `release/CandidateID_Workbench.zip` (29.80 MB)
  * `release/CandidateID_Workbench.sha256`
  * `README.md`, `docs/PROJECT_OVERVIEW.md`, `docs/TEST_PLAN.md`, `docs/TEST_RESULTS.md`
  * `docs/DEMO_WALKTHROUGH.md`, `docs/OWNERSHIP_STATEMENT.md`, `docs/EFFORT_LOG.csv`, `docs/SCORECARD.md`
  * `docs/reports/FINAL_TECHNICAL_REPORT.md`
* **Real Test Results**:
  * `CandidateID_Workbench.zip` created ($29.80\text{ MB}$).
  * SHA256: `FD7036A55F1EE3C911D5253F54BEBD8E8CBB94C49C5992878D8D4E4F4D16FC2E`.
  * Out-of-tree standalone execution gate verified: `dist/UnifiedEngineeringWorkbench/Launch.exe` spawned `WorkbenchApp.exe` (PID 38688) cleanly without console or terminal prompt.
* **Next Step**: Phase 9 — Final Rubric Audit, Empirical Plot Rendering, PDF Export, and Milestone Closure.
