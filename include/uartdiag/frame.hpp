#pragma once

#include <cstdint>
#include <vector>

namespace uartdiag {

constexpr std::uint8_t FRAME_START = 0xAA;

enum class FrameType : std::uint8_t {
    SensorData = 0x01,
    Command    = 0x02,
    Response   = 0x03
};

struct Frame {
    FrameType type;
    std::vector<std::uint8_t> payload;
    std::uint8_t crc;
};

std::vector<std::uint8_t> encode_frame(const Frame& frame);

} // namespace uartdiag