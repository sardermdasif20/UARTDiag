<div align="center">

# ⚡ UARTDiag

### UART Protocol Diagnostics & Fault Analysis

**A professional C++17 tool for UART frame validation, CRC analysis, fault injection, simulation, serial communication, configuration, and diagnostic logging.**

<br>

![C++17](https://img.shields.io/badge/C%2B%2B-17-00599C?style=for-the-badge\&logo=cplusplus\&logoColor=white)
![CMake](https://img.shields.io/badge/CMake-3.20%2B-064F8C?style=for-the-badge\&logo=cmake\&logoColor=white)
![Tests](https://img.shields.io/badge/Tests-71%20Passing-2ea44f?style=for-the-badge)
![Platform](https://img.shields.io/badge/Platform-Windows-0078D6?style=for-the-badge\&logo=windows\&logoColor=white)

<br>

**Protocol Engineering · Embedded Systems · Diagnostics · Fault Injection**

</div>

---

# 🚀 What is UARTDiag?

**UARTDiag** is a C++17 diagnostic and fault-analysis tool for framed UART communication.

It takes raw UART bytes, reconstructs protocol frames, validates their structure and CRC, detects communication problems, classifies diagnostic severity, and produces structured reports.

The project also includes:

* Software UART simulation
* Fault injection
* Windows serial communication
* Stream resynchronization
* Diagnostic severity classification
* CSV logging
* External configuration files
* Automated GoogleTest/CTest testing

The architecture is designed to keep protocol processing independent from the physical communication layer.

```text
Raw UART Data
      │
      ▼
┌──────────────────┐
│  Stream Parser   │
└────────┬─────────┘
         │
         ▼
┌──────────────────┐
│  Frame Decoder   │
└────────┬─────────┘
         │
         ▼
┌──────────────────┐
│  CRC Validation  │
└────────┬─────────┘
         │
         ▼
┌──────────────────┐
│   Diagnostics    │
└────────┬─────────┘
         │
    ┌────┴─────┐
    ▼          ▼
 Console       CSV
 Report       Logger
```

---

# ✨ Features

<table>
<tr>
<td width="50%">

### 🔌 Protocol

* UART-style framed protocol
* Frame encoding / decoding
* CRC-8 validation
* Payload validation
* Binary data processing
* Stream synchronization
* Noise recovery
* Hex frame decoding

</td>
<td width="50%">

### 🧪 Testing

* GoogleTest
* CTest integration
* 71 automated tests
* Parser recovery tests
* Fault injection tests
* Simulation tests
* Logger tests
* Configuration tests

</td>
</tr>

<tr>
<td>

### 🛠 Diagnostics

* Structured diagnostic reports
* Diagnostic severity classification
* CRC error detection
* Invalid-length detection
* Unknown-frame detection
* Frame numbering
* Timestamps
* Session statistics

</td>
<td>

### 📊 Logging & Configuration

* CSV diagnostic logs
* Configurable filenames
* Simulation logging
* Serial logging
* External configuration files
* Configurable serial read size
* CLI configuration overrides
* Persistent diagnostic history

</td>
</tr>
</table>

---

# 📡 Protocol

UARTDiag uses a simple framed binary protocol:

```text
┌────────┬────────┬────────┬───────────────────┬────────┐
│ START  │  TYPE  │ LENGTH │      PAYLOAD      │  CRC   │
├────────┼────────┼────────┼───────────────────┼────────┤
│  1 B   │  1 B   │  1 B   │      0–255 B      │  1 B   │
└────────┴────────┴────────┴───────────────────┴────────┘
```

### Frame fields

| Field     |     Size    | Description                |
| --------- | :---------: | -------------------------- |
| `START`   |    1 byte   | Frame synchronization byte |
| `TYPE`    |    1 byte   | Message type               |
| `LENGTH`  |    1 byte   | Payload size               |
| `PAYLOAD` | 0–255 bytes | Application data           |
| `CRC`     |    1 byte   | CRC-8 checksum             |

### Synchronization

```text
START = 0xAA
```

### CRC

```text
Algorithm : CRC-8
Polynomial: 0x07
Initial   : 0x00
```

### Example

```text
AA 01 04 10 20 30 40 BC
```

Decoded:

```text
START   = AA
TYPE    = 01
LENGTH  = 04
PAYLOAD = 10 20 30 40
CRC     = BC
```

---

# 🏗 Architecture

```text
                         UARTDiag
                            │
             ┌──────────────┴──────────────┐
             │                             │
       Physical UART                  Simulation
             │                             │
             └──────────────┬──────────────┘
                            ▼
                    ┌───────────────┐
                    │ Stream Parser │
                    └───────┬───────┘
                            ▼
                    ┌───────────────┐
                    │ Frame Decoder │
                    └───────┬───────┘
                            ▼
                    ┌───────────────┐
                    │ CRC Validation│
                    └───────┬───────┘
                            ▼
                    ┌───────────────┐
                    │  Diagnostics  │
                    └───────┬───────┘
                            │
                 ┌──────────┴──────────┐
                 ▼                     ▼
          Console Report          CSV Logger
```

### Project structure

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
│   └── test_config.cpp
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

# 🧪 Simulation

One of the key features of UARTDiag is the ability to reproduce UART problems without physical hardware.

### Start simulation

```powershell
.\build\uartdiag.exe --simulate
```

### Valid frame

```powershell
.\build\uartdiag.exe --simulate valid
```

### CRC fault

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

### Custom simulation log

```powershell
.\build\uartdiag.exe --simulate crc --log crc_test.csv
```

Simulation and physical serial input use the same stream-processing architecture.

---

# 🔌 Serial Mode

UARTDiag can communicate with a real Windows serial port.

### Basic

```powershell
.\build\uartdiag.exe --serial COM3
```

### Specify baud rate

```powershell
.\build\uartdiag.exe --serial COM3 115200
```

### Serial logging

```powershell
.\build\uartdiag.exe --serial COM3 115200 --log session.csv
```

The serial input and simulation input eventually use the same processing pipeline:

```text
Serial / Simulation
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
        ├──────────► Console
        │
        └──────────► CSV
```

This keeps the protocol logic independent from the physical communication layer.

---

# ⚙️ Configuration

UARTDiag supports an external configuration file using:

```text
--config <file>
```

This allows common runtime settings to be stored separately from the command line.

### Example configuration

Create a file named:

```text
uartdiag.conf
```

with:

```text
# UARTDiag configuration

baud_rate=115200
serial_read_size=256
simulation_log=uartdiag_configured.csv
serial_log=uartdiag_serial_configured.csv
```

### Supported settings

| Key                | Description                                      | Default                   |
| ------------------ | ------------------------------------------------ | ------------------------- |
| `baud_rate`        | Default serial baud rate                         | `115200`                  |
| `serial_read_size` | Number of bytes read from serial input at a time | `256`                     |
| `simulation_log`   | Default simulation CSV filename                  | `uartdiag_simulation.csv` |
| `serial_log`       | Default serial CSV filename                      | `uartdiag_serial.csv`     |

### Use a configuration file

```powershell
.\build\uartdiag.exe --config uartdiag.conf --simulate valid
```

The application reports:

```text
Configuration loaded: uartdiag.conf
```

### Configuration with serial mode

```powershell
.\build\uartdiag.exe --config uartdiag.conf --serial COM3
```

The configured baud rate and serial read size are used unless explicitly overridden by the command line.

### Command-line override

Explicit command-line options take precedence over configuration defaults.

For example:

```powershell
.\build\uartdiag.exe --config uartdiag.conf --simulate valid --log override.csv
```

The configuration file may specify one simulation log filename, but:

```text
--log override.csv
```

overrides it for that execution.

### Configuration behavior

The configuration parser supports:

* `key=value` syntax
* Blank lines
* Comments beginning with `#`
* Whitespace around keys and values
* Numeric validation
* Unknown-key detection
* Invalid-value detection
* Configuration validation

Invalid configuration files are rejected with a descriptive error message.

Example:

```text
Configuration error: Could not open configuration file: uartdiag.conf
```

---

# 📊 Diagnostic Logging

UARTDiag can persist diagnostic results into CSV files.

Example:

```powershell
.\build\uartdiag.exe --simulate valid --log session.csv
```

Generated CSV structure:

```csv
timestamp,frame_number,type,payload_length,raw_data,crc_expected,crc_received,status,message,severity
"2026-10-01 16:24:23",1,"SENSOR_DATA",4,"AA 01 04 10 20 30 40 BC",BC,BC,"VALID","Frame validated successfully.","INFO"
```

### Logged information

| Field          | Purpose                               |
| -------------- | ------------------------------------- |
| Timestamp      | When the frame was processed          |
| Frame number   | Sequential frame identifier           |
| Type           | Protocol message type                 |
| Payload length | Number of payload bytes               |
| Raw data       | Complete received frame               |
| Expected CRC   | CRC calculated by UARTDiag            |
| Received CRC   | CRC contained in frame                |
| Status         | Diagnostic classification             |
| Message        | Human-readable diagnostic information |
| Severity       | Diagnostic severity level             |

### Default filenames

```text
uartdiag_simulation.csv
uartdiag_serial.csv
```

### Custom filename

```powershell
.\build\uartdiag.exe --simulate crc --log crc_test.csv
```

---

# 🚦 Diagnostic Severity

UARTDiag classifies diagnostic results using severity levels.

```text
INFO
WARNING
ERROR
CRITICAL
```

Current protocol diagnostics include classifications such as:

| Diagnostic condition | Severity  |
| -------------------- | --------- |
| Valid frame          | `INFO`    |
| Frame too short      | `WARNING` |
| Invalid start byte   | `WARNING` |
| Unknown frame type   | `ERROR`   |
| Invalid length       | `ERROR`   |
| CRC error            | `ERROR`   |

Severity is available in:

* Console reports
* Diagnostic objects
* CSV logs
* Session statistics

This makes the diagnostic output more useful when processing large communication sessions.

---

# 📈 Session Statistics

UARTDiag tracks session-level statistics while processing input.

Statistics include:

```text
Frames processed
Valid frames
Invalid frames
Info diagnostics
Warning diagnostics
Error diagnostics
Critical diagnostics
```

The same statistics system is used for both:

```text
--serial
```

and:

```text
--simulate
```

This provides consistent diagnostic behavior across physical and simulated UART input.

---

# 🧨 Fault Analysis

UARTDiag is designed not only to process valid communication, but also to identify failures.

### Example diagnostic flow

```text
Incoming Frame
      │
      ▼
Is START valid?
      │
      ├── NO ──► Resynchronize
      │
      ▼
Is LENGTH valid?
      │
      ├── NO ──► INVALID_LENGTH
      │
      ▼
Calculate CRC
      │
      ▼
CRC matches?
      │
      ├── NO ──► CRC_ERROR
      │
      ▼
    VALID
```

The project can reproduce faults such as:

* Invalid start bytes
* CRC corruption
* Invalid lengths
* Unknown frame types
* Stream noise
* Partial frames
* Frame synchronization loss

This makes the project useful for investigating communication failures rather than simply decoding successful frames.

---

# 🔎 Hex Frame Decoding

UARTDiag also provides a CLI interface for decoding hexadecimal frames.

Example:

```powershell
.\build\uartdiag.exe --decode "AA 01 04 10 20 30 40 BC"
```

This allows a captured UART frame to be analyzed directly without connecting a physical serial device.

---

# 🧰 Build

## Requirements

* Windows
* C++17 compiler
* CMake 3.20+
* GoogleTest
* MinGW/MSYS2 or another compatible C++ toolchain

The current development environment uses:

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

Current result:

<div align="center">

### ✅ 71 / 71 TESTS PASSING

</div>

---

# 🧪 Testing Strategy

The test suite covers multiple layers of the application.

```text
┌──────────────────────────────┐
│       GoogleTest Suite       │
├──────────────────────────────┤
│                              │
│ Frame / CRC                  │
│ Decoder                      │
│ Diagnostics                 │
│ Diagnostic Severity          │
│ Stream Parser                │
│ Simulation                   │
│ Fault Injection              │
│ CSV Logger                   │
│ Configuration                │
│                              │
└──────────────────────────────┘
```

Testing is integrated with CTest:

```powershell
ctest --test-dir build --output-on-failure
```

Current automated test count:

```text
71 tests
71 passing
```

The test suite verifies both normal operation and failure conditions.

---

# 💻 Technology Stack

| Technology       | Usage                  |
| ---------------- | ---------------------- |
| **C++17**        | Core implementation    |
| **CMake**        | Build system           |
| **GoogleTest**   | Unit testing           |
| **CTest**        | Test execution         |
| **GCC / MinGW**  | Compiler               |
| **Windows API**  | Serial communication   |
| **CSV**          | Diagnostic persistence |
| **Git / GitHub** | Version control        |

---

# 🎯 Engineering Concepts Demonstrated

This project focuses on practical engineering skills:

```text
C++
│
├── Object-Oriented Design
├── RAII
├── STL
├── Binary Data Processing
├── Error Handling
│
├── Embedded Concepts
│   ├── UART
│   ├── Framing
│   ├── CRC
│   └── Stream Parsing
│
├── Software Engineering
│   ├── CMake
│   ├── Unit Testing
│   ├── Modular Architecture
│   ├── Configuration Management
│   └── Git
│
└── Diagnostics
    ├── Fault Injection
    ├── Simulation
    ├── Severity Classification
    ├── Structured Reports
    ├── Session Statistics
    └── Persistent Logging
```

---

# 🛣 Roadmap

Potential future improvements:

* [ ] Linux serial-port support
* [ ] Cross-platform serial abstraction
* [ ] Configurable protocol definitions
* [ ] Additional CRC algorithms
* [ ] JSON diagnostic reports
* [ ] Real-time terminal visualization
* [ ] Log rotation
* [ ] Configurable logging levels
* [ ] Performance benchmarking
* [ ] Hardware-in-the-loop testing

---

# 📌 Project Status

<div align="center">

| Component              | Status |
| ---------------------- | :----: |
| Protocol framing       |    ✅   |
| CRC validation         |    ✅   |
| Frame decoder          |    ✅   |
| Hex frame decoding     |    ✅   |
| Stream parser          |    ✅   |
| Diagnostics            |    ✅   |
| Diagnostic severity    |    ✅   |
| Fault injection        |    ✅   |
| Automated testing      |    ✅   |
| Windows serial support |    ✅   |
| UART simulation        |    ✅   |
| CSV logging            |    ✅   |
| Session statistics     |    ✅   |
| External configuration |    ✅   |
| Configurable logging   |    ✅   |

<br>

**UARTDiag is actively developed as an embedded/protocol diagnostics portfolio project.**

</div>

---

# 👨‍💻 Why This Project?

UARTDiag was built to demonstrate how low-level communication problems can be approached using structured software engineering techniques.

Instead of treating UART communication as a simple byte stream, the project provides:

```text
RAW DATA
   ↓
PROTOCOL PARSING
   ↓
VALIDATION
   ↓
FAULT DETECTION
   ↓
SEVERITY CLASSIFICATION
   ↓
DIAGNOSTIC REPORT
   ↓
PERSISTENT LOG
```

The goal is to combine **embedded-systems concepts with modern C++ engineering practices**.

The project demonstrates a complete path from raw communication data to structured, testable, and persistent diagnostic information.

---

<div align="center">

### ⚡ UARTDiag

**UART Protocol Diagnostics & Fault Analysis**

Built with C++17 · CMake · GoogleTest

</div>
