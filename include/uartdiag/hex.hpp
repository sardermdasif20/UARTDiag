#pragma once

#include <cstdint>
#include <string>
#include <vector>

namespace uartdiag {

std::vector<std::uint8_t> parse_hex(
    const std::string& input
);

} // namespace uartdiag