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
