# Data Provenance and Synthetic Data Notice

**Project**: PASS Science Works — Unified Engineering Workbench  
**Compliance**: Transparent synthetic dataset declaration per Section 3 & Task B.

---

## 1. Explicit Disclosure
All geometry and nodal temperature datasets distributed with this repository are:

> **SYNTHETIC DATA — NOT AN ACTUAL ANSYS EXPORT OR COMMERCIAL CAD FILE**

No proprietary commercial ANSYS results, proprietary CAD solid models, or third-party intellectual property were utilized or reproduced.

---

## 2. Dataset Specifications

### A. ASCII STL Geometry (`data/geometry/sample_bracket.stl`)
* **Generation Method**: Synthesized using exact vertex coordinates of an orthogonal structural bracket.
* **Format**: Standard ASCII format adhering to 3D Systems specification (`solid <name> ... facet normal nx ny nz ... vertex x y z ... endloop ... endfacet ... endsolid`).
* **Units**: STL files are dimensionless; coordinates are treated in SI meters ($[0.0, 0.1]\text{ m}$).
* **Integrity Checks**:
  * Outward-pointing normal vectors verified.
  * Right-hand rule vertex ordering verified.
  * Manifold closure verified across 12 triangular facets.
* **Limitations**: Documented tested limit is up to 500,000 triangles. Files exceeding 100 MB or binary format trigger graceful rejection.

### B. ANSYS Nodal Temperature Export (`data/thermal/synthetic_ansys_temperatures.csv`)
* **Generation Method**: Synthesized across a regular $3 \times 3 \times 3$ grid spanning $x \in [0, 0.1]\text{ m}$, $y \in [0, 0.05]\text{ m}$, $z \in [0, 0.05]\text{ m}$.
* **Equation Used**:
  $$T(x, y, z) = 100.0 - 150.0 x - 400.0 y - 1000.0 z$$
  yielding physical temperatures bounded in $[20.0^\circ\text{C}, 100.0^\circ\text{C}]$.
* **Schema**:
  `node_id,x_m,y_m,z_m,temperature_C`
* **Handling Policy**:
  * Headers: Verified case-insensitively.
  * Coordinate & Temperature Data: Validated for IEEE 754 finite values (rejecting NaN, Inf).
  * Duplicate Node IDs: Retain first occurrence, record count in dataset statistics, log warning.
  * Degenerate Temperature Ranges ($T_\text{min} == T_\text{max}$): Safely maps to midpoint color (0.5) avoiding division by zero.
