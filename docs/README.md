# Unified Engineering Workbench — Technical Documentation Portal

Welcome to the comprehensive engineering documentation for the **PASS SCIENCE WORKS Unified Engineering Workbench**. This repository provides an industrial-grade, verified scientific desktop application satisfying the R&D Internship 2026–2027 assignment brief.

All documentation is organized into domain-specific directories to support technical reviewers, hiring managers, and collaborating engineers.

---

## Documentation Directory Map

```
docs/
├── README.md                                # Central documentation portal (this file)
├── getting-started/
│   └── BUILD_AND_RUN.md                     # Compiler setup, prerequisites, build & execution
├── architecture/
│   ├── SYSTEM_ARCHITECTURE.md               # High-level architecture, module isolation & concurrency
│   ├── COMPONENT_INTERFACES.md              # C++ class interfaces, abstract adapters & Qt signal decoupling
│   └── DATA_FLOW.md                         # End-to-end data pipelines (STL/CSV, FEA, ThreadPool)
├── engineering/
│   ├── THERMAL_FEA.md                       # Weak form, Thomas TDMA, convergence derivation & nodal superconvergence
│   ├── STL_AND_CSV_FORMATS.md               # 3D geometry and ANSYS temperature cloud specifications
│   └── BACKPROPAGATION.md                   # Computational graph, chain rule, central difference verification
├── hardware/
│   ├── EMBEDDED_INTEGRATION.md              # MCU, FPGA, and JTAG coprocessor architecture & telemetry
│   └── INTERFACE_AND_VOLTAGE_TABLE.md       # Device-specific voltage domains, connector pinouts & level shifting
├── performance/
│   ├── BENCHMARK_METHODOLOGY.md             # Thread pool design, Amdahl bounds & timing methodology
│   └── BENCHMARK_RESULTS.md                 # Empirical scaling results (1,000 & 10,000 cases on Zen 5)
├── verification/
│   ├── TEST_STRATEGY.md                     # Multi-tier testing philosophy (unit, numerical, integration)
│   ├── TEST_RESULTS.md                      # Real test receipts across 7 independent CTest executable suites
│   └── REQUIREMENTS_TRACEABILITY.md         # Full requirement-to-evidence matrix across tasks A–F
├── development/
│   ├── DECISION_LOG.md                      # Architecture Decision Records (ADRs 001–006)
│   ├── FAILURES_AND_FIXES.md                # Comprehensive defect ledger (DEF-001 to DEF-011)
│   ├── AI_USE_LOG.md                        # Transparent disclosure of AI assistance and human engineering
│   └── DEPENDENCIES_AND_LICENSES.md         # Open-source licensing disclosures and third-party notices
├── reports/
│   ├── FINAL_TECHNICAL_REPORT.md            # Markdown source of the 6-page technical report
│   ├── FINAL_TECHNICAL_REPORT.html          # Styled HTML distribution report
│   └── FINAL_TECHNICAL_REPORT.pdf           # Print-ready compiled 6-page PDF report
├── audits/
│   └── FINAL_INDEPENDENT_AUDIT.md           # Independent zero-fabrication audit of previous claims
└── RELEASE_AND_LIMITATIONS.md               # Standalone packaging guide, release checklist & limitations
```

---

## Essential Document Quick Links

| Category | Document | Primary Contents | Target Audience |
|:---|:---|:---|:---|
| **Quick Start** | [`getting-started/BUILD_AND_RUN.md`](getting-started/BUILD_AND_RUN.md) | Windows 11 toolchain, MSVC, CMake, Qt 6.8.2, one-click `Launch.exe` | Developers, Evaluators |
| **Architecture** | [`architecture/SYSTEM_ARCHITECTURE.md`](architecture/SYSTEM_ARCHITECTURE.md) | Decoupled C++ core, Qt GUI, memory safety, thread models | Architects, Senior Engineers |
| **Thermal FEA** | [`engineering/THERMAL_FEA.md`](engineering/THERMAL_FEA.md) | Galerkin FEM, Thomas solver ($O(N)$), midpoint error ($e = \frac{qh^2}{8k} = 125h^2$) | Numerical Researchers, FEA Engineers |
| **Backpropagation**| [`engineering/BACKPROPAGATION.md`](engineering/BACKPROPAGATION.md) | Tanh neural engine, gradient derivation, finite differences ($\Delta \le 1.13\times 10^{-10}$) | ML Engineers, Mathematicians |
| **Hardware** | [`hardware/EMBEDDED_INTEGRATION.md`](hardware/EMBEDDED_INTEGRATION.md) | Pico RP2040, Arty A7, JTAG-HS2, dynamic VREF sensing, fault recovery | Embedded Engineers, Hardware Leads |
| **Performance** | [`performance/BENCHMARK_RESULTS.md`](performance/BENCHMARK_RESULTS.md) | 1,000 & 10,000 cases scaling on Ryzen AI 9 HX 370, zero numerical drift | Performance Engineers |
| **Verification** | [`verification/REQUIREMENTS_TRACEABILITY.md`](verification/REQUIREMENTS_TRACEABILITY.md) | 100% traceability from internship brief to verified C++ code | QA Auditors, Evaluators |
| **Defect Ledger** | [`development/FAILURES_AND_FIXES.md`](development/FAILURES_AND_FIXES.md) | 11 authentic defect analyses (DEF-001 to DEF-011) with root-cause fixes | Lead Maintainers |
| **Submission Report**| [`reports/FINAL_TECHNICAL_REPORT.pdf`](reports/FINAL_TECHNICAL_REPORT.pdf) | Formal 6-page internship technical report with high-res figures | Internship Committee |

---

## Verification & Reproducibility Guarantee

Every numerical figure, convergence rate, speedup metric, and gradient discrepancy documented across this portal is generated from verified source execution and recorded in raw CSV files under `results/`. Zero fabricated data, synthetic green checks, or simulated passes are tolerated.
