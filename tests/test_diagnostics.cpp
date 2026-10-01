#include "uartdiag/decoder.hpp"
#include "uartdiag/diagnostics.hpp"
#include "uartdiag/frame.hpp"

#include <cstdint>
#include <string>
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

TEST(DiagnosticSummaryFormatTest, FormatsEmptySummary) {

    const uartdiag::DiagnosticSummary summary{};

    const std::string output =
        uartdiag::format_summary(summary);

    EXPECT_NE(
        output.find("UARTDiag Diagnostic Summary"),
        std::string::npos
    );

    EXPECT_NE(
        output.find("Frames Processed:  0"),
        std::string::npos
    );

    EXPECT_NE(
        output.find("Valid Frames:      0"),
        std::string::npos
    );

    EXPECT_NE(
        output.find("Invalid Frames:    0"),
        std::string::npos
    );

    EXPECT_NE(
        output.find("Severity Summary"),
        std::string::npos
    );

    EXPECT_NE(
        output.find("Diagnostic Status"),
        std::string::npos
    );
}

TEST(DiagnosticSummaryFormatTest, FormatsCompleteSummary) {

    uartdiag::DiagnosticSummary summary{};

    const uartdiag::DiagnosticResult valid_result{
        uartdiag::DiagnosticStatus::Valid,
        uartdiag::DiagnosticSeverity::Info,
        0xBC,
        0xBC,
        "Valid frame."
    };

    const uartdiag::DiagnosticResult warning_result{
        uartdiag::DiagnosticStatus::FrameTooShort,
        uartdiag::DiagnosticSeverity::Warning,
        0,
        0,
        "Frame too short."
    };

    const uartdiag::DiagnosticResult start_error_result{
        uartdiag::DiagnosticStatus::InvalidStartByte,
        uartdiag::DiagnosticSeverity::Warning,
        0,
        0,
        "Invalid start byte."
    };

    const uartdiag::DiagnosticResult length_error_result{
        uartdiag::DiagnosticStatus::InvalidLength,
        uartdiag::DiagnosticSeverity::Error,
        0,
        0,
        "Invalid length."
    };

    const uartdiag::DiagnosticResult unknown_type_result{
        uartdiag::DiagnosticStatus::UnknownFrameType,
        uartdiag::DiagnosticSeverity::Error,
        0,
        0,
        "Unknown frame type."
    };

    const uartdiag::DiagnosticResult crc_error_result{
        uartdiag::DiagnosticStatus::CrcError,
        uartdiag::DiagnosticSeverity::Error,
        0xBC,
        0x00,
        "CRC error."
    };

    const uartdiag::DiagnosticResult critical_result{
        uartdiag::DiagnosticStatus::InvalidLength,
        uartdiag::DiagnosticSeverity::Critical,
        0,
        0,
        "Critical condition."
    };

    summary.record(valid_result);
    summary.record(valid_result);
    summary.record(warning_result);
    summary.record(start_error_result);
    summary.record(length_error_result);
    summary.record(unknown_type_result);
    summary.record(crc_error_result);
    summary.record(critical_result);

    const std::string output =
        uartdiag::format_summary(summary);

    EXPECT_NE(
        output.find("Frames Processed:  8"),
        std::string::npos
    );

    EXPECT_NE(
        output.find("Valid Frames:      2"),
        std::string::npos
    );

    EXPECT_NE(
        output.find("Invalid Frames:    6"),
        std::string::npos
    );

    EXPECT_NE(
        output.find("Info:              2"),
        std::string::npos
    );

    EXPECT_NE(
        output.find("Warning:           2"),
        std::string::npos
    );

    EXPECT_NE(
        output.find("Error:             3"),
        std::string::npos
    );

    EXPECT_NE(
        output.find("Critical:          1"),
        std::string::npos
    );

    EXPECT_NE(
        output.find("Valid:             2"),
        std::string::npos
    );

    EXPECT_NE(
        output.find("Frame Too Short:   1"),
        std::string::npos
    );

    EXPECT_NE(
        output.find("Invalid Start:     1"),
        std::string::npos
    );

    EXPECT_NE(
        output.find("Invalid Length:    2"),
        std::string::npos
    );

    EXPECT_NE(
        output.find("Unknown Type:      1"),
        std::string::npos
    );

    EXPECT_NE(
        output.find("CRC Error:         1"),
        std::string::npos
    );
}

} // namespace 