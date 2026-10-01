#pragma once

#include <cstddef>
#include <string>

namespace uartdiag {

struct DiagnosticConfig {
    unsigned int default_baud_rate{115200};

    std::size_t serial_read_size{256};

    std::string simulation_log_filename{
        "uartdiag_simulation.csv"
    };

    std::string serial_log_filename{
        "uartdiag_serial.csv"
    };

    bool is_valid() const;
};

} // namespace uartdiag