# Data Directory and Provenance Notice

**IMPORTANT COMPLIANCE NOTICE**:
All test data files included in this directory are **SYNTHETIC DATA — NOT AN ACTUAL ANSYS EXPORT OR COMMERCIAL CAD MODEL**. They were created from first mathematical principles specifically for testing the PASS Science Works Unified Engineering Workbench.

---

## Included Datasets

### 1. `data/geometry/sample_bracket.stl`
* **Format**: Standard ASCII STL (3D triangular facets with unit outward normals).
* **Subject**: Solid 3D structural bracket block ($100 \times 50 \times 50\text{ mm}$).
* **Units**: Unitless in STL standard; interpreted by Workbench as meters ($[0, 0.1] \times [0, 0.05] \times [0, 0.05]\text{ m}$).
* **Facet Count**: 12 triangular facets (36 vertices).
* **Provenance**: Self-authored analytical parametric geometry under MIT License.
* **Limitations**: Maximum recommended triangle count for the software is 500,000 facets. Binary STL is rejected per assignment rules.

### 2. `data/thermal/synthetic_ansys_temperatures.csv`
* **Format**: Standard RFC 4180 Comma-Separated Values (CSV).
* **Schema**: `node_id,x_m,y_m,z_m,temperature_C`.
* **Node Count**: 27 nodes structured as a $3 \times 3 \times 3$ volumetric grid matching the bracket extents.
* **Temperature Range**: $T_\text{min} = 20.0^\circ\text{C}$ to $T_\text{max} = 100.0^\circ\text{C}$, Mean = $60.0^\circ\text{C}$.
* **Provenance**: Synthetically evaluated using linear thermal conduction gradient:
  $$T(x, y, z) = 100.0 - 150.0 x - 400.0 y - 1000.0 z$$
* **Limitations**: Demonstrates 3D point cloud visualization, spatial bounding, colormap legend mapping, and interactive node inspection. Does not claim physical registration with the STL geometry.
