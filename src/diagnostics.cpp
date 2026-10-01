#include "uartdiag/diagnostics.hpp"

namespace uartdiag {

bool DiagnosticResult::is_valid() const {
    return status == DiagnosticStatus::Valid;
}

std::string to_string(
    DiagnosticStatus status
) {
    switch (status) {

        case DiagnosticStatus::Valid:
            return "VALID";

        case DiagnosticStatus::FrameTooShort:
            return "FRAME_TOO_SHORT";

        case DiagnosticStatus::InvalidStartByte:
            return "INVALID_START_BYTE";

        case DiagnosticStatus::InvalidLength:
            return "INVALID_LENGTH";

        case DiagnosticStatus::UnknownFrameType:
            return "UNKNOWN_FRAME_TYPE";

        case DiagnosticStatus::CrcError:
            return "CRC_ERROR";
    }

    return "UNKNOWN";
}

std::string to_string(
    DiagnosticSeverity severity
) {
    switch (severity) {

        case DiagnosticSeverity::Info:
            return "INFO";

        case DiagnosticSeverity::Warning:
            return "WARNING";

        case DiagnosticSeverity::Error:
            return "ERROR";

        case DiagnosticSeverity::Critical:
            return "CRITICAL";
    }

    return "UNKNOWN";
}

} // namespace uartdiag