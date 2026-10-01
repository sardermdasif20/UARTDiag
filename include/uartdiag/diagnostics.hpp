#pragma once

#include <cstddef>
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

struct DiagnosticSummary {
    std::size_t frames_processed{0};

    std::size_t valid_frames{0};
    std::size_t invalid_frames{0};

    std::size_t info_count{0};
    std::size_t warning_count{0};
    std::size_t error_count{0};
    std::size_t critical_count{0};

    std::size_t valid_count{0};
    std::size_t frame_too_short_count{0};
    std::size_t invalid_start_byte_count{0};
    std::size_t invalid_length_count{0};
    std::size_t unknown_frame_type_count{0};
    std::size_t crc_error_count{0};

    void record(
        const DiagnosticResult& result
    );
};

std::string to_string(
    DiagnosticStatus status
);

std::string to_string(
    DiagnosticSeverity severity
);

std::string format_summary(
    const DiagnosticSummary& summary
);

} // namespace uartdiag