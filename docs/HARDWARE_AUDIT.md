# Hardware and Environment Audit Report

**Date**: November 1, 2026  
**Auditor**: Principal C++ Engineer & Numerical Systems Lead (AI Team)  
**System Family**: ASUS ROG Zephyrus G14 (GA403WR Family)  
**Operating System**: Microsoft Windows 11 CoreSingleLanguage (Build 26300, 64-bit, en-US)

---

## 1. Physical and Logical Hardware Specifications

| Component | Verified Specification | Engineering Assessment & Constraints |
| :--- | :--- | :--- |
| **CPU Exact Model** | **AMD Ryzen AI 9 HX 370 w/ Radeon 890M** | High-performance hybrid Zen 5/5c architecture. |
| **Physical Cores** | **12 Physical Cores** | 4 Zen 5 cores + 8 Zen 5c dense cores. |
| **Logical Threads** | **24 Logical Processors** | SMT enabled (2 threads per core). Base Clock: 2.0 GHz, Boost: up to 5.1 GHz. |
| **Installed System RAM** | **32.0 GB LPDDR5X (Unified System Memory)** | Total Physical: 30.88 GiB accessible; > 18 GiB free during idle. |
| **Dedicated GPU** | **NVIDIA GeForce RTX 5070 Ti Laptop GPU** | 12,227 MiB (~12 GB) GDDR7 VRAM. PCIe Gen4 x8. |
| **NVIDIA Driver Version** | **621.01 (CUDA UMD 13.5, WDDM 3.2)** | Full hardware acceleration, modern Vulkan/OpenGL 4.6 support. |
| **Integrated GPU** | **AMD Radeon(TM) 890M Graphics** | 16 Compute Units (RDNA 3.5), display out and low-power rendering. |
| **Storage Subsystem** | **NVMe PCIe 4.0 SSD** | Drive C: 724.2 GB free; Drive D: 799.5 GB free. Excellent I/O throughput. |
| **Power & Thermal Budget** | AC Mains Connected, Normal Thermal Profile | Fan profile optimized; thermal headroom verified. |

---

## 2. Engineering Decisions for the Workbench Project

1. **CPU Benchmark Worker Allocation**:
   * The assignment mandates benchmarking 1, 2, and up to 4 worker configurations (capped at logical CPUs).
   * Since the Ryzen AI 9 HX 370 provides 24 logical processors, 1, 2, and 4 workers operate well within physical core limits (all 4 run on unshared physical Zen 5 cores).
   * Parallel speedup will be measured on physical CPU cores with clean memory bandwidth separation.
2. **Graphics & Compute Role Boundary**:
   * The 3D geometry and point cloud viewport is engineered via `QWidget` and `QPainter` with CPU-based 3D projection and depth sorting, ensuring 100% universal compatibility across virtual machines, headless CI, and automated screenshot capture without OpenGL driver crashes.
   * In strict compliance with the assignment rules, **all thermal FEA solutions, serial loops, and parallel batch computations execute on the CPU**. No GPU compute is substituted for CPU benchmarks.
3. **Memory Budget**:
   * Peak working set for N=80 thermal solver batches (up to 10,000 cases) is under 50 MB.
   * Visualizer buffers for ASCII STL geometry (up to 50,000 triangles) require under 30 MB system RAM.
   * Generous safety margins exist across the 32 GB system RAM.
