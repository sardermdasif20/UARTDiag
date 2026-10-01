#include "uartdiag/decoder.hpp"
#include "uartdiag/frame.hpp"

#include <cstdint>
#include <vector>

#include <gtest/gtest.h>

namespace {

TEST(DiagnosticSeverityTest, ValidFrameIsInfo) {

    uartdiag::Frame frame{
        uartdiag::FrameType::SensorData,
        {0x10, 0x20, 0x30, 0x40},
        0
    };

    const auto encoded =
        uartdiag::encode_frame(frame);

    uartdiag::Decoder decoder;

    uartdiag::Frame decoded{};

    const auto result =
        decoder.decode(
            encoded,
            decoded
        );

    EXPECT_EQ(
        result.status,
        uartdiag::DiagnosticStatus::Valid
    );

    EXPECT_EQ(
        result.severity,
        uartdiag::DiagnosticSeverity::Info
    );

    EXPECT_TRUE(
        result.is_valid()
    );
}

TEST(DiagnosticSeverityTest, FrameTooShortIsWarning) {

    const std::vector<std::uint8_t> raw_data{
        uartdiag::FRAME_START,
        0x01,
        0x00
    };

    uartdiag::Decoder decoder;

    uartdiag::Frame decoded{};

    const auto result =
        decoder.decode(
            raw_data,
            decoded
        );

    EXPECT_EQ(
        result.status,
        uartdiag::DiagnosticStatus::FrameTooShort
    );

    EXPECT_EQ(
        result.severity,
        uartdiag::DiagnosticSeverity::Warning
    );

    EXPECT_FALSE(
        result.is_valid()
    );
}

TEST(DiagnosticSeverityTest, InvalidStartByteIsWarning) {

    const std::vector<std::uint8_t> raw_data{
        0x55,
        0x01,
        0x00,
        0x00
    };

    uartdiag::Decoder decoder;

    uartdiag::Frame decoded{};

    const auto result =
        decoder.decode(
            raw_data,
            decoded
        );

    EXPECT_EQ(
        result.status,
        uartdiag::DiagnosticStatus::InvalidStartByte
    );

    EXPECT_EQ(
        result.severity,
        uartdiag::DiagnosticSeverity::Warning
    );

    EXPECT_FALSE(
        result.is_valid()
    );
}

TEST(DiagnosticSeverityTest, InvalidLengthIsError) {

    const std::vector<std::uint8_t> raw_data{
        uartdiag::FRAME_START,
        0x01,
        0x05,
        0x10,
        0x20,
        0x30,
        0x40,
        0x00
    };

    uartdiag::Decoder decoder;

    uartdiag::Frame decoded{};

    const auto result =
        decoder.decode(
            raw_data,
            decoded
        );

    EXPECT_EQ(
        result.status,
        uartdiag::DiagnosticStatus::InvalidLength
    );

    EXPECT_EQ(
        result.severity,
        uartdiag::DiagnosticSeverity::Error
    );

    EXPECT_FALSE(
        result.is_valid()
    );
}

TEST(DiagnosticSeverityTest, UnknownFrameTypeIsError) {

    const std::vector<std::uint8_t> raw_data{
        uartdiag::FRAME_START,
        0xFF,
        0x00,
        0x00
    };

    uartdiag::Decoder decoder;

    uartdiag::Frame decoded{};

    const auto result =
        decoder.decode(
            raw_data,
            decoded
        );

    EXPECT_EQ(
        result.status,
        uartdiag::DiagnosticStatus::UnknownFrameType
    );

    EXPECT_EQ(
        result.severity,
        uartdiag::DiagnosticSeverity::Error
    );

    EXPECT_FALSE(
        result.is_valid()
    );
}

TEST(DiagnosticSeverityTest, CrcErrorIsError) {

    uartdiag::Frame frame{
        uartdiag::FrameType::SensorData,
        {0x10, 0x20, 0x30, 0x40},
        0
    };

    auto encoded =
        uartdiag::encode_frame(frame);

    encoded.back() ^= 0xFF;

    uartdiag::Decoder decoder;

    uartdiag::Frame decoded{};

    const auto result =
        decoder.decode(
            encoded,
            decoded
        );

    EXPECT_EQ(
        result.status,
        uartdiag::DiagnosticStatus::CrcError
    );

    EXPECT_EQ(
        result.severity,
        uartdiag::DiagnosticSeverity::Error
    );

    EXPECT_FALSE(
        result.is_valid()
    );
}

} // namespace