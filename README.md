# 🧠 Modular Diagnostic & Control System
### Universal Hardware Platform for Quantum, Scientific and Embedded Applications

---

## 🌍 Overview

This project defines a **modular diagnostic and control platform** for **quantum computing, scientific instrumentation, and advanced embedded systems**.

It provides a unified hardware and firmware ecosystem that enables automatic **enumeration**, **identification**, and **configuration** of dozens of functional modules — all interconnected via a shared **I²C / SPI backplane**, synchronized by a **10 MHz reference clock**, and fully integrated with **ESPHome** and **Home Assistant (HA)**.

---

## 🧩 Purpose and Applications

The system was developed for:
- **Quantum control and diagnostics** (ion traps, NV centers, optical clocks),
- **Laser and RF front-end electronics** (DDS, synthesizers, mixers, drivers),
- **Environmental and power monitoring** in precision setups,
- **General laboratory automation** and **multi-module testbenches**.

Typical module types include:
- High-precision **ADC / DAC** converters,
- **DDS / RF / microwave synthesizers** and frequency converters,
- **Laser and TEC controllers**,
- **Magnetic, thermal, and current sensors**,
- **Programmable amplifiers / digital I/O**.

---

## ⚙️ Hardware Architecture

Each PCB module performs a specific analog, digital, or RF function and connects **side-by-side** via edge connectors (LED-strip-type clips), forming an electrically continuous chain.

### Shared Backplane Signals

| Signal | Function | Description |
|---------|-----------|-------------|
| `SCL`, `SDA` | I²C bus | Management, discovery, configuration |
| `MOSI`, `MISO`, `SCLK` | Shared SPI lines | High-speed control/data |
| `SPI_AD[5:0]` | SPI address lines | 6-bit address, decoded in each CPLD |
| `INIT_CHAIN` | Module enumeration | Unidirectional chain (IN→OUT) |
| `CLK10` | 10 MHz reference clock | Single-ended or differential, continuous |
| `TRIG` | Global trigger (single-ended) | Synchronous acquisition/actuation |
| `+5 V`, `+12 V`, `GND` | Power rails | Shared via connectors |

Up to **16 modules per chain** can be connected.  
Each module can host up to **four SPI-addressable sub-devices**, selected locally by the CPLD.

---

## 🧠 Universal CPLD Controller

All modules share **one CPLD firmware design**, which implements:

- **I²C slave interface** (enumeration, configuration, metadata access),
- **SPI buffer and 6-bit address decoder** (`SPI_AD[5:0]`),
- **4-channel chip-select decoder** (`CS0..CS3`) for local SPI peripherals,
- **MISO tri-state gating** – only active when the address matches,
- **INIT_CHAIN logic** – automatic address assignment along the chain,
- **PROJECT_ID** and **REV_ID** read from dedicated pins,
- Optional **LED/status outputs** (showing assigned address or activity),
- **Read-only FLASH memory** (shared image for all modules).

The same CPLD bitstream is used in every module.  
Each board’s **identity** is defined purely by `PROJECT_ID` and `REV_ID` pins.

---

## 🔍 Identification and Metadata

After startup, each CPLD exposes the following I²C register map:

| Addr | Name | R/W | Description |
|-----:|------|:---:|-------------|
| 0x00 | WHOAMI | R | Constant signature (0xA5) |
| 0x01 | PROJECT_ID | R | Module type (wired pins) |
| 0x02 | REV_ID | R | Board revision (wired pins) |
| 0x03 | STATUS | R | INIT state, address valid, etc. |
| 0x04 | CONTROL | W | LOCK, RELEASE_NEXT, SOFT_RESET |
| 0x05 | NEW_I2C_ADDR | W | Assign new I²C address |
| 0x06 | CS_ID_NIBBLE | W | 0–15 SPI sub-address index |
| 0x10–0x6F | FLASH window | R | Static system metadata |

The internal FLASH (identical on all modules) stores:
- Schema version,
- Global metadata,
- YAML template identifiers,
- Feature bitmask,
- CRC checksum.

---

## 🔗 Automatic Enumeration (INIT_CHAIN)

A single line, **`INIT_CHAIN`**, replaces jumpers and dip-switches for module addressing.

### Sequence:
1. All CPLDs boot with default I²C address `0x50`.  
2. Only the first module (`INIT_IN = 0`) responds.  
3. The ESP32 controller:
   - Reads `PROJECT_ID` and `REV_ID`,
   - Assigns a unique I²C address and `CS_ID_NIBBLE`,
   - Sets the **LOCK** bit and triggers `RELEASE_NEXT`.
4. The first CPLD asserts `INIT_OUT = 0`, activating the next module.
5. Process repeats until all modules are enumerated.

Result: each board obtains a unique I²C and SPI address automatically.

---

## 🧩 Addressable SPI Architecture

The SPI bus is **shared by all modules**, but only the device whose **6-bit address** on `SPI_AD[5:0]` matches its **assigned ID** becomes active.

Within each module:
- CPLD compares `SPI_AD[5:0]` with its own `CS_ID_NIBBLE` (plus module bits if needed),
- When matched, the CPLD enables MISO output and activates one of four local chip-select lines (`CS0..CS3`),
- Each module can therefore host **up to four SPI devices** (ADC, DAC, DDS, etc.) without bus contention.

This allows clean, scalable SPI addressing across up to **64 possible modules** (6-bit space), while using only **a single SPI bus**.

---

## 🧩 Integration with ESPHome and Home Assistant

### Workflow
1. ESP32 runs a small **enumeration firmware** (loaded to RAM).  
2. It scans I²C, enumerates modules via INIT_CHAIN, and records all IDs.  
3. A **JSON inventory** is sent to a PC tool.  
4. The YAML generator matches each `PROJECT_ID` to its ESPHome template.  
5. It builds a complete `esphome.yaml` describing the assembled system.  
6. The firmware is compiled and flashed to the ESP32.

ESPHome and Home Assistant then display all submodules as one logical device with many sensors and actuators.

### Example Auto-Generated YAML
```yaml
spi:
  clk_pin: 18
  mosi_pin: 23
  miso_pin: 19
  address_pins: [25,26,27,32,33,34]  # SPI_AD[5:0]

sensor:
  - platform: quantum_adc
    spi_address: 0x03
    cs_index: 0
    name: "ADC Channel 0"
  - platform: rf_dds
    spi_address: 0x04
    cs_index: 1
    frequency: 40MHz

