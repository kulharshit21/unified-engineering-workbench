# Embedded System Integration Architecture (Task E)

## 1. Overview and System Topology

The **Unified Engineering Workbench** interfaces with a heterogeneous embedded architecture comprising:
1. **Host Workstation**: Running the Unified Engineering Workbench (Windows 11 x64).
2. **Microcontroller Unit (MCU)**: **Raspberry Pi Pico** (Dual-core ARM Cortex-M0+ RP2040 @ 133 MHz).
3. **Field-Programmable Gate Array (FPGA)**: **Digilent Arty A7-35T** (AMD/Xilinx Artix-7 XC7A35T-1CPG236C).
4. **JTAG Debug Adapter**: **Digilent JTAG-HS2** (High-Speed FTDI FT232H-based JTAG/SPI probe).

This configuration models an edge-compute coprocessor system where the MCU serves as an auxiliary telemetry acquisition and power sequencer node, the FPGA serves as a high-throughput hardware accelerator (e.g. accelerating numerical matrix operations or digital signal processing), and the JTAG adapter provides hardware configuration and non-intrusive runtime debugging.

```mermaid
graph TD
    subgraph Host ["Host Workstation (Windows 11 x64)"]
        UEW["Unified Engineering Workbench<br/>(Qt 6 + C++17 Backend)"]
        DM["Hardware::DeviceManager"]
        UEW --> DM
    end

    subgraph Adapters ["Device Adapters Layer (IDeviceAdapter)"]
        MCU_A["McuMockAdapter<br/>(USB CDC-ACM)"]
        FPGA_A["FpgaMockAdapter<br/>(FT2232HQ UART/AXI)"]
        JTAG_A["JtagMockAdapter<br/>(FT232H JTAG TAP)"]
        DM --> MCU_A
        DM --> FPGA_A
        DM --> JTAG_A
    end

    subgraph PhysicalTarget ["Physical Hardware Specification (or Emulated Mocks)"]
        subgraph MCU_NODE ["MCU: Raspberry Pi Pico (RP2040)"]
            M0["Dual ARM Cortex-M0+ @ 133 MHz<br/>264 KB SRAM, 2 MB QSPI Flash"]
            ADC["On-chip Temperature Sensor & ADC"]
            M0 --- ADC
        end

        subgraph FPGA_NODE ["FPGA: Digilent Arty A7-35T"]
            A7["Artix-7 XC7A35T Logic Fabric<br/>33,280 Logic Cells, 90 DSP Slices"]
            DDR3["256 MB DDR3L Memory"]
            XADC["On-chip XADC Sensor<br/>Die Temp, VCCINT, VCCAUX"]
            A7 --- DDR3
            A7 --- XADC
        end

        subgraph JTAG_PROBE ["JTAG Adapter: Digilent JTAG-HS2"]
            FTDI["FT232H High-Speed USB Controller<br/>Up to 30 MHz TCK Clock"]
            VREF_SENSE["Target VREF Voltage Sensor (1.8V - 5.0V)"]
            FTDI --- VREF_SENSE
        end
    end

    MCU_A -.->|"USB CDC-ACM (COM3)<br/>115,200 baud, 8N1"| MCU_NODE
    FPGA_A -.->|"FT2232HQ Ch B (COM5)<br/>12 MBaud AXI-Stream / UART"| FPGA_NODE
    JTAG_A -.->|"USB 2.0 High-Speed<br/>IEEE 1149.1 JTAG TAP"| JTAG_PROBE

    MCU_NODE <-->|"Inter-IC Telemetry SPI<br/>(SCK, MOSI, MISO, CS# @ 12 MHz)"| FPGA_NODE
    JTAG_PROBE ==>|"JTAG Bus (TCK, TMS, TDI, TDO)<br/>VREF = 3.3V"| FPGA_NODE
```

---

## 2. Signal Interconnect and Communication Protocols

### 2.1 Host-to-MCU Interface
- **Physical Transport**: USB 1.1 Full Speed (12 Mbps) via Micro-USB connector.
- **Protocol**: USB CDC-ACM Virtual COM port (`COM3` default).
- **Packet Structure**: Binary packet framing with 2-byte sync header (`0xAA 0x55`), 1-byte command ID, 1-byte length, $N$-byte payload, and CRC-16-CCITT checksum.
- **Buffer Depth**: 512 bytes dual-buffered ring buffer in RP2040 SRAM.
- **Telemetry Query**: Host requests diagnostic packets at 10 Hz; response includes internal ADC temperature (°C), $V_{DD}$ rail (3.3V), and packet counter.

### 2.2 Host-to-FPGA Interface
- **Physical Transport**: Dedicated high-speed UART via FTDI FT2232HQ Channel B.
- **Protocol**: AXI4-Lite command transactions encapsulated over framed serial protocol operating at up to 12 Mbaud.
- **Telemetry Source**: Xilinx internal XADC primitive continuously digitizing die temperature, $V_\text{CCINT}$ (1.0V), and $V_\text{CCAUX}$ (1.8V).
- **Buffer Depth**: 4 KB FIFO in FTDI chip + 16 KB BRAM FIFO in Artix-7 fabric.

### 2.3 JTAG Debug Interface
- **Physical Transport**: Digilent JTAG-HS2 High-Speed USB 2.0 (480 Mbps).
- **Target Interface**: 6-pin 100-mil standard JTAG header matching Digilent pinout (`VREF`, `TMS`, `TCK`, `TDO`, `TDI`, `GND`).
- **Clock Rate**: Programmable from 30 kHz to 30 MHz.
- **Capabilities**: Full IEEE 1149.1 boundary scan, Artix-7 bitstream programming, and internal MicroBlaze / core debugging via Xilinx BSCANE2 primitives.

---

## 3. Power, Grounding, and Reset Topology

```
                  +----------------------------------------------+
                  |              Host 5V USB Bus                 |
                  +-------+--------------------+-----------------+
                          |                    |
                          v                    v
                   +--------------+     +--------------+
                   |  Pico USB    |     |  Arty USB    |
                   |  5V -> 3.3V  |     |  5V -> LDOs  |
                   | (RT6150 B/B) |     | (ADP2118 SW) |
                   +------+-------+     +------+-------+
                          |                    |
              +-----------+                    +---------------+
              | 3.3V Rail                      | 3.3V, 1.8V, 1.0V
              v                                v
       [ Raspberry Pi Pico ]            [ Artix-7 FPGA ]
              |                                |
              +-------- COMMON GND ------------+
              |                                |
              +---- Active-Low Reset (NRST) ---+
```

1. **Common Ground Star Topology**: All grounds (Host PC ground, Pico GND pins 3/8/13/18/23/28/38, Arty GND headers, and JTAG-HS2 pin 2) are tied together to prevent ground loops and signal integrity degradation.
2. **Reset Coordination**:
   - Master hardware reset can be asserted by the Host through the JTAG interface (SRST) or programmatically by the RP2040 via a designated open-drain GPIO line with a 10 kΩ pull-up to 3.3V.
   - Power-on Reset (POR) sequencing ensures the FPGA core rail ($V_\text{CCINT} = 1.0\text{ V}$) stabilizes before auxiliary ($V_\text{CCAUX} = 1.8\text{ V}$) and I/O ($V_\text{CCO} = 3.3\text{ V}$) rails ramp.

---

## 4. Software Abstraction: The `IDeviceAdapter` Pattern

To decouple the desktop user interface and numerical solvers from physical hardware dependencies:
- All physical devices and mock models implement `IDeviceAdapter`.
- `DeviceManager` acts as the single point of coordination, handling device registration, asynchronous or synchronous telemetry queries, error detection, timeout handling, and automatic recovery sequences.
- When running in an environment without physical laboratory hardware attached, the software transparently utilizes validated mock adapters (`McuMockAdapter`, `FpgaMockAdapter`, `JtagMockAdapter`).
- **Zero-Fabrication Disclosure**: All mock adapters explicitly report `isSimulated = true` via `getInfo().isSimulated`, clearly visible in the UI and diagnostic logs.
