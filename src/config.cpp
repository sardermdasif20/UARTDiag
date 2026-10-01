#include "uartdiag/config.hpp"

namespace uartdiag {

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

} // namespace uartdiag