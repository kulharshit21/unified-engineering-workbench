# Data Flow & Execution Pipelines

This document details the primary data transformation pipelines implemented in the **PASS SCIENCE WORKS Unified Engineering Workbench**.

---

## 1. 3D Geometry & ANSYS CSV Import Pipeline (Diagram C)

The import pipeline transforms raw ASCII disk files into interactive, GPU-rendered 3D entities:

```mermaid
flowchart TD
    subgraph Disk ["File System Storage"]
        F_STL["ASCII STL File<br/>(sample_bracket.stl)"]
        F_CSV["ANSYS CSV File<br/>(synthetic_ansys_temperatures.csv)"]
    end

    subgraph CoreParsing ["WorkbenchCore Parsers"]
        P_STL["StlParser::parseFile()<br/>- 'solid' header tokenization<br/>- Facet loop extraction<br/>- Vertex normal cross-product<br/>- Bounding box computation"]
        P_CSV["TemperatureCsvParser::parseFile()<br/>- Schema verification: 5 columns<br/>- NaN / Inf rejection<br/>- Duplicate node ID deduplication<br/>- Min/Max temperature calculation"]
    end

    subgraph DataStructs ["In-Memory Data Models"]
        M_MESH["Mesh Model<br/>std::vector<Triangle><br/>BoundingBox (min/max extents)"]
        M_CLOUD["TemperatureDataset<br/>std::vector<TemperatureNode><br/>minTemp, maxTemp, span"]
    end

    subgraph RenderEngine ["OpenGLViewport (QOpenGLWidget)"]
        CAM["Camera Transform Matrix<br/>LookAt, Perspective, Orbit/Pan"]
        TRI_SORT["Painter's Depth Sorting<br/>std::sort by camera distance"]
        COLOR["Turbo / Jet Colormapper<br/>Normalized t = (T - Tmin)/(Tmax - Tmin)<br/>RGB polynomial evaluation"]
        DRAW["Hardware-Accelerated Render<br/>- Shaded geometry facets<br/>- Scalar-colored point cloud<br/>- Cartesian tripod overlay<br/>- Legend & temperature scale"]
    end

    F_STL --> P_STL --> M_MESH --> CAM --> TRI_SORT --> DRAW
    F_CSV --> P_CSV --> M_CLOUD --> COLOR --> DRAW
```

---

## 2. Thermal FEA Computational Pipeline (Diagram D)

The 1D thermal finite element solver discretizes the continuous boundary value problem into a linear system:

```mermaid
flowchart LR
    subgraph InputParameters ["Input Parameters (Task C)"]
        P["L = 1.0 m, A = 1e-4 m²<br/>k = 10.0 W/(m·K)<br/>q = 10000.0 W/m³<br/>T(0) = 100°C, T(L) = 20°C"]
    end

    subgraph Discretization ["Mesh Generation"]
        MESH["Mesh Discretization<br/>N uniform elements<br/>h = L / N<br/>Node coordinates: x_i = i * h"]
    end

    subgraph LocalElement ["Local Element Formulation"]
        LOCAL["Elemental Matrices:<br/>K_e = (k·A/h) * [[1,-1],[-1,1]]<br/>F_e = (q·A·h/2) * [1, 1]^T"]
    end

    subgraph AssemblySystem ["Global System Assembly"]
        GLOBAL["Assemble Tridiagonal System:<br/>d_i = 2, a_i = -1, c_i = -1<br/>RHS_i = (q·h²/k)<br/>Dirichlet BC: RHS_1 += T0, RHS_M += TL"]
    end

    subgraph LinearSolver ["Thomas Algorithm TDMA"]
        TDMA["O(N) Direct Forward Elimination<br/>& Backward Substitution<br/>Zero Dynamic Allocation"]
    end

    subgraph PostProcessing ["Verification & Convergence"]
        POST["Evaluate Midpoint Interpolation:<br/>T_fem(x_mid) = 0.5*(T_i + T_{i+1})<br/>Compare with T_exact(x_mid)<br/>Verify error = q·h² / (8k)"]
    end

    P --> MESH --> LOCAL --> GLOBAL --> TDMA --> POST
```

---

## 3. Multicore CPU Concurrency Pipeline (Diagram E)

The multicore performance harness demonstrates how independent FEA batch cases are distributed across worker threads:

```mermaid
flowchart TD
    subgraph Dispatch ["Task Generation & Scheduling"]
        GEN["BatchGenerator<br/>Generates M independent cases<br/>Deterministic PRNG (Seed 42)"]
        QUEUE["Concurrent Job Queue<br/>Thread-safe std::queue<br/>std::mutex + std::condition_variable"]
    end

    subgraph WorkerPool ["Bounded ThreadPool (Workers p = 1..N)"]
        W1["Worker Thread 1<br/>Pops case from queue<br/>Executes Thomas TDMA"]
        W2["Worker Thread 2<br/>Pops case from queue<br/>Executes Thomas TDMA"]
        WP["Worker Thread p<br/>Pops case from queue<br/>Executes Thomas TDMA"]
    end

    subgraph ResultAggregation ["Deterministic Collection"]
        RES["Thread-Safe Result Vector<br/>Indexed by case ID<br/>Preserves input-to-output ordering"]
        BENCH["Benchmark Analyzer<br/>- Computes median, mean, min, max (5 runs)<br/>- Computes speedup S_p = T_1 / T_p<br/>- Computes efficiency E_p = S_p / p<br/>- Verifies max delta vs serial <= 1e-8°C"]
    end

    GEN --> QUEUE
    QUEUE --> W1 --> RES
    QUEUE --> W2 --> RES
    QUEUE --> WP --> RES
    RES --> BENCH
```
