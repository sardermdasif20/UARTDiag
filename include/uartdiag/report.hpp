#pragma once

#include "uartdiag/diagnostics.hpp"
#include "uartdiag/frame.hpp"

#include <cstdint>
#include <string>
#include <vector>

namespace uartdiag {

struct DiagnosticReport {
    std::vector<std::uint8_t> raw_data;

    bool has_frame_type{false};
    FrameType frame_type{FrameType::SensorData};

    bool has_payload_length{false};
    std::size_t payload_length{0};

    std::vector<std::uint8_t> payload;

    std::uint8_t expected_crc{0};
    std::uint8_t received_crc{0};

    DiagnosticResult result;

    /*
     * Optional serial-session metadata.
     */
    bool has_frame_number{false};
    std::size_t frame_number{0};

    bool has_timestamp{false};
    std::string timestamp;
};

std::string format_report(
    const DiagnosticReport& report
);

} // namespace uartdiag