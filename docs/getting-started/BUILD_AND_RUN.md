# Build, Run & Deployment Guide

This guide provides step-by-step instructions for building, testing, running, and packaging the **PASS SCIENCE WORKS Unified Engineering Workbench** on Windows 11 x64.

---

## 1. System Requirements & Toolchain Prerequisites

To build from source, the workstation must have:
- **Operating System**: Windows 11 x64 (Build 22000+)
- **C++ Compiler**: Microsoft Visual Studio 2026 / 2022 C++ Build Tools (MSVC 19.30+ / 19.51+) supporting ISO C++17
- **Build System**: CMake 3.25+ (CMake 4.2.3 verified) and Ninja 1.12+ (Ninja 1.13 verified)
- **GUI Framework**: Qt 6.8.2 MSVC 2022 x64 (`QtCore`, `QtGui`, `QtWidgets`, `QtOpenGL`, `QtOpenGLWidgets`)
- **Python (Optional, for documentation & PDF export only)**: Python 3.10+ with `matplotlib`, `numpy`, and `pypdf`

### 1.1 Automated Non-Admin Qt 6 Installation
If Qt 6.8.2 is not installed, install it without administrator rights via `aqtinstall`:
```powershell
pip install aqtinstall
python -m aqt install-qt windows desktop 6.8.2 win64_msvc2022_64 -O C:\Qt
```

---

## 2. One-Click Command Pipeline

Automated batch wrappers in `scripts/` initialize the 64-bit MSVC environment (`vcvars64.bat`) automatically and execute all pipeline tasks:

### 2.1 Compile Release Build
```cmd
scripts\build_msvc.cmd Release
```
This configures CMake using Ninja and compiles `lib/WorkbenchCore.lib`, `bin/WorkbenchApp.exe`, `Launch.exe`, and all 7 standalone test binaries.

### 2.2 Run Automated CTest Suite
```cmd
scripts\test_msvc.cmd Release
```
Executes all 7 test suites via CTest with `--output-on-failure`. Expected duration is ~7 seconds.

### 2.3 Package Standalone Distribution Archive
```cmd
scripts\package_msvc.cmd CandidateID
```
This automated pipeline:
1. Deploys required Qt shared libraries (`Qt6Core.dll`, `Qt6Gui.dll`, `platforms/qwindows.dll`, etc.) via `windeployqt`.
2. Deploys Microsoft Visual C++ CRT Redistributable DLLs (`vcruntime140.dll`, `msvcp140.dll`).
3. Packages source code, test suites, CSV outputs, and documentation.
4. Generates an offline Git history bundle (`repository_bundle.bundle`).
5. Compresses the entire tree into `release/CandidateID_Workbench.zip`.
6. Computes and saves the SHA-256 hash into `release/CandidateID_Workbench.sha256`.

---

## 3. Standalone End-User Launch Instructions

For end users and evaluators who do not wish to compile from source:
1. Extract `release/CandidateID_Workbench.zip` into any directory on Windows 11 x64.
2. Double-click [`Launch.exe`](file:///D:/PASS%20Science%20Works/dist/UnifiedEngineeringWorkbench/Launch.exe).
3. The application opens immediately. **No command prompt window appears, no administrator rights are requested, and no Internet connection is required.**

---

## 4. CMake Presets for IDEs (VS Code / Visual Studio)

The repository provides [`CMakePresets.json`](file:///D:/PASS%20Science%20Works/CMakePresets.json):
```powershell
# Configure using preset
cmake --preset msvc-release

# Build using preset
cmake --build --preset msvc-release

# Test using preset
ctest --preset msvc-release
```
In VS Code, press `Ctrl+Shift+B` to trigger the default build task configured in [`.vscode/tasks.json`](file:///D:/PASS%20Science%20Works/.vscode/tasks.json).
