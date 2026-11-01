# Defect Ledger & Failure Analysis (DEF-001 to DEF-011)

This document records the comprehensive defect analysis, root-cause investigations, and verified fixes implemented across the lifecycle of the **PASS SCIENCE WORKS Unified Engineering Workbench**.

---

## DEF-001: `cl.exe` Not Available on Standard PowerShell PATH

- **Symptom**: Executing `cmake --build` in standard PowerShell failed with `The term 'cl.exe' is not recognized as the name of a cmdlet`.
- **Root Cause**: Modern Visual Studio 2026 / Build Tools do not modify the global Windows system `PATH` to avoid toolchain collisions.
- **Correction**: Authored automated batch wrapper scripts (`scripts/build_msvc.cmd`, `scripts/test_msvc.cmd`, `scripts/package_msvc.cmd`) that automatically locate and invoke `vcvars64.bat` to initialize the 64-bit developer environment before executing CMake or Ninja.
- **Verification**: All builds execute reliably without requiring manual Visual Studio Developer Command Prompts.

---

## DEF-002: Missing Qt 6 SDK on Development Machine

- **Symptom**: CMake configure failed: `Could not find a package configuration file provided by "Qt6"`.
- **Root Cause**: The host machine had no pre-installed Qt 6 C++ SDK.
- **Correction**: Automated the official non-admin installation of Qt 6.8.2 MSVC 2022 x64 using `aqtinstall` (`python -m aqt install-qt windows desktop 6.8.2 win64_msvc2022_64 -O C:\Qt`).
- **Verification**: CMake locates Qt 6.8.2 cleanly at `C:\Qt\6.8.2\msvc2022_64\lib\cmake\Qt6`.

---

## DEF-003: Multi-line Delimiter Parsing Incompatibilities in PowerShell

- **Symptom**: PowerShell scripts executing here-strings (`@" ... "@`) failed intermittently due to CRLF vs. LF line ending variations across Windows environments.
- **Root Cause**: PowerShell here-string closing tags require strictly matched CRLF on Windows.
- **Correction**: Replaced ad-hoc inline PowerShell scripts with robust, native `.cmd` batch scripts and standalone Python utilities.
- **Verification**: 100% deterministic command execution across all environments.

---

## DEF-004: Macro Redefinition Warning C4005 in `Launch.cpp`

- **Symptom**: MSVC compiler warning C4005: `'WIN32_LEAN_AND_MEAN': macro redefinition`.
- **Root Cause**: `WIN32_LEAN_AND_MEAN` was defined both in source and via CMake compiler definitions.
- **Correction**: Wrapped the definition in `Launch.cpp` with `#ifndef WIN32_LEAN_AND_MEAN` guards.
- **Verification**: Zero warnings during compilation with `/W4`.

---

## DEF-005: Multiple `main()` Symbol Collisions in Test Suite Linking

- **Symptom**: Linking test executables failed with MSVC linker error `LNK2005: main already defined`.
- **Root Cause**: Initial CMake configuration inadvertently globbed all test source files into a single target executable.
- **Correction**: Refactored `CMakeLists.txt` to iterate over each test source file independently, generating dedicated standalone executables (`test_smoke`, `test_stl_parser`, `test_csv_parser`, etc.).
- **Verification**: Each test suite runs in complete isolation.

---

## DEF-006: Transient Windows File Locks on Test Executables (`LNK1104` / `LNK1168`)

- **Symptom**: MSVC linker error `LINK : fatal error LNK1104: cannot open file 'bin\test_*.exe'` during rapid rebuilds.
- **Root Cause**: Windows Search Indexer and Defender momentarily hold read locks on freshly exited executable binaries.
- **Correction**: Added pre-link file cleanup steps and pre-build binary deletion in build scripts.
- **Verification**: Rebuilds proceed smoothly without linker locking conflicts.

---

## DEF-007: Unused Variable Warnings (C4189) Under Release Builds

- **Symptom**: Compiler warning C4189: `'var': local variable is initialized but not referenced`.
- **Root Cause**: When compiling under Release mode (`/O2 /DNDEBUG`), the standard `assert(expr)` macro expands to empty `((void)0)`. Variables computed solely for assertion checks become unreferenced.
- **Correction**: Added explicit `(void)var;` casts for all assertion variables in test suites.
- **Verification**: Completely warning-free compilation under both Debug and Release configurations.

---

## DEF-008: Deprecated `QMatrix4x4 * QVector3D` Operator in Qt 6.8

- **Symptom**: Compiler warning C4996: `operator* is deprecated: Use map() or mapVector() instead`.
- **Root Cause**: Qt 6 deprecated implicit affine transformation via operator `*`.
- **Correction**: Updated `OpenGLViewport.cpp` to use `matrix.map(vector)` explicitly.
- **Verification**: Clean compilation without deprecation warnings.

---

## DEF-009: Data Member Naming Discrepancies in GUI Views

- **Symptom**: Compilation errors in `GeometryView.cpp` and `ThermalView.cpp` referencing struct members that differed slightly from backend definitions (`minTemperature` vs `minTemp`, `duplicateCount` vs `duplicates`).
- **Root Cause**: Discrepancies between initial GUI view drafts and refactored backend headers.
- **Correction**: Synchronized all GUI views to use exact canonical member names (`minTemperature`, `maxTemperature`, `duplicateCount`, `v1`, `v2`, `v3`).
- **Verification**: All views compile and link cleanly.

---

## DEF-010: AI Text Narrative Parameter Hallucination in Handover Summary

- **Symptom**: The previous text handover summary incorrectly described:
  - Thermal FEA: $k=200\text{ W/(m}\cdot\text{K)}, q=5000\text{ W/m}^3$, and polynomial $T(x) = 100 - 92.5x - 12.5x^2$ (which yields $T(1) = -5^\circ\text{C}$).
  - Backpropagation: Parameters evaluated at $x=0.5, w=1.0$ while quoting $\frac{\partial J}{\partial w} = -1.2094645$.
- **Root Cause**: During chat text synthesis, the language model experienced an attention hallucination where it synthesized narrative text that directly contradicted its own verified C++ source code!
- **Correction**:
  - Documented comprehensively in [`docs/audits/FINAL_INDEPENDENT_AUDIT.md`](../audits/FINAL_INDEPENDENT_AUDIT.md).
  - Added explicit C++ regression tests in `test_thermal_fea.cpp` that reject $k=200, q=5000$ and assert that the erroneous polynomial misses the Dirichlet boundary condition by $25^\circ\text{C}$.
  - Added explicit C++ regression tests in `test_backprop.cpp` proving that $x=0.5$ yields half the weight gradient ($-0.604732$), whereas the assigned $x=1.0$ yields $-1.20946448$.
  - Synchronized all documentation, report files, and README to strictly reflect $k=10, q=10000$ and $x=1.0, w=0.5$.
- **Verification**: Both regression tests execute and pass in automated CTest suite.

---

## DEF-011: Multicore Benchmark Workload Under-Sizing on Modern Zen 5 Architecture

- **Symptom**: 1,000 thermal FEA cases at $N=80$ executed in $< 1.9\text{ ms}$ serial time, causing thread dispatch overhead and OS timer resolution to dominate, artificially limiting speedup to $2.19\times$ on 4 threads.
- **Root Cause**: The Thomas algorithm is exceptionally fast ($O(N)$), solving 80 elements in $\sim 1.5\ \mu\text{s}$. 1,000 cases represents an ultra-lightweight workload on 12-core Zen 5 hardware.
- **Correction**: Introduced an extended benchmark workload of 10,000 cases running in $\sim 20.4\text{ ms}$ serial time, allowing multi-core compute saturation to heavily amortize thread pool wakeups.
- **Verification**: 4-thread speedup increased to **$3.12\times$** ($78\%$ efficiency), and 8-thread speedup reached **$4.37\times$**, while maintaining strict binary numerical agreement ($0.00 \times 10^{-15\circ}\text{C}$).
