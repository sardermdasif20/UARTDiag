#include "uartdiag/diagnostics.hpp"

namespace uartdiag {

bool DiagnosticResult::is_valid() const {
    return status == DiagnosticStatus::Valid;
}

std::string to_string(DiagnosticStatus status) {

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

} // namespace uartdiag