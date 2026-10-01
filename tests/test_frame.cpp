#include "uartdiag/crc.hpp"
#include "uartdiag/decoder.hpp"
#include "uartdiag/diagnostics.hpp"
#include "uartdiag/frame.hpp"
#include "uartdiag/hex.hpp"
#include "uartdiag/report.hpp"

#include <gtest/gtest.h>

using namespace uartdiag;

TEST(FrameTest, ValidSensorDataFrameIsAccepted) {
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
    EXPECT_EQ(decoded.type, FrameType::SensorData);
    EXPECT_EQ(decoded.payload, frame.payload);
}

TEST(FrameTest, ValidCommandFrameIsAccepted) {
    Frame frame{
        FrameType::Command,
        {0x01, 0x02},
        0
    };

    const auto encoded = encode_frame(frame);

    Decoder decoder;
    Frame decoded{};

    const auto result =
        decoder.decode(encoded, decoded);

    EXPECT_TRUE(result.is_valid());
    EXPECT_EQ(decoded.type, FrameType::Command);
    EXPECT_EQ(decoded.payload, frame.payload);
}

TEST(FrameTest, ValidResponseFrameIsAccepted) {
    Frame frame{
        FrameType::Response,
        {0xAA, 0x55},
        0
    };

    const auto encoded = encode_frame(frame);

    Decoder decoder;
    Frame decoded{};

    const auto result =
        decoder.decode(encoded, decoded);

    EXPECT_TRUE(result.is_valid());
    EXPECT_EQ(decoded.type, FrameType::Response);
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

TEST(FrameTest, UnknownFrameTypeIsRejected) {
    std::vector<std::uint8_t> raw{
        FRAME_START,
        0x99,
        0x00,
        0x00
    };

    Decoder decoder;
    Frame decoded{};

    const auto result =
        decoder.decode(raw, decoded);

    EXPECT_EQ(
        result.status,
        DiagnosticStatus::UnknownFrameType
    );
}

TEST(FrameTest, EmptyPayloadFrameIsAccepted) {
    Frame frame{
        FrameType::Command,
        {},
        0
    };

    const auto encoded = encode_frame(frame);

    Decoder decoder;
    Frame decoded{};

    const auto result =
        decoder.decode(encoded, decoded);

    EXPECT_TRUE(result.is_valid());
    EXPECT_TRUE(decoded.payload.empty());
}

TEST(HexTest, ParsesValidHexString) {
    const auto bytes =
        parse_hex("AA 01 04 10 20 30 40 BC");

    const std::vector<std::uint8_t> expected{
        0xAA,
        0x01,
        0x04,
        0x10,
        0x20,
        0x30,
        0x40,
        0xBC
    };

    EXPECT_EQ(bytes, expected);
}

TEST(HexTest, RejectsInvalidHexByteLength) {
    EXPECT_THROW(
        parse_hex("AA 1 04"),
        std::invalid_argument
    );
}

TEST(HexTest, RejectsInvalidHexCharacters) {
    EXPECT_THROW(
        parse_hex("AA ZZ 04"),
        std::invalid_argument
    );
}

TEST(HexTest, RejectsEmptyInput) {
    EXPECT_THROW(
        parse_hex(""),
        std::invalid_argument
    );
}

TEST(DiagnosticsTest, StatusStringsAreStable) {
    EXPECT_EQ(
        to_string(DiagnosticStatus::Valid),
        "VALID"
    );

    EXPECT_EQ(
        to_string(DiagnosticStatus::FrameTooShort),
        "FRAME_TOO_SHORT"
    );

    EXPECT_EQ(
        to_string(DiagnosticStatus::InvalidStartByte),
        "INVALID_START_BYTE"
    );

    EXPECT_EQ(
        to_string(DiagnosticStatus::InvalidLength),
        "INVALID_LENGTH"
    );

    EXPECT_EQ(
        to_string(DiagnosticStatus::UnknownFrameType),
        "UNKNOWN_FRAME_TYPE"
    );

    EXPECT_EQ(
        to_string(DiagnosticStatus::CrcError),
        "CRC_ERROR"
    );
}

TEST(DiagnosticsTest, ValidResultReportsValid) {
    DiagnosticResult result{};

    result.status = DiagnosticStatus::Valid;

    EXPECT_TRUE(result.is_valid());
}

TEST(DiagnosticsTest, ErrorResultReportsInvalid) {
    DiagnosticResult result{};

    result.status = DiagnosticStatus::CrcError;

    EXPECT_FALSE(result.is_valid());
}

TEST(ReportTest, ValidFrameReportContainsFrameInformation) {
    const auto raw_data =
        parse_hex("AA 01 04 10 20 30 40 BC");

    Decoder decoder;
    Frame decoded{};

    const auto result =
        decoder.decode(raw_data, decoded);

    ASSERT_TRUE(result.is_valid());

    DiagnosticReport report;

    report.raw_data = raw_data;
    report.has_frame_type = true;
    report.frame_type = decoded.type;
    report.has_payload_length = true;
    report.payload_length = decoded.payload.size();
    report.payload = decoded.payload;
    report.expected_crc = result.expected_crc;
    report.received_crc = result.received_crc;
    report.result = result;

    const auto output =
        format_report(report);

    EXPECT_NE(
        output.find("UARTDiag Diagnostic Report"),
        std::string::npos
    );

    EXPECT_NE(
        output.find("SENSOR_DATA"),
        std::string::npos
    );

    EXPECT_NE(
        output.find("Payload Length: 4"),
        std::string::npos
    );

    EXPECT_NE(
        output.find("10 20 30 40"),
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

TEST(ReportTest, CrcErrorReportShowsFailure) {
    auto raw_data =
        parse_hex("AA 01 04 10 20 30 40 72");

    Decoder decoder;
    Frame decoded{};

    const auto result =
        decoder.decode(raw_data, decoded);

    ASSERT_EQ(
        result.status,
        DiagnosticStatus::CrcError
    );

    DiagnosticReport report;

    report.raw_data = raw_data;
    report.has_frame_type = true;
    report.frame_type = decoded.type;
    report.has_payload_length = true;
    report.payload_length = decoded.payload.size();
    report.payload = decoded.payload;
    report.expected_crc = result.expected_crc;
    report.received_crc = result.received_crc;
    report.result = result;

    const auto output =
        format_report(report);

    EXPECT_NE(
        output.find("CRC Status:     FAIL"),
        std::string::npos
    );

    EXPECT_NE(
        output.find("Status:         CRC_ERROR"),
        std::string::npos
    );

    EXPECT_NE(
        output.find("Expected"),
        std::string::npos
    );

    EXPECT_NE(
        output.find("Received"),
        std::string::npos
    );
}