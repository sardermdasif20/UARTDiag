#pragma once

#include <cstdint>
#include <string>
#include <vector>

namespace uartdiag {

struct SerialConfig {
    std::string port;
    unsigned int baud_rate{115200};
};

class SerialPort {
public:
    SerialPort() = default;

    ~SerialPort();

    SerialPort(const SerialPort&) = delete;
    SerialPort& operator=(const SerialPort&) = delete;

    bool open(
        const SerialConfig& config,
        std::string& error
    );

    void close();

    bool is_open() const;

    bool read(
        std::vector<std::uint8_t>& data,
        std::size_t max_bytes,
        std::string& error
    );

private:
    void* handle_{nullptr};
};

} // namespace uartdiag