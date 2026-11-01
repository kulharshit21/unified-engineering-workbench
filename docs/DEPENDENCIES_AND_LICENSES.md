# Dependencies and License Compliance Ledger

**Project**: PASS Science Works — Unified Engineering Workbench  

---

## 1. Direct and Indirect Dependencies

| Dependency | Exact Version | Official Source / URL | Purpose | License | Distribution & Legal Obligations | Alternatives Considered | Selection Rationale |
| :--- | :--- | :--- | :--- | :--- | :--- | :--- | :--- |
| **C++ Standard Library** | ISO C++17 (MSVC STL) | [Microsoft STL GitHub](https://github.com/microsoft/STL) | Data structures, algorithms, threads, file I/O, math | Apache 2.0 with LLVM Exception | Permissive; permits binary redistribution with notice | Boost, Abseil | Standard library provides zero runtime overhead and zero third-party licensing baggage. |
| **Qt Framework** | Qt 6.8.2 LTS | [Qt Official](https://www.qt.io/) | GUI widgets, OpenGL viewport, event loop, resources | LGPLv3 / Commercial | Dynamically linked DLLs; user can replace Qt libraries; copyright notice and license text included in package | wxWidgets, Dear ImGui, Electron | Qt provides high-DPI scaling, native Windows styling, robust OpenGL integration, and professional widgets. |
| **MSVC CRT** | MSVC 14.51 (2026/2022) | [Microsoft Visual C++](https://visualstudio.microsoft.com/) | C/C++ runtime support (`vcruntime140.dll`, etc.) | Microsoft Software License Terms (Redistributable) | Lawfully redistributable as application-local runtime binaries | MinGW-w64 runtime | Native MSVC toolchain is installed on the laptop, fully supported, and highly optimized for Windows 11. |
| **OpenGL API** | OpenGL 3.3 Core | [Khronos Group](https://www.khronos.org/opengl/) | Hardware-accelerated 3D rendering for STL and point cloud | Open Standard (OS Driver Interface) | Native system driver interface (`opengl32.dll`); no external license fee | Vulkan, DirectX 12 | OpenGL 3.3 is universally supported on Windows 11, rock solid on RTX 5070 Ti, and seamlessly integrated into `QOpenGLWidget`. |

---

## 2. Prohibited & Excluded Modules

1. **Qt Graphs / Qt DataVisualization**:
   * Evaluated and rejected due to potential GPLv3 copyleft restrictions and heavy dependencies. Replaced by a custom antialiased 2D plotting engine built with `QPainter`.
2. **CUDA / CuDNN**:
   * Excluded from numerical solving. The assignment explicitly prohibits substituting GPU compute for required CPU thermal benchmarks.
3. **External Matrix Algebra (Eigen, Armadillo, LAPACK)**:
   * Excluded. The 1D finite-element problem is solved directly via an exact, hand-coded Thomas algorithm tridiagonal solver, proving fundamental numerical competence.
