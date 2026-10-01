#pragma once

#include <cstdint>
#include <string>

namespace uartdiag {

enum class DiagnosticStatus {
    Valid,
    FrameTooShort,
    InvalidStartByte,
    InvalidLength,
    UnknownFrameType,
    CrcError
};

enum class DiagnosticSeverity {
    Info,
    Warning,
    Error,
    Critical
};

struct DiagnosticResult {
    DiagnosticStatus status;

    DiagnosticSeverity severity{
        DiagnosticSeverity::Info
    };

    std::uint8_t expected_crc{0};
    std::uint8_t received_crc{0};

    std::string message;

    bool is_valid() const;
};

std::string to_string(
    DiagnosticStatus status
);

std::string to_string(
    DiagnosticSeverity severity
);

} // namespace uartdiag
