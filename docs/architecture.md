# UARTDiag Architecture

## Overview

UARTDiag is a lightweight C++ tool for diagnosing UART-style
communication frames.

The system is divided into four primary components:

1. Frame
2. CRC
3. Decoder
4. Diagnostics

## Data Flow

Raw Bytes
    |
    v
+-----------+
|  Decoder  |
+-----------+
    |
    v
+-----------+
|   Frame   |
+-----------+
    |
    v
+-----------+
| CRC Check |
+-----------+
    |
    v
+---------------+
| Diagnostics   |
+---------------+
    |
    v
Diagnostic Report

## Components

### Frame

Represents a decoded communication frame.

### CRC

Calculates and validates the frame checksum.

### Decoder

Converts raw bytes into a structured Frame.

### Diagnostics

Converts protocol errors into human-readable diagnostic results.

## Design Goals

- Small and modular
- Easy to test
- Deterministic behavior
- Clear error reporting
- Suitable for embedded protocol experimentation