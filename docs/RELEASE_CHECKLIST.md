# Standalone Release Verification Checklist

**Target Deliverable**: `CandidateID_Workbench.zip` (Windows 11 x64 Standalone Executable)  
**Package Pipeline**: `scripts/package_msvc.cmd`  
**Checksum Receipt**: `release/CandidateID_Workbench.sha256`

---

## 1. Release Verification Checklist

- [x] **Release Binary Compilation**:
  - `WorkbenchCore.lib`, `WorkbenchApp.exe`, `Launch.exe`, and 7 test executables compiled with `/O2 /W4 /permissive-` in MSVC Release mode.
  - Zero compiler errors, zero linker warnings.

- [x] **Automated Regression Suite**:
  - All 7 CTest targets executed via `scripts/test_msvc.cmd Release`:
    - `test_smoke.exe`: **Passed** (0.98s)
    - `test_stl_parser.exe`: **Passed** (1.00s)
    - `test_csv_parser.exe`: **Passed** (0.97s)
    - `test_thermal_fea.exe`: **Passed** (0.95s)
    - `test_parallel_batch.exe`: **Passed** (1.26s)
    - `test_embedded_mocks.exe`: **Passed** (0.90s)
    - `test_backprop.exe`: **Passed** (0.89s)
  - 100% pass rate recorded.

- [x] **Runtime Dependency Audit (windeployqt)**:
  - Deployed Qt 6 libraries: `Qt6Core.dll`, `Qt6Gui.dll`, `Qt6Widgets.dll`, `Qt6OpenGL.dll`, `Qt6OpenGLWidgets.dll`, `Qt6Network.dll`, `Qt6Svg.dll`.
  - Deployed Qt platform plugin: `bin/platforms/qwindows.dll`.
  - Deployed MSVC CRT runtime: `bin/msvcp140.dll`, `bin/vcruntime140.dll`.

- [x] **Clean Out-of-Tree Execution Test**:
  - Extracted archive staged in `dist/UnifiedEngineeringWorkbench/`.
  - Executed native Win32 `Launch.exe` directly outside the build tree.
  - Confirmed `Launch.exe` spawns `bin/WorkbenchApp.exe` (PID 38688 verified) with zero terminal prompts.
  - Confirmed offline execution with zero internet connectivity required.

- [x] **Repository Bundle & Commit History**:
  - Generated offline Git history bundle: `repository_bundle.bundle` included in archive root.
  - All 9 milestone commits present and verifiable offline via `git clone repository_bundle.bundle`.

- [x] **Complete Documentation & Deliverables**:
  - Bundled `docs/`, `src/`, `tests/`, `data/`, `results/`, `CMakeLists.txt`, `README.md`, `LICENSE`.
  - Included 6-page comprehensive technical report (`docs/reports/FINAL_TECHNICAL_REPORT.md` and `.pdf`).

- [x] **Cryptographic Checksum Verification**:
  - Archive: `CandidateID_Workbench.zip` ($29.80\text{ MB}$).
  - SHA256: `FD7036A55F1EE3C911D5253F54BEBD8E8CBB94C49C5992878D8D4E4F4D16FC2E`.
