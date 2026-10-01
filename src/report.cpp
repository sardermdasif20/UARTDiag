#include "uartdiag/report.hpp"

#include <iomanip>
#include <sstream>

namespace uartdiag {

namespace {

std::string format_bytes(
    const std::vector<std::uint8_t>& bytes
) {
    std::ostringstream output;

    for (const auto byte : bytes) {
        output
            << std::uppercase
            << std::hex
            << std::setw(2)
            << std::setfill('0')
            << static_cast<int>(byte)
            << ' ';
    }

    return output.str();
}

const char* frame_type_to_string(
    FrameType type
) {
    switch (type) {

        case FrameType::SensorData:
            return "SENSOR_DATA";

        case FrameType::Command:
            return "COMMAND";

        case FrameType::Response:
            return "RESPONSE";
    }

    return "UNKNOWN";
}

} // namespace

std::string format_report(
    const DiagnosticReport& report
) {
    std::ostringstream output;

    output
        << "\nUARTDiag Diagnostic Report\n"
        << "========================================\n";

    output
        << "\nInput\n"
        << "-----\n"
        << format_bytes(report.raw_data)
        << '\n';

    output
        << "\nFrame\n"
        << "-----\n";

    if (report.has_frame_type) {
        output
            << "Type:           "
            << frame_type_to_string(report.frame_type)
            << '\n';
    }
    else {
        output
            << "Type:           UNKNOWN\n";
    }

    if (report.has_payload_length) {
        output
            << "Payload Length: "
            << report.payload_length
            << '\n';
    }

    if (!report.payload.empty() ||
        report.result.status == DiagnosticStatus::Valid) {

        output
            << "Payload:        "
            << format_bytes(report.payload)
            << '\n';
    }

    output
        << "\nIntegrity\n"
        << "---------\n";

    output
        << "CRC Expected:   0x"
        << std::uppercase
        << std::hex
        << std::setw(2)
        << std::setfill('0')
        << static_cast<int>(report.expected_crc)
        << '\n';

    output
        << "CRC Received:   0x"
        << std::uppercase
        << std::hex
        << std::setw(2)
        << std::setfill('0')
        << static_cast<int>(report.received_crc)
        << '\n';

    if (report.result.status == DiagnosticStatus::Valid) {
        output
            << "CRC Status:     PASS\n";
    }
    else if (
        report.result.status ==
        DiagnosticStatus::CrcError) {

        output
            << "CRC Status:     FAIL\n";
    }
    else {
        output
            << "CRC Status:     NOT VERIFIED\n";
    }

    output
        << "\nResult\n"
        << "------\n"
        << "Status:         "
        << to_string(report.result.status)
        << '\n'
        << "Message:        "
        << report.result.message
        << '\n';

    output
        << "========================================\n";

    return output.str();
}

} // namespace uartdiag