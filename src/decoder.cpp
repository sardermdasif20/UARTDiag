#include "uartdiag/decoder.hpp"
#include "uartdiag/crc.hpp"
#include "uartdiag/frame.hpp"

namespace uartdiag {

DiagnosticResult Decoder::decode(
    const std::vector<std::uint8_t>& raw_data,
    Frame& frame
) {
    DiagnosticResult result{};

    // --------------------------------
    // 1. Minimum frame size
    // --------------------------------

    if (raw_data.size() < 4) {

        result.status =
            DiagnosticStatus::FrameTooShort;

        result.severity =
            DiagnosticSeverity::Warning;

        result.message =
            "Frame contains insufficient bytes.";

        return result;
    }

    // --------------------------------
    // 2. Start byte validation
    // --------------------------------

    if (raw_data[0] != FRAME_START) {

        result.status =
            DiagnosticStatus::InvalidStartByte;

        result.severity =
            DiagnosticSeverity::Warning;

        result.message =
            "Invalid frame start byte.";

        return result;
    }

    // --------------------------------
    // 3. Validate frame type
    // --------------------------------

    const auto type =
        static_cast<FrameType>(raw_data[1]);

    if (type != FrameType::SensorData &&
        type != FrameType::Command &&
        type != FrameType::Response) {

        result.status =
            DiagnosticStatus::UnknownFrameType;

        result.severity =
            DiagnosticSeverity::Error;

        result.message =
            "Unknown frame type.";

        return result;
    }

    // --------------------------------
    // 4. Validate payload length
    // --------------------------------

    const std::size_t payload_length =
        raw_data[2];

    if (raw_data.size() != payload_length + 4) {

        result.status =
            DiagnosticStatus::InvalidLength;

        result.severity =
            DiagnosticSeverity::Error;

        result.message =
            "Payload length does not match frame size.";

        return result;
    }

    // --------------------------------
    // 5. Extract frame
    // --------------------------------

    frame.type = type;

    frame.payload.assign(
        raw_data.begin() + 3,
        raw_data.begin() + 3 + payload_length
    );

    frame.crc = raw_data.back();

    // --------------------------------
    // 6. Calculate CRC
    // --------------------------------

    std::vector<std::uint8_t> crc_data(
        raw_data.begin() + 1,
        raw_data.end() - 1
    );

    const auto expected_crc =
        calculate_crc(crc_data);

    result.expected_crc =
        expected_crc;

    result.received_crc =
        frame.crc;

    // --------------------------------
    // 7. Validate CRC
    // --------------------------------

    if (expected_crc != frame.crc) {

        result.status =
            DiagnosticStatus::CrcError;

        result.severity =
            DiagnosticSeverity::Error;

        result.message =
            "CRC validation failed. "
            "Possible data corruption.";

        return result;
    }

    // --------------------------------
    // Everything is valid
    // --------------------------------

    result.status =
        DiagnosticStatus::Valid;

    result.severity =
        DiagnosticSeverity::Info;

    result.message =
        "Frame validated successfully.";

    return result;
}

} // namespace uartdiag