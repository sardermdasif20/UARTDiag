#include "uartdiag/logger.hpp"

#include "uartdiag/diagnostics.hpp"

#include <iomanip>
#include <sstream>

namespace uartdiag {

namespace {

std::string format_bytes(
    const std::vector<std::uint8_t>& bytes
) {
    std::ostringstream output;

    for (std::size_t i = 0; i < bytes.size(); ++i) {

        if (i > 0) {
            output << ' ';
        }

        output
            << std::uppercase
            << std::hex
            << std::setw(2)
            << std::setfill('0')
            << static_cast<int>(bytes[i]);
    }

    return output.str();
}

std::string frame_type_to_string(
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

std::string escape_csv(
    const std::string& value
) {
    std::string escaped;

    escaped.reserve(
        value.size() + 2
    );

    escaped.push_back('"');

    for (const char character : value) {

        if (character == '"') {
            escaped += "\"\"";
        }
        else {
            escaped.push_back(character);
        }
    }

    escaped.push_back('"');

    return escaped;
}

} // namespace

DiagnosticLogger::~DiagnosticLogger() {
    close();
}

bool DiagnosticLogger::open(
    const std::string& filename,
    std::string& error
) {
    close();

    if (filename.empty()) {
        error = "Log filename cannot be empty.";
        return false;
    }

    file_.open(
        filename,
        std::ios::out |
        std::ios::app
    );

    if (!file_.is_open()) {
        error =
            "Could not open log file: " +
            filename;

        return false;
    }

    /*
     * Write the CSV header only when the file
     * is empty.
     */
    file_.seekp(
        0,
        std::ios::end
    );

    const auto file_size =
        file_.tellp();

    if (file_size == std::streampos(0)) {

        file_
            << "timestamp,"
            << "frame_number,"
            << "type,"
            << "payload_length,"
            << "raw_data,"
            << "crc_expected,"
            << "crc_received,"
            << "status,"
            << "message\n";

        if (!file_) {
            error =
                "Could not write CSV header.";

            close();

            return false;
        }
    }

    return true;
}

void DiagnosticLogger::close() {
    if (file_.is_open()) {
        file_.close();
    }
}

bool DiagnosticLogger::is_open() const {
    return file_.is_open();
}

bool DiagnosticLogger::write(
    const DiagnosticReport& report,
    std::string& error
) {
    if (!file_.is_open()) {
        error = "Diagnostic log is not open.";
        return false;
    }

    const std::string timestamp =
        report.has_timestamp
            ? report.timestamp
            : "";

    const std::string frame_number =
        report.has_frame_number
            ? std::to_string(report.frame_number)
            : "";

    const std::string type =
        report.has_frame_type
            ? frame_type_to_string(
                report.frame_type
            )
            : "UNKNOWN";

    file_
        << escape_csv(timestamp)
        << ','
        << frame_number
        << ','
        << escape_csv(type)
        << ','
        << report.payload_length
        << ','
        << escape_csv(
            format_bytes(report.raw_data)
        )
        << ','
        << std::uppercase
        << std::hex
        << std::setw(2)
        << std::setfill('0')
        << static_cast<int>(
            report.expected_crc
        )
        << ','
        << std::setw(2)
        << static_cast<int>(
            report.received_crc
        )
        << std::dec
        << ','
        << escape_csv(
            to_string(report.result.status)
        )
        << ','
        << escape_csv(
            report.result.message
        )
        << '\n';

    if (!file_) {
        error = "Could not write diagnostic log entry.";
        return false;
    }

    file_.flush();

    if (!file_) {
        error = "Could not flush diagnostic log entry.";
        return false;
    }

    return true;
}

} // namespace uartdiag