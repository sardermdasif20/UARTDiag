<div align="center">

# ⚡ UARTDiag

### UART Protocol Diagnostics & Fault Analysis

**A professional C++17 tool for UART frame validation, CRC analysis, fault injection, simulation, serial communication, and diagnostic logging.**

<br>

![C++17](https://img.shields.io/badge/C%2B%2B-17-00599C?style=for-the-badge\&logo=cplusplus\&logoColor=white)
![CMake](https://img.shields.io/badge/CMake-3.20%2B-064F8C?style=for-the-badge\&logo=cmake\&logoColor=white)
![Tests](https://img.shields.io/badge/Tests-46%20Passing-2ea44f?style=for-the-badge)
![Platform](https://img.shields.io/badge/Platform-Windows-0078D6?style=for-the-badge\&logo=windows\&logoColor=white)

<br>

**Protocol Engineering · Embedded Systems · Diagnostics · Fault Injection**

</div>

---

## 🚀 What is UARTDiag?

**UARTDiag** is a C++17 diagnostic and fault-analysis tool for framed UART communication.

It takes raw UART bytes, reconstructs protocol frames, validates their structure and CRC, detects communication problems, and produces structured diagnostic reports.

The project also includes a **software UART simulation mode**, allowing protocol faults to be reproduced without physical hardware.

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
└───────┬──────────┘
        │
   ┌────┴─────┐
   ▼          ▼
Console      CSV
Report       Logger
```

---

## ✨ Features

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

</td>
<td width="50%">

### 🧪 Testing

* GoogleTest
* CTest integration
* 46 automated tests
* Parser recovery tests
* Fault injection tests
* Logger tests

</td>
</tr>

<tr>
<td>

### 🛠 Diagnostics

* Structured diagnostic reports
* CRC error detection
* Invalid-length detection
* Noise recovery
* Frame numbering
* Timestamps

</td>
<td>

### 📊 Logging

* CSV diagnostic logs
* Configurable filenames
* Simulation logging
* Serial logging
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
       Physical UART                 Simulation
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
│   └── test_logger.cpp
│
├── docs/
├── examples/
│
├── CMakeLists.txt
├── README.md
└── LICENSE
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

---

# 🔌 Serial Mode

UARTDiag can also communicate with a real Windows serial port.

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

# 📊 Diagnostic Logging

UARTDiag can persist diagnostic results into CSV files.

Example:

```powershell
.\build\uartdiag.exe --simulate valid --log session.csv
```

Generated CSV:

```csv
timestamp,frame_number,type,payload_length,raw_data,crc_expected,crc_received,status,message
"2026-10-01 16:24:23",1,"SENSOR_DATA",4,"AA 01 04 10 20 30 40 BC",BC,BC,"VALID","Frame validated successfully."
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

Default filenames:

```text
uartdiag_simulation.csv
uartdiag_serial.csv
```

Custom filename:

```powershell
.\build\uartdiag.exe --simulate crc --log crc_test.csv
```

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

This makes the project useful for reproducing and investigating communication problems rather than simply decoding successful frames.

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
  -DCMAKE_CXX_COMPILER=C:/msys64/ucrt64/bin/g++.exe
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

### ✅ 46 / 46 TESTS PASSING

</div>

---

# 🧪 Testing Strategy

The test suite covers multiple layers of the application.

```text
┌──────────────────────────────┐
│        GoogleTest Suite      │
├──────────────────────────────┤
│                              │
│ Frame / CRC                  │
│ Decoder                      │
│ Diagnostics                 │
│ Stream Parser                │
│ Simulation                   │
│ Fault Injection              │
│ CSV Logger                   │
│                              │
└──────────────────────────────┘
```

Testing is integrated with CTest:

```powershell
ctest --test-dir build --output-on-failure
```

Current automated test count:

```text
46 tests
46 passing
```

---

# 💻 Technology Stack

<div align="center">

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

</div>

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
 │   └── Git
 │
 └── Diagnostics
     ├── Fault Injection
     ├── Simulation
     ├── Structured Reports
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
| Stream parser          |    ✅   |
| Diagnostics            |    ✅   |
| Fault injection        |    ✅   |
| Automated testing      |    ✅   |
| Windows serial support |    ✅   |
| UART simulation        |    ✅   |
| CSV logging            |    ✅   |
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
DIAGNOSTIC REPORT
   ↓
PERSISTENT LOG
```

The goal is to combine **embedded-systems concepts with modern C++ engineering practices**.

---

<div align="center">

### ⚡ UARTDiag

**UART Protocol Diagnostics & Fault Analysis**

Built with C++17 · CMake · GoogleTest

</div>
