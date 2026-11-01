# Multicore Benchmark Methodology and Protocol

**Project**: PASS Science Works — Unified Engineering Workbench (Task D)  
**Host Processor**: AMD Ryzen AI 9 HX 370 w/ Radeon 890M (12 Cores, 24 Logical Processors)  
**Compiler**: MSVC 19.51 Release (`/O2 /Oi /Ot /GL /MD /fp:precise`)  
**Operating System**: Windows 11 x64 (Build 26300)  

---

## 1. Workload Specification

* **Workload Size**: 1,000 independent 1D thermal finite-element problems.
* **Element Discretization**: $N = 80$ uniform linear elements per case (81 nodes, 80 elemental matrix calculations, Thomas algorithm solve per case).
* **RNG Seed**: Fixed seed = `42` (`std::mt19937`).
* **Parameter Ranges**:
  * Thermal Conductivity $k \in [5.0, 50.0]\text{ W/(m}\cdot\text{K)}$
  * Volumetric Heat Generation $q \in [1{,}000.0, 50{,}000.0]\text{ W/m}^3$
  * Length $L \in [0.5, 2.0]\text{ m}$
  * Boundary Temperatures $T_0 \in [80.0, 150.0]^\circ\text{C}, T_L \in [10.0, 40.0]^\circ\text{C}$
* **Determinism Guarantee**:
  * Cases are generated in deterministic order.
  * Solvers output results strictly indexed by `caseId` ($0$ to $M-1$), ensuring zero race-induced reordering.

---

## 2. Timing Protocol

* **Monotonic High-Resolution Clock**: `std::chrono::steady_clock` with nanosecond precision.
* **Warm-up Phase**: One complete execution of the 1,000-case batch executed prior to measurements to prime CPU core power states, L1/L2/L3 instruction caches, and worker thread initialization.
* **Repetition Count**: 5 independent runs per worker configuration ($p = 1, 2, 4$).
* **Metric Reporting**:
  * Wall-clock compute time per repetition.
  * Median execution time $T_p$ (resistant to background OS interrupts).
  * Speedup ratio $S_p = T_1 / T_p$.
  * Parallel efficiency $E_p = S_p / p$.
* **Exclusions**: File I/O, disk writes, and GUI rendering are strictly excluded from the measured compute intervals.

---

## 3. Numerical Agreement Requirement

To prove that parallel computation does not introduce thread races, truncation errors, or out-of-order data corruption:
$$\max_{i, j} \left| T_\text{serial}(i, j) - T_\text{parallel}(i, j) \right| \le 10^{-8\circ}\text{C}$$
for every case $i \in \{0, \dots, 999\}$ and every node $j \in \{0, \dots, 80\}$.
