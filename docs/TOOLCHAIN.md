# Software Toolchain and Build Configuration

**Date**: November 1, 2026  
**Project**: PASS SCIENCE WORKS — Unified Engineering Workbench  
**Build System**: CMake 4.2.3 with Ninja multi-threaded build generator  
**Compiler**: Microsoft Visual C++ (MSVC) 19.51 (Visual Studio 2026 Developer Tools v18.7.2)  
**Target Platform**: Windows 11 x64 (AMD64)  
**GUI & 3D Framework**: Qt 6.8.2 LTS (MSVC 2022 x64 ABI compatible) with `Qt6Widgets`, `Qt6OpenGL`, `Qt6OpenGLWidgets`

---

## 1. Verified Toolchain Component Table

| Tool | Verified Version | Absolute Executable Path / Environment | Purpose |
| :--- | :--- | :--- | :--- |
| **C++ Compiler** | `cl.exe` (19.51.36231 x64) | `C:\Program Files\Microsoft Visual Studio\18\Community\VC\Tools\MSVC\14.51.36231\bin\Hostx64\x64\cl.exe` | C++17/20 compilation, optimization, code generation |
| **C++ Linker** | `link.exe` (14.51.36231) | MSVC 14.51 Linker | Target PE32+ executable creation and runtime linking |
| **Build Generator** | CMake `4.2.3` | `C:\Users\kulha\AppData\Local\Programs\Python\Python310\Scripts\cmake.exe` | Cross-platform build target configuration |
| **Build Driver** | Ninja `1.13.0` | `C:\Users\kulha\AppData\Local\Programs\Python\Python310\Scripts\ninja.exe` | Ultra-fast parallel compilation graph execution |
| **Qt Framework** | Qt `6.8.2` (Commercial / LGPLv3) | `C:\Qt\6.8.2\msvc2022_64` | Native desktop UI, 3D OpenGL viewport, signal/slot concurrency |
| **Deployment Utility**| `windeployqt.exe` | `C:\Qt\6.8.2\msvc2022_64\bin\windeployqt.exe` | Automated collection of Qt platform plugins and shared DLLs |
| **VCS & Hub** | Git `2.54.0` & GitHub CLI `gh` | `C:\Program Files\Git\cmd\git.exe`, `C:\Program Files\GitHub CLI\gh.exe` | Source revision tracking and private repository hosting |
| **Test Framework** | CTest & Custom C++ Test Runner | CMake CTest runner | Automated regression, unit, and numerical verification testing |

---

## 2. Compilation and Optimization Flags

* **C++ Standard**: C++17 (`/std:c++17` on MSVC, `-std=c++17` on Clang/GCC)
* **Release Optimization**: `/O2 /Oi /Ot /GL /MD` (Full speed optimization, intrinsic functions, fast floating-point code, multithreaded dynamic runtime)
* **Debug Diagnostics**: `/Zi /Od /RTC1 /MDd`
* **Warning Level**: `/W4 /WX- /permissive-` (Strict standards compliance, high diagnostic warning level)
* **Math Model**: Standard IEEE 754 precision (`/fp:precise`) to ensure numerical determinism across runs and exact adherence to analytical verification tolerances.
