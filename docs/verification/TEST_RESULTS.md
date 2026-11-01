# Automated Test Results & Verification Receipts

**Execution Timestamp**: 2026-11-01  
**Test Harness**: CMake / CTest 4.2.3  
**Build Configuration**: Release (`/O2 /DNDEBUG`), MSVC 19.51.36248.0, Ninja 1.13  
**Target Architecture**: Windows 11 x64 (AMD Ryzen AI 9 HX 370, 12 Cores / 24 Threads)  
**Total Test Suites**: 7  
**Pass Rate**: **100% (7 Passed, 0 Failed)**  
**Total Wall-Clock Time**: 7.37 seconds  

---

## 1. CTest Summary Table

| Test # | Test Name | Executable Binary | Category | Execution Duration | Status | Key Verification Output |
|:---:|:---|:---|:---:|:---:|:---:|:---|
| **1** | `test_parallel_batch` | `bin/test_parallel_batch.exe` | Multicore Parallel | 1.40 s | **PASS** | $S_4 = 3.12\times$ (10k cases); Max Delta $= 0.0000 \times 10^{-15\circ}\text{C} \le 10^{-8\circ}\text{C}$ |
| **2** | `test_backprop` | `bin/test_backprop.exe` | Numerical Neural | 0.89 s | **PASS** | Analytical vs FD gradient error $\le 1.13 \times 10^{-10}$; $x=1.0$ vs $x=0.5$ regression pass |
| **3** | `test_thermal_fea` | `bin/test_thermal_fea.exe` | Numerical FEA | 0.97 s | **PASS** | Exact quadratic convergence ($p=2.0000$); nodal superconvergence; BC checks |
| **4** | `test_csv_parser` | `bin/test_csv_parser.exe` | Unit Parsing | 0.96 s | **PASS** | 5-col schema, empty file rejection, NaN/Inf rejection, duplicate ID deduplication |
| **5** | `test_embedded_mocks` | `bin/test_embedded_mocks.exe` | Embedded HAL | 1.04 s | **PASS** | `isSimulated=true` verified; timeout fault injection & 3-retry auto-recovery verified |
| **6** | `test_smoke` | `bin/test_smoke.exe` | Smoke Linkage | 1.00 s | **PASS** | Static library linkage, C++17 symbols, core version metadata `v1.0.0-rc1` |
| **7** | `test_stl_parser` | `bin/test_stl_parser.exe` | Unit Parsing | 1.06 s | **PASS** | Valid ASCII facets, truncated facet rejection, NaN rejection, cross-product normal |

---

## 2. Detailed Test Execution Receipts

### Suite 1: `test_parallel_batch.exe`
```
========================================================
[TEST SUITE] Running Multicore Parallel Benchmark Suite
========================================================
[WORKLOAD 1: STANDARD ASSIGNMENT] 1000 cases, N = 80
[CONFIG] Repetitions: 5 runs per setting, Seed: 42

--- STANDARD BENCHMARK RESULTS (1,000 Cases) ---
 Workers (p) | Median (ms) | Mean (ms) | Min (ms) | Max (ms) | Speedup (S_p) | Efficiency (E_p) | Max Delta (C)
-------------+-------------+-----------+----------+----------+---------------+------------------+--------------
       1      |        1.85 |      1.87 |     1.79 |     1.94 |          1.00x |             1.00 |   0.0000e+00
       2      |        1.24 |      1.33 |     1.15 |     1.78 |          1.49x |             0.74 |   0.0000e+00
       4      |        0.85 |      0.90 |     0.80 |     1.08 |          2.19x |             0.55 |   0.0000e+00
---------------------------------
[PASS] Serial / Parallel numerical agreement verified (all deltas <= 1e-8 C).

[WORKLOAD 2: EXTENDED BENCHMARK] 10000 cases, N = 80
--- EXTENDED BENCHMARK RESULTS (10,000 Cases) ---
 Workers (p) | Median (ms) | Mean (ms) | Min (ms) | Max (ms) | Speedup (S_p) | Efficiency (E_p) | Max Delta (C)
-------------+-------------+-----------+----------+----------+---------------+------------------+--------------
       1      |       20.41 |     20.62 |    19.25 |    21.69 |          1.00x |             1.00 |   0.0000e+00
       2      |       11.25 |     11.14 |    10.30 |    11.66 |          1.81x |             0.91 |   0.0000e+00
       4      |        6.54 |      6.57 |     6.11 |     6.91 |          3.12x |             0.78 |   0.0000e+00
       8      |        4.67 |      4.57 |     3.96 |     5.02 |          4.37x |             0.55 |   0.0000e+00
---------------------------------
[PASS] Extended workload scaling and numerical agreement verified.
[PASS] Verified performance CSV files exported to results/performance/.
========================================================
ALL MULTICORE BENCHMARK TESTS PASSED!
========================================================
```

### Suite 2: `test_backprop.exe`
```
========================================================
[TEST SUITE] Running Backpropagation Numerical Tests
========================================================
[PASS] Step 0 forward values exact:
       z1 = 0.5
       h = 0.46211716
       y_hat = 0.46211716
       loss = 1.1825418
[PASS] Step 0 analytical gradients exact:
       dJ/dw = -1.2094645 (expected: -1.20946448)
       dJ/db = -1.2094645 (expected: -1.20946448)
       dJ/dv = -0.71068205 (expected: -0.71068205)
       dJ/dc = -1.5378828 (expected: -1.53788284)
[PASS] Regression test: Confirmed x=1.0, w=0.5 vs x=0.5, w=1.0 gradient divergence:
       For x=1.0, w=0.5: dJ/dw = -1.2094645
       For x=0.5, w=1.0: dJ/dw = -0.60473224 (ratio = 2)

--- FINITE-DIFFERENCE GRADIENT VERIFICATION (eps = 1e-6) ---
 Param | Analytical   | Numerical    | Absolute Error | Status
-------+--------------+--------------+----------------+--------
   w   | -1.209464e+00 | -1.209464e+00 | 1.130189e-10   | PASS
   b   | -1.209464e+00 | -1.209464e+00 | 1.130189e-10   | PASS
   v   | -7.106820e-01 | -7.106820e-01 | 5.253831e-11   | PASS
   c   | -1.537883e+00 | -1.537883e+00 | 1.117548e-10   | PASS
[PASS] All 4 parameters pass finite-difference gradient test within tolerance 1e-5.

[PASS] Simultaneous parameter update confirmed without parameter mutation leakage.
[PASS] Loss decreases strictly:
       Step 0 Loss = 1.182542e+00
       Step 10 Loss = 1.021102e-02
       Step 100 Loss = 1.702622e-20
       Final y_hat = 2.000000 (target y = 2)
[PASS] Exported results/backprop/loss_history.csv and results/backprop/gradient_verification.csv
========================================================
ALL BACKPROPAGATION TESTS PASSED!
========================================================
```

### Suite 3: `test_thermal_fea.exe`
```
========================================================
[TEST SUITE] Running 1D Thermal FEA Numerical Verification
========================================================
[PASS] Analytical solution T(x) = 100 + 420x - 500x^2 verified:
       T(0) = 100 C
       T_peak(0.42) = 188.2 C
       T(1.0) = 20 C
[PASS] Regression test: Erroneous polynomial (yielding T(1) = -5 C) correctly rejected.
[PASS] Regression test: Erroneous parameter set (k=200, q=5000, e=0.03125 C) correctly distinguished from assigned specification (e=1.25 C).

--- CONVERGENCE TABLE ---
  N   |  h (m)   | Observed Max Error (C) | Expected (C) | Ratio  | Order (p)
------+----------+------------------------+--------------+--------+----------
  10  |   0.1000 |             1.25000000 |   1.25000000 |   0.00 |   0.0000
  20  |   0.0500 |             0.31250000 |   0.31250000 |   4.00 |   2.0000
  40  |   0.0250 |             0.07812500 |   0.07812500 |   4.00 |   2.0000
  80  |   0.0125 |             0.01953125 |   0.01953125 |   4.00 |   2.0000
-------------------------
[PASS] Quadratic convergence rate (p=2.0000, 4.0000x error reduction per mesh doubling) verified.
[PASS] Nodal superconvergence verified (zero error at all element nodes).
[PASS] Negative conductivity correctly rejected.
[PASS] Zero rod length correctly rejected.
[PASS] Real verification results exported to results/thermal/.
========================================================
ALL THERMAL FEA VERIFICATION TESTS PASSED!
========================================================
```
