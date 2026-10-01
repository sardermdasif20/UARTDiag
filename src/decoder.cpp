#include "uartdiag/decoder.hpp"

namespace uartdiag {

bool Decoder::decode(
    const std::vector<std::uint8_t>& raw_data,
    Frame& frame
) {
    (void)raw_data;
    (void)frame;

    return false;
}

} // namespace uartdiag