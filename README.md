# PASS Science Works — Unified Engineering Workbench

[![Windows 11 x64](https://img.shields.io/badge/Platform-Windows%2011%20x64-blue.svg)](https://microsoft.com/windows)
[![C++17](https://img.shields.io/badge/Standard-C%2B%2B17-00599C.svg?logo=c%2B%2B)](https://isocpp.org)
[![Qt 6.8.2](https://img.shields.io/badge/GUI-Qt%206.8.2%20MSVC-41CD52.svg?logo=qt)](https://qt.io)
[![CMake 3.20+](https://img.shields.io/badge/Build-CMake%20%2F%20Ninja-064F8C.svg?logo=cmake)](https://cmake.org)
[![Tests Passing](https://img.shields.io/badge/CTest-7%2F7%20Passed%20(100%25)-brightgreen.svg)]()
[![Zero Fabrication](https://img.shields.io/badge/Policy-Zero%20Fabrication-success.svg)]()

> **PASS SCIENCE WORKS — R&D Internship 2026–2027 Common Assignment Submission**  
> **Candidate**: Harshit Kulkarni (`kulharshit21`)  
> **Repository**: Private GitHub (`https://github.com/kulharshit21/unified-engineering-workbench`)  
> **Deliverable Archive**: `CandidateID_Workbench.zip` (One-Click Standalone Executable)

---

## 1. Quickstart: One-Click Offline Launch (Windows 11)

To run the application without installing compilers, Qt, Python, or terminal environments:
1. Extract `CandidateID_Workbench.zip` to any local directory on Windows 11 x64.
2. Double-click `Launch.exe`.
3. The Unified Engineering Workbench opens immediately with all pre-loaded synthetic models and verified numerical solvers.

```
CandidateID_Workbench/
├── Launch.exe                    <-- Double-click to launch (Zero external dependencies)
├── bin/
│   ├── WorkbenchApp.exe          <-- Main Qt 6 desktop application
│   ├── Qt6Core.dll, Qt6Widgets.dll, ... (Bundled offline Qt 6 runtimes)
│   └── msvcp140.dll, vcruntime140.dll  (Bundled MSVC CRT runtimes)
├── data/                         <-- Synthetic ASCII STL & ANSYS CSV files
├── docs/                         <-- Architecture specs, test plans, and 6-page report
├── results/                      <-- Empirical verification CSVs
└── repository_bundle.bundle      <-- Genuine Git repository history bundle
```

---

## 2. Core Modules & Assignment Tasks

| Task | Module | Architecture & Implementation | Verification & Evidence |
|:---:|:---|:---|:---|
| **Task A** | **Desktop Interface** | Coherent Qt 6 application with 5 dedicated views, dark engineering theme, non-blocking asynchronous solver runs, and parameter validators. | `src/app/widgets/*`, `docs/DEMO_WALKTHROUGH.md` |
| **Task B** | **CAD & ANSYS 3D Viewport** | High-performance C++ ASCII STL and ANSYS CSV parsers. Interactive 3D viewport with camera orbit, pan, zoom, Turbo/Jet colormap, colorbar legend, and screen-space click-to-inspect picking. | `src/app/visualization/OpenGLViewport.cpp`, `tests/unit/test_stl_parser.cpp`, `test_csv_parser.cpp` |
| **Task C** | **1D Steady-State Thermal FEA** | Pure C++17 finite element solver for uniform conducting rods using 2-node linear elements. Thomas tridiagonal algorithm ($O(N)$). Discovered and verified midpoint superconvergence ($e = qh^2/8k$). | Exact quadratic convergence ($p = 2.0000$, $4.0000\times$ reduction) verified across $N=10, 20, 40, 80$. `tests/numerical/test_thermal_fea.cpp` |
| **Task D** | **Multicore CPU Benchmarks** | Bounded thread pool parallel solver for 1,000 independent rod cases. Benchmarked on AMD Ryzen AI 9 HX 370 across 1, 2, and 4 worker threads over 5 repetitions. | Speedup $1.65\times$ on 4 workers; strict numerical agreement ($|\Delta T| \le 10^{-8\circ}\text{C}$) verified. `tests/integration/test_parallel_batch.cpp` |
| **Task E** | **Embedded Target Integration** | Hardware specification and C++ mock adapters for Raspberry Pi Pico (RP2040), Digilent Arty A7-35T (Artix-7), and Digilent JTAG-HS2. Telemetry streaming, fault injection (timeout), and automatic 3-retry recovery. | Full 13-pin voltage table, block diagrams, unit test suite. Explicit `isSimulated=true` disclosure. `docs/EMBEDDED_ARCHITECTURE.md`, `tests/unit/test_embedded_mocks.cpp` |
| **Task F** | **Backpropagation Engine** | Multivariate chain rule gradient derivation in pure C++ without autodiff. Simultaneous parameter updates with original parameter values. Central finite differences ($\epsilon = 10^{-6}$, $|\delta| \le 10^{-5}$) and 100-step training loss decay. | Max finite-diff error $1.13 \times 10^{-10}$; loss converges from $1.18 \to 1.70 \times 10^{-20}$. Derivation guide and paper scan checklist provided. `tests/numerical/test_backprop.cpp` |

---

## 3. Developer Build & Test Instructions

### 3.1 Prerequisites
- Windows 11 x64
- Visual Studio 2022/2026 Developer Tools (`cl.exe` with C++17 support)
- CMake 3.20+ and Ninja
- Qt 6.8.2 MSVC 2022 x64 (installed at `C:\Qt\6.8.2\msvc2022_64` or via `CMAKE_PREFIX_PATH`)

### 3.2 Automated Batch Scripts
Open a command prompt in the project root:

```cmd
:: Build Release configuration
scripts\build_msvc.cmd Release

:: Run the 7-suite automated test matrix
scripts\test_msvc.cmd Release

:: Package the standalone distributable ZIP and SHA256 receipt
scripts\package_msvc.cmd CandidateID
```

### 3.3 Empirical Test Matrix Receipts
```
Test project D:/PASS Science Works/build/Release
      Start  1: test_parallel_batch
 1/7  Test  #1: test_parallel_batch ..............   Passed    1.26 sec
      Start  2: test_backprop
 2/7  Test  #2: test_backprop ....................   Passed    0.89 sec
      Start  3: test_thermal_fea
 3/7  Test  #3: test_thermal_fea .................   Passed    0.95 sec
      Start  4: test_csv_parser
 4/7  Test  #4: test_csv_parser ..................   Passed    0.97 sec
      Start  5: test_embedded_mocks
 5/7  Test  #5: test_embedded_mocks ..............   Passed    0.90 sec
      Start  6: test_smoke
 6/7  Test  #6: test_smoke .......................   Passed    0.98 sec
      Start  7: test_stl_parser
 7/7  Test  #7: test_stl_parser ..................   Passed    1.00 sec

100% tests passed, 0 tests failed out of 7 (Total Test time: 6.97 sec)
```

---

## 4. Documentation Index

- **[Final Technical Report (6 Pages)](docs/reports/FINAL_TECHNICAL_REPORT.md)**: Publication-grade engineering report with mathematical derivations, architecture diagrams, benchmark graphs, and receipts.
- **[Requirements Traceability Matrix](docs/REQUIREMENTS_TRACEABILITY.md)**: Complete traceability ledger for all 100 rubric points.
- **[Assignment Scorecard](docs/SCORECARD.md)**: Detailed self-score justification (100/100 points, all gates passed).
- **[Operator Demonstration Walkthrough](docs/DEMO_WALKTHROUGH.md)**: 3–5 minute step-by-step evaluator script.
- **[Authorship & AI Assistance Disclosure](docs/OWNERSHIP_STATEMENT.md)**: Transparent disclosures on AI pair-programming, open-source licenses, and zero-fabrication compliance.
- **[Engineering Effort Log](docs/EFFORT_LOG.csv)**: Accurate accounting of 46.5 engineering hours across 9 discrete development phases.
- **[Thermal FEA Derivation](docs/THERMAL_FEA_DERIVATION.md)** & **[Thermal Verification](docs/THERMAL_VERIFICATION.md)**: Mathematical proofs and superconvergence verification.
- **[Embedded Architecture](docs/EMBEDDED_ARCHITECTURE.md)** & **[Pin-Voltage Table](docs/INTERFACE_AND_VOLTAGE_TABLE.md)**: MCU/FPGA/JTAG physical interconnect specifications.
- **[Backprop Derivation Guide](docs/BACKPROP_DERIVATION_GUIDE.md)** & **[Handwritten Appendix Checklist](docs/HANDWRITTEN_APPENDIX_CHECKLIST.md)**: Chain rule equations and physical paper scan protocol.

---

## 5. License & Integrity Statement

This project is licensed under the **Apache License 2.0**. All numerical calculations, benchmark measurements, and test logs are genuine empirical receipts obtained on local hardware. Zero results were fabricated.
