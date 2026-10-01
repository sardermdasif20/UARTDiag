#include "uartdiag/decoder.hpp"
#include "uartdiag/frame.hpp"
#include "uartdiag/report.hpp"

#include <cstdint>
#include <string>
#include <vector>

#include <gtest/gtest.h>

namespace {

TEST(DiagnosticReportTest, FormatsValidReport) {
    uartdiag::DiagnosticReport report;

    report.raw_data = {
        0xAA,
        0x01,
        0x03,
        0x10,
        0x20,
        0x30,
        0x00
    };

    report.has_frame_type = true;
    report.frame_type =
        uartdiag::FrameType::SensorData;

    report.has_payload_length = true;
    report.payload_length = 3;

    report.payload = {
        0x10,
        0x20,
        0x30
    };

    report.expected_crc = 0xBC;
    report.received_crc = 0xBC;

    report.result.status =
        uartdiag::DiagnosticStatus::Valid;

    report.result.severity =
        uartdiag::DiagnosticSeverity::Info;

    report.result.message =
        "Frame is valid.";

    const std::string output =
        uartdiag::format_report(report);

    EXPECT_NE(
        output.find("UARTDiag Diagnostic Report"),
        std::string::npos
    );

    EXPECT_NE(
        output.find("Type:           SENSOR_DATA"),
        std::string::npos
    );

    EXPECT_NE(
        output.find("Payload Length: 3"),
        std::string::npos
    );

    EXPECT_NE(
        output.find("Payload:        10 20 30 "),
        std::string::npos
    );

    EXPECT_NE(
        output.find("CRC Expected:   0xBC"),
        std::string::npos
    );

    EXPECT_NE(
        output.find("CRC Received:   0xBC"),
        std::string::npos
    );

    EXPECT_NE(
        output.find("CRC Status:     PASS"),
        std::string::npos
    );

    EXPECT_NE(
        output.find("Status:         VALID"),
        std::string::npos
    );

    EXPECT_NE(
        output.find("Severity:       INFO"),
        std::string::npos
    );

    EXPECT_NE(
        output.find("Message:        Frame is valid."),
        std::string::npos
    );
}

TEST(DiagnosticReportTest, FormatsFrameMetadata) {
    uartdiag::DiagnosticReport report;

    report.raw_data = {
        0xAA,
        0x02,
        0x00,
        0x00
    };

    report.has_frame_type = true;
    report.frame_type =
        uartdiag::FrameType::Command;

    report.has_payload_length = true;
    report.payload_length = 0;

    report.expected_crc = 0x5A;
    report.received_crc = 0x5A;

    report.result.status =
        uartdiag::DiagnosticStatus::Valid;

    report.result.severity =
        uartdiag::DiagnosticSeverity::Info;

    report.result.message =
        "Command frame is valid.";

    report.has_frame_number = true;
    report.frame_number = 42;

    report.has_timestamp = true;
    report.timestamp =
        "2026-10-01T12:00:00Z";

    const std::string output =
        uartdiag::format_report(report);

    EXPECT_NE(
        output.find("Frame Number:   42"),
        std::string::npos
    );

    EXPECT_NE(
        output.find(
            "Timestamp:      2026-10-01T12:00:00Z"
        ),
        std::string::npos
    );

    EXPECT_NE(
        output.find("Type:           COMMAND"),
        std::string::npos
    );

    EXPECT_NE(
        output.find("Payload Length: 0"),
        std::string::npos
    );
}

TEST(DiagnosticReportTest, FormatsResponseFrame) {
    uartdiag::DiagnosticReport report;

    report.raw_data = {
        0xAA,
        0x03,
        0x02,
        0xAB,
        0xCD,
        0x12
    };

    report.has_frame_type = true;
    report.frame_type =
        uartdiag::FrameType::Response;

    report.has_payload_length = true;
    report.payload_length = 2;

    report.payload = {
        0xAB,
        0xCD
    };

    report.expected_crc = 0x12;
    report.received_crc = 0x12;

    report.result.status =
        uartdiag::DiagnosticStatus::Valid;

    report.result.severity =
        uartdiag::DiagnosticSeverity::Info;

    report.result.message =
        "Response frame is valid.";

    const std::string output =
        uartdiag::format_report(report);

    EXPECT_NE(
        output.find("Type:           RESPONSE"),
        std::string::npos
    );

    EXPECT_NE(
        output.find("Payload:        AB CD "),
        std::string::npos
    );
}

TEST(DiagnosticReportTest, FormatsCrcErrorReport) {
    uartdiag::DiagnosticReport report;

    report.raw_data = {
        0xAA,
        0x01,
        0x02,
        0x10,
        0x20,
        0xFF
    };

    report.has_frame_type = true;
    report.frame_type =
        uartdiag::FrameType::SensorData;

    report.has_payload_length = true;
    report.payload_length = 2;

    report.payload = {
        0x10,
        0x20
    };

    report.expected_crc = 0x42;
    report.received_crc = 0xFF;

    report.result.status =
        uartdiag::DiagnosticStatus::CrcError;

    report.result.severity =
        uartdiag::DiagnosticSeverity::Error;

    report.result.message =
        "CRC mismatch detected.";

    const std::string output =
        uartdiag::format_report(report);

    EXPECT_NE(
        output.find("CRC Expected:   0x42"),
        std::string::npos
    );

    EXPECT_NE(
        output.find("CRC Received:   0xFF"),
        std::string::npos
    );

    EXPECT_NE(
        output.find("CRC Status:     FAIL"),
        std::string::npos
    );

    EXPECT_NE(
        output.find("Status:         CRC_ERROR"),
        std::string::npos
    );

    EXPECT_NE(
        output.find("Severity:       ERROR"),
        std::string::npos
    );

    EXPECT_NE(
        output.find("Message:        CRC mismatch detected."),
        std::string::npos
    );
}

TEST(DiagnosticReportTest, FormatsInvalidFrameWithoutType) {
    uartdiag::DiagnosticReport report;

    report.raw_data = {
        0x55,
        0x01
    };

    report.has_frame_type = false;
    report.has_payload_length = false;

    report.result.status =
        uartdiag::DiagnosticStatus::FrameTooShort;

    report.result.severity =
        uartdiag::DiagnosticSeverity::Warning;

    report.result.message =
        "Frame is too short.";

    const std::string output =
        uartdiag::format_report(report);

    EXPECT_NE(
        output.find("Type:           UNKNOWN"),
        std::string::npos
    );

    EXPECT_NE(
        output.find("CRC Status:     NOT VERIFIED"),
        std::string::npos
    );

    EXPECT_NE(
        output.find("Status:         FRAME_TOO_SHORT"),
        std::string::npos
    );

    EXPECT_NE(
        output.find("Severity:       WARNING"),
        std::string::npos
    );

    EXPECT_NE(
        output.find("Message:        Frame is too short."),
        std::string::npos
    );
}

TEST(DiagnosticReportTest, FormatsCriticalSeverity) {
    uartdiag::DiagnosticReport report;

    report.raw_data = {
        0xAA
    };

    report.result.status =
        uartdiag::DiagnosticStatus::InvalidLength;

    report.result.severity =
        uartdiag::DiagnosticSeverity::Critical;

    report.result.message =
        "Critical frame error.";

    const std::string output =
        uartdiag::format_report(report);

    EXPECT_NE(
        output.find("Status:         INVALID_LENGTH"),
        std::string::npos
    );

    EXPECT_NE(
        output.find("Severity:       CRITICAL"),
        std::string::npos
    );

    EXPECT_NE(
        output.find("Message:        Critical frame error."),
        std::string::npos
    );
}

/*
 * Phase 14.5:
 * Verify that a real encoded frame can be decoded and then
 * represented consistently inside DiagnosticReport.
 */
TEST(DiagnosticReportTest, EncodedFrameDataMatchesReportFields) {
    uartdiag::Frame frame;

    frame.type =
        uartdiag::FrameType::SensorData;

    frame.payload = {
        0x10,
        0x20,
        0x30
    };

    const auto encoded =
        uartdiag::encode_frame(frame);

    uartdiag::Decoder decoder;
    uartdiag::Frame decoded;

    const auto result =
        decoder.decode(
            encoded,
            decoded
        );

    ASSERT_EQ(
        result.status,
        uartdiag::DiagnosticStatus::Valid
    );

    uartdiag::DiagnosticReport report;

    report.raw_data = encoded;

    report.has_frame_type = true;
    report.frame_type = decoded.type;

    report.has_payload_length = true;
    report.payload_length =
        decoded.payload.size();

    report.payload =
        decoded.payload;

    report.expected_crc =
        result.expected_crc;

    report.received_crc =
        result.received_crc;

    report.result =
        result;

    EXPECT_EQ(
        report.frame_type,
        frame.type
    );

    EXPECT_EQ(
        report.payload_length,
        frame.payload.size()
    );

    EXPECT_EQ(
        report.payload,
        frame.payload
    );

    EXPECT_EQ(
        report.expected_crc,
        report.received_crc
    );

    EXPECT_TRUE(
        report.result.is_valid()
    );

    const std::string output =
        uartdiag::format_report(report);

    EXPECT_NE(
        output.find("Type:           SENSOR_DATA"),
        std::string::npos
    );

    EXPECT_NE(
        output.find("Payload Length: 3"),
        std::string::npos
    );

    EXPECT_NE(
        output.find("Payload:        10 20 30 "),
        std::string::npos
    );

    EXPECT_NE(
        output.find("CRC Status:     PASS"),
        std::string::npos
    );

    EXPECT_NE(
        output.find("Status:         VALID"),
        std::string::npos
    );
}

TEST(DiagnosticReportTest, CrcErrorReportPreservesDecodedPayload) {
    uartdiag::Frame frame;

    frame.type =
        uartdiag::FrameType::Command;

    frame.payload = {
        0xAA,
        0x55
    };

    auto encoded =
        uartdiag::encode_frame(frame);

    ASSERT_FALSE(
        encoded.empty()
    );

    encoded.back() ^= 0xFF;

    uartdiag::Decoder decoder;
    uartdiag::Frame decoded;

    const auto result =
        decoder.decode(
            encoded,
            decoded
        );

    ASSERT_EQ(
        result.status,
        uartdiag::DiagnosticStatus::CrcError
    );

    uartdiag::DiagnosticReport report;

    report.raw_data = encoded;

    report.has_frame_type = true;
    report.frame_type = decoded.type;

    report.has_payload_length = true;
    report.payload_length =
        decoded.payload.size();

    report.payload =
        decoded.payload;

    report.expected_crc =
        result.expected_crc;

    report.received_crc =
        result.received_crc;

    report.result =
        result;

    EXPECT_EQ(
        report.frame_type,
        uartdiag::FrameType::Command
    );

    EXPECT_EQ(
        report.payload,
        frame.payload
    );

    EXPECT_NE(
        report.expected_crc,
        report.received_crc
    );

    EXPECT_EQ(
        report.result.status,
        uartdiag::DiagnosticStatus::CrcError
    );

    EXPECT_EQ(
        report.result.severity,
        uartdiag::DiagnosticSeverity::Error
    );

    const std::string output =
        uartdiag::format_report(report);

    EXPECT_NE(
        output.find("Type:           COMMAND"),
        std::string::npos
    );

    EXPECT_NE(
        output.find("Payload:        AA 55 "),
        std::string::npos
    );

    EXPECT_NE(
        output.find("CRC Status:     FAIL"),
        std::string::npos
    );

    EXPECT_NE(
        output.find("Status:         CRC_ERROR"),
        std::string::npos
    );

    EXPECT_NE(
        output.find("Severity:       ERROR"),
        std::string::npos
    );
}

} // namespace