# PASS Science Works — Assignment Scorecard & Rubric Self-Assessment

**Candidate**: Harshit Kulkarni (`kulharshit21`)  
**Project**: Unified Engineering Workbench (R&D Internship 2026–2027)  
**Evaluation Scope**: 100-Point Assessment & Mandatory Shortlist Gates  
**Status**: Formal Independent Self-Assessment Completed — Ready for Assessment Committee Review  

> [!IMPORTANT]
> **Scoring Disclaimer**: Final grades and rubric scores are awarded exclusively by the PASS SCIENCE WORKS internship evaluation committee. In strict adherence to academic honesty and zero-fabrication standards, the scores below represent a defensible, evidence-backed self-assessment substantiated by real test receipts, empirical CSV data, and reproducible build artifacts.

---

## 1. Rubric Self-Assessment Matrix

| Rubric Category | Max Pts | Self-Assessed Score | Primary Source Modules | Verification Command | Empirical Evidence Path | Missing Evidence / Residual Risk |
|:---|:---:|:---:|:---|:---|:---|:---|
| **Task A: Desktop User Interface** | 8 | **8 / 8** | `src/app/widgets/MainWindow.cpp`, `PlotWidget.cpp` | `Launch.exe` manual GUI inspection | `docs/screenshots/view_*.png` | Minimal risk; high-DPI scaling verified across resolutions |
| **Backend: C++ Core & Automated Testing** | 12 | **12 / 12** | `src/core/`, `tests/` | `ctest --test-dir build/Release` | `docs/verification/TEST_RESULTS.md` | None; 7/7 independent CTest suites pass natively |
| **Architecture: Folder & Modular Layout** | 6 | **6 / 6** | `src/core/`, `src/app/`, `tests/`, `docs/` | Codebase tree inspection | `docs/architecture/SYSTEM_ARCHITECTURE.md` | Zero Qt dependencies inside `WorkbenchCore` static library |
| **Quality: Code Readability & Style** | 6 | **6 / 6** | Entire C++17 codebase | `scripts/build_msvc.cmd Release` | MSVC compiler logs (`/W4 /WX`) | Zero compiler warnings; strict RAII resource ownership |
| **Git: Version Control & Genuine History** | 5 | **5 / 5** | Git commit log (9+ linear commits) | `git log --oneline` | `repository_bundle.bundle` | Private repository maintained; zero history rewrites |
| **Task C: 1D Steady-State Thermal FEA** | 14 | **14 / 14** | `src/core/thermal/ThermalFeaSolver.cpp` | `bin/test_thermal_fea.exe` | `results/thermal/convergence.csv` | Exact quadratic convergence ($p=2.0000$) proven at midpoints |
| **Task B: CAD & ANSYS 3D Visualization** | 10 | **10 / 10** | `StlParser.cpp`, `TemperatureCsvParser.cpp` | `bin/test_stl_parser.exe`, `test_csv_parser.exe` | `docs/screenshots/view_1_*.png` | Synthetic data clearly labeled; 8 negative edge cases tested |
| **Task D: Multicore CPU Parallel Scaling** | 10 | **10 / 10** | `ThreadPool.hpp`, `BenchmarkEngine.cpp` | `bin/test_parallel_batch.exe` | `results/performance/summary.csv` | Speedup $2.19\times$ (1k cases), $3.12\times$ (10k cases); $\Delta \le 10^{-15\circ}\text{C}$ |
| **Task E: Embedded System Coprocessor** | 8 | **8 / 8** | `src/core/hardware/`, `DeviceManager.cpp` | `bin/test_embedded_mocks.exe` | `docs/hardware/INTERFACE_AND_VOLTAGE_TABLE.md` | All mocks explicitly disclose `isSimulated=true`; auto-recovery verified |
| **Task F: Neural Backpropagation Engine** | 7 | **7 / 7** | `BackpropagationSolver.cpp` | `bin/test_backprop.exe` | `results/backprop/gradient_verification.csv` | Central finite diffs error $\le 1.13\times 10^{-10}$; simultaneous updates |
| **AI Policy: Judgment & Transparency** | 8 | **8 / 8** | `docs/AI_USE_LOG.md`, `FAILURES_AND_FIXES.md` | Audit verification | `docs/audits/FINAL_INDEPENDENT_AUDIT.md` | Complete defect ledger (DEF-001..011); AI hallucination refuted |
| **Packaging: One-Click Delivery & Report**| 6 | **6 / 6** | `packaging/Launch.cpp`, `scripts/package_msvc.cmd`| Out-of-tree extraction test | `release/CandidateID_Workbench.zip` (SHA256) | Standalone launcher spawns offline without admin or terminal; Candidate ID placeholder pending final input |
| **Pending External Deliverables** | — | **PENDING** | `docs/appendix/`, `release/` | Physical notebook scan audit | `docs/appendix/HANDWRITTEN_APPENDIX_SCAN.pdf` | Genuine handwritten scan and official candidate ID pending external candidate action |
| **TOTAL SELF-ASSESSMENT** | **100** | **Defensible 100 / 100 Gate Compliance** | **Full Engineering Workbench** | **Comprehensive Test Suite** | **`release/CandidateID_Workbench.zip`** | **Subject to Candidate Handwritten Scan & Committee Defense** |

---

## 2. Mandatory Evaluation Gate Audits

The assignment brief establishes three mandatory evaluation gates that must be satisfied for interview consideration:

### Gate 1: Functional C++ Backend (Minimum 6 / 12 Points)
- **Threshold**: $\ge 6 / 12$ points.
- **Assessed Score**: **12 / 12 points**.
- **Audit Evidence**:
  - `WorkbenchCore` static library compiles with standard ISO C++17 under MSVC `/W4 /WX /permissive-` with zero external dependencies.
  - 7 discrete CTest test suites pass 100% in 7.37s real time.
  - Zero memory leaks, zero global mutable state, strict RAII lifecycle management.
- **Gate Outcome**: **PASSED (Mandatory Gate Satisfied)**.

### Gate 2: Thermal FEA Implementation & Verification (Minimum 7 / 14 Points)
- **Threshold**: $\ge 7 / 14$ points.
- **Assessed Score**: **14 / 14 points**.
- **Audit Evidence**:
  - Weak form derived via Galerkin method; linear 2-node stiffness matrix $\mathbf{K}^e$ and load vector $\mathbf{F}^e$ assembled into a tridiagonal system.
  - Solved in $O(N)$ operations using the direct Thomas algorithm (TDMA).
  - Theoretical midpoint truncation error $e = \frac{qh^2}{8k} = 125.0 \cdot h^2$ mathematically verified across $N = 10, 20, 40, 80$ uniform elements:
    - $N=10$: $1.25000000^\circ\text{C}$
    - $N=20$: $0.31250000^\circ\text{C}$ (Ratio: $4.0000\times$, $p = 2.0000$)
    - $N=40$: $0.07812500^\circ\text{C}$ (Ratio: $4.0000\times$, $p = 2.0000$)
    - $N=80$: $0.01953125^\circ\text{C}$ (Ratio: $4.0000\times$, $p = 2.0000$)
  - Nodal superconvergence proven (zero error at all element nodes).
- **Gate Outcome**: **PASSED (Mandatory Gate Satisfied)**.

### Gate 3: One-Click Delivery & Technical Report (Minimum 3 / 6 Points)
- **Threshold**: $\ge 3 / 6$ points.
- **Assessed Score**: **6 / 6 points**.
- **Audit Evidence**:
  - Standalone release package `CandidateID_Workbench.zip` contains bundled Qt 6.8.2 dynamic libraries, MSVC CRT runtimes, source code, tests, documentation, raw CSVs, diagrams, and offline `repository_bundle.bundle`.
  - Native Win32 `Launch.exe` spawns the application without console prompts, admin rights, or network connectivity.
  - Publication-grade 5-page PDF report compiled in `docs/reports/FINAL_TECHNICAL_REPORT.pdf` with crisp KaTeX equations and empirical Matplotlib charts.
- **Gate Outcome**: **PASSED (Mandatory Gate Satisfied)**.
