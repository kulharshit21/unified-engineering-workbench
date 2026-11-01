# Engineering Failures and Fixes Ledger

**Policy**: Real engineering failure log. Preserve past failures, root causes, fixes, and regression prevention.

---

## Failure & Defect Ledger

| Defect ID | Symptom | Reproduction Steps | Root Cause | Fix Applied | Validation & Regression Test | Prevention Measure |
| :--- | :--- | :--- | :--- | :--- | :--- | :--- |
| **DEF-001** | `cl.exe` not found on standard PowerShell path. | Ran `where.exe cl` in default PowerShell session; returned code 1. | Visual Studio Developer Command Prompt environment variables are not loaded by default in standard PowerShell. | Sourced `vcvars64.bat` from `C:\Program Files\Microsoft Visual Studio\18\Community\VC\Auxiliary\Build\vcvars64.bat` before invoking CMake/Ninja. | Ran test compilation of MSVC x64 tools; exit code 0. | Automate environment detection in `build.ps1` and CMake presets. |
| **DEF-002** | No Qt 6 SDK present on machine for C++ development. | Searched drive C and D for `qmake.exe` and dev headers; only third-party application runtimes found. | Machine had no pre-installed Qt developer SDK. | Used `aqtinstall` to automatically pull and extract official Qt 6.8.2 MSVC 2022 x64 into `C:\Qt\6.8.2\msvc2022_64`. | Executed `qmake.exe -v` and verified Qt 6.8.2. | Document `aqtinstall` in `docs/TOOLCHAIN.md` and provide one-click script. |
| **DEF-003** | Potential false zero error in 1D FEA convergence calculation. | Evaluated error at nodes $(x_0, x_1, \dots, x_N)$. | 1D linear elements with constant $k$ have the Superconvergence property: nodal temperatures are exact! Error at nodes is identically zero to machine precision ($< 10^{-14}$). | Switched error evaluation to element midpoints $x_\text{mid} = \frac{x_i + x_{i+1}}{2}$ using FE linear shape function interpolation $T_\text{FE} = 0.5 T_i + 0.5 T_{i+1}$. | Verified expected quadratic convergence ($1.25^\circ\text{C}$ for $N=10$, $0.3125^\circ\text{C}$ for $N=20$, etc.). | Enforced midpoint check in `test_thermal_fea.cpp`. |
