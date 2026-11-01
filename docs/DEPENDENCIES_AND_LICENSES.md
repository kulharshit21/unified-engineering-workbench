# Open-Source Dependencies, Licensing & Third-Party Disclosures

## 1. Project Licensing

The source code of the **PASS SCIENCE WORKS Unified Engineering Workbench** is licensed under the permissive **MIT License**:

```
MIT License

Copyright (c) 2026 PASS SCIENCE WORKS Candidate

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
```

---

## 2. Third-Party Dependencies & Compliance Matrix

| Component / Library | Version | License | Linkage | Compliance Mechanism |
|:---|:---:|:---:|:---:|:---|
| **C++ Standard Library** | C++17 (ISO/IEC 14882:2017) | Microsoft MSVC Runtime EULA | Dynamic CRT (`vcruntime140.dll`, `msvcp140.dll`) | Official redistributable binaries bundled in distribution |
| **Qt Framework** | Qt 6.8.2 | GNU LGPLv3 / Qt Commercial | Dynamic Linkage (`Qt6Core.dll`, `Qt6Gui.dll`, etc.) | Shared DLLs dynamically linked; users may replace DLLs per LGPLv3 §4; source available from The Qt Company |
| **CMake Build System** | 4.2.3 | BSD 3-Clause License | Build-time Tool | Permissive open-source license; build scripts included |
| **Ninja Build System** | 1.13 | Apache License 2.0 | Build-time Tool | Permissive build tool |
| **Python Tooling** | Python 3.10 / 3.12 | PSF License | Offline Documentation Only | Not required for end-user execution or runtime launch |
| **Matplotlib** | 3.8+ | BSD-Compatible (PSF) | Offline Plot Generation | Used only to generate publication figures from CSVs |
| **NumPy** | 1.26+ | BSD 3-Clause License | Offline Plot Generation | Numerical support for plotting scripts |

---

## 3. Synthetic Benchmark Datasets

All sample geometry files and thermal CSV point clouds bundled in this repository:
- `data/geometry/sample_bracket.stl`
- `data/thermal/synthetic_ansys_temperatures.csv`
are **100% synthetically created procedural datasets** published under Creative Commons Zero (CC-0) / Public Domain terms. They contain zero proprietary commercial data, confidential customer IP, or export-controlled information.
