#pragma once

#include <string>

namespace uartdiag {

enum class DiagnosticStatus {
    Valid,
    CrcError,
    FrameError,
    Timeout,
    InvalidData
};

std::string to_string(DiagnosticStatus status);

} // namespace uartdiag