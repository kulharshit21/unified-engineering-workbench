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
