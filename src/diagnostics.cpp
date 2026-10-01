#include "uartdiag/diagnostics.hpp"

namespace uartdiag {

bool DiagnosticResult::is_valid() const {
    return status == DiagnosticStatus::Valid;
}

void DiagnosticSummary::record(
    const DiagnosticResult& result
) {
    ++frames_processed;

    if (result.is_valid()) {
        ++valid_frames;
    }
    else {
        ++invalid_frames;
    }

    switch (result.severity) {

        case DiagnosticSeverity::Info:
            ++info_count;
            break;

        case DiagnosticSeverity::Warning:
            ++warning_count;
            break;

        case DiagnosticSeverity::Error:
            ++error_count;
            break;

        case DiagnosticSeverity::Critical:
            ++critical_count;
            break;
    }

    switch (result.status) {

        case DiagnosticStatus::Valid:
            ++valid_count;
            break;

        case DiagnosticStatus::FrameTooShort:
            ++frame_too_short_count;
            break;

        case DiagnosticStatus::InvalidStartByte:
            ++invalid_start_byte_count;
            break;

        case DiagnosticStatus::InvalidLength:
            ++invalid_length_count;
            break;

        case DiagnosticStatus::UnknownFrameType:
            ++unknown_frame_type_count;
            break;

        case DiagnosticStatus::CrcError:
            ++crc_error_count;
            break;
    }
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