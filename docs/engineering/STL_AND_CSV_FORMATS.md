# 3D Geometry (STL) and ANSYS Nodal Temperature (CSV) Specifications (Task B)

## 1. ASCII STL Geometry Specification

The workbench imports 3D surface meshes encoded in standard ASCII Stereolithography (STL) format.

### 1.1 Format Grammar
```
solid <mesh_name>
  facet normal <n_i> <n_j> <n_k>
    outer loop
      vertex <x_1> <y_1> <z_1>
      vertex <x_2> <y_2> <z_2>
      vertex <x_3> <y_3> <z_3>
    endloop
  endfacet
  ...
endsolid <mesh_name>
```

### 1.2 Parsing & Normal Recalculation Rules
1. **Header Validation**: File must begin with the keyword `solid`.
2. **Facet Extraction**: Exactly three vertices per facet are parsed. Files with incomplete facets (e.g., 2 vertices) are rejected with a line-numbered `std::runtime_error`.
3. **Robust Normal Recalculation**: STL files frequently contain degenerate or unnormalized facet normals $(0, 0, 0)$. The parser automatically recalculates the true geometric outward normal via the right-handed vector cross-product:
   $$\mathbf{e}_1 = \mathbf{v}_2 - \mathbf{v}_1, \quad \mathbf{e}_2 = \mathbf{v}_3 - \mathbf{v}_1$$
   $$\mathbf{n} = \frac{\mathbf{e}_1 \times \mathbf{e}_2}{\|\mathbf{e}_1 \times \mathbf{e}_2\|}$$
4. **Bounding Box & Centering**: During parsing, extrema $[x_\text{min}, x_\text{max}]$, $[y_\text{min}, y_\text{max}]$, and $[z_\text{min}, z_\text{max}]$ are computed to derive the centroid and bounding diagonal $D = \sqrt{\Delta x^2 + \Delta y^2 + \Delta z^2}$. This enables one-click "Fit-to-View" camera repositioning in the 3D viewport.
5. **Binary STL Rejection**: Binary STL files (non-ASCII byte distributions or null bytes in header) are detected and rejected with an explicit error message prompting the user to supply ASCII STL.

---

## 2. ANSYS-Compatible Nodal Temperature CSV Specification

The thermal visualization module parses ANSYS nodal temperature export datasets to project scalar thermal fields onto the 3D workspace.

### 2.1 File Schema
The CSV file must contain a single header row matching the following column schema:
```csv
node_id,x_m,y_m,z_m,temperature_C
1,0.000000,0.000000,0.000000,100.000000
2,0.050000,0.000000,0.000000,92.500000
...
```

| Column Index | Field Name | Data Type | Units | Description |
|:---:|:---|:---:|:---:|:---|
| 0 | `node_id` | Integer / String | — | Unique finite element node identifier |
| 1 | `x_m` | Floating-point (IEEE 754) | Metres (m) | Cartesian X coordinate |
| 2 | `y_m` | Floating-point (IEEE 754) | Metres (m) | Cartesian Y coordinate |
| 3 | `z_m` | Floating-point (IEEE 754) | Metres (m) | Cartesian Z coordinate |
| 4 | `temperature_C` | Floating-point (IEEE 754) | Degrees Celsius (°C) | Steady-state scalar temperature |

### 2.2 Defensive Validation & Rejection Policies
1. **Schema Check**: Any CSV file with fewer or greater than 5 columns or incorrect column names is immediately rejected.
2. **Non-Finite Value Rejection**: Records containing `NaN`, `Inf`, or non-numeric tokens are rejected with the specific line number.
3. **Duplicate Node ID Policy**: If a duplicate `node_id` is encountered, the parser retains the first occurrence, records the duplicate in `duplicateCount`, and warns the user without crashing.
4. **Degenerate Span Protection**: If $T_\text{min} == T_\text{max}$ (isothermal field), normalized temperature $t = \frac{T - T_\text{min}}{T_\text{max} - T_\text{min}}$ avoids division by zero by clamping to $0.5$ (mid-range color).

---

## 3. Data Provenance & Synthetic Benchmark Labeling

In compliance with the internship assignment rules:
- All datasets located in `data/geometry/sample_bracket.stl` and `data/thermal/synthetic_ansys_temperatures.csv` are **100% synthetically generated benchmarks** created under MIT / CC-0 open terms.
- No proprietary CAD models, ITAR-controlled geometry, or confidential commercial FEM meshes are bundled.
- Supported file size limits: The ASCII parser supports files up to 25 MB (~50,000 triangles) smoothly in real-time. For larger industrial meshes, level-of-detail (LOD) subsampling is recommended.
