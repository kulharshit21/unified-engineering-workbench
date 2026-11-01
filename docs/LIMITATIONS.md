# Known Engineering Limitations & Physical Assumptions

In accordance with the PASS SCIENCE WORKS internship principles of empirical rigor and transparent engineering, this document details the architectural boundaries, physical assumptions, and operational limitations of the Unified Engineering Workbench.

---

## 1. Thermal Finite Element Analysis (Task C)

1. **Dimensionality & Steady-State Assumption**:
   - The FEA solver is strictly formulated for 1D steady-state heat conduction along a uniform rod. Transient effects ($\rho c_p \frac{\partial T}{\partial t}$) and multidimensional thermal gradients ($\nabla^2 T$ in 2D/3D) are not modeled in this solver.
2. **Material Homogeneity**:
   - Thermal conductivity $k$ and cross-sectional area $A$ are assumed constant along the rod length $x \in [0, L]$. Piecewise variation or temperature-dependent nonlinear conductivity $k(T)$ is not currently supported.
3. **Lateral Insulation**:
   - The lateral boundary surface of the rod is assumed perfectly insulated ($\frac{dT}{dr} = 0$). Convective or radiative heat loss to the ambient environment along the rod perimeter is excluded from the 1D model.

---

## 2. CAD Geometry & ANSYS Data (Task B)

1. **ASCII STL Parsing Throughput**:
   - The geometry engine natively parses ASCII STL files. While optimal for readability, portability, and validation, ASCII text decoding (`std::stod`) has higher parsing overhead than raw binary STL streams. Tested and recommended models range up to 500,000 facets; beyond this, file I/O loading times increase linearly.
2. **CAD Units Ambiguity**:
   - The STL format does not encode unit metadata (e.g. millimeters vs meters). The workbench assumes coordinates correspond to meters for coordinate inspection.
3. **Point Cloud Co-registration**:
   - Nodal temperature CSV data and ASCII STL geometry represent independent spatial datasets. The workbench does not claim an arbitrary external point cloud is geometrically co-registered with the CAD solid unless generated from the same underlying geometry.

---

## 3. Multicore CPU Parallel Benchmarks (Task D)

1. **Micro-Workload Thread Dispatch Overhead**:
   - Solving 1,000 tridiagonal systems of size $80 \times 80$ in optimized C++ takes less than $2\text{ ms}$ total serial wall-clock time on modern Zen 5 / AMD Ryzen architectures.
   - At this sub-millisecond execution scale, thread creation, condition-variable notification, and mutex contention introduce bounded overhead, resulting in $1.65\times$ speedup on 4 workers rather than ideal $4\times$. Larger workloads or higher-order 2D/3D systems amortize this overhead significantly further.

---

## 4. Embedded Target Integration (Task E)

1. **Simulation Disclosure**:
   - Target devices (Raspberry Pi Pico, Digilent Arty A7-35T, and Digilent JTAG-HS2) are high-fidelity C++ mock adapters running in host user-space.
   - Physical FTDI D2XX USB drivers and COM port access are not engaged during automated unit testing to guarantee relocatable, offline pass rates without laboratory bench hardware attached.
2. **JTAG Capability Scope**:
   - The JTAG adapter is treated as a configuration, boundary-scan, and runtime debug interface, not an interchangeable compute coprocessor.

---

## 5. Neural Backpropagation Engine (Task F)

1. **Topology Scope**:
   - Formulated specifically for the assigned single-input, single-hidden-unit ($\tanh$), single-output architecture.
   - The solver is designed for exact calculus verification and finite-difference gradient checking, not general-purpose deep learning tensor operations.
