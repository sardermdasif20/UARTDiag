#include "uartdiag/crc.hpp"

namespace uartdiag {

std::uint8_t calculate_crc(
    const std::vector<std::uint8_t>& data
) {
    std::uint8_t crc = 0x00;

    for (std::uint8_t byte : data) {
        crc ^= byte;

        for (int i = 0; i < 8; ++i) {
            if (crc & 0x80) {
                crc = static_cast<std::uint8_t>(
                    (crc << 1) ^ 0x07
                );
            } else {
                crc <<= 1;
            }
        }
    }

    return crc;
}

} // namespace uartdiag