# Architecture Decision Records (ADR)

**Project**: PASS Science Works — Unified Engineering Workbench  

---

## ADR-001: Separation of Core Numerical Engine from Presentation Framework

* **Status**: Accepted
* **Context**: The assignment requires a C++17 or later backend handling all numerical computation, file processing, and concurrency. The frontend is built with Qt 6 Widgets.
* **Decision**: All business logic, finite element formulation, file parsing (STL, CSV), multicore worker pools, embedded mock adapters, and neural backpropagation solvers are implemented in a standalone, pure C++ library (`WorkbenchCore`) with **zero dependencies on Qt**. The UI layer (`WorkbenchApp`) consumes `WorkbenchCore` through clean C++ interfaces and value types.
* **Consequences**:
  * Positive: `WorkbenchCore` can be compiled, benchmarked, and unit-tested in headless CTest runs without initializing GUI event loops. Zero coupling between math and graphics.
  * Negative: Requires translation between core data structures (e.g. `std::vector<double>`) and Qt types (`QVector`, `QByteArray`) at the boundary.

---

## ADR-002: Direct Tridiagonal Solver (Thomas Algorithm) for 1D Thermal FEA

* **Status**: Accepted
* **Context**: The governing 1D steady-state heat equation with 2-node linear elements produces a symmetric positive-definite tridiagonal global stiffness matrix.
* **Decision**: Implement an exact Thomas algorithm ($O(N)$ time, $O(N)$ space) tailored for 1D tridiagonal systems with Dirichlet boundary condition elimination.
* **Consequences**:
  * Positive: Optimal numerical efficiency ($O(N)$ vs $O(N^3)$ Gaussian elimination), exact precision without external dense matrix algebra libraries (e.g. Eigen/LAPACK), minimal binary footprint, completely verifiable in source.
  * Negative: Specialized for 1D tridiagonal systems; higher-dimensional extensions would require general sparse solvers.

---

## ADR-003: Software 3D Viewport (`QWidget` + `QPainter` with 3D Projection & Depth Sorting)

* **Status**: Accepted (Supersedes initial OpenGL 3.3 prototype)
* **Context**: Need to render ASCII STL triangles (rotate, pan, zoom, fit) and CSV temperature nodes (colored point cloud, legend, node inspection) reliably across diverse environments, including virtual machines, remote desktops, and automated headless screenshot capture (`QWidget::grab()`).
* **Decision**: Implement the 3D viewport (`OpenGLViewport`) as a direct `QWidget` utilizing Qt's antialiased `QPainter` with manual 3D perspective projection matrices (`QMatrix4x4`), depth-sorted painter's algorithm, and directional Lambertian surface shading.
* **Consequences**:
  * Positive: 100% portable and robust across all Windows display configurations, graphics drivers, and virtual machines without requiring hardware OpenGL 3.3 core profiles. Guarantees deterministic rendering during offscreen/headless capture without FBO blanking. Zero third-party CAD dependencies.
  * Negative: CPU-bound rasterization; optimal for engineering meshes up to ~50,000 facets rather than massive multi-million polygon models.

---

## ADR-004: Lightweight Native 2D Plotting Engine

* **Status**: Accepted
* **Context**: Need to display temperature profiles ($T(x)$ vs $T_\text{exact}(x)$), mesh convergence curves ($\log \text{Error}$ vs $\log N$), benchmark speedup/efficiency, and backpropagation loss curves.
* **Decision**: Implement a clean, high-performance custom 2D plotting widget using Qt's `QPainter` rather than introducing complex third-party charting libraries (which often have GPL licensing risks or heavy runtime dependencies).
* **Consequences**:
  * Positive: 100% license safety (LGPLv3 compatible), zero external dependencies, beautiful antialiased rendering, crisp export to high-res PNG/PDF, complete control over axes, grid, legends, and units.
  * Negative: Requires implementing axis auto-scaling, tick generation, and legend layout in C++.

---

## ADR-005: Bounded CPU Thread Pool for Multicore Concurrency

* **Status**: Accepted
* **Context**: Task D mandates serial and CPU-parallel batch solves for 1, 2, and up to 4 workers. The laptop has 12 physical / 24 logical cores.
* **Decision**: Implement a clean `ThreadPool` class using modern `std::thread`, `std::mutex`, `std::condition_variable`, and a task queue with cooperative cancellation.
* **Consequences**:
  * Positive: Completely portable C++17 standard library implementation without OpenMP compiler runtime dependencies. Guaranteed deterministic scheduling and ordering of output results.
  * Negative: Requires careful thread synchronization to eliminate data races and false sharing.

---

## ADR-006: Standalone Windows Packaging Strategy

* **Status**: Accepted
* **Context**: End users must be able to extract `CandidateID_Workbench.zip` and double-click `Launch.exe` with zero installation or admin rights.
* **Decision**: Use `windeployqt` to collect Qt6 platform plugins (`platforms/qwindows.dll`, `styles/`, etc.) and bundle tested Microsoft Visual C++ 2022/2026 redistributable DLLs (`vcruntime140.dll`, `msvcp140.dll`, `vcruntime140_1.dll`) locally in the application directory. Provide a lightweight root launcher `Launch.exe` that delegates directly to the relocatable payload.
* **Consequences**:
  * Positive: True offline, portable execution on any standard Windows 11 x64 machine.
  * Negative: ZIP archive payload size is ~40-60 MB due to bundled Qt DLLs.
