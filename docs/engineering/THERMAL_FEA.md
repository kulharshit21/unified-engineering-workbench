# 1D Steady-State Thermal Finite Element Analysis (Task C)

## 1. Problem Formulation & Boundary Value Problem

Consider steady-state, one-dimensional heat conduction along a uniform cylindrical rod of length $L$, cross-sectional area $A$, constant isotropic thermal conductivity $k$, and uniform internal volumetric heat generation $q$.

$$\begin{aligned}
-k \frac{d^2T}{dx^2} &= q, \quad x \in (0, L) \\
T(0) &= T_0 = 100^\circ\text{C} \quad (\text{Left Dirichlet Boundary}) \\
T(L) &= T_L = 20^\circ\text{C} \quad (\text{Right Dirichlet Boundary})
\end{aligned}$$

### Assigned Parameter Values
- Rod length: $L = 1.0\text{ m}$
- Cross-sectional area: $A = 1.0 \times 10^{-4}\text{ m}^2$
- Thermal conductivity: $k = 10.0\text{ W/(m}\cdot\text{K)}$
- Volumetric heat source: $q = 10000.0\text{ W/m}^3$
- Left boundary: $T(0) = 100.0^\circ\text{C}$
- Right boundary: $T(L) = 20.0^\circ\text{C}$

---

## 2. Analytical Solution Derivation

Integrating the governing differential equation twice with respect to $x$:

$$\frac{d}{dx}\left(-k \frac{dT}{dx}\right) = q \implies -k \frac{dT}{dx} = qx + C_1 \implies \frac{dT}{dx} = -\frac{q}{k}x - \frac{C_1}{k}$$

$$T(x) = -\frac{q}{2k}x^2 - \frac{C_1}{k}x + C_2$$

Applying the left boundary condition $T(0) = T_0$:
$$C_2 = T_0 = 100.0^\circ\text{C}$$

Applying the right boundary condition $T(L) = T_L$:
$$T_L = -\frac{qL^2}{2k} - \frac{C_1 L}{k} + T_0 \implies -\frac{C_1}{k} = -\frac{T_0 - T_L}{L} + \frac{qL}{2k}$$

Substituting back yields the closed-form analytical solution:

$$\mathbf{T(x) = T_0 - (T_0 - T_L)\frac{x}{L} + \frac{q}{2k}x(L - x)}$$

Substituting the assigned numerical values:
$$T(x) = 100.0 - (100.0 - 20.0)\frac{x}{1.0} + \frac{10000.0}{2(10.0)}x(1.0 - x)$$
$$T(x) = 100.0 - 80.0x + 500.0(x - x^2)$$
$$\mathbf{T(x) = 100.0 + 420.0x - 500.0x^2}$$

### Physical Validation Checks
1. **Left Boundary**: $T(0) = 100.0 + 420.0(0) - 500.0(0)^2 = \mathbf{100.0^\circ\text{C}}$ (**Exact**)
2. **Right Boundary**: $T(1.0) = 100.0 + 420.0(1.0) - 500.0(1.0)^2 = 100.0 + 420.0 - 500.0 = \mathbf{20.0^\circ\text{C}}$ (**Exact**)
3. **Internal Peak Temperature**: Due to internal volumetric generation $q$, the maximum temperature occurs internally where $\frac{dT}{dx} = 0$:
   $$\frac{dT}{dx} = 420.0 - 1000.0x = 0 \implies x^* = \mathbf{0.420\text{ m}}$$
   $$T(0.420) = 100.0 + 420.0(0.42) - 500.0(0.42)^2 = 100.0 + 176.4 - 88.2 = \mathbf{188.20^\circ\text{C}}$$
   This confirms that internal temperatures rise significantly above both boundary values ($188.20^\circ\text{C} > 100^\circ\text{C}$).

---

## 3. Finite Element Formulation (Galerkin Method)

Let $v(x) \in H_0^1(0, L)$ be an arbitrary test function satisfying $v(0) = v(L) = 0$. Multiplying the differential equation and integrating by parts:

$$\int_0^L \left(-k \frac{d^2T}{dx^2}\right) v(x) A\, dx = \int_0^L q v(x) A\, dx$$

Applying integration by parts on the conduction term:
$$\left[ -k A \frac{dT}{dx} v(x) \right]_0^L + \int_0^L k A \frac{dT}{dx} \frac{dv}{dx}\, dx = \int_0^L q A v(x)\, dx$$

Since $v(0) = v(L) = 0$, the boundary flux term vanishes identically, giving the **Weak Form**:
$$\int_0^L k A \frac{dT}{dx} \frac{dv}{dx}\, dx = \int_0^L q A v(x)\, dx$$

### 3.1 Discretization into Linear Elements
The rod is divided into $N$ uniform elements of length $h = L/N$. On local element $e = [x_i, x_{i+1}]$, the temperature field is interpolated using standard linear Lagrange shape functions:
$$T^e(\xi) = N_1(\xi) T_i + N_2(\xi) T_{i+1}, \quad N_1(\xi) = 1 - \frac{\xi}{h}, \quad N_2(\xi) = \frac{\xi}{h}$$

Evaluating elemental integrals yields the element stiffness matrix $\mathbf{K}^e$ and element load vector $\mathbf{F}^e$:
$$\mathbf{K}^e = \frac{k A}{h} \begin{bmatrix} 1 & -1 \\ -1 & 1 \end{bmatrix}, \quad \mathbf{F}^e = \frac{q A h}{2} \begin{bmatrix} 1 \\ 1 \end{bmatrix}$$

### 3.2 Global Assembly & Thomas Tridiagonal Solver (TDMA)
Assembling across all elements and factoring out $\frac{kA}{h}$:
$$-T_{i-1} + 2T_i - T_{i+1} = \frac{q h^2}{k}, \quad i = 1, \dots, N-1$$

Applying the Dirichlet boundaries $T_0 = 100^\circ\text{C}$ and $T_N = 20^\circ\text{C}$ to the first and last interior equations:
- $i=1$: $2T_1 - T_2 = \frac{qh^2}{k} + T_0$
- $i=N-1$: $-T_{N-2} + 2T_{N-1} = \frac{qh^2}{k} + T_N$

This strictly tridiagonal, symmetric positive-definite system is solved in $O(N)$ operations using the **Thomas algorithm** (TDMA), avoiding dense matrix allocations ($O(N^3)$ LU decomposition).

---

## 4. Convergence Theory: Midpoint Superconvergence Discovery

In 1D linear FEA with constant coefficients and uniform source terms, the Galerkin solution possesses the remarkable mathematical property of **nodal superconvergence**:
$$T_i^\text{FEA} = T_\text{exact}(x_i) \quad \forall i = 0, \dots, N$$
The nodal error is identically zero to machine floating-point precision ($< 10^{-11\circ}\text{C}$). Evaluating FEA error strictly at mesh nodes would give a false impression of zero truncation error for any mesh size $N$.

### 4.1 Midpoint Truncation Error Derivation
To observe the true asymptotic spatial truncation error, the finite element solution must be evaluated at element midpoints $x_\text{mid} = x_i + \frac{h}{2}$:
$$T_\text{FEA}(x_\text{mid}) = \frac{1}{2}\left(T_i^\text{FEA} + T_{i+1}^\text{FEA}\right) = \frac{1}{2}\left(T_\text{exact}(x_i) + T_\text{exact}(x_{i+1})\right)$$

Expanding $T_\text{exact}(x)$ via Taylor series about $x_\text{mid}$:
$$T_\text{exact}\left(x_\text{mid} \pm \frac{h}{2}\right) = T_\text{exact}(x_\text{mid}) \pm T'(x_\text{mid})\frac{h}{2} + \frac{1}{2} T''(x_\text{mid})\left(\frac{h}{2}\right)^2 + O(h^3)$$

Averaging the two endpoints:
$$\frac{1}{2}\left(T_\text{exact}(x_i) + T_\text{exact}(x_{i+1})\right) = T_\text{exact}(x_\text{mid}) + \frac{h^2}{8} T''(x_\text{mid}) + O(h^4)$$

Since $-k T''(x) = q \implies T''(x) = -\frac{q}{k}$ everywhere:
$$T_\text{FEA}(x_\text{mid}) = T_\text{exact}(x_\text{mid}) - \frac{q h^2}{8k}$$

Therefore, the exact maximum midpoint interpolation error is:
$$\mathbf{e(x_\text{mid}) = |T_\text{exact}(x_\text{mid}) - T_\text{FEA}(x_\text{mid})| = \frac{q h^2}{8k}}$$

For the assigned parameters $q = 10000\text{ W/m}^3, k = 10\text{ W/(m}\cdot\text{K)}$:
$$\frac{q}{8k} = \frac{10000}{8(10)} = \mathbf{125.0} \implies \mathbf{e(x_\text{mid}) = 125.0 \cdot h^2}$$

---

## 5. Empirical Convergence Verification Table

Evaluating across $N = 10, 20, 40, 80$ uniform elements:

| Elements $N$ | Element Size $h$ (m) | Theoretical Midpoint Error $125 h^2$ (°C) | Observed Midpoint Error (°C) | Error Reduction Ratio $\frac{e_{2h}}{e_h}$ | Convergence Order $p = \log_2(\text{Ratio})$ | Status |
|:---:|:---:|:---:|:---:|:---:|:---:|:---:|
| **10** | 0.1000 | 1.25000000 | 1.25000000 | Base | Base | **PASS** |
| **20** | 0.0500 | 0.31250000 | 0.31250000 | **4.0000** | **2.0000** | **PASS** |
| **40** | 0.0250 | 0.07812500 | 0.07812500 | **4.0000** | **2.0000** | **PASS** |
| **80** | 0.0125 | 0.01953125 | 0.01953125 | **4.0000** | **2.0000** | **PASS** |

Doubling mesh density reduces midpoint error by exactly $4.0000\times$, confirming quadratic asymptotic convergence ($p = 2.0000$) with zero discrepancy.

---

## 6. Audit Note on Erroneous Handover Parameters

During the independent engineering audit (`docs/audits/FINAL_INDEPENDENT_AUDIT.md`), a previous chat summary was audited that referenced $k = 200, q = 5000$, and polynomial $T(x) = 100 - 92.5x - 12.5x^2$. 
1. Evaluating that polynomial at $x = 1.0\text{ m}$ gives $T(1.0) = 100 - 92.5 - 12.5 = -5.0^\circ\text{C}$, which violates the assigned Dirichlet boundary condition $T(1) = 20.0^\circ\text{C}$ by $25.0^\circ\text{C}$.
2. Evaluating theoretical midpoint error with $k = 200, q = 5000$ gives $\frac{q}{8k} = \frac{5000}{1600} = 3.125$, leading to $e(N=10) = 0.03125^\circ\text{C}$ (40 times smaller than the assigned $1.25^\circ\text{C}$).
3. Dedicated regression tests in [`tests/numerical/test_thermal_fea.cpp`](../../tests/numerical/test_thermal_fea.cpp) now explicitly test and reject both the erroneous polynomial and the incorrect parameters.
