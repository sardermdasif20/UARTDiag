#include "uartdiag/frame.hpp"
#include "uartdiag/crc.hpp"

namespace uartdiag {

std::vector<std::uint8_t> encode_frame(const Frame& frame) {
    std::vector<std::uint8_t> raw;

    raw.push_back(FRAME_START);
    raw.push_back(static_cast<std::uint8_t>(frame.type));
    raw.push_back(
        static_cast<std::uint8_t>(frame.payload.size())
    );

    raw.insert(
        raw.end(),
        frame.payload.begin(),
        frame.payload.end()
    );

    std::vector<std::uint8_t> crc_data(
        raw.begin() + 1,
        raw.end()
    );

    raw.push_back(calculate_crc(crc_data));

    return raw;
}

} // namespace uartdiag