# PASS Science Works — Automated Test Plan

## 1. Test Strategy & Engineering Principles

The **Unified Engineering Workbench** adheres to an empirical, zero-fabrication testing philosophy. Every numerical algorithm, file parser, concurrency primitive, hardware adapter, and deployment packaging artifact is backed by rigorous, automated tests executed natively under MSVC on Windows 11 x64.

### 1.1 Testing Hierarchy
```
+-------------------------------------------------------------------------------+
|                       CTEST AUTOMATED SUITE (7 EXECUTABLES)                  |
|                                                                               |
|  [ Unit Tests ]          [ Numerical Verifications ]    [ Integration Tests ] |
|  - test_smoke            - test_thermal_fea             - test_parallel_batch |
|  - test_stl_parser       - test_backprop                                      |
|  - test_csv_parser                                                            |
|  - test_embedded_mocks                                                        |
+-------------------------------------------------------------------------------+
                                        |
                                        v
+-------------------------------------------------------------------------------+
|                     SYSTEM & PACKAGING ACCEPTANCE GATES                       |
|  - scripts/build_msvc.cmd (Debug & Release /W4 clean build)                   |
|  - scripts/test_msvc.cmd (CTest 100% pass verification)                       |
|  - scripts/package_msvc.cmd (Release bundling, windeployqt, SHA256)           |
|  - Launch.exe Acceptance (Out-of-tree double-click execution test)            |
+-------------------------------------------------------------------------------+
```

---

## 2. Test Suite Specifications

### 2.1 `test_smoke.exe` (Core Framework Smoke Test)
- **Scope**: Core library link verification, API versioning queries, project metadata.
- **Assertions**:
  - `Core::getVersionString()` matches `1.0.0-rc1`.
  - `Core::getProjectName()` returns valid application string.

### 2.2 `test_stl_parser.exe` (CAD Geometry Parser)
- **Scope**: Robustness testing for ASCII STL parsing (`StlParser.cpp`).
- **Test Cases**:
  1. *Valid Minimal ASCII STL*: Parses 1 facet (3 vertices), computes exact bounding box ($[0,1] \times [0,1] \times [0,0]$).
  2. *Empty File Rejection*: Throws `std::runtime_error("STL content is completely empty")`.
  3. *Missing 'solid' Keyword*: Rejects files lacking the mandatory initial `solid` token.
  4. *Incomplete Facet*: Rejects facets terminating prematurely with fewer than 3 vertices.
  5. *Non-Finite Coordinates*: Rejects vertices containing `NaN` or `Inf`.
  6. *Synthetic Sample Load*: Parses packaged `data/geometry/sample_bracket.stl` (12 triangles, $0.1\text{ m}$ extent).

### 2.3 `test_csv_parser.exe` (ANSYS Thermal Nodal CSV Parser)
- **Scope**: Robustness and schema enforcement for ANSYS nodal data (`TemperatureCsvParser.cpp`).
- **Test Cases**:
  1. *Valid Schema*: Parses 5 columns (`node_id,x_m,y_m,z_m,temperature_C`), validates bounding box and temperature range.
  2. *Empty CSV Rejection*: Throws exception on 0-byte input.
  3. *Incomplete Header*: Rejects files with fewer than 5 columns.
  4. *Mismatched Header Names*: Rejects headers deviating from standard column names.
  5. *Non-Numeric Field Rejection*: Throws on malformed floating-point text.
  6. *NaN / Inf Rejection*: Rejects records containing non-finite floats.
  7. *Duplicate Node ID Policy*: Preserves first occurrence, increments duplicate counter, prevents map collisions.
  8. *Degenerate Range*: Prevents divide-by-zero when $T_\text{min} == T_\text{max}$.
  9. *Packaged Data Load*: Parses `data/thermal/synthetic_ansys_temperatures.csv` (27 nodes, $20^\circ\text{C}$ to $100^\circ\text{C}$).

### 2.4 `test_thermal_fea.exe` (1D Thermal FEA & Quadratic Convergence)
- **Scope**: Numerical correctness and convergence rate validation (`ThermalFeaSolver.cpp`, `ThermalConvergence.cpp`).
- **Analytical Benchmark**:
  - Uniform rod: $L=1.0\text{ m}, A=10^{-4}\text{ m}^2, k=10.0\text{ W/(m}\cdot\text{K)}, q=10000.0\text{ W/m}^3, T(0)=100.0^\circ\text{C}, T(L)=20.0^\circ\text{C}$.
- **Assertions**:
  - Boundary temperatures at $x=0$ and $x=L$ match prescribed Dirichlet conditions within $10^{-12\circ}\text{C}$.
  - Mesh resolutions $N=10, 20, 40, 80$ elements evaluated at midpoints $x_\text{mid} = x_i + h/2$.
  - Maximum observed midpoint errors match theoretical formula $e = \frac{qh^2}{8k}$ identically:
    - $N=10\ (h=0.1000\text{ m}) \to e = 1.25000000^\circ\text{C}$
    - $N=20\ (h=0.0500\text{ m}) \to e = 0.31250000^\circ\text{C}$
    - $N=40\ (h=0.0250\text{ m}) \to e = 0.07812500^\circ\text{C}$
    - $N=80\ (h=0.0125\text{ m}) \to e = 0.01953125^\circ\text{C}$
  - Error reduction ratio between successive doublings equals $4.0000\times$.
  - Estimated asymptotic order of accuracy $p = \log_2(E_{2h} / E_h) = 2.0000$ (exact quadratic convergence).
  - Physical parameter validation: Rejects non-positive length, area, conductivity, and negative heat source.

### 2.5 `test_parallel_batch.exe` (Multicore Parallel Scaling & Consistency)
- **Scope**: Concurrency determinism, scaling, and numerical consistency (`ThreadPool.hpp`, `BatchSolver.cpp`, `BenchmarkEngine.cpp`).
- **Assertions**:
  - Evaluates 1,000 independent thermal rod configurations across $N=80$ elements.
  - Compares serial solution ($p=1$) against parallel solutions ($p=2, 4$ worker threads).
  - Maximum absolute temperature difference between serial and parallel must satisfy $|\Delta T| \le 10^{-8\circ}\text{C}$ across all nodes of all 1,000 cases.
  - Measures wall-clock execution time across 5 repetitions per worker count, computing median, min, max, speedup $S_p$, and efficiency $E_p$.

### 2.6 `test_embedded_mocks.exe` (Embedded Target Integration & Recovery)
- **Scope**: Heterogeneous device management, telemetry streams, and fault injection (`DeviceManager.cpp`, `McuMockAdapter.cpp`, `FpgaMockAdapter.cpp`, `JtagMockAdapter.cpp`).
- **Assertions**:
  - Verifies registration of Raspberry Pi Pico (RP2040), Digilent Arty A7-35T (Artix-7), and Digilent JTAG-HS2 adapters.
  - Verifies all adapters explicitly report `isSimulated == true` (zero-fabrication policy).
  - Verifies hardware specs (Pico 133 MHz, Arty A7 100 MHz, JTAG-HS2 30 MHz, nominal 3.3V).
  - Connect and disconnect lifecycle tests.
  - Telemetry verification: RP2040 internal temp ($25^\circ\text{C}$–$45^\circ\text{C}$), Arty A7 die temp ($35^\circ\text{C}$–$55^\circ\text{C}$), JTAG-HS2 VREF ($3.2\text{V}$–$3.4\text{V}$).
  - Fault injection: Enabling timeout transitions adapter to `ConnectionStatus::TimedOut` with invalid packet flag.
  - Automatic recovery: `attemptRecovery` executes 3-retry sequence, re-synchronizes communication, and restores valid telemetry.

### 2.7 `test_backprop.exe` (Neural Backpropagation & Gradient Verification)
- **Scope**: Multivariate calculus, chain rule, finite-difference verification, and training convergence (`BackpropagationSolver.cpp`).
- **Network**: $x=1.0, y=2.0, w=0.5, b=0.0, v=1.0, c=0.0, \eta=0.1, J=0.5(\hat{y}-y)^2$.
- **Assertions**:
  - Step 0 forward values match analytical calculations: $z_1 = 0.5, h \approx 0.46211716, \hat{y} \approx 0.46211716, J \approx 1.18255182$.
  - Step 0 analytical gradients match exact derivatives:
    $\frac{\partial J}{\partial v} \approx -0.71068193, \frac{\partial J}{\partial c} \approx -1.53788284, \frac{\partial J}{\partial w} \approx -1.20946440, \frac{\partial J}{\partial b} \approx -1.20946440$.
  - Central finite-difference gradient checking with perturbation $\epsilon = 10^{-6}$:
    $|\nabla_\text{ana} - \nabla_\text{num}| \le 10^{-5}$ across all 4 parameters.
  - Simultaneous parameter update: Confirms all gradients are evaluated on old parameters before any update occurs.
  - 100-step training convergence: Loss decreases monotonically ($J^{(0)} \approx 1.18 \to J^{(10)} \approx 1.02 \times 10^{-2} \to J^{(100)} \approx 1.70 \times 10^{-20}$), predicting $\hat{y} = 2.00000000$.
