#include "uartdiag/decoder.hpp"
#include "uartdiag/diagnostics.hpp"
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

TEST(DiagnosticSummaryTest, StartsEmpty) {

    const uartdiag::DiagnosticSummary summary{};

    EXPECT_EQ(
        summary.frames_processed,
        0u
    );

    EXPECT_EQ(
        summary.valid_frames,
        0u
    );

    EXPECT_EQ(
        summary.invalid_frames,
        0u
    );

    EXPECT_EQ(
        summary.info_count,
        0u
    );

    EXPECT_EQ(
        summary.warning_count,
        0u
    );

    EXPECT_EQ(
        summary.error_count,
        0u
    );

    EXPECT_EQ(
        summary.critical_count,
        0u
    );

    EXPECT_EQ(
        summary.valid_count,
        0u
    );

    EXPECT_EQ(
        summary.frame_too_short_count,
        0u
    );

    EXPECT_EQ(
        summary.invalid_start_byte_count,
        0u
    );

    EXPECT_EQ(
        summary.invalid_length_count,
        0u
    );

    EXPECT_EQ(
        summary.unknown_frame_type_count,
        0u
    );

    EXPECT_EQ(
        summary.crc_error_count,
        0u
    );
}

TEST(DiagnosticSummaryTest, RecordsValidFrame) {

    uartdiag::DiagnosticSummary summary{};

    const uartdiag::DiagnosticResult result{
        uartdiag::DiagnosticStatus::Valid,
        uartdiag::DiagnosticSeverity::Info,
        0xBC,
        0xBC,
        "Frame validated successfully."
    };

    summary.record(result);

    EXPECT_EQ(
        summary.frames_processed,
        1u
    );

    EXPECT_EQ(
        summary.valid_frames,
        1u
    );

    EXPECT_EQ(
        summary.invalid_frames,
        0u
    );

    EXPECT_EQ(
        summary.info_count,
        1u
    );

    EXPECT_EQ(
        summary.valid_count,
        1u
    );
}

TEST(DiagnosticSummaryTest, RecordsWarningFrame) {

    uartdiag::DiagnosticSummary summary{};

    const uartdiag::DiagnosticResult result{
        uartdiag::DiagnosticStatus::FrameTooShort,
        uartdiag::DiagnosticSeverity::Warning,
        0,
        0,
        "Frame is too short."
    };

    summary.record(result);

    EXPECT_EQ(
        summary.frames_processed,
        1u
    );

    EXPECT_EQ(
        summary.valid_frames,
        0u
    );

    EXPECT_EQ(
        summary.invalid_frames,
        1u
    );

    EXPECT_EQ(
        summary.warning_count,
        1u
    );

    EXPECT_EQ(
        summary.frame_too_short_count,
        1u
    );
}

TEST(DiagnosticSummaryTest, RecordsErrorFrame) {

    uartdiag::DiagnosticSummary summary{};

    const uartdiag::DiagnosticResult result{
        uartdiag::DiagnosticStatus::CrcError,
        uartdiag::DiagnosticSeverity::Error,
        0xBC,
        0x00,
        "CRC validation failed."
    };

    summary.record(result);

    EXPECT_EQ(
        summary.frames_processed,
        1u
    );

    EXPECT_EQ(
        summary.valid_frames,
        0u
    );

    EXPECT_EQ(
        summary.invalid_frames,
        1u
    );

    EXPECT_EQ(
        summary.error_count,
        1u
    );

    EXPECT_EQ(
        summary.crc_error_count,
        1u
    );
}

TEST(DiagnosticSummaryTest, RecordsMultipleDiagnosticResults) {

    uartdiag::DiagnosticSummary summary{};

    const uartdiag::DiagnosticResult valid_result{
        uartdiag::DiagnosticStatus::Valid,
        uartdiag::DiagnosticSeverity::Info,
        0xBC,
        0xBC,
        "Frame validated successfully."
    };

    const uartdiag::DiagnosticResult warning_result{
        uartdiag::DiagnosticStatus::InvalidStartByte,
        uartdiag::DiagnosticSeverity::Warning,
        0,
        0,
        "Invalid start byte."
    };

    const uartdiag::DiagnosticResult error_result{
        uartdiag::DiagnosticStatus::CrcError,
        uartdiag::DiagnosticSeverity::Error,
        0xBC,
        0x00,
        "CRC validation failed."
    };

    summary.record(valid_result);
    summary.record(valid_result);
    summary.record(warning_result);
    summary.record(error_result);

    EXPECT_EQ(
        summary.frames_processed,
        4u
    );

    EXPECT_EQ(
        summary.valid_frames,
        2u
    );

    EXPECT_EQ(
        summary.invalid_frames,
        2u
    );

    EXPECT_EQ(
        summary.info_count,
        2u
    );

    EXPECT_EQ(
        summary.warning_count,
        1u
    );

    EXPECT_EQ(
        summary.error_count,
        1u
    );

    EXPECT_EQ(
        summary.critical_count,
        0u
    );

    EXPECT_EQ(
        summary.valid_count,
        2u
    );

    EXPECT_EQ(
        summary.invalid_start_byte_count,
        1u
    );

    EXPECT_EQ(
        summary.crc_error_count,
        1u
    );
}

TEST(DiagnosticSummaryTest, RecordsCriticalSeverity) {

    uartdiag::DiagnosticSummary summary{};

    const uartdiag::DiagnosticResult result{
        uartdiag::DiagnosticStatus::InvalidLength,
        uartdiag::DiagnosticSeverity::Critical,
        0,
        0,
        "Critical diagnostic condition."
    };

    summary.record(result);

    EXPECT_EQ(
        summary.frames_processed,
        1u
    );

    EXPECT_EQ(
        summary.invalid_frames,
        1u
    );

    EXPECT_EQ(
        summary.critical_count,
        1u
    );

    EXPECT_EQ(
        summary.invalid_length_count,
        1u
    );
}

} // namespace