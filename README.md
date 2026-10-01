<div align="center">

# ⚡ UARTDiag

### UART Protocol Diagnostics & Fault Analysis

**A C++17 tool for working with UART data, checking frames, finding communication errors, and testing different failure cases.**

<br>

[![C++17](https://img.shields.io/badge/C%2B%2B-17-00599C?style=for-the-badge\&logo=cplusplus\&logoColor=white)](#)
[![CMake](https://img.shields.io/badge/CMake-3.20%2B-064F8C?style=for-the-badge\&logo=cmake\&logoColor=white)](#)
[![Tests](https://img.shields.io/badge/Tests-90%2F90-2ea043?style=for-the-badge)](#testing)
[![Platform](https://img.shields.io/badge/Platform-Windows-0078D4?style=for-the-badge\&logo=windows\&logoColor=white)](#)

</div>

---

## About the Project

UARTDiag is a small C++17 project I built to practice working with **UART communication and binary protocols**.

The program takes UART byte streams, finds and decodes frames, checks their length and CRC, and then reports whether the frame is valid or what went wrong.

I also added a software simulation mode so the protocol can be tested without needing actual UART hardware.

The basic flow looks like this:

```text
Serial / Simulation
        │
        ▼
  Stream Parser
        │
        ▼
  Frame Decoder
        │
        ▼
 CRC / Validation
        │
        ▼
   Diagnostics
      │    │
      ▼    ▼
   Console CSV
```

The main idea was to keep the input side separate from the protocol logic. This lets the same processing code work with both simulated data and a real serial connection.

---

## What It Can Do

### UART / Protocol

* Encode and decode binary frames
* Check CRC-8
* Validate payload length
* Validate frame types
* Decode hexadecimal frames
* Process UART data as a continuous stream
* Handle incomplete frames
* Recover synchronization after invalid data

### Diagnostics

* Report valid and invalid frames
* Assign severity levels
* Identify different types of errors
* Keep track of frame numbers
* Record timestamps
* Generate diagnostic summaries and reports

### Simulation

The simulator can reproduce different situations without requiring a physical UART device.

* Valid frames
* CRC errors
* Invalid lengths
* Noise
* Synchronization recovery
* Fault injection

### Logging & Configuration

* Save diagnostic results to CSV
* Configure log filenames
* Configure serial settings
* Use external configuration files
* Override configuration from the command line

---

## Architecture

The project has two possible sources of UART data:

```text
              UARTDiag
                 │
        ┌────────┴────────┐
        │                 │
     Serial          Simulation
        │                 │
        └────────┬────────┘
                 ▼
          Stream Parser
                 │
                 ▼
          Frame Decoder
                 │
                 ▼
        CRC / Validation
                 │
                 ▼
           Diagnostics
             │     │
             ▼     ▼
          Console  CSV
```

One of the things I wanted to avoid was having separate protocol logic for the simulator and the serial connection.

Both input methods therefore go through the same parser, decoder, validation, and diagnostic stages.

---

## UART Frame Format

UARTDiag uses a simple binary frame:

```text
┌────────┬────────┬────────┬───────────────────┬────────┐
│ START  │  TYPE  │ LENGTH │      PAYLOAD      │  CRC   │
├────────┼────────┼────────┼───────────────────┼────────┤
│  1 B   │  1 B   │  1 B   │      0–255 B      │  1 B   │
└────────┴────────┴────────┴───────────────────┴────────┘
```

| Field     |        Size | Description                |
| --------- | ----------: | -------------------------- |
| `START`   |      1 byte | Start/synchronization byte |
| `TYPE`    |      1 byte | Frame type                 |
| `LENGTH`  |      1 byte | Payload length             |
| `PAYLOAD` | 0–255 bytes | Actual data                |
| `CRC`     |      1 byte | CRC-8 checksum             |

### Protocol settings

```text
START       0xAA
CRC         CRC-8
Polynomial  0x07
Initial     0x00
```

Example frame:

```text
AA 01 04 10 20 30 40 BC
```

---

## Stream Parsing

UART data does not always arrive as a complete frame.

For example, a frame might arrive in pieces:

```text
AA 01 04
```

followed later by:

```text
10 20 30 40 BC
```

The parser needs to keep the first part until enough bytes arrive to complete the frame.

UARTDiag also deals with unwanted bytes in the stream and attempts to find the next valid start byte so that processing can continue.

The general process is:

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
Check Length
      │
      ▼
Wait for Complete Frame
      │
      ▼
   Check CRC
      │
      ▼
Diagnostic Result
```

---

## Fault Analysis

The simulator can intentionally create bad frames so that the error-handling code can be tested.

Supported cases include:

* CRC corruption
* Invalid lengths
* Invalid start bytes
* Unknown frame types
* Noise
* Partial frames
* Lost synchronization

This makes it possible to test how the parser behaves when the incoming UART data is not perfect.

---

## Diagnostics

Each processed frame produces a diagnostic result.

| Condition          | Severity  |
| ------------------ | --------- |
| Valid frame        | `INFO`    |
| Frame too short    | `WARNING` |
| Invalid start byte | `WARNING` |
| Unknown frame type | `ERROR`   |
| Invalid length     | `ERROR`   |
| CRC error          | `ERROR`   |

A report can contain information such as:

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

## Building

### Requirements

* Windows
* C++17 compiler
* CMake 3.20+
* GCC / MinGW
* GoogleTest

### Configure the project

```powershell
cmake -S . -B build -G "MinGW Makefiles" `
  -DCMAKE_C_COMPILER=C:/msys64/ucrt64/bin/gcc.exe `
  -DCMAKE_CXX_COMPILER=C:/msys64/ucrt64/bin/g++.exe `
  -DCMAKE_MAKE_PROGRAM=C:/msys64/ucrt64/bin/mingw32-make.exe
```

### Build

```powershell
cmake --build build
```

---

## Usage

### Run the simulator

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

For example:

```powershell
.\build\uartdiag.exe --simulate crc
```

Run everything:

```powershell
.\build\uartdiag.exe --simulate all
```

Save the results to a custom CSV file:

```powershell
.\build\uartdiag.exe --simulate crc --log crc_test.csv
```

---

## Serial Mode

UARTDiag can also read from a Windows COM port.

```powershell
.\build\uartdiag.exe --serial COM3
```

Specify the baud rate:

```powershell
.\build\uartdiag.exe --serial COM3 115200
```

Save the session:

```powershell
.\build\uartdiag.exe --serial COM3 115200 --log session.csv
```

---

## Hex Decoder

A captured frame can also be checked directly:

```powershell
.\build\uartdiag.exe --decode "AA 01 04 10 20 30 40 BC"
```

This is useful when I want to test a captured frame without connecting a physical UART device.

---

## Configuration

UARTDiag supports a configuration file.

Example:

```text
baud_rate=115200
serial_read_size=256
simulation_log=uartdiag_simulation.csv
serial_log=uartdiag_serial.csv
```

Run with the configuration file:

```powershell
.\build\uartdiag.exe --config uartdiag.conf --simulate
```

| Setting            | Default                   |
| ------------------ | ------------------------- |
| `baud_rate`        | `115200`                  |
| `serial_read_size` | `256`                     |
| `simulation_log`   | `uartdiag_simulation.csv` |
| `serial_log`       | `uartdiag_serial.csv`     |

Command-line options can override the configuration values.

---

## Logging

Diagnostic information can be saved as CSV.

```powershell
.\build\uartdiag.exe --simulate valid --log session.csv
```

The CSV contains fields such as:

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

Default log files are:

```text
uartdiag_simulation.csv
uartdiag_serial.csv
```

---

## Testing

The project currently has:

### 🟢 90 / 90 tests passing

The tests cover:

* Frame encoding and decoding
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

There are also CLI regression tests for:

```text
cli_simulate_valid
cli_simulate_crc
cli_simulate_all
```

Run the tests with:

```powershell
ctest --test-dir build --output-on-failure
```

Expected result:

```text
100% tests passed, 0 tests failed out of 90
```

---

## Project Structure

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

## Technologies Used

| Technology       | Used For             |
| ---------------- | -------------------- |
| **C++17**        | Main application     |
| **CMake**        | Build system         |
| **GoogleTest**   | Testing              |
| **CTest**        | Running tests        |
| **GCC / MinGW**  | Compilation          |
| **Windows API**  | Serial communication |
| **CSV**          | Logging              |
| **Git / GitHub** | Version control      |

---

## What I Practiced With This Project

This project gave me practical experience with:

```text
C++17
├── Modular code
├── STL
├── RAII
├── Error handling
└── Binary data

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
├── Severity levels
├── Reporting
├── Statistics
└── Logging
```

---

## Current Status

| Component         |    Status   |
| ----------------- | :---------: |
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

---

## Possible Next Steps

Some things I would like to add in the future:

* Linux serial support
* Cross-platform serial abstraction
* Additional CRC algorithms
* JSON diagnostic output
* Log rotation
* Performance benchmarks
* Hardware-in-the-loop testing
* More protocol fault models

---

<div align="center">

# ⚡ UARTDiag

### UART Protocol Diagnostics & Fault Analysis

**C++17 · CMake · GoogleTest · CTest**

<br>

Built as a practical embedded-systems and software-engineering project.

</div>
