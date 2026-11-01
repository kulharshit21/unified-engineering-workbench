# Thermal FEA Numerical Verification Report

**Date**: November 1, 2026  
**Status**: Independently Verified via CTest Automated Execution  

---

## 1. Measured Convergence Evidence

| Mesh Size ($N$) | Element Length $h$ (m) | Observed Max Midpoint Error ($^\circ$C) | Theoretical Error $125 h^2$ ($^\circ$C) | Error Reduction Ratio ($E_{N/2} / E_N$) | Empirical Convergence Order ($p$) | Status |
| :---: | :---: | :---: | :---: | :---: | :---: | :---: |
| **10** | 0.1000 | **1.25000000** | 1.25000000 | Base | Base | **PASS** |
| **20** | 0.0500 | **0.31250000** | 0.31250000 | **4.0000** | **2.0000** | **PASS** |
| **40** | 0.0250 | **0.07812500** | 0.07812500 | **4.0000** | **2.0000** | **PASS** |
| **80** | 0.0125 | **0.01953125** | 0.01953125 | **4.0000** | **2.0000** | **PASS** |

### Boundary Condition Check
* $x = 0\text{ m}$: Prescribed $100.00000000^\circ\text{C}$ $\to$ Observed $100.00000000^\circ\text{C}$ (Delta = $0.000\times 10^{-15}$)
* $x = 1\text{ m}$: Prescribed $20.00000000^\circ\text{C}$ $\to$ Observed $20.00000000^\circ\text{C}$ (Delta = $0.000\times 10^{-15}$)

---

## 2. Real Engineering Debugging Insight

During the mathematical formulation, we investigated the classic pitfall of evaluating finite element convergence exclusively at the node vertices. Because 1D linear elements have constant coefficients ($k, A$), the nodal temperatures computed by the FE method coincide **identically with the exact analytical temperature at those node points** (superconvergence). 

If an engineer evaluates error only at node vertices, the observed error is $\approx 0.0$ for all mesh sizes ($N=10, 20, 40, 80$). This would lead to a catastrophic false conclusion that "mesh refinement produces no improvement" or that "the coarse mesh is already exact everywhere."

By evaluating the interpolated finite-element field at element midpoints:
$$T_\text{FE}(x_\text{mid}) = \frac{T_i + T_{i+1}}{2}$$
and comparing against $T_\text{exact}(x_\text{mid})$, the true interior discretization error is revealed, demonstrating exact quadratic convergence ($O(h^2)$) with exactly $4.00\times$ reduction per mesh doubling.

---

## 3. Evidence Artifacts
* Underlying Convergence CSV: [`results/thermal/convergence.csv`](file:///d:/PASS%20Science%20Works/results/thermal/convergence.csv)
* Temperature Profiles CSV: [`results/thermal/temperature_profiles.csv`](file:///d:/PASS%20Science%20Works/results/thermal/temperature_profiles.csv)
* Automated Test Source: [`tests/numerical/test_thermal_fea.cpp`](file:///d:/PASS%20Science%20Works/tests/numerical/test_thermal_fea.cpp)
