#pragma once

#include <cstdint>
#include <vector>

namespace uartdiag {

struct Frame {
    std::uint8_t type;
    std::vector<std::uint8_t> payload;
    std::uint8_t crc;
};

} // namespace uartdiag