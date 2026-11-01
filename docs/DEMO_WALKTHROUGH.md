# PASS Science Works — Operator Demonstration Walkthrough

**Target**: 3–5 Minute Evaluator Demonstration of the Unified Engineering Workbench  
**Host Environment**: Windows 11 x64  
**Prerequisites**: Extract `CandidateID_Workbench.zip` to any local folder (e.g. `C:\Users\<user>\Desktop\Workbench\`). Double-click `Launch.exe`.

---

## 1. Launch & Initialization (0:00 – 0:30)

1. Double-click `Launch.exe` from the unzipped release folder.
2. Observe that the application starts immediately without displaying a terminal console, command prompt, Python prompt, or requiring administrative privileges.
3. The application window appears titled:
   `PASS Science Works — Unified Engineering Workbench v1.0.0-rc1`.
4. Observe the bottom status bar displaying:
   `System Operational | All Modules Loaded | Standalone Offline Mode`.

---

## 2. Tab 1: Geometry & ANSYS 3D Viewport (0:30 – 1:15)

1. **Automatic Synthetic Data**:
   - The workbench automatically loads `sample_bracket.stl` and `synthetic_ansys_temperatures.csv`.
   - The statistics table on the left immediately displays:
     - STL Triangles: `12`
     - Bounding Extent: `0.100 m`
     - Thermal Nodes: `27`
     - Temperature Range: `[20.00 °C, 100.00 °C]` (Mean: `57.69 °C`)
2. **Interactive 3D Navigation**:
   - **Rotate**: Click and drag the left mouse button to rotate the bracket.
   - **Zoom**: Scroll the mouse wheel to zoom smoothly in and out.
   - **Pan**: Click and drag the middle/right mouse button to translate the camera.
   - **Reset**: Click the `Reset / Fit Camera View` button to re-center the model.
3. **Display Toggles**:
   - Toggle `Show STL Solid Mesh`, `Show Mesh Wireframe`, and `Show Thermal Point Cloud`.
   - Adjust the `Node Point Size` slider from 3 to 16.
4. **Node Inspection**:
   - Click directly on any colored node in the 3D viewport.
   - Observe the top banner and left panel updating to display the selected node:
     `Selected Node #14 | Position: (0.050, 0.050, 0.000) m | Temperature: 60.00 °C`.

---

## 3. Tab 2: 1D Steady-State Thermal FEA (1:15 – 2:00)

1. Switch to the **2. Thermal FEA Analysis** tab.
2. Note default parameters matching assignment brief:
   - $L = 1.0\text{ m}, A = 10^{-4}\text{ m}^2, k = 10.0\text{ W/(m}\cdot\text{K)}, q = 10,000\text{ W/m}^3, T(0) = 100^\circ\text{C}, T(L) = 20^\circ\text{C}$.
3. Click `Run Convergence Study`:
   - The right `PlotWidget` displays the smooth continuous blue analytical curve $T(x)$ and dashed orange FEA nodal solution.
   - The convergence table immediately reports the exact theoretical and observed midpoint errors:
     - $N=10 \to 1.250000^\circ\text{C}$
     - $N=20 \to 0.312500^\circ\text{C}$ ($4.00\times$ reduction, $p=2.0000$)
     - $N=40 \to 0.078125^\circ\text{C}$ ($4.00\times$ reduction, $p=2.0000$)
     - $N=80 \to 0.019531^\circ\text{C}$ ($4.00\times$ reduction, $p=2.0000$)
4. Click `Export Solution & Convergence CSV...` to save underlying CSV data.

---

## 4. Tab 3: Multicore CPU Parallel Benchmarks (2:00 – 2:45)

1. Switch to the **3. Multicore Performance** tab.
2. Observe system hardware detection: Host CPU detected with 24 logical threads.
3. Click `Execute Parallel Benchmark`:
   - Solves 1,000 independent rod cases ($N=80$) across 1, 2, and 4 worker threads over 5 repetitions.
   - The UI remains completely responsive during computation.
   - The results table reports:
     - 1 Worker: $\sim 1.91\text{ ms}$ ($1.00\times$, $100\%$ efficiency)
     - 2 Workers: $\sim 1.72\text{ ms}$ ($1.11\times$ speedup)
     - 4 Workers: $\sim 1.16\text{ ms}$ ($1.65\times$ speedup)
     - Max Delta vs Serial: `0.00e+00 °C` (strict numerical agreement verified $\le 10^{-8\circ}\text{C}$).
   - The speedup plot visualizes measured green scaling curve against the ideal dashed linear curve.

---

## 5. Tab 4: Embedded Integration & Fault Recovery (2:45 – 3:30)

1. Switch to the **4. Embedded Integration** tab.
2. Observe 3 device cards:
   - **Raspberry Pi Pico (RP2040)**: `CONNECTED`, USB CDC-ACM (COM3), internal temp $\approx 32.5^\circ\text{C}$, supply $3.31\text{V}$, core $1.10\text{V}$.
   - **Digilent Arty A7 (Artix-7)**: `CONNECTED`, FT2232HQ UART/AXI (COM5), die temp $\approx 41.8^\circ\text{C}$, core $1.00\text{V}$ (VCCINT).
   - **Digilent JTAG-HS2**: `CONNECTED`, USB 2.0 (FT232H), VREF sensed $3.31\text{V}$, 30 MHz.
   - Note the blue badge: `MOCK ADAPTER SIMULATION (Zero-Fabrication Disclosed)`.
3. **Simulate Recoverable Failure**:
   - Check the box `Inject FPGA Timeout Fault`.
   - Observe the Arty A7 card immediately transition from `CONNECTED` (green) to `TIMED OUT` (amber) with log message:
     `[HH:mm:ss] FAULT INJECTION ACTIVATED: Simulating communication timeout on FPGA-ARTY-A7-002.`
4. **Trigger Automatic Recovery**:
   - Click `Attempt Recovery`.
   - Observe the 3-step recovery sequence re-establish the communication link, returning the badge to `CONNECTED` (green).

---

## 6. Tab 5: Backpropagation & Finite Differences (3:30 – 4:15)

1. Switch to the **5. Backpropagation** tab.
2. Note input parameters: $x=1.0, y=2.0, w=0.5, b=0.0, v=1.0, c=0.0, \eta=0.1, \text{Steps}=100$.
3. Click `Verify Gradients (ε=1e-6)`:
   - Table displays analytical vs numerical finite-difference gradients for $w, b, v, c$.
   - Absolute errors are $\approx 1.1 \times 10^{-10}$, displaying bright green `PASS` badges ($< 10^{-5}$ threshold).
4. Click `Run Training`:
   - The loss curve plots exponential decay from initial loss $1.182552 \to 1.70 \times 10^{-20}$.
   - Final predicted $\hat{y} = 2.000000$ exactly matches target $y=2.0$.
   - The training history table lists parameter updates at every step.
5. Click `Export Loss & Gradient CSVs...` to save results.

---

## 7. Shutdown (4:15 – 4:30)

1. Close the application via `File -> Exit` or the window close box.
2. Verify that all processes exit cleanly with zero orphaned background tasks.
