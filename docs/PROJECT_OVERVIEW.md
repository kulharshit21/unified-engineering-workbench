# PASS Science Works — Unified Engineering Workbench
# Architectural Overview & Technical Specification

## 1. Executive Mission and Architecture

The **Unified Engineering Workbench** is a professional, high-performance, offline-first engineering workstation application developed for the PASS SCIENCE WORKS R&D Common Internship Assignment (2026–2027).

It unites multi-physics numerical solvers, finite element analysis (FEA), 3D CAD/ANSYS visualization, multicore parallel computing, embedded target coprocessor integration, and gradient-based neural network optimization into a coherent C++17 architecture with an intuitive Qt 6 desktop interface.

```
+--------------------------------------------------------------------------------------------------+
|                                    DESKTOP USER INTERFACE LAYER                                  |
|                                         (Qt 6.8.2 Widgets)                                       |
|  +--------------------+---------------------+--------------------+-------------------+---------+ |
|  | 1. CAD / ANSYS 3D  | 2. Thermal 1D FEA   | 3. Parallel Compute| 4. Embedded Mocks | 5. Back-| |
|  |    Viewport View   |    Convergence View |    Benchmark View  |    Dashboard View |    prop | |
|  +--------------------+---------------------+--------------------+-------------------+---------+ |
|  | - OpenGL 3D Orbit  | - 2D PlotWidget     | - Sp vs Ideal Plot | - Live Telemetry  | - Loss  | |
|  | - Node Inspector   | - Analytical vs FEA | - Thread pool stats| - Fault Injection |   Plot  | |
|  | - Colorbar Legend  | - p=2.0 Conv. Table | - Repetition table | - Auto-Recovery   | - Check | |
|  +--------------------+---------------------+--------------------+-------------------+---------+ |
+--------------------------------------------------------------------------------------------------+
                                                 |
                                                 v
+--------------------------------------------------------------------------------------------------+
|                            CORE NUMERICAL & ENGINEERING BACKEND (C++17)                          |
|                                (Zero Qt / Pure Standard Library)                                 |
|  +-----------------------+-----------------------+-----------------------+---------------------+ |
|  |      Geometry         |       Thermal         |       Parallel        |      Hardware       | |
|  | - StlParser           | - ThermalFeaSolver    | - ThreadPool          | - McuMockAdapter    | |
|  | - TempCsvParser       | - Thomas Tridiagonal  | - BatchGenerator      | - FpgaMockAdapter   | |
|  | - BoundingBox         | - Midpoint Superconv. | - BatchSolver         | - JtagMockAdapter   | |
|  | - Colormap (Turbo/Jet)| - ConvergenceAnalyzer | - BenchmarkEngine     | - DeviceManager     | |
|  +-----------------------+-----------------------+-----------------------+---------------------+ |
|  |                     Neural Backpropagation Engine (BackpropagationSolver)                   | |
|  | - Analytical Gradients, Central Finite Differences (eps=1e-6), Simultaneous Parameter Update| |
|  +---------------------------------------------------------------------------------------------+ |
+--------------------------------------------------------------------------------------------------+
                                                 |
                                                 v
+--------------------------------------------------------------------------------------------------+
|                                 STANDALONE ONE-CLICK RELEASE RUNTIME                             |
|  +--------------------------+------------------------------+----------------------------------+  |
|  | Launch.exe (Win32 Native)| Bundled Qt 6 Libraries       | CandidateID_Workbench.zip        |  |
|  | Zero-dependency starter  | Pre-deployed via windeployqt | Offline standalone distribution  |  |
|  +--------------------------+------------------------------+----------------------------------+  |
+--------------------------------------------------------------------------------------------------+
```

---

## 2. Structural Layering & Separation of Concerns

The workbench strictly enforces separation between the **Numerical Core** and the **User Interface**:
1. **`WorkbenchCore` Static Library (`src/core/`)**:
   - Zero dependencies on Qt or external GUI frameworks.
   - Built with strict C++17 standards (`/std:c++17`, `/W4`, `/permissive-`).
   - Highly testable: 7 automated test suites run directly against this library via CTest.
2. **`WorkbenchApp` Executable (`src/app/`)**:
   - Built on Qt 6.8.2 MSVC x64.
   - Hosts high-DPI custom widgets: `PlotWidget` (custom antialiased plotting engine) and `OpenGLViewport` (interactive 3D orbit and picking).
   - Coordinates multi-threaded background solver runs without blocking the main event loop.
3. **`Launch.exe` Native Win32 Launcher (`packaging/Launch.cpp`)**:
   - Completely independent Win32 binary with zero runtime DLL dependencies beyond `kernel32.dll`.
   - Resolves the relative path to `bin\WorkbenchApp.exe`, sets the working directory, forwards command-line arguments, and starts the workbench with clean error handling if files are missing.

---

## 3. Directory Structure

```
PASS Science Works/
├── CMakeLists.txt                 # Master CMake configuration (Core, App, Launch, Tests)
├── CMakePresets.json              # Developer presets for MSVC x64 (Debug & Release)
├── LICENSE                        # Apache 2.0 Open Source License
├── README.md                      # Comprehensive project presentation & build instructions
├── .vscode/                       # One-click VS Code task & launch configurations
├── packaging/
│   ├── Launch.cpp                 # Win32 zero-dependency starter executable
│   └── Launch.rc                  # Windows executable resource file
├── scripts/
│   ├── build_msvc.cmd             # Reproducible developer build script (Debug / Release)
│   ├── test_msvc.cmd              # Automated test execution script
│   └── package_msvc.cmd           # Standalone packaging & SHA256 generation script
├── data/
│   ├── geometry/                  # Synthetic ASCII STL test geometries (sample_bracket.stl)
│   ├── thermal/                   # Synthetic ANSYS nodal temperature CSVs
│   └── README.md                  # Data format specifications and synthetic provenance
├── docs/
│   ├── PROJECT_OVERVIEW.md        # Architecture & module breakdown (this document)
│   ├── REQUIREMENTS_TRACEABILITY.md # Traceability matrix for all 100 rubric points
│   ├── TEST_PLAN.md               # Empirical testing methodology and assertions
│   ├── TEST_RESULTS.md            # Verified execution logs and test receipts
│   ├── DEMO_WALKTHROUGH.md        # Step-by-step 3-5 minute demonstration script
│   ├── OWNERSHIP_STATEMENT.md     # Authorship, toolchain, and AI disclosure
│   ├── EFFORT_LOG.csv             # Accurate logging of engineering effort
│   ├── SCORECARD.md               # Rubric self-evaluation & scoring justification
│   ├── THERMAL_FEA_DERIVATION.md  # Complete mathematical derivation of 1D FEA
│   ├── THERMAL_VERIFICATION.md    # N=10,20,40,80 convergence results & receipts
│   ├── PERFORMANCE_METHODOLOGY.md # Multicore benchmarking protocol
│   ├── PERFORMANCE_RESULTS.md     # Actual Ryzen AI 9 HX 370 benchmark measurements
│   ├── EMBEDDED_ARCHITECTURE.md   # MCU, FPGA, and JTAG hardware topology
│   ├── INTERFACE_AND_VOLTAGE_TABLE.md # Pinout, drive strength, and voltage specifications
│   ├── HARDWARE_SAFETY_AND_LIMITATIONS.md # Operating limits and simulation disclosures
│   ├── BACKPROP_DERIVATION_GUIDE.md # Multivariate chain rule derivation for Task F
│   ├── HANDWRITTEN_APPENDIX_CHECKLIST.md # Physical paper checklist (zero-fabrication)
│   └── reports/
│       └── FINAL_TECHNICAL_REPORT.md # 6-page comprehensive technical report
├── results/
│   ├── thermal/                   # Convergence and profile CSV exports
│   ├── performance/               # Raw timings and scaling summaries
│   └── backprop/                  # Loss progression and finite-difference validation CSVs
├── src/
│   ├── core/                      # Pure C++17 Numerical Engine (zero Qt dependency)
│   │   ├── geometry/              # STL & ANSYS CSV parsers and bounding box math
│   │   ├── thermal/               # 1D FEA solver, Thomas algorithm, convergence analysis
│   │   ├── parallel/              # ThreadPool, batch solver, benchmark engine
│   │   ├── hardware/              # IDeviceAdapter, Mcu, Fpga, Jtag mocks, DeviceManager
│   │   └── neural/                # Analytical backpropagation and gradient checker
│   └── app/                       # Qt 6 Desktop Workbench Application
│       ├── main.cpp               # Application entry point
│       ├── visualization/         # OpenGL 3D viewport, camera orbit, node picking
│       └── widgets/               # MainWindow, PlotWidget, and 5 dedicated view modules
└── tests/
    ├── unit/                      # Unit tests (smoke, STL parser, CSV parser, embedded mocks)
    ├── numerical/                 # Numerical verification (thermal FEA, backprop gradients)
    └── integration/               # Parallel multicore consistency and benchmark tests
```
