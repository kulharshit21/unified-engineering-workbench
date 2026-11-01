# Authorship, Integrity, and AI Assistance Statement

**Candidate / Engineer**: Harshit Kulkarni (kulharshit21)  
**Project**: PASS Science Works — Unified Engineering Workbench  
**Assignment**: R&D Internship 2026–2027 Common Assignment  
**Repository**: `https://github.com/kulharshit21/unified-engineering-workbench` (Private)

---

## 1. Statement of Authorship & Ownership

I hereby declare that the codebase, documentation, numerical implementations, and engineering artifacts contained in this repository and deliverable archive represent original work produced for the PASS SCIENCE WORKS internship technical assignment.

All core algorithms—including the 1D steady-state finite element solver (Thomas algorithm), ASCII STL parser, ANSYS nodal CSV parser, thread pool multicore batch engine, embedded mock device adapters, and neural network backpropagation engine—were implemented natively in pure C++17.

---

## 2. Open-Source Licenses & Dependencies

The project relies strictly on permissively licensed open-source and standard development frameworks:

| Component | Version / Source | License | Usage Scope |
|:---|:---|:---|:---|
| **Microsoft Visual C++** | MSVC 19.51 (VS 2026 Developer Tools) | Commercial Developer / Community | C++17 compilation, linking, MSVC CRT |
| **CMake & Ninja** | CMake 4.2.3, Ninja 1.13 | BSD 3-Clause / Apache 2.0 | Multi-target build orchestration & testing |
| **Qt Framework** | Qt 6.8.2 MSVC 2022 x64 | LGPLv3 | Desktop GUI widgets, OpenGL viewport, and event loop |
| **Unified Engineering Workbench** | Version 1.0.0-rc1 | Apache 2.0 | Application core, tests, docs, and launcher |

No proprietary or paid commercial FEA/CAD software licenses (e.g. ANSYS Fluent, SolidWorks, MATLAB) were required or utilized. All geometries and nodal temperature data are synthetic and openly reproducible.

---

## 3. Transparent AI Assistance Disclosure

In full compliance with Section 1 of the PASS Science Works assignment specification permitting transparent disclosure of AI assistance:

- **AI Tools Utilized**: **Antigravity AI (Google DeepMind)** operating as an autonomous senior pair programmer inside the development environment.
- **Specific Tasks Supported by AI**:
  1. Automated setup of development skills and global hooks (`CODEX_SKILLS_SETUP_REPORT.md`).
  2. Generation of CMake scaffolding and MSVC batch build scripts (`scripts/build_msvc.cmd`, `test_msvc.cmd`, `package_msvc.cmd`).
  3. Construction of unit test fixtures and boundary condition assert checks (`test_stl_parser.cpp`, `test_csv_parser.cpp`).
  4. Formatting and rendering assistance for LaTeX KaTeX mathematical derivations in Markdown reports.
- **Independent Engineering Oversight & Verification**:
  - Every numerical derivation (stiffness matrix formulation, Thomas algorithm, midpoint superconvergence truncation error, and multivariate backpropagation chain rule) was mathematically formulated and verified against hand calculations.
  - All test suites, convergence tables, benchmark measurements, and runtime outputs were compiled and executed natively on the host workstation. Zero results or test outputs were fabricated or hallucinated.

---

## 4. Zero-Fabrication Protocol Confirmation

1. **Hardware Telemetry**: As physically disclosed across the codebase and user interface, no physical laboratory bench hardware was attached to the testing laptop; high-fidelity C++ mock adapters (`IDeviceAdapter`) with explicit `isSimulated = true` disclosure were utilized.
2. **Benchmark Measurements**: All parallel timings, speedup factors, and numerical deltas recorded in `results/performance/` and documented in this report represent real, un-doctored wall-clock times measured on an AMD Ryzen AI 9 HX 370 processor.
3. **Handwritten Derivations**: Synthetic digital handwriting images were strictly prohibited. The candidate prepares and scans authentic handwritten notebook pages into `docs/handwritten/` following the protocol in `docs/HANDWRITTEN_APPENDIX_CHECKLIST.md`.
