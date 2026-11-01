# Hardware Interconnect & Device-Specific Voltage Specification (Task E)

## 1. System Power Distribution & Voltage Domain Hierarchy

The integrated hardware architecture partitions power across five discrete voltage domains:

```
[Host PC USB Root Hub (5.0V ±5%, max 2.0A total)]
        |
        +---> [Raspberry Pi Pico (MCU)]
        |       |-- VBUS: 5.0V Input (Pin 40)
        |       |-- RT6150 Buck-Boost: VSYS (1.8V - 5.5V)
        |       |-- IOVDD (3.3V, max 300 mA): GPIO Banks 0 & QSPI
        |       +-- DVDD (1.1V internal LDO): Dual ARM Cortex-M0+ Core Logic
        |
        +---> [Digilent Arty A7-35T (FPGA)]
        |       |-- VBUS / External Barrel Jack (5.0V - 15V)
        |       |-- LTC3615 Dual Buck:
        |       |     |-- VCCINT (1.00V ±3%, max 2.5A): CLBs, DSP48E1, Block RAM
        |       |     +-- VCCO_3V3 (3.30V ±3%, max 3.0A): I/O Banks 0, 14, 34, 35
        |       +-- Linear Regulator:
        |             +-- VCCAUX (1.80V ±3%, max 1.0A): Clock Managers & XADC
        |
        +---> [Digilent JTAG-HS2 (Programming Cable)]
                |-- VBUS: 5.0V USB Power (max 100 mA)
                +-- VREF Sense Input (1.8V - 5.0V): Dynamic Biasing for Level Translators
```

| Voltage Domain | Nominal Voltage | Tolerance | Sourced By | Sinks / Applied To |
|:---|:---:|:---:|:---|:---|
| **$V_\text{BUS}$** | 5.00 V | $\pm 5\%$ (4.75V – 5.25V) | Host PC USB 3.0 / Root Hub | Board power inputs, buck/boost converters |
| **$V_\text{CCO\_3V3}$** | 3.30 V | $\pm 3\%$ (3.20V – 3.40V) | Onboard Buck Converters | RP2040 GPIO, Artix-7 Banks 0/14/34/35, JTAG-HS2 VREF |
| **$V_\text{CCAUX}$** | 1.80 V | $\pm 3\%$ (1.75V – 1.85V) | Arty A7 LDO Regulator | Artix-7 MMCM/PLL clocks, internal XADC reference |
| **$V_\text{CCINT}$** | 1.00 V | $\pm 3\%$ (0.97V – 1.03V) | Arty A7 Synchronous Buck | Artix-7 internal fabric, logic cells, DSP48 cores |
| **$V_\text{CORE\_MCU}$** | 1.10 V | $\pm 5\%$ (1.05V – 1.15V) | RP2040 Internal LDO | RP2040 dual ARM Cortex-M0+ processing cores |

---

## 2. Dedicated Device-Specific Connector Pinouts

Rather than assuming a universal interconnect table, each physical interface is defined according to its authentic hardware specification:

### 2.1 Interface A: Digilent JTAG-HS2 to Arty A7 Programming Header (J1)
The Digilent JTAG-HS2 interfaces to the Artix-7 JTAG configuration chain via the dedicated 6-pin 100-mil header `J1`. The HS2 incorporates dynamic level translation (TI SN74AVC4T245) biased by the sensed target reference voltage `VREF`:

| Pin # | Signal Name | Net Origin | Net Destination | Voltage Domain | Direction | Drive Spec / Standard | Pull-Up / Pull-Down | ESD Protection |
|:---:|:---|:---|:---|:---:|:---:|:---|:---|:---|
| **1** | `VREF` | Arty J1-1 ($V_\text{CCO\_0}$) | JTAG-HS2 Pin 1 | 3.3 V | Sense Input | High-Z ($>100\text{ k}\Omega$) | None (direct sensing) | PESD5V0X1BCAL |
| **2** | `TMS` | JTAG-HS2 Pin 2 | Arty J1-2 (Bank 0) | 3.3 V | Output (Host $\to$ FPGA) | LVCMOS33 ($8\text{ mA}$) | $10\text{ k}\Omega$ pull-up to 3.3V | TVS Array |
| **3** | `TCK` | JTAG-HS2 Pin 3 | Arty J1-3 (Bank 0) | 3.3 V | Output (Host $\to$ FPGA) | LVCMOS33 ($12\text{ mA}$, up to 30 MHz) | $10\text{ k}\Omega$ pull-down to GND | TVS Array |
| **4** | `TDO` | Arty J1-4 (Bank 0) | JTAG-HS2 Pin 4 | 3.3 V | Input (FPGA $\to$ Host) | LVCMOS33 ($8\text{ mA}$, Tri-state) | Active drive when enabled | TVS Array |
| **5** | `TDI` | JTAG-HS2 Pin 5 | Arty J1-5 (Bank 0) | 3.3 V | Output (Host $\to$ FPGA) | LVCMOS33 ($8\text{ mA}$) | $10\text{ k}\Omega$ pull-up to 3.3V | TVS Array |
| **6** | `GND` | Arty Header J1-6 | JTAG-HS2 Pin 6 | 0.0 V | Reference | Star Ground Plane | Continuous ground return | Direct chassis/PCB |

**Dynamic Level Translation Operation**:
The JTAG-HS2 continuously senses $V_\text{REF}$ (valid range: $1.8\text{ V} \le V_\text{REF} \le 5.0\text{ V}$). If $V_\text{REF} < 1.6\text{ V}$ (target board unpowered), the HS2 drivers automatically place all JTAG outputs (`TCK`, `TMS`, `TDI`) in High-Impedance (Hi-Z) state, preventing parasite back-powering of unpowered FPGA pins.

---

### 2.2 Interface B: High-Speed Telemetry SPI Bus (RP2040 MCU to Arty A7 FPGA)
High-rate thermal telemetry and matrix commands are transferred between the Raspberry Pi Pico (SPI Master) and Digilent Arty A7 Pmod Port JA/JB (SPI Slave):

| Signal Name | Pico Header Pin | Arty Pmod Pin | FPGA Pin | Voltage Domain | Direction | Standard & Damping | Pull-Up / Down |
|:---|:---:|:---:|:---:|:---:|:---:|:---|:---|
| `SPI_SCK` | Pin 24 (GP18) | JA-Pin 4 | D10 | 3.3 V | MCU Out $\to$ FPGA In | LVCMOS33, 12 MHz, $33\ \Omega$ series damping | $47\text{ k}\Omega$ pull-down |
| `SPI_MOSI` | Pin 25 (GP19) | JA-Pin 2 | C10 | 3.3 V | MCU Out $\to$ FPGA In | LVCMOS33, $33\ \Omega$ series damping | None |
| `SPI_MISO` | Pin 21 (GP16) | JA-Pin 3 | C11 | 3.3 V | FPGA Out $\to$ MCU In | LVCMOS33, Tri-state, $33\ \Omega$ series damping | Weak pull-up |
| `SPI_CS#` | Pin 22 (GP17) | JA-Pin 1 | A10 | 3.3 V | MCU Out $\to$ FPGA In | LVCMOS33, Active Low | $10\text{ k}\Omega$ pull-up to 3.3V |
| `SYS_RESET#`| Pin 29 (GP22) | JA-Pin 7 | C2 | 3.3 V | Open-Drain Bidirectional | Open-Drain, $4\text{ mA}$ sink | $4.7\text{ k}\Omega$ pull-up to 3.3V |
| `GND` | Pin 23 / 28 | JA-Pin 5 / 11| GND | 0.0 V | Common Ground | Shielded star ground plane | Direct bond |

---

### 2.3 Interface C: Host Workstation USB Virtual COM Ports (UART)
- **Raspberry Pi Pico**: USB 2.0 Full-Speed (12 Mbps) CDC-ACM Virtual COM port (`COM3`), configured at 115,200 baud, 8N1, emitting ASCII telemetry packets (`$PICO,T=32.4,V=3.32*CHECKSUM\r\n`).
- **Digilent Arty A7**: Onboard FTDI FT2232HQ USB-UART bridge (`COM4`), configured at 115,200 baud, 8N1, emitting Artix-7 XADC telemetry (`$A7,CORE_T=41.8,VCCINT=1.002*CHECKSUM\r\n`).

---

## 3. Logic Threshold Safety & Noise Margins

Under standard 3.3V LVCMOS specifications (JEDEC JESD8-7A):

$$\begin{aligned}
\text{High-Level Noise Margin: } NM_H &= V_{OH} - V_{IH} = 2.60\text{ V} - 2.00\text{ V} = \mathbf{0.60\text{ V}} \\
\text{Low-Level Noise Margin: } NM_L &= V_{IL} - V_{OL} = 0.80\text{ V} - 0.40\text{ V} = \mathbf{0.40\text{ V}}
\end{aligned}$$

Both $NM_H$ and $NM_L$ exceed the mandatory minimum noise margin of $0.3\text{ V}$, guaranteeing stable digital switching in electrically noisy laboratory environments. Series $33\ \Omega$ damping resistors on high-speed SPI clock lines prevent transmission-line reflections and ringing on fast edge transitions ($t_r, t_f < 2\text{ ns}$).
