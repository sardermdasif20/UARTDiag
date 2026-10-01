#include "uartdiag/crc.hpp"
#include "uartdiag/decoder.hpp"
#include "uartdiag/diagnostics.hpp"
#include "uartdiag/frame.hpp"
#include "uartdiag/hex.hpp"
#include "uartdiag/report.hpp"
#include "uartdiag/serial.hpp"
#include "uartdiag/stream.hpp"

#include <gtest/gtest.h>

#include <cstdint>
#include <string>
#include <vector>

using namespace uartdiag;

// ============================================================
// CRC TESTS
// ============================================================

TEST(CrcTest, CalculatesKnownCrc) {
    const std::vector<std::uint8_t> data{
        0x01, 0x04, 0x10, 0x20, 0x30, 0x40
    };

    EXPECT_EQ(
        calculate_crc(data),
        0xBC
    );
}

TEST(CrcTest, EmptyDataProducesZero) {
    const std::vector<std::uint8_t> data;

    EXPECT_EQ(
        calculate_crc(data),
        0x00
    );
}

TEST(CrcTest, DifferentDataProducesDifferentCrc) {
    const std::vector<std::uint8_t> data_a{
        0x01, 0x02
    };

    const std::vector<std::uint8_t> data_b{
        0x01, 0x03
    };

    EXPECT_NE(
        calculate_crc(data_a),
        calculate_crc(data_b)
    );
}

// ============================================================
// FRAME ENCODING TESTS
// ============================================================

TEST(FrameTest, EncodesValidFrame) {
    Frame frame{
        FrameType::SensorData,
        {0x10, 0x20, 0x30, 0x40},
        0
    };

    const auto encoded =
        encode_frame(frame);

    ASSERT_EQ(
        encoded.size(),
        8
    );

    EXPECT_EQ(
        encoded[0],
        FRAME_START
    );

    EXPECT_EQ(
        encoded[1],
        0x01
    );

    EXPECT_EQ(
        encoded[2],
        0x04
    );

    EXPECT_EQ(
        encoded[3],
        0x10
    );

    EXPECT_EQ(
        encoded[4],
        0x20
    );

    EXPECT_EQ(
        encoded[5],
        0x30
    );

    EXPECT_EQ(
        encoded[6],
        0x40
    );

    EXPECT_EQ(
        encoded[7],
        0xBC
    );
}

TEST(FrameTest, EncodesEmptyPayload) {
    Frame frame{
        FrameType::Command,
        {},
        0
    };

    const auto encoded =
        encode_frame(frame);

    ASSERT_EQ(
        encoded.size(),
        4
    );

    EXPECT_EQ(
        encoded[0],
        FRAME_START
    );

    EXPECT_EQ(
        encoded[1],
        0x02
    );

    EXPECT_EQ(
        encoded[2],
        0x00
    );
}

// ============================================================
// DECODER TESTS
// ============================================================

TEST(DecoderTest, DecodesValidFrame) {
    const auto raw_data =
        parse_hex(
            "AA 01 04 10 20 30 40 BC"
        );

    Decoder decoder;
    Frame frame{};

    const auto result =
        decoder.decode(
            raw_data,
            frame
        );

    ASSERT_TRUE(
        result.is_valid()
    );

    EXPECT_EQ(
        result.status,
        DiagnosticStatus::Valid
    );

    EXPECT_EQ(
        frame.type,
        FrameType::SensorData
    );

    EXPECT_EQ(
        frame.payload,
        std::vector<std::uint8_t>({
            0x10,
            0x20,
            0x30,
            0x40
        })
    );

    EXPECT_EQ(
        frame.crc,
        0xBC
    );

    EXPECT_EQ(
        result.expected_crc,
        0xBC
    );

    EXPECT_EQ(
        result.received_crc,
        0xBC
    );
}

TEST(DecoderTest, RejectsTooShortFrame) {
    const auto raw_data =
        parse_hex(
            "AA 01 02"
        );

    Decoder decoder;
    Frame frame{};

    const auto result =
        decoder.decode(
            raw_data,
            frame
        );

    EXPECT_EQ(
        result.status,
        DiagnosticStatus::FrameTooShort
    );

    EXPECT_FALSE(
        result.is_valid()
    );
}

TEST(DecoderTest, RejectsInvalidStartByte) {
    const auto raw_data =
        parse_hex(
            "55 01 04 10 20 30 40 BC"
        );

    Decoder decoder;
    Frame frame{};

    const auto result =
        decoder.decode(
            raw_data,
            frame
        );

    EXPECT_EQ(
        result.status,
        DiagnosticStatus::InvalidStartByte
    );

    EXPECT_FALSE(
        result.is_valid()
    );
}

TEST(DecoderTest, RejectsUnknownFrameType) {
    const auto raw_data =
        parse_hex(
            "AA FF 00 00"
        );

    Decoder decoder;
    Frame frame{};

    const auto result =
        decoder.decode(
            raw_data,
            frame
        );

    EXPECT_EQ(
        result.status,
        DiagnosticStatus::UnknownFrameType
    );

    EXPECT_FALSE(
        result.is_valid()
    );
}

TEST(DecoderTest, RejectsInvalidPayloadLength) {
    const auto raw_data =
        parse_hex(
            "AA 01 05 10 20 30 40 BC"
        );

    Decoder decoder;
    Frame frame{};

    const auto result =
        decoder.decode(
            raw_data,
            frame
        );

    EXPECT_EQ(
        result.status,
        DiagnosticStatus::InvalidLength
    );

    EXPECT_FALSE(
        result.is_valid()
    );
}

TEST(DecoderTest, RejectsCrcError) {
    const auto raw_data =
        parse_hex(
            "AA 01 04 10 20 30 40 BD"
        );

    Decoder decoder;
    Frame frame{};

    const auto result =
        decoder.decode(
            raw_data,
            frame
        );

    EXPECT_EQ(
        result.status,
        DiagnosticStatus::CrcError
    );

    EXPECT_FALSE(
        result.is_valid()
    );

    EXPECT_EQ(
        result.expected_crc,
        0xBC
    );

    EXPECT_EQ(
        result.received_crc,
        0xBD
    );
}

TEST(DecoderTest, DecodesCommandFrame) {
    const auto raw_data =
        encode_frame(
            Frame{
                FrameType::Command,
                {0x01, 0x02},
                0
            }
        );

    Decoder decoder;
    Frame frame{};

    const auto result =
        decoder.decode(
            raw_data,
            frame
        );

    ASSERT_TRUE(
        result.is_valid()
    );

    EXPECT_EQ(
        frame.type,
        FrameType::Command
    );

    EXPECT_EQ(
        frame.payload,
        std::vector<std::uint8_t>({
            0x01,
            0x02
        })
    );
}

TEST(DecoderTest, DecodesResponseFrame) {
    const auto raw_data =
        encode_frame(
            Frame{
                FrameType::Response,
                {0xAA, 0x55},
                0
            }
        );

    Decoder decoder;
    Frame frame{};

    const auto result =
        decoder.decode(
            raw_data,
            frame
        );

    ASSERT_TRUE(
        result.is_valid()
    );

    EXPECT_EQ(
        frame.type,
        FrameType::Response
    );

    EXPECT_EQ(
        frame.payload,
        std::vector<std::uint8_t>({
            0xAA,
            0x55
        })
    );
}

// ============================================================
// DIAGNOSTIC TESTS
// ============================================================

TEST(DiagnosticsTest, ValidResultReportsValid) {
    DiagnosticResult result;

    result.status =
        DiagnosticStatus::Valid;

    EXPECT_TRUE(
        result.is_valid()
    );
}

TEST(DiagnosticsTest, InvalidResultReportsFalse) {
    DiagnosticResult result;

    result.status =
        DiagnosticStatus::CrcError;

    EXPECT_FALSE(
        result.is_valid()
    );
}

TEST(DiagnosticsTest, ConvertsStatusToString) {
    EXPECT_EQ(
        to_string(
            DiagnosticStatus::Valid
        ),
        "VALID"
    );

    EXPECT_EQ(
        to_string(
            DiagnosticStatus::CrcError
        ),
        "CRC_ERROR"
    );

    EXPECT_EQ(
        to_string(
            DiagnosticStatus::InvalidLength
        ),
        "INVALID_LENGTH"
    );
}

// ============================================================
// HEX PARSER TESTS
// ============================================================

TEST(HexTest, ParsesValidHex) {
    const auto bytes =
        parse_hex(
            "AA 01 04 10 20 30 40 BC"
        );

    EXPECT_EQ(
        bytes,
        std::vector<std::uint8_t>({
            0xAA,
            0x01,
            0x04,
            0x10,
            0x20,
            0x30,
            0x40,
            0xBC
        })
    );
}

TEST(HexTest, ParsesLowercaseHex) {
    const auto bytes =
        parse_hex(
            "aa 0a ff"
        );

    EXPECT_EQ(
        bytes,
        std::vector<std::uint8_t>({
            0xAA,
            0x0A,
            0xFF
        })
    );
}

TEST(HexTest, RejectsInvalidHexByte) {
    EXPECT_THROW(
        parse_hex("GG"),
        std::invalid_argument
    );
}

TEST(HexTest, RejectsWrongByteLength) {
    EXPECT_THROW(
        parse_hex("A"),
        std::invalid_argument
    );

    EXPECT_THROW(
        parse_hex("AAA"),
        std::invalid_argument
    );
}

TEST(HexTest, RejectsEmptyInput) {
    EXPECT_THROW(
        parse_hex(""),
        std::invalid_argument
    );
}

// ============================================================
// REPORT TESTS
// ============================================================

TEST(ReportTest, FormatsValidReport) {
    const auto raw_data =
        parse_hex(
            "AA 01 04 10 20 30 40 BC"
        );

    Decoder decoder;
    Frame decoded{};

    const auto result =
        decoder.decode(
            raw_data,
            decoded
        );

    ASSERT_TRUE(
        result.is_valid()
    );

    DiagnosticReport report;

    report.raw_data =
        raw_data;

    report.has_frame_type =
        true;

    report.frame_type =
        decoded.type;

    report.has_payload_length =
        true;

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

    const auto output =
        format_report(report);

    EXPECT_NE(
        output.find(
            "UARTDiag Diagnostic Report"
        ),
        std::string::npos
    );

    EXPECT_NE(
        output.find(
            "SENSOR_DATA"
        ),
        std::string::npos
    );

    EXPECT_NE(
        output.find(
            "Payload Length: 4"
        ),
        std::string::npos
    );

    EXPECT_NE(
        output.find(
            "CRC Status:     PASS"
        ),
        std::string::npos
    );

    EXPECT_NE(
        output.find(
            "Status:         VALID"
        ),
        std::string::npos
    );
}

TEST(ReportTest, FormatsCrcFailure) {
    const auto raw_data =
        parse_hex(
            "AA 01 04 10 20 30 40 BD"
        );

    Decoder decoder;
    Frame decoded{};

    const auto result =
        decoder.decode(
            raw_data,
            decoded
        );

    EXPECT_EQ(
        result.status,
        DiagnosticStatus::CrcError
    );

    DiagnosticReport report;

    report.raw_data =
        raw_data;

    report.has_frame_type =
        true;

    report.frame_type =
        decoded.type;

    report.has_payload_length =
        true;

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

    const auto output =
        format_report(report);

    EXPECT_NE(
        output.find(
            "CRC Status:     FAIL"
        ),
        std::string::npos
    );

    EXPECT_NE(
        output.find(
            "Status:         CRC_ERROR"
        ),
        std::string::npos
    );
}

TEST(ReportTest, FrameMetadataAppearsInReport) {
    const auto raw_data =
        parse_hex(
            "AA 01 04 10 20 30 40 BC"
        );

    Decoder decoder;
    Frame decoded{};

    const auto result =
        decoder.decode(
            raw_data,
            decoded
        );

    ASSERT_TRUE(
        result.is_valid()
    );

    DiagnosticReport report;

    report.raw_data =
        raw_data;

    report.has_frame_type =
        true;

    report.frame_type =
        decoded.type;

    report.has_payload_length =
        true;

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

    report.has_frame_number =
        true;

    report.frame_number =
        42;

    report.has_timestamp =
        true;

    report.timestamp =
        "2026-10-01 14:30:00";

    const auto output =
        format_report(report);

    EXPECT_NE(
        output.find(
            "Frame Number:   42"
        ),
        std::string::npos
    );

    EXPECT_NE(
        output.find(
            "Timestamp:      2026-10-01 14:30:00"
        ),
        std::string::npos
    );
}

// ============================================================
// STREAM PARSER TESTS
// ============================================================

TEST(StreamTest, ExtractsCompleteFrame) {
    FrameStreamParser parser;

    const auto data =
        parse_hex(
            "AA 01 04 10 20 30 40 BC"
        );

    parser.push(data);

    std::vector<std::uint8_t> frame;

    ASSERT_TRUE(
        parser.next_frame(frame)
    );

    EXPECT_EQ(
        frame,
        data
    );

    EXPECT_EQ(
        parser.buffered_bytes(),
        0
    );
}

TEST(StreamTest, HandlesPartialFrame) {
    FrameStreamParser parser;

    parser.push(
        parse_hex(
            "AA 01 04"
        )
    );

    std::vector<std::uint8_t> frame;

    EXPECT_FALSE(
        parser.next_frame(frame)
    );

    parser.push(
        parse_hex(
            "10 20 30 40 BC"
        )
    );

    ASSERT_TRUE(
        parser.next_frame(frame)
    );

    EXPECT_EQ(
        frame,
        parse_hex(
            "AA 01 04 10 20 30 40 BC"
        )
    );
}

TEST(StreamTest, ExtractsMultipleFrames) {
    FrameStreamParser parser;

    const auto first =
        parse_hex(
            "AA 01 02 10 20 7A"
        );

    const auto second =
        encode_frame(
            Frame{
                FrameType::Command,
                {0x30},
                0
            }
        );

    std::vector<std::uint8_t> combined;

    combined.insert(
        combined.end(),
        first.begin(),
        first.end()
    );

    combined.insert(
        combined.end(),
        second.begin(),
        second.end()
    );

    parser.push(combined);

    std::vector<std::uint8_t> frame;

    ASSERT_TRUE(
        parser.next_frame(frame)
    );

    EXPECT_EQ(
        frame,
        first
    );

    ASSERT_TRUE(
        parser.next_frame(frame)
    );

    EXPECT_EQ(
        frame,
        second
    );

    EXPECT_FALSE(
        parser.next_frame(frame)
    );
}

TEST(StreamTest, NoiseWithoutStartByteIsDiscarded) {
    FrameStreamParser parser;

    parser.push(
        parse_hex(
            "10 20 30 40 50"
        )
    );

    std::vector<std::uint8_t> frame;

    EXPECT_FALSE(
        parser.next_frame(frame)
    );

    EXPECT_EQ(
        parser.buffered_bytes(),
        0
    );
}

TEST(StreamTest, BufferNeverExceedsMaximumSize) {
    FrameStreamParser parser;

    std::vector<std::uint8_t> noise(
        FrameStreamParser::MAX_BUFFER_SIZE + 500,
        0x55
    );

    parser.push(noise);

    EXPECT_LE(
        parser.buffered_bytes(),
        FrameStreamParser::MAX_BUFFER_SIZE
    );
}

TEST(StreamTest, ValidFrameSurvivesNoiseAfterBufferTrim) {
    FrameStreamParser parser;

    std::vector<std::uint8_t> data(
        FrameStreamParser::MAX_BUFFER_SIZE + 100,
        0x55
    );

    const auto frame =
        parse_hex(
            "AA 01 04 10 20 30 40 BC"
        );

    data.insert(
        data.end(),
        frame.begin(),
        frame.end()
    );

    parser.push(data);

    std::vector<std::uint8_t> extracted;

    ASSERT_TRUE(
        parser.next_frame(extracted)
    );

    EXPECT_EQ(
        extracted,
        frame
    );
}

TEST(StreamTest, StartByteInsideNoiseCanResynchronize) {
    FrameStreamParser parser;

    /*
     * The first AA is inside noise and is followed by
     * bytes that do not form the intended valid frame.
     *
     * The second AA is the actual start of the valid frame.
     */
    parser.push(
        parse_hex(
            "10 20 AA 55 66 AA"
        )
    );

    std::vector<std::uint8_t> frame;

    EXPECT_FALSE(
        parser.next_frame(frame)
    );

    parser.push(
        parse_hex(
            "01 04 10 20 30 40 BC"
        )
    );

    ASSERT_TRUE(
        parser.next_frame(frame)
    );

    EXPECT_EQ(
        frame,
        parse_hex(
            "AA 01 04 10 20 30 40 BC"
        )
    );
}

// ============================================================
// SERIAL TESTS
// ============================================================

TEST(SerialTest, DefaultPortStartsClosed) {
    SerialPort serial;

    EXPECT_FALSE(
        serial.is_open()
    );
}

TEST(SerialTest, EmptyPortIsRejected) {
    SerialPort serial;

    SerialConfig config;

    config.port = "";

    std::string error;

    EXPECT_FALSE(
        serial.open(
            config,
            error
        )
    );

    EXPECT_EQ(
        error,
        "Serial port name cannot be empty."
    );

    EXPECT_FALSE(
        serial.is_open()
    );
}

TEST(SerialTest, UnsupportedBaudRateIsRejected) {
    SerialPort serial;

    SerialConfig config;

    config.port = "COM999";
    config.baud_rate = 12345;

    std::string error;

    EXPECT_FALSE(
        serial.open(
            config,
            error
        )
    );

    EXPECT_EQ(
        error,
        "Unsupported baud rate: 12345"
    );

    EXPECT_FALSE(
        serial.is_open()
    );
}

TEST(SerialTest, ReadFailsWhenPortIsClosed) {
    SerialPort serial;

    std::vector<std::uint8_t> data{
        0x10,
        0x20
    };

    std::string error;

    EXPECT_FALSE(
        serial.read(
            data,
            16,
            error
        )
    );

    EXPECT_TRUE(
        data.empty()
    );

    EXPECT_EQ(
        error,
        "Serial port is not open."
    );
}