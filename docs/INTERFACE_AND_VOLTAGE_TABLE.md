# Hardware Interface and Pin-Voltage Specification (Task E)

## 1. Voltage Domain Hierarchy

The integrated system operates across four discrete voltage domains:

| Voltage Domain | Nominal Voltage | Tolerance | Sourced By | Applied To |
|:---|:---:|:---:|:---|:---|
| **$V_\text{BUS}$** | 5.00 V | $\pm 5\%$ (4.75V – 5.25V) | Host USB Root Hub | Board power input, LDO/buck regulator inputs |
| **$V_\text{CCO\_3V3}$** | 3.30 V | $\pm 3\%$ (3.20V – 3.40V) | Onboard Switching Regulators | RP2040 I/O banks, Artix-7 Bank 14/34/35 I/O, JTAG-HS2 VREF |
| **$V_\text{CCAUX}$** | 1.80 V | $\pm 3\%$ (1.75V – 1.85V) | Arty A7 Buck Regulator | Artix-7 internal auxiliary circuits & XADC references |
| **$V_\text{CCINT}$** | 1.00 V | $\pm 3\%$ (0.97V – 1.03V) | Arty A7 Synchronous Buck | Artix-7 internal configurable logic blocks (CLB) & DSP cores |

---

## 2. Pin-Level Interconnect and Voltage Table

The following table details the physical signals connecting the Host PC, Raspberry Pi Pico (MCU), Digilent Arty A7 (FPGA), and Digilent JTAG-HS2 debug adapter:

| Pin # | Signal Name | Net Origin | Net Destination | Voltage Domain | Direction | Drive Strength / I/O Standard | Pull-Up / Down | ESD Protection | Logic Thresholds ($V_{IL} / V_{IH} / V_{OL} / V_{OH}$) |
|:---:|:---|:---|:---|:---:|:---:|:---:|:---:|:---:|:---|
| **P1** | `USB_D+` | Host USB | Pico / Arty USB | 3.3 V | Bidir | USB 2.0 PHY Spec | $1.5\text{ k}\Omega$ pull-up (D+) | USBLC6-2SC6 | $V_{IL} \le 0.8\text{ V}, V_{IH} \ge 2.0\text{ V}$ |
| **P2** | `USB_D-` | Host USB | Pico / Arty USB | 3.3 V | Bidir | USB 2.0 PHY Spec | Differential matched | USBLC6-2SC6 | $V_{IL} \le 0.8\text{ V}, V_{IH} \ge 2.0\text{ V}$ |
| **P3** | `GND_SYS` | Host Workstation | All Boards | 0.0 V | Ref | Common Reference | Star Ground Plane | TVS Array | Ground reference 0.00 V |
| **P4** | `JTAG_VREF` | Arty Header J1-1 | JTAG-HS2 Pin 1 | 3.3 V | Input (Sense) | High Impedance ($>100\text{ k}\Omega$) | None | PESD5V0X1BCAL | Sensed range: 1.8V to 5.0V |
| **P5** | `JTAG_TMS` | JTAG-HS2 Pin 2 | Arty Header J1-2 | 3.3 V | Output (Host) | LVCMOS33 ($8\text{ mA}$) | $10\text{ k}\Omega$ pull-up | TVS Diode | $V_{IL} \le 0.8\text{ V}, V_{IH} \ge 2.0\text{ V}, V_{OH} \ge 2.4\text{ V}$ |
| **P6** | `JTAG_TCK` | JTAG-HS2 Pin 3 | Arty Header J1-3 | 3.3 V | Output (Host) | LVCMOS33 ($12\text{ mA}$, up to 30 MHz) | $10\text{ k}\Omega$ pull-down | TVS Diode | $V_{IL} \le 0.8\text{ V}, V_{IH} \ge 2.0\text{ V}, V_{OH} \ge 2.4\text{ V}$ |
| **P7** | `JTAG_TDO` | Arty Header J1-4 | JTAG-HS2 Pin 4 | 3.3 V | Input (Host) | LVCMOS33 ($8\text{ mA}$, Tri-state) | None (Active low drive) | TVS Diode | $V_{IL} \le 0.8\text{ V}, V_{IH} \ge 2.0\text{ V}, V_{OL} \le 0.4\text{ V}$ |
| **P8** | `JTAG_TDI` | JTAG-HS2 Pin 5 | Arty Header J1-5 | 3.3 V | Output (Host) | LVCMOS33 ($8\text{ mA}$) | $10\text{ k}\Omega$ pull-up | TVS Diode | $V_{IL} \le 0.8\text{ V}, V_{IH} \ge 2.0\text{ V}, V_{OH} \ge 2.4\text{ V}$ |
| **P9** | `SPI_SCK` | Pico GP18 (Pin 24) | Arty IO13 (Pin D10) | 3.3 V | Output (Pico) | LVCMOS33 ($4\text{ mA}$, 12 MHz) | Series $33\ \Omega$ damping | ESD Array | $V_{OL} \le 0.4\text{ V}, V_{OH} \ge 2.6\text{ V}$ |
| **P10** | `SPI_MOSI` | Pico GP19 (Pin 25) | Arty IO11 (Pin C10) | 3.3 V | Output (Pico) | LVCMOS33 ($4\text{ mA}$) | Series $33\ \Omega$ damping | ESD Array | $V_{OL} \le 0.4\text{ V}, V_{OH} \ge 2.6\text{ V}$ |
| **P11** | `SPI_MISO` | Arty IO12 (Pin C11) | Pico GP16 (Pin 21) | 3.3 V | Input (Pico) | LVCMOS33 ($4\text{ mA}$) | Series $33\ \Omega$ damping | ESD Array | $V_{IL} \le 0.8\text{ V}, V_{IH} \ge 2.0\text{ V}$ |
| **P12** | `SPI_CS#` | Pico GP17 (Pin 22) | Arty IO10 (Pin A10) | 3.3 V | Output (Pico) | LVCMOS33 ($4\text{ mA}$) | $10\text{ k}\Omega$ pull-up | ESD Array | $V_{OL} \le 0.4\text{ V}, V_{OH} \ge 2.6\text{ V}$ |
| **P13** | `SYS_RESET#` | Pico GP22 (Pin 29) | Arty RESET (Pin C2) | 3.3 V | Open-Drain Bidir | Open-Drain ($4\text{ mA}$) | $4.7\text{ k}\Omega$ pull-up to 3.3V | TVS Diode | Active low pulse ($>10\ \mu\text{s}$) |

---

## 3. Signal Integrity and High-Speed Routing Rules

1. **JTAG TCK / TMS Integrity**:
   - `TCK` clock line operates up to 30 MHz. Length matching between `TCK`, `TMS`, `TDI`, and `TDO` must remain within $\pm 2.5\text{ mm}$ (approx. $15\text{ ps}$ propagation skew).
   - Solid continuous ground plane directly beneath all JTAG traces to prevent transmission line ringing and false TAP state transitions.
2. **SPI Telemetry Bus**:
   - $33\ \Omega$ series damping resistors placed adjacent to driving pins damp ringing on fast edge transitions ($t_r / t_f < 2\text{ ns}$).
3. **Logic Threshold Safety Margins**:
   - Both RP2040 and Artix-7 standard LVCMOS33 banks adhere strictly to JEDEC JESD8-7A:
     - Minimum Output High ($V_{OH}$): $2.4\text{ V}$ (Pico delivers $\ge 2.6\text{ V}$).
     - Maximum Output Low ($V_{OL}$): $0.4\text{ V}$ (Pico delivers $\le 0.2\text{ V}$).
     - Minimum Input High ($V_{IH}$): $2.0\text{ V}$.
     - Maximum Input Low ($V_{IL}$): $0.8\text{ V}$.
   - Noise margin high ($NM_H = V_{OH} - V_{IH}$) exceeds $0.6\text{ V}$; noise margin low ($NM_L = V_{IL} - V_{OL}$) exceeds $0.4\text{ V}$.
