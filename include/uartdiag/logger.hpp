#pragma once

#include "uartdiag/report.hpp"

#include <fstream>
#include <string>

namespace uartdiag {

class DiagnosticLogger {
public:
    DiagnosticLogger() = default;

    ~DiagnosticLogger();

    DiagnosticLogger(const DiagnosticLogger&) = delete;
    DiagnosticLogger& operator=(const DiagnosticLogger&) = delete;

    bool open(
        const std::string& filename,
        std::string& error
    );

    void close();

    bool is_open() const;

    bool write(
        const DiagnosticReport& report,
        std::string& error
    );

private:
    std::ofstream file_;
};

} // namespace uartdiag