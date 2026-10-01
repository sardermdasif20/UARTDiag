#include "uartdiag/crc.hpp"
#include "uartdiag/decoder.hpp"
#include "uartdiag/frame.hpp"

#include <gtest/gtest.h>

using namespace uartdiag;

TEST(FrameTest, ValidFrameIsAccepted) {
    Frame frame{
        FrameType::SensorData,
        {0x10, 0x20, 0x30, 0x40},
        0
    };

    const auto encoded = encode_frame(frame);

    Decoder decoder;
    Frame decoded{};

    const auto result =
        decoder.decode(encoded, decoded);

    EXPECT_TRUE(result.is_valid());
    EXPECT_EQ(decoded.payload, frame.payload);
}

TEST(FrameTest, CorruptedPayloadIsRejected) {
    Frame frame{
        FrameType::SensorData,
        {0x10, 0x20, 0x30, 0x40},
        0
    };

    auto encoded = encode_frame(frame);

    encoded[4] ^= 0xFF;

    Decoder decoder;
    Frame decoded{};

    const auto result =
        decoder.decode(encoded, decoded);

    EXPECT_FALSE(result.is_valid());
    EXPECT_EQ(
        result.status,
        DiagnosticStatus::CrcError
    );
}

TEST(FrameTest, InvalidStartByteIsRejected) {
    Frame frame{
        FrameType::SensorData,
        {0x10, 0x20},
        0
    };

    auto encoded = encode_frame(frame);

    encoded[0] = 0x55;

    Decoder decoder;
    Frame decoded{};

    const auto result =
        decoder.decode(encoded, decoded);

    EXPECT_EQ(
        result.status,
        DiagnosticStatus::InvalidStartByte
    );
}

TEST(FrameTest, InvalidLengthIsRejected) {
    Frame frame{
        FrameType::SensorData,
        {0x10, 0x20},
        0
    };

    auto encoded = encode_frame(frame);

    encoded[2] = 0x20;

    Decoder decoder;
    Frame decoded{};

    const auto result =
        decoder.decode(encoded, decoded);

    EXPECT_EQ(
        result.status,
        DiagnosticStatus::InvalidLength
    );
}

TEST(FrameTest, TruncatedFrameIsRejected) {
    std::vector<std::uint8_t> raw{
        FRAME_START,
        0x01
    };

    Decoder decoder;
    Frame decoded{};

    const auto result =
        decoder.decode(raw, decoded);

    EXPECT_EQ(
        result.status,
        DiagnosticStatus::FrameTooShort
    );
}