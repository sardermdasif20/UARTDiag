#include "uartdiag/config.hpp"

#include <fstream>
#include <sstream>
#include <string>

namespace uartdiag {

namespace {

std::string trim(
    const std::string& value
) {
    const std::string whitespace =
        " \t\r\n";

    const std::size_t first =
        value.find_first_not_of(whitespace);

    if (first == std::string::npos) {
        return "";
    }

    const std::size_t last =
        value.find_last_not_of(whitespace);

    return value.substr(
        first,
        last - first + 1
    );
}

bool parse_unsigned_integer(
    const std::string& value,
    unsigned long long& result
) {
    try {
        std::size_t position = 0;

        result = std::stoull(
            value,
            &position
        );

        return position == value.size();
    }
    catch (const std::exception&) {
        return false;
    }
}

} // namespace

bool DiagnosticConfig::is_valid() const {
    if (default_baud_rate == 0) {
        return false;
    }

    if (serial_read_size == 0) {
        return false;
    }

    if (simulation_log_filename.empty()) {
        return false;
    }

    if (serial_log_filename.empty()) {
        return false;
    }

    return true;
}

bool load_config_file(
    const std::string& filename,
    DiagnosticConfig& config,
    std::string& error
) {
    std::ifstream file(filename);

    if (!file.is_open()) {
        error =
            "Could not open configuration file: " +
            filename;

        return false;
    }

    std::string line;
    std::size_t line_number = 0;

    while (std::getline(file, line)) {
        ++line_number;

        line = trim(line);

        if (
            line.empty() ||
            line[0] == '#'
        ) {
            continue;
        }

        const std::size_t separator =
            line.find('=');

        if (separator == std::string::npos) {
            error =
                "Invalid configuration line " +
                std::to_string(line_number) +
                ": expected key=value.";

            return false;
        }

        const std::string key =
            trim(
                line.substr(
                    0,
                    separator
                )
            );

        const std::string value =
            trim(
                line.substr(
                    separator + 1
                )
            );

        if (key.empty()) {
            error =
                "Invalid configuration line " +
                std::to_string(line_number) +
                ": empty key.";

            return false;
        }

        if (key == "baud_rate") {
            unsigned long long parsed_value = 0;

            if (
                !parse_unsigned_integer(
                    value,
                    parsed_value
                ) ||
                parsed_value > 0xFFFFFFFFULL
            ) {
                error =
                    "Invalid baud_rate on line " +
                    std::to_string(line_number) +
                    ".";

                return false;
            }

            config.default_baud_rate =
                static_cast<unsigned int>(
                    parsed_value
                );

            continue;
        }

        if (key == "serial_read_size") {
            unsigned long long parsed_value = 0;

            if (
                !parse_unsigned_integer(
                    value,
                    parsed_value
                )
            ) {
                error =
                    "Invalid serial_read_size on line " +
                    std::to_string(line_number) +
                    ".";

                return false;
            }

            config.serial_read_size =
                static_cast<std::size_t>(
                    parsed_value
                );

            continue;
        }

        if (key == "simulation_log") {
            if (value.empty()) {
                error =
                    "simulation_log cannot be empty "
                    "on line " +
                    std::to_string(line_number) +
                    ".";

                return false;
            }

            config.simulation_log_filename =
                value;

            continue;
        }

        if (key == "serial_log") {
            if (value.empty()) {
                error =
                    "serial_log cannot be empty "
                    "on line " +
                    std::to_string(line_number) +
                    ".";

                return false;
            }

            config.serial_log_filename =
                value;

            continue;
        }

        error =
            "Unknown configuration key '" +
            key +
            "' on line " +
            std::to_string(line_number) +
            ".";

        return false;
    }

    if (!config.is_valid()) {
        error =
            "Configuration file contains invalid values.";

        return false;
    }

    return true;
}

} // namespace uartdiag