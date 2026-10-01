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

struct DiagnosticResult {
    DiagnosticStatus status;

    std::uint8_t expected_crc{0};
    std::uint8_t received_crc{0};

    std::string message;

    bool is_valid() const;
};

std::string to_string(DiagnosticStatus status);

} // namespace uartdiag