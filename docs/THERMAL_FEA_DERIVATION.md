# Mathematical Derivation: 1D Steady-State Thermal Finite Element Formulation

**Author**: Numerical Methods Researcher & Principal C++ Engineer  
**Project**: PASS Science Works — Unified Engineering Workbench (Task C)  

---

## 1. Strong Form of the Governing Equation

Consider steady-state, one-dimensional heat conduction along a uniform cylindrical rod of length $L$, cross-sectional area $A$, and constant thermal conductivity $k$, subjected to a uniform volumetric heat generation rate $q$. The lateral surface is perfectly insulated ($\frac{\partial T}{\partial r} = 0$).

The differential equation expressing conservation of thermal energy is:

$$-\frac{d}{dx}\left( k A \frac{dT}{dx} \right) = q A, \quad 0 < x < L$$

subject to essential (Dirichlet) boundary conditions at the ends:

$$T(0) = T_0 = 100^\circ\text{C}, \quad T(L) = T_L = 20^\circ\text{C}$$

Physical parameters:
* $L = 1.0\text{ m}$
* $A = 10^{-4}\text{ m}^2 = 0.0001\text{ m}^2$
* $k = 10.0\text{ W/(m}\cdot\text{K)}$
* $q = 10{,}000.0\text{ W/m}^3$

---

## 2. Weak Form Formulation (Galerkin Weighted Residuals)

Multiplying the governing equation by an arbitrary test function $v(x) \in H_0^1(0, L)$ (such that $v(0) = 0$ and $v(L) = 0$) and integrating over the domain $[0, L]$:

$$\int_0^L -v(x) \frac{d}{dx}\left( k A \frac{dT}{dx} \right) dx = \int_0^L v(x) q A \, dx$$

Applying integration by parts to the conduction term:

$$\left[ -v(x) k A \frac{dT}{dx} \right]_0^L + \int_0^L \frac{dv}{dx} k A \frac{dT}{dx} \, dx = \int_0^L v(x) q A \, dx$$

Since $v(0) = v(L) = 0$, the boundary term vanishes identically, giving the symmetric bilinear weak form:

$$\int_0^L k A \frac{dv}{dx} \frac{dT}{dx} \, dx = \int_0^L v(x) q A \, dx$$

---

## 3. Element Discretization and Shape Functions

Discretize $[0, L]$ into $N$ uniform elements of length $h = L/N$. Within element $e = [x_1, x_2]$ (where $x_2 - x_1 = h$), we interpolate the trial temperature field $T^e(x)$ and test function $v^e(x)$ using standard two-node linear Lagrange shape functions:

$$T^e(x) = N_1(x) T_1^e + N_2(x) T_2^e = \mathbf{N} \mathbf{T}^e$$

where:
$$N_1(x) = \frac{x_2 - x}{h}, \quad N_2(x) = \frac{x - x_1}{h}$$

Spatial derivatives are constant across the element:
$$\frac{dN_1}{dx} = -\frac{1}{h}, \quad \frac{dN_2}{dx} = \frac{1}{h}$$

### Elemental Stiffness Matrix $K_e$
$$\mathbf{K}_e = \int_{x_1}^{x_2} k A \left( \frac{d\mathbf{N}}{dx} \right)^T \left( \frac{d\mathbf{N}}{dx} \right) dx = k A \int_{x_1}^{x_2} \begin{bmatrix} -\frac{1}{h} \\ \frac{1}{h} \end{bmatrix} \begin{bmatrix} -\frac{1}{h} & \frac{1}{h} \end{bmatrix} dx$$

$$\mathbf{K}_e = \frac{k A}{h^2} \begin{bmatrix} 1 & -1 \\ -1 & 1 \end{bmatrix} (x_2 - x_1) = \frac{k A}{h} \begin{bmatrix} 1 & -1 \\ -1 & 1 \end{bmatrix}$$

### Elemental Load Vector $F_e$
$$\mathbf{F}_e = \int_{x_1}^{x_2} q A \mathbf{N}^T dx = q A \int_{x_1}^{x_2} \begin{bmatrix} \frac{x_2 - x}{h} \\ \frac{x - x_1}{h} \end{bmatrix} dx = \frac{q A h}{2} \begin{bmatrix} 1 \\ 1 \end{bmatrix}$$

---

## 4. Global Assembly and Boundary Value Elimination

Assembling all $N$ elements produces an $(N+1) \times (N+1)$ symmetric tridiagonal global system:

$$\mathbf{K} \mathbf{T} = \mathbf{F}$$

For interior node $i \in \{1, 2, \dots, N-1\}$:
$$-\frac{kA}{h} T_{i-1} + \frac{2kA}{h} T_i - \frac{kA}{h} T_{i+1} = q A h$$

Dividing through by $\frac{kA}{h}$:
$$-T_{i-1} + 2 T_i - T_{i+1} = \frac{q h^2}{k}$$

Substituting Dirichlet boundary values $T_0 = 100^\circ\text{C}$ and $T_N = 20^\circ\text{C}$ shifts the known boundary terms to the RHS:
* For $i = 1$: $2 T_1 - T_2 = \frac{q h^2}{k} + T_0$
* For $i = N-1$: $-T_{N-2} + 2 T_{N-1} = \frac{q h^2}{k} + T_N$

This reduced interior system of order $(N-1)$ is strictly symmetric, diagonally dominant, and solved in $O(N)$ operations using the **Thomas algorithm (TDMA)**.

---

## 5. Analytical Reference Solution

Integrating the strong form directly:
$$-\frac{d^2 T}{dx^2} = \frac{q}{k}$$
$$\frac{dT}{dx} = -\frac{q}{k} x + C_1$$
$$T(x) = -\frac{q}{2k} x^2 + C_1 x + C_2$$

Applying $T(0) = T_0 \implies C_2 = T_0$.  
Applying $T(L) = T_L \implies T_L = -\frac{q L^2}{2k} + C_1 L + T_0 \implies C_1 = \frac{T_L - T_0}{L} + \frac{q L}{2k}$.

Substituting back gives the exact closed-form solution:
$$T_\text{exact}(x) = T_0 - (T_0 - T_L)\frac{x}{L} + \frac{q}{2k} x (L - x)$$

With specified values:
$$T_\text{exact}(x) = 100 - 80 x + 500 x (1 - x) = 100 + 420 x - 500 x^2$$

---

## 6. Mathematical Origin of Midpoint Errors and Quadratic Convergence

Because 1D linear finite elements possess the **superconvergence** property for constant coefficients, the finite element solution at the mesh vertices is **exact**:
$$T_i = T_\text{exact}(x_i) \quad \forall i = 0, \dots, N$$

Consequently, evaluating error exclusively at the nodal points yields $0.0$, which obscures the true discretization error!

Between nodes $x_i$ and $x_{i+1}$, the exact solution is a quadratic parabola, while the FE solution is a straight secant line. At the element midpoint $x_\text{mid} = x_i + \frac{h}{2}$:
$$T_\text{FE}(x_\text{mid}) = \frac{T_i + T_{i+1}}{2}$$
$$T_\text{exact}(x_\text{mid}) - T_\text{FE}(x_\text{mid}) = \frac{q}{8k} h^2$$

For $q = 10{,}000\text{ W/m}^3$ and $k = 10\text{ W/(m}\cdot\text{K)}$:
$$e_\text{mid} = \frac{10000}{8 \times 10} h^2 = 125 h^2$$

Since $h = 1/N$:
* $N = 10 \implies h = 0.1 \implies e_\text{mid} = 125(0.01) = \mathbf{1.25000000^\circ\text{C}}$
* $N = 20 \implies h = 0.05 \implies e_\text{mid} = 125(0.0025) = \mathbf{0.31250000^\circ\text{C}}$
* $N = 40 \implies h = 0.025 \implies e_\text{mid} = 125(0.000625) = \mathbf{0.07812500^\circ\text{C}}$
* $N = 80 \implies h = 0.0125 \implies e_\text{mid} = 125(0.00015625) = \mathbf{0.01953125^\circ\text{C}}$

The error scales as $O(h^2)$, decreasing by exactly $4.0000\times$ every time the mesh element count doubles.
