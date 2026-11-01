# Hardware Safety, Operating Limits, and Simulation Disclosures (Task E)

## 1. Absolute Maximum Ratings and Safe Operating Limits

When connecting physical target hardware to the Unified Engineering Workbench testbench, the following physical limits must be strictly maintained:

| Parameter | Digilent Arty A7 (XC7A35T) | Raspberry Pi Pico (RP2040) | Digilent JTAG-HS2 |
|:---|:---:|:---:|:---:|
| **Max Supply Voltage ($V_\text{BUS}$)** | 5.50 V | 5.50 V | 5.25 V |
| **Max I/O Bank Voltage ($V_\text{CCO}$)** | 3.60 V (LVCMOS33) | 3.63 V | Sensed 1.8V to 5.0V |
| **Max Core Voltage ($V_\text{CCINT}$)** | 1.05 V (Nominal 1.00V) | 1.21 V (Nominal 1.10V) | N/A (Internal LDO) |
| **Max Output Current per Pin** | $16\text{ mA}$ | $12\text{ mA}$ | $8\text{ mA}$ |
| **Max Operating Junction Temp ($T_j$)** | $85.0^\circ\text{C}$ (Commercial) | $85.0^\circ\text{C}$ | $70.0^\circ\text{C}$ |
| **Human Body Model (HBM) ESD** | $\pm 2.0\text{ kV}$ | $\pm 2.0\text{ kV}$ | $\pm 2.0\text{ kV}$ |

### 1.1 Over-Voltage and Reverse Polarity Protections
- **Host Workstation USB**: All host USB ports are fused to $500\text{ mA}$ (USB 2.0) or $900\text{ mA}$ (USB 3.0). An external self-powered USB hub with per-port polyfuse overcurrent protection is recommended.
- **Pico Protection**: Features an onboard Schottky diode (MBR120VLS) preventing back-powering from $V_\text{BUS}$ to external supplies.
- **Artix-7 Protection**: I/O pins do NOT have 5V tolerance. Driving any Arty A7 FPGA pin directly from a 5V TTL logic level will cause substrate clamp diode breakdown and catastrophic silicon failure. Level shifters (e.g. TXB0108) must be used if 5V peripheral modules are attached.

---

## 2. Recovery Sequences and Fault Injection Handling

The C++ software layer (`DeviceManager` and `IDeviceAdapter`) incorporates a robust state machine for detecting and mitigating connection disruptions:

1. **Timeout Detection**:
   - Every hardware request is subject to a bounded timeout (`timeoutMs`, default 500 ms).
   - If a target fails to respond within the deadline, the adapter transitions immediately to `ConnectionStatus::TimedOut`.
2. **Automatic Retry and Re-synchronization**:
   - When a fault occurs, `DeviceManager::attemptRecovery(deviceId, maxRetries)` initiates a 3-step recovery sequence:
     1. Disconnect active handle and flush internal buffers.
     2. Send hardware reset pulse (or clear internal fault register).
     3. Reconnect interface and perform a ping/telemetry handshake.
3. **Graceful Degradation**:
   - If target hardware fails to recover, the Workbench retains all cached telemetry, updates the user interface status indicator to amber/red, and continues executing non-dependent desktop tasks (e.g. 3D geometry inspection and thermal FEA).

---

## 3. Explicit Simulation Disclosure (Zero-Fabrication Policy)

> [!IMPORTANT]
> In accordance with the PASS SCIENCE WORKS internship requirements and non-negotiable engineering principles:
> 1. **No physical bench hardware is claimed to be physically plugged into the testing laptop during automated test execution.**
> 2. The embedded adapters (`McuMockAdapter`, `FpgaMockAdapter`, `JtagMockAdapter`) are **high-fidelity C++ mock implementations** running natively in user-space.
> 3. Each adapter explicitly exposes `isSimulated = true` in its `DeviceInfo` structure, visible in diagnostic reports, API queries, and the GUI telemetry dashboard.
> 4. The mock implementation adheres 100% to the production `IDeviceAdapter` interface, allowing drop-in replacement with real Win32 FTDI D2XX (`FT_OpenEx`) and Win32 serial communication (`CreateFile`, `SetCommState`) drivers without modifying application logic.
