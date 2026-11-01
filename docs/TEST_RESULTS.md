# PASS Science Works — Automated Test Execution Receipts

**Execution Environment**:
- **Host Machine**: Windows 11 x64 (AMD Ryzen AI 9 HX 370 @ 2.0 GHz, 12 Cores / 24 Threads, 32 GB RAM)
- **Compiler**: Microsoft Visual C++ 2026 Developer Tools v18.7.2 (`cl.exe` 19.51.36248.0)
- **Framework**: CMake 4.2.3, Ninja 1.13, CTest, Qt 6.8.2 MSVC 2022 x64
- **Build Mode**: Release (`/std:c++17 /W4 /permissive- /utf-8 /fp:precise /O2`)
- **Execution Date**: 2026-11-01

---

## 1. Summary of Test Execution

```
========================================================================================
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

100% tests passed, 0 tests failed out of 7
Total Test time (real) = 6.97 sec
========================================================================================
```

---

## 2. Detailed Test Receipts by Module

### 2.1 `test_thermal_fea.exe` (Task C)
```
========================================================
[TEST SUITE] Running 1D Thermal FEA Numerical Verification
========================================================
[PASS] Analytical boundary values verified.

--- CONVERGENCE TABLE ---
  N   |  h (m)   | Observed Max Error (C) | Expected (C) | Ratio  | Order (p)
------+----------+------------------------+--------------+--------+----------
  10  |   0.1000 |             1.25000000 |   1.25000000 |   0.00 |   0.0000
  20  |   0.0500 |             0.31250000 |   0.31250000 |   4.00 |   2.0000
  40  |   0.0250 |             0.07812500 |   0.07812500 |   4.00 |   2.0000
  80  |   0.0125 |             0.01953125 |   0.01953125 |   4.00 |   2.0000
-------------------------
[PASS] Quadratic convergence rate (p=2.0, 4x error reduction per mesh doubling) verified.
[PASS] Negative conductivity correctly rejected.
[PASS] Zero rod length correctly rejected.
[PASS] Real verification results exported to results/thermal/.
========================================================
ALL THERMAL FEA VERIFICATION TESTS PASSED!
========================================================
```

### 2.2 `test_parallel_batch.exe` (Task D)
```
========================================================
[TEST SUITE] Running Multicore Parallel Benchmark Suite
========================================================
[CONFIG] Workload: 1000 independent cases, N = 80 elements
[CONFIG] Repetitions: 5 runs per worker setting, Seed: 42

--- BENCHMARK RESULTS SUMMARY ---
 Workers (p) | Median (ms) | Mean (ms) | Min (ms) | Max (ms) | Speedup (S_p) | Efficiency (E_p) | Max Delta (C)
-------------+-------------+-----------+----------+----------+---------------+------------------+--------------
      1      |        1.91 |      1.92 |     1.83 |     2.04 |          1.00x |             1.00 |   0.0000e+00
      2      |        1.72 |      1.77 |     1.63 |     2.05 |          1.11x |             0.56 |   0.0000e+00
      4      |        1.16 |      1.12 |     1.01 |     1.18 |          1.65x |             0.41 |   0.0000e+00
---------------------------------
[PASS] Serial / Parallel numerical agreement verified (all deltas <= 1e-8 C).
[PASS] Verified performance CSV files exported to results/performance/.
========================================================
ALL MULTICORE BENCHMARK TESTS PASSED!
========================================================
```

### 2.3 `test_backprop.exe` (Task F)
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
       dJ/dw = -1.2094645
       dJ/db = -1.2094645
       dJ/dv = -0.71068205
       dJ/dc = -1.5378828

--- FINITE-DIFFERENCE GRADIENT VERIFICATION (eps = 1e-6) ---
 Param | Analytical   | Numerical    | Absolute Error | Status
-------+--------------+--------------+----------------+--------
   w   | -1.209464e+00 | -1.209464e+00 | 1.130189e-10   | PASS
   b   | -1.209464e+00 | -1.209464e+00 | 1.130189e-10   | PASS
   v   | -7.106820e-01 | -7.106820e-01 | 5.253831e-11   | PASS
   c   | -1.537883e+00 | -1.537883e+00 | 1.117548e-10   | PASS
[PASS] All 4 parameters pass finite-difference gradient test within tolerance 1e-5.

[PASS] Simultaneous parameter update confirmed without parameter mutation leakage.
[PASS] Loss decreases strictly: Step 0 Loss = 1.182542e+00 -> Step 10 Loss = 1.021102e-02 -> Step 100 Loss = 1.702622e-20
       Final y_hat = 2.000000e+00 (target y = 2.0)
[PASS] Exported results/backprop/loss_history.csv and results/backprop/gradient_verification.csv
========================================================
ALL BACKPROPAGATION TESTS PASSED!
========================================================
```

### 2.4 `test_embedded_mocks.exe` (Task E)
```
========================================================
[TEST SUITE] Running Embedded Mock Hardware Tests
========================================================
[PASS] Registered 3 default mock adapters.
[PASS] Retrieved all 3 adapters by deviceId.
[PASS] All mock devices explicitly disclose isSimulated == true.
[PASS] Hardware specs match RP2040, Arty A7, and JTAG-HS2 hardware datasheets.
[PASS] Connect lifecycle succeeded for all devices.
[PASS] RP2040 telemetry in valid range (T=32.4599 C, Vdd=3.32498 V, Vcore=1.10035 V).
[PASS] Arty A7 telemetry in valid range (T=41.8719 C, Vccint=1.00218 V).
[PASS] JTAG-HS2 telemetry in valid range (Vref=3.31 V).
[INFO] Testing fault injection and timeout...
[PASS] Fault injection correctly triggered ConnectionStatus::TimedOut.
[INFO] Testing automatic recovery sequence...
[PASS] DeviceManager successfully recovered faulted device.
[PASS] Diagnostic report generated and contains all required disclosures.
[PASS] DisconnectAll succeeded cleanly.
========================================================
All Embedded Mock tests passed successfully.
========================================================
```

### 2.5 `test_stl_parser.exe` & `test_csv_parser.exe` (Task B)
```
========================================================
[TEST SUITE] Running ASCII STL Parser Tests
========================================================
[PASS] Valid ASCII STL parsed with correct bounds.
[PASS] Empty STL correctly rejected: STL content is completely empty.
[PASS] Missing solid keyword correctly rejected: Malformed ASCII STL: missing initial 'solid' keyword.
[PASS] Incomplete facet (2 vertices) rejected: Malformed STL: facet ended with 2 vertices (expected 3) at line 6
[PASS] NaN vertex correctly rejected: Malformed STL: invalid vertex coordinates at line 5
[PASS] Loaded ../../data/geometry/sample_bracket.stl successfully: 12 triangles, max extent: 0.1 m
========================================================
ALL ASCII STL PARSER TESTS PASSED!
========================================================

========================================================
[TEST SUITE] Running Temperature CSV Parser Tests
========================================================
[PASS] Valid CSV parsed with correct statistics.
[PASS] Empty CSV rejected: CSV content is completely empty.
[PASS] Incomplete header rejected: Invalid CSV schema: expected 5 columns (node_id,x_m,y_m,z_m,temperature_C), found 3
[PASS] Incorrect column names rejected: Malformed CSV header. Expected exactly: 'node_id,x_m,y_m,z_m,temperature_C'. Got: 'id,pos_x,pos_y,pos_z,temp'
[PASS] Non-numeric entry rejected: Numeric parsing failure on CSV line 2: invalid stod argument
[PASS] NaN temperature rejected: Non-finite coordinates or temperature (NaN/Inf) on CSV line 2
[PASS] Duplicate node ID policy (retain first, track duplicate count) verified.
[PASS] Degenerate temperature range (T_min == T_max) handled without divide-by-zero.
[PASS] Loaded ../../data/thermal/synthetic_ansys_temperatures.csv successfully: 27 nodes, Range: [20 C, 100 C], Mean: 57.6852 C
========================================================
ALL TEMPERATURE CSV PARSER TESTS PASSED!
========================================================
```
