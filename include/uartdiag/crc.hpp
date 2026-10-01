#pragma once

#include <cstdint>
#include <vector>

namespace uartdiag {

std::uint8_t calculate_crc(
    const std::vector<std::uint8_t>& data
);

} // namespace uartdiag