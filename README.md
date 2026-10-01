<div align="center">

# UARTDiag

### UART Protocol Diagnostics & Fault Analysis

**A C++17 diagnostic tool for framed UART communication, protocol validation, fault analysis, simulation, and logging.**

<br>

[![C++17](https://img.shields.io/badge/C%2B%2B-17-00599C?style=flat-square\&logo=cplusplus\&logoColor=white)](#)
[![CMake](https://img.shields.io/badge/CMake-3.20%2B-064F8C?style=flat-square\&logo=cmake\&logoColor=white)](#)
[![Tests](https://img.shields.io/badge/Tests-90%2F90%20Passing-2da44e?style=flat-square)](#testing)
[![Platform](https://img.shields.io/badge/Platform-Windows-0078D4?style=flat-square\&logo=windows\&logoColor=white)](#)

<br>

[Features](#features) ·
[Architecture](#architecture) ·
[Usage](#usage) ·
[Testing](#testing) ·
[Configuration](#configuration)

</div>

---

## Overview

UARTDiag is a modular C++17 application for analyzing framed UART communication.

It accepts raw byte streams, reconstructs protocol frames, validates their structure and CRC, identifies communication faults, classifies diagnostic results, and records the results for later analysis.

The application supports both **software simulation** and **Windows serial communication**, while keeping protocol processing independent from the input source.

<div align="center">

|    90 / 90    |              6              |           2           |
| :-----------: | :-------------------------: | :-------------------: |
| Tests passing | Diagnostic error conditions |      Input modes      |
|     `100%`    |        `INFO → ERROR`       | `Serial + Simulation` |

</div>

---

## Features

<table>
<tr>
<td width="50%" valign="top">

### Protocol

* Binary frame encoding and decoding
* CRC-8 validation
* Payload and length validation
* Frame type validation
* Hexadecimal frame decoding
* Incremental stream parsing
* Stream resynchronization
* Partial-frame handling
* Noisy input handling

</td>
<td width="50%" valign="top">

### Diagnostics

* Structured diagnostic results
* Severity classification
* Frame numbering
* Timestamps
* Session statistics
* Diagnostic summaries
* Structured reports
* Fault classification

</td>
</tr>

<tr>
<td width="50%" valign="top">

### Input & Simulation

* Windows serial-port support
* Software UART simulation
* Configurable simulation scenarios
* CRC fault injection
* Invalid-length scenarios
* Noise and synchronization scenarios
* Shared processing pipeline

</td>
<td width="50%" valign="top">

### Logging & Configuration

* CSV diagnostic logging
* Configurable log filenames
* External configuration files
* Serial configuration
* Simulation configuration
* Command-line overrides
* Configuration validation

</td>
</tr>
</table>

---

## Protocol

UARTDiag uses a compact framed binary protocol.

```text
┌────────┬────────┬────────┬───────────────────┬────────┐
│ START  │  TYPE  │ LENGTH │      PAYLOAD      │  CRC   │
├────────┼────────┼────────┼───────────────────┼────────┤
│  1 B   │  1 B   │  1 B   │      0–255 B      │  1 B   │
└────────┴────────┴────────┴───────────────────┴────────┘
```

### Frame definition

| Field     |        Size | Description          |
| --------- | ----------: | -------------------- |
| `START`   |      1 byte | Synchronization byte |
| `TYPE`    |      1 byte | Message type         |
| `LENGTH`  |      1 byte | Payload size         |
| `PAYLOAD` | 0–255 bytes | Application data     |
| `CRC`     |      1 byte | CRC-8 checksum       |

### Protocol parameters

```text
START       0xAA
CRC         CRC-8
Polynomial  0x07
Initial     0x00
```

### Example

```text
AA 01 04 10 20 30 40 BC
```

```text
START   → AA
TYPE    → 01
LENGTH  → 04
PAYLOAD → 10 20 30 40
CRC     → BC
```

---

# Architecture

UARTDiag separates communication input from protocol processing.

```text
                         UARTDiag
                            │
              ┌─────────────┴─────────────┐
              │                           │
          Serial                      Simulation
              │                           │
              └─────────────┬─────────────┘
                            │
                            ▼
                    ┌───────────────┐
                    │ Stream Parser │
                    └───────┬───────┘
                            │
                            ▼
                    ┌───────────────┐
                    │ Frame Decoder │
                    └───────┬───────┘
                            │
                            ▼
                    ┌───────────────┐
                    │ CRC Validation│
                    └───────┬───────┘
                            │
                            ▼
                    ┌───────────────┐
                    │  Diagnostics  │
                    └───────┬───────┘
                            │
                   ┌────────┴────────┐
                   │                 │
                   ▼                 ▼
              Console Report     CSV Logger
```

The important design principle is that the protocol pipeline does not depend on whether the bytes originated from a real serial device or the simulation layer.

---

## Project Structure

```text
UARTDiag/
│
├── include/
│   └── uartdiag/
│       ├── config.hpp
│       ├── frame.hpp
│       ├── crc.hpp
│       ├── decoder.hpp
│       ├── diagnostics.hpp
│       ├── hex.hpp
│       ├── report.hpp
│       ├── serial.hpp
│       ├── stream.hpp
│       └── logger.hpp
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
│   ├── test_frame.cpp
│   ├── test_simulation.cpp
│   ├── test_logger.cpp
│   ├── test_diagnostics.cpp
│   ├── test_config.cpp
│   └── test_report.cpp
│
├── docs/
│   └── architecture.md
│
├── examples/
│
├── CMakeLists.txt
├── README.md
├── LICENSE
└── .gitignore
```

---

# Usage

## Simulation

Simulation mode provides deterministic UART input without requiring physical hardware.

### Default simulation

```powershell
.\build\uartdiag.exe --simulate
```

### Valid frame

```powershell
.\build\uartdiag.exe --simulate valid
```

### CRC error

```powershell
.\build\uartdiag.exe --simulate crc
```

### Invalid length

```powershell
.\build\uartdiag.exe --simulate length
```

### Noise / resynchronization

```powershell
.\build\uartdiag.exe --simulate noise
```

### Run all scenarios

```powershell
.\build\uartdiag.exe --simulate all
```

### Custom log

```powershell
.\build\uartdiag.exe --simulate crc --log crc_test.csv
```

---

## Serial Mode

UARTDiag can process data from a Windows serial port.

### Basic

```powershell
.\build\uartdiag.exe --serial COM3
```

### With baud rate

```powershell
.\build\uartdiag.exe --serial COM3 115200
```

### With logging

```powershell
.\build\uartdiag.exe --serial COM3 115200 --log session.csv
```

Serial data enters the same processing pipeline used by simulation mode:

```text
Serial
  │
  ▼
Stream Parser
  │
  ▼
Decoder
  │
  ▼
Diagnostics
  │
  ├──► Console
  │
  └──► CSV
```

---

# Configuration

UARTDiag supports external configuration files through:

```text
--config <file>
```

Example `uartdiag.conf`:

```text
# UARTDiag configuration

baud_rate=115200
serial_read_size=256
simulation_log=uartdiag_configured.csv
serial_log=uartdiag_serial_configured.csv
```

### Available settings

| Setting            | Default                   | Description              |
| ------------------ | ------------------------- | ------------------------ |
| `baud_rate`        | `115200`                  | Default serial baud rate |
| `serial_read_size` | `256`                     | Serial read buffer size  |
| `simulation_log`   | `uartdiag_simulation.csv` | Simulation log filename  |
| `serial_log`       | `uartdiag_serial.csv`     | Serial log filename      |

### Example

```powershell
.\build\uartdiag.exe --config uartdiag.conf --simulate valid
```

Configuration can also be used with serial mode:

```powershell
.\build\uartdiag.exe --config uartdiag.conf --serial COM3
```

Command-line options override configuration defaults.

For example:

```powershell
.\build\uartdiag.exe --config uartdiag.conf --simulate valid --log override.csv
```

The configuration parser supports:

* `key=value` syntax
* Blank lines
* `#` comments
* Whitespace around keys and values
* Numeric validation
* Unknown-key detection
* Invalid-value detection
* Configuration validation

---

# Diagnostics

Each processed frame produces a structured diagnostic result.

### Diagnostic classification

| Condition          | Severity  |
| ------------------ | --------- |
| Valid frame        | `INFO`    |
| Frame too short    | `WARNING` |
| Invalid start byte | `WARNING` |
| Unknown frame type | `ERROR`   |
| Invalid length     | `ERROR`   |
| CRC error          | `ERROR`   |

Supported severity levels:

```text
INFO
WARNING
ERROR
CRITICAL
```

`CRITICAL` is available in the diagnostic model for future or higher-level conditions.

---

## Diagnostic Reports

Reports provide a consistent representation of frame-level diagnostic information.

A report can contain:

```text
Frame number
Timestamp
Frame type
Payload length
Raw frame
Expected CRC
Received CRC
Status
Diagnostic message
Severity
```

This information is available to both the console reporting and CSV logging layers.

---

# Session Statistics

UARTDiag maintains aggregate statistics during a processing session.

```text
Frames processed
Valid frames
Invalid frames

INFO diagnostics
WARNING diagnostics
ERROR diagnostics
CRITICAL diagnostics

Valid frames
Frame-too-short errors
Invalid-start errors
Invalid-length errors
Unknown-type errors
CRC errors
```

The same statistics model is used by both serial and simulation processing.

---

# Fault Analysis

The validation pipeline can be summarized as:

```text
Incoming Bytes
      │
      ▼
Find START
      │
      ▼
Read Header
      │
      ▼
Validate LENGTH
      │
      ▼
Wait for Complete Frame
      │
      ▼
Calculate CRC
      │
      ▼
Compare CRC
      │
      ▼
Diagnostic Result
```

UARTDiag can reproduce and analyze:

* Invalid start bytes
* CRC corruption
* Invalid lengths
* Unknown frame types
* Noise in the stream
* Partial frames
* Lost synchronization

The simulation layer makes these conditions reproducible without requiring a physical UART device.

---

# Logging

Diagnostic results can be written to CSV.

```powershell
.\build\uartdiag.exe --simulate valid --log session.csv
```

Example CSV structure:

```csv
timestamp,frame_number,type,payload_length,raw_data,crc_expected,crc_received,status,message,severity
```

Example record:

```csv
"2026-10-01 16:24:23",1,"SENSOR_DATA",4,"AA 01 04 10 20 30 40 BC",BC,BC,"VALID","Frame validated successfully.","INFO"
```

The logger records:

| Field          | Description                 |
| -------------- | --------------------------- |
| Timestamp      | Frame processing time       |
| Frame number   | Sequential frame identifier |
| Type           | Protocol message type       |
| Payload length | Payload size                |
| Raw data       | Complete frame              |
| Expected CRC   | Calculated CRC              |
| Received CRC   | CRC from frame              |
| Status         | Diagnostic status           |
| Message        | Diagnostic description      |
| Severity       | Diagnostic severity         |

Default filenames:

```text
uartdiag_simulation.csv
uartdiag_serial.csv
```

---

# Hex Frame Decoding

A captured frame can be analyzed directly from the command line:

```powershell
.\build\uartdiag.exe --decode "AA 01 04 10 20 30 40 BC"
```

This provides a convenient way to inspect captured UART traffic without connecting a serial device.

---

# Testing

UARTDiag uses **GoogleTest** for component-level testing and **CTest** for test execution and CLI regression testing.

<div align="center">

### Test Status

| Metric      |   Result |
| :---------- | -------: |
| Total tests |   **90** |
| Passing     |   **90** |
| Failing     |    **0** |
| Pass rate   | **100%** |

</div>

### Test coverage

The test suite covers:

* Frame encoding and decoding
* CRC calculation
* Frame validation
* Diagnostic classification
* Diagnostic severity
* Diagnostic summaries
* Stream parsing
* Stream resynchronization
* Simulation scenarios
* Fault injection
* CSV logging
* Configuration parsing
* Diagnostic reports
* Report consistency

### CLI regression tests

CTest also runs the actual executable for:

```text
cli_simulate_valid
cli_simulate_crc
cli_simulate_all
```

Run the complete suite:

```powershell
ctest --test-dir build --output-on-failure
```

Expected result:

```text
100% tests passed, 0 tests failed out of 90
```

---

# Build

## Requirements

* Windows
* C++17-compatible compiler
* CMake 3.20+
* GoogleTest
* MinGW/MSYS2 or another compatible toolchain

Development environment:

```text
Compiler : GCC 16.2.0
Toolchain: MSYS2 UCRT64
Language : C++17
Build    : CMake
Platform : Windows
```

## Configure

```powershell
cmake -S . -B build -G "MinGW Makefiles" `
  -DCMAKE_C_COMPILER=C:/msys64/ucrt64/bin/gcc.exe `
  -DCMAKE_CXX_COMPILER=C:/msys64/ucrt64/bin/g++.exe `
  -DCMAKE_MAKE_PROGRAM=C:/msys64/ucrt64/bin/mingw32-make.exe
```

## Build

```powershell
cmake --build build
```

## Test

```powershell
ctest --test-dir build --output-on-failure
```

---

# Testing Strategy

Testing is divided into component-level and executable-level validation.

```text
                    UARTDiag
                       │
              ┌────────┴────────┐
              │                 │
         Unit Tests          CLI Tests
              │                 │
              ▼                 ▼
        GoogleTest             CTest
              │                 │
     ┌────────┼────────┐        │
     │        │        │        │
   Frame   Stream   Config   Simulation
   CRC     Parser   Logger   Scenarios
   Decoder Reports  Reports  End-to-End
```

This provides coverage from individual protocol components through complete CLI execution.

---

# Technology Stack

<table>
<tr>
<td><strong>Language</strong></td>
<td>C++17</td>
</tr>
<tr>
<td><strong>Build System</strong></td>
<td>CMake</td>
</tr>
<tr>
<td><strong>Testing</strong></td>
<td>GoogleTest + CTest</td>
</tr>
<tr>
<td><strong>Compiler</strong></td>
<td>GCC / MinGW</td>
</tr>
<tr>
<td><strong>Serial Communication</strong></td>
<td>Windows API</td>
</tr>
<tr>
<td><strong>Logging</strong></td>
<td>CSV</td>
</tr>
<tr>
<td><strong>Version Control</strong></td>
<td>Git / GitHub</td>
</tr>
</table>

---

# Engineering Focus

UARTDiag demonstrates practical experience with:

```text
C++17
├── Modular design
├── RAII
├── STL
├── Error handling
└── Binary data processing

Embedded / Protocols
├── UART
├── Framing
├── CRC
└── Stream parsing

Software Engineering
├── CMake
├── Unit testing
├── Integration testing
├── Configuration management
└── Git

Diagnostics
├── Fault injection
├── Simulation
├── Severity classification
├── Reporting
├── Statistics
└── Persistent logging
```

The main design goal is separation of responsibilities: communication input, protocol parsing, diagnostics, reporting, and persistence are implemented as distinct components.

---

# Project Status

| Component                |        Status       |
| ------------------------ | :-----------------: |
| Protocol framing         |       Complete      |
| CRC validation           |       Complete      |
| Frame decoder            |       Complete      |
| Hex frame decoding       |       Complete      |
| Stream parser            |       Complete      |
| Stream resynchronization |       Complete      |
| Diagnostics              |       Complete      |
| Diagnostic severity      |       Complete      |
| Diagnostic summaries     |       Complete      |
| Structured reports       |       Complete      |
| Fault injection          |       Complete      |
| UART simulation          |       Complete      |
| Windows serial support   |       Complete      |
| CSV logging              |       Complete      |
| Session statistics       |       Complete      |
| External configuration   |       Complete      |
| CLI regression tests     |       Complete      |
| Automated tests          | **90 / 90 passing** |

---

# Future Work

Potential extensions include:

* Linux serial-port support
* Cross-platform serial abstraction
* Configurable protocol definitions
* Additional CRC algorithms
* JSON diagnostic output
* Log rotation
* Performance benchmarking
* Hardware-in-the-loop testing
* Additional protocol fault models

---

# License

This project is licensed under the terms provided in [`LICENSE`](LICENSE).

---

<div align="center">

## UARTDiag

**UART Protocol Diagnostics & Fault Analysis**

C++17 · CMake · GoogleTest · CTest

</div>
