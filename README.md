<div align="center">

# ⚡ UARTDiag

### UART Protocol Diagnostics & Fault Analysis

**A C++17 diagnostic tool for validating UART frames, reproducing communication faults, analyzing streams, and generating structured diagnostic logs.**

<br>

[![C++17](https://img.shields.io/badge/C%2B%2B-17-00599C?style=for-the-badge\&logo=cplusplus\&logoColor=white)](#)
[![CMake](https://img.shields.io/badge/CMake-3.20%2B-064F8C?style=for-the-badge\&logo=cmake\&logoColor=white)](#)
[![Tests](https://img.shields.io/badge/Tests-90%2F90-2ea043?style=for-the-badge)](#testing)
[![Platform](https://img.shields.io/badge/Platform-Windows-0078D4?style=for-the-badge\&logo=windows\&logoColor=white)](#)

<br>

**[Features](#-features)** ·
**[Architecture](#-architecture)** ·
**[Usage](#-usage)** ·
**[Testing](#-testing)** ·
**[Configuration](#-configuration)**

</div>

---

<div align="center">

### Built for protocol debugging, fault analysis, and embedded-software practice.

</div>

---

# ✦ Overview

UARTDiag processes raw UART byte streams and turns them into structured diagnostic information.

```text
┌──────────────┐
│ Serial Input │
└──────┬───────┘
       │
       │
┌──────▼───────┐
│  Simulation  │
└──────┬───────┘
       │
       ▼
┌─────────────────┐
│  Stream Parser  │
└────────┬────────┘
         ▼
┌─────────────────┐
│  Frame Decoder  │
└────────┬────────┘
         ▼
┌─────────────────┐
│  CRC Validation │
└────────┬────────┘
         ▼
┌─────────────────┐
│   Diagnostics   │
└───────┬─┬───────┘
        │ │
        ▼ ▼
   Console  CSV
```

<div align="center">

|  **90 / 90**  |         **6**         |         **2**         |
| :-----------: | :-------------------: | :-------------------: |
| Tests Passing | Diagnostic Conditions |      Input Modes      |
|     `100%`    |     `INFO → ERROR`    | `Serial + Simulation` |

</div>

---

# ✦ Features

<table>
<tr>
<td width="50%" valign="top">

### 🔌 Protocol Engine

* Binary frame encoding / decoding
* CRC-8 validation
* Payload validation
* Length validation
* Frame-type validation
* Hex frame decoding
* Incremental stream parsing
* Stream resynchronization
* Partial-frame handling

</td>

<td width="50%" valign="top">

### 🩺 Diagnostics

* Structured diagnostic results
* Severity classification
* Fault classification
* Frame numbering
* Timestamps
* Session statistics
* Diagnostic summaries
* Structured reports

</td>
</tr>

<tr>
<td valign="top">

### 🧪 Simulation

* Software UART simulation
* Deterministic test scenarios
* CRC fault injection
* Invalid-length scenarios
* Noise injection
* Synchronization recovery
* Shared processing pipeline

</td>

<td valign="top">

### 📊 Logging & Config

* CSV diagnostic logging
* Configurable log filenames
* External configuration
* Serial configuration
* Simulation configuration
* CLI overrides
* Configuration validation

</td>
</tr>
</table>

---

# ✦ Architecture

UARTDiag separates **data acquisition** from **protocol processing**.

```text
                     UARTDiag
                        │
              ┌─────────┴─────────┐
              │                   │
          ┌───▼───┐          ┌────▼────┐
          │ Serial│          │Simulation│
          └───┬───┘          └────┬────┘
              │                   │
              └─────────┬─────────┘
                        ▼
                 ┌─────────────┐
                 │    Stream   │
                 │    Parser   │
                 └──────┬──────┘
                        ▼
                 ┌─────────────┐
                 │    Frame    │
                 │   Decoder   │
                 └──────┬──────┘
                        ▼
                 ┌─────────────┐
                 │ CRC / Frame │
                 │  Validation │
                 └──────┬──────┘
                        ▼
                 ┌─────────────┐
                 │ Diagnostics │
                 └──────┬──────┘
                        │
                 ┌──────┴──────┐
                 ▼             ▼
             Console          CSV
```

**Key design principle:** serial communication and simulation feed the same protocol-processing pipeline.

---

# ✦ Protocol

UARTDiag uses a compact binary frame format:

```text
┌────────┬────────┬────────┬───────────────────┬────────┐
│ START  │  TYPE  │ LENGTH │      PAYLOAD      │  CRC   │
├────────┼────────┼────────┼───────────────────┼────────┤
│  1 B   │  1 B   │  1 B   │      0–255 B      │  1 B   │
└────────┴────────┴────────┴───────────────────┴────────┘
```

| Field     |    Size | Purpose          |
| --------- | ------: | ---------------- |
| `START`   |     1 B | Synchronization  |
| `TYPE`    |     1 B | Message type     |
| `LENGTH`  |     1 B | Payload size     |
| `PAYLOAD` | 0–255 B | Application data |
| `CRC`     |     1 B | Integrity check  |

### Protocol parameters

```text
START       0xAA
CRC         CRC-8
Polynomial  0x07
Initial     0x00
```

Example:

```text
AA 01 04 10 20 30 40 BC
```

---

# ✦ Usage

## Build

```powershell
cmake -S . -B build -G "MinGW Makefiles" `
  -DCMAKE_C_COMPILER=C:/msys64/ucrt64/bin/gcc.exe `
  -DCMAKE_CXX_COMPILER=C:/msys64/ucrt64/bin/g++.exe `
  -DCMAKE_MAKE_PROGRAM=C:/msys64/ucrt64/bin/mingw32-make.exe

cmake --build build
```

---

## Simulation

```powershell
.\build\uartdiag.exe --simulate
```

Available scenarios:

```text
valid
crc
length
noise
all
```

Examples:

```powershell
.\build\uartdiag.exe --simulate crc
```

```powershell
.\build\uartdiag.exe --simulate all
```

With custom logging:

```powershell
.\build\uartdiag.exe --simulate crc --log crc_test.csv
```

---

## Serial

```powershell
.\build\uartdiag.exe --serial COM3
```

With baud rate:

```powershell
.\build\uartdiag.exe --serial COM3 115200
```

With logging:

```powershell
.\build\uartdiag.exe --serial COM3 115200 --log session.csv
```

---

## Hex Decode

Analyze a captured frame directly:

```powershell
.\build\uartdiag.exe --decode "AA 01 04 10 20 30 40 BC"
```

Useful for inspecting captured UART traffic without physical hardware.

---

# ✦ Configuration

UARTDiag supports external configuration files.

```text
baud_rate=115200
serial_read_size=256
simulation_log=uartdiag_simulation.csv
serial_log=uartdiag_serial.csv
```

Use:

```powershell
.\build\uartdiag.exe --config uartdiag.conf --simulate
```

| Setting            | Default                   |
| ------------------ | ------------------------- |
| `baud_rate`        | `115200`                  |
| `serial_read_size` | `256`                     |
| `simulation_log`   | `uartdiag_simulation.csv` |
| `serial_log`       | `uartdiag_serial.csv`     |

Command-line options can override configuration values.

---

# ✦ Diagnostics

Every processed frame produces a structured diagnostic result.

| Condition          | Severity  |
| ------------------ | --------- |
| Valid frame        | `INFO`    |
| Frame too short    | `WARNING` |
| Invalid start byte | `WARNING` |
| Unknown frame type | `ERROR`   |
| Invalid length     | `ERROR`   |
| CRC error          | `ERROR`   |

Reports can contain:

```text
Frame number
Timestamp
Frame type
Payload length
Raw frame
Expected CRC
Received CRC
Status
Message
Severity
```

---

# ✦ Fault Analysis

UARTDiag can reproduce common communication faults:

```text
                    Incoming Bytes
                           │
                           ▼
                    ┌─────────────┐
                    │ Find START  │
                    └──────┬──────┘
                           ▼
                    ┌─────────────┐
                    │ Read Header │
                    └──────┬──────┘
                           ▼
                    ┌─────────────┐
                    │ Check Length│
                    └──────┬──────┘
                           ▼
                    ┌─────────────┐
                    │ Wait Frame  │
                    └──────┬──────┘
                           ▼
                    ┌─────────────┐
                    │ Check CRC   │
                    └──────┬──────┘
                           ▼
                    ┌─────────────┐
                    │ Diagnostic  │
                    │   Result    │
                    └─────────────┘
```

Supported fault scenarios include:

* CRC corruption
* Invalid lengths
* Invalid start bytes
* Unknown frame types
* Noise
* Partial frames
* Lost synchronization

---

# ✦ Logging

Diagnostic results can be persisted as CSV:

```powershell
.\build\uartdiag.exe --simulate valid --log session.csv
```

CSV records include:

```text
timestamp
frame_number
type
payload_length
raw_data
crc_expected
crc_received
status
message
severity
```

Default log files:

```text
uartdiag_simulation.csv
uartdiag_serial.csv
```

---

# ✦ Testing

<div align="center">

## 🟢 90 / 90 TESTS PASSING

**100% pass rate**

</div>

The test suite covers:

* Frame encoding / decoding
* CRC calculation
* Frame validation
* Stream parsing
* Stream recovery
* Diagnostics
* Severity classification
* Diagnostic summaries
* Simulation
* Fault injection
* CSV logging
* Configuration
* Reports
* Report consistency

### CLI Regression Tests

```text
cli_simulate_valid
cli_simulate_crc
cli_simulate_all
```

Run the complete suite:

```powershell
ctest --test-dir build --output-on-failure
```

Expected:

```text
100% tests passed, 0 tests failed out of 90
```

---

# ✦ Project Structure

```text
UARTDiag/
│
├── include/uartdiag/
│   ├── config.hpp
│   ├── frame.hpp
│   ├── crc.hpp
│   ├── decoder.hpp
│   ├── diagnostics.hpp
│   ├── hex.hpp
│   ├── report.hpp
│   ├── serial.hpp
│   ├── stream.hpp
│   └── logger.hpp
│
├── src/
│   ├── config.cpp
│   ├── frame.cpp
│   ├── crc.cpp
│   ├── decoder.cpp
│   ├── diagnostics.cpp
│   ├── hex.cpp
│   ├── report.cpp
│   ├── serial.cpp
│   ├── stream.cpp
│   ├── logger.cpp
│   └── main.cpp
│
├── tests/
├── docs/
├── examples/
│
├── CMakeLists.txt
├── README.md
├── LICENSE
└── .gitignore
```

---

# ✦ Technology

<div align="center">

| Technology       | Role                 |
| :--------------- | :------------------- |
| **C++17**        | Application          |
| **CMake**        | Build system         |
| **GoogleTest**   | Unit testing         |
| **CTest**        | Test execution       |
| **GCC / MinGW**  | Compiler             |
| **Windows API**  | Serial communication |
| **CSV**          | Diagnostic logging   |
| **Git / GitHub** | Version control      |

</div>

---

# ✦ Engineering Focus

```text
C++17
  ├── Modular architecture
  ├── STL
  ├── RAII
  ├── Error handling
  └── Binary processing

Embedded / Protocols
  ├── UART
  ├── Framing
  ├── CRC
  └── Stream parsing

Software Engineering
  ├── CMake
  ├── Unit testing
  ├── Integration testing
  ├── Configuration
  └── Git

Diagnostics
  ├── Fault injection
  ├── Simulation
  ├── Severity
  ├── Reporting
  ├── Statistics
  └── Logging
```

---

# ✦ Project Status

<div align="center">

| Component         |    Status   |
| :---------------- | :---------: |
| UART protocol     |      ✅      |
| CRC validation    |      ✅      |
| Stream parser     |      ✅      |
| Resynchronization |      ✅      |
| Diagnostics       |      ✅      |
| Fault injection   |      ✅      |
| Simulation        |      ✅      |
| Windows serial    |      ✅      |
| CSV logging       |      ✅      |
| Configuration     |      ✅      |
| CLI testing       |      ✅      |
| Automated tests   | **90 / 90** |

</div>

---

# ✦ Future Extensions

Possible next steps:

* Linux serial support
* Cross-platform serial abstraction
* Additional CRC algorithms
* JSON diagnostic output
* Log rotation
* Performance benchmarking
* Hardware-in-the-loop testing
* Additional protocol fault models

---

<div align="center">

# ⚡ UARTDiag

### UART Protocol Diagnostics & Fault Analysis

**C++17 · CMake · GoogleTest · CTest**

<br>

*Built as an embedded-systems and software-engineering portfolio project.*

</div>
