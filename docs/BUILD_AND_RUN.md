# PASS Science Works — Build and Execution Guide

This document provides developer instructions for building, running, testing, and packaging the **Unified Engineering Workbench** on Windows 11 x64.

---

## 1. Prerequisites and Environment Setup

### 1.1 Required Toolchains
- **Operating System**: Windows 11 x64 (Build 22000+)
- **Compiler**: Visual Studio 2022 or 2026 Developer Command Prompt (`cl.exe` 19.30+ with C++17 support)
- **Build System**: CMake 3.20+ and Ninja (available on PATH)
- **GUI Framework**: Qt 6.8.2 MSVC 2022 x64 (installed at `C:\Qt\6.8.2\msvc2022_64` or on `CMAKE_PREFIX_PATH`)
- **Version Control**: Git 2.30+

---

## 2. One-Click Command Line Scripts

The repository includes pre-configured, tested batch scripts located in `scripts/`:

### 2.1 Build Release Binaries
```cmd
scripts\build_msvc.cmd Release
```
This automatically invokes `vcvars64.bat`, configures CMake using the Ninja generator with `/std:c++17 /W4 /O2`, and compiles all targets:
- `WorkbenchCore.lib`: Static numerical engine.
- `WorkbenchApp.exe`: Desktop GUI application.
- `Launch.exe`: Zero-dependency native Win32 starter.
- 7 automated test executables in `bin/`.

### 2.2 Run Automated CTest Suite
```cmd
scripts\test_msvc.cmd Release
```
Executes all 7 test suites via CTest with detailed output.

### 2.3 Package Standalone Distribution
```cmd
scripts\package_msvc.cmd CandidateID
```
Bundles the application using `windeployqt`, includes VC CRT DLLs, data, docs, results, Git commit bundle, generates `CandidateID_Workbench.zip` in `release/`, and outputs `CandidateID_Workbench.sha256`.

---

## 3. CMake Presets Support

The workbench includes standard `CMakePresets.json`:

```cmd
:: Configure and build using presets
cmake --preset msvc-x64-release
cmake --build --preset msvc-x64-release
ctest --preset msvc-x64-release
```

---

## 4. One-Click IDE Execution (VS Code / Antigravity)

The repository provides `.vscode/launch.json` and `.vscode/tasks.json`:
1. Open the project folder in VS Code / Antigravity.
2. Press **Ctrl+Shift+B** to execute the default `Build Workbench (MSVC Release)` task.
3. Press **F5** to start debugging `Launch.exe` or `WorkbenchApp.exe`.
