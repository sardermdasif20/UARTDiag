#include "uartdiag/decoder.hpp"
#include "uartdiag/frame.hpp"
#include "uartdiag/stream.hpp"

#include <cstdint>
#include <vector>

#include <gtest/gtest.h>

namespace {

using uartdiag::Decoder;
using uartdiag::DiagnosticStatus;
using uartdiag::Frame;
using uartdiag::FrameStreamParser;
using uartdiag::FrameType;
using uartdiag::encode_frame;

TEST(SimulationTest, ValidFramePassesThroughStreamParser) {

    Frame frame{
        FrameType::SensorData,
        {0x10, 0x20, 0x30, 0x40},
        0
    };

    const auto encoded =
        encode_frame(frame);

    FrameStreamParser parser;

    parser.push(encoded);

    std::vector<std::uint8_t> received;

    ASSERT_TRUE(
        parser.next_frame(received)
    );

    EXPECT_EQ(
        received,
        encoded
    );

    Decoder decoder;

    Frame decoded{};

    const auto result =
        decoder.decode(
            received,
            decoded
        );

    EXPECT_EQ(
        result.status,
        DiagnosticStatus::Valid
    );

    EXPECT_EQ(
        decoded.type,
        FrameType::SensorData
    );

    EXPECT_EQ(
        decoded.payload,
        frame.payload
    );
}

TEST(SimulationTest, CorruptedFrameProducesCrcError) {

    Frame frame{
        FrameType::Command,
        {0x01, 0x02, 0x03},
        0
    };

    auto encoded =
        encode_frame(frame);

    /*
     * Corrupt one payload byte without updating
     * the CRC.
     */
    encoded[4] ^= 0xFF;

    FrameStreamParser parser;

    parser.push(encoded);

    std::vector<std::uint8_t> received;

    ASSERT_TRUE(
        parser.next_frame(received)
    );

    Decoder decoder;

    Frame decoded{};

    const auto result =
        decoder.decode(
            received,
            decoded
        );

    EXPECT_EQ(
        result.status,
        DiagnosticStatus::CrcError
    );

    EXPECT_FALSE(
        result.is_valid()
    );
}

TEST(SimulationTest, NoiseBeforeFrameIsIgnored) {

    Frame frame{
        FrameType::Response,
        {0xA1, 0xB2},
        0
    };

    const auto encoded =
        encode_frame(frame);

    const std::vector<std::uint8_t> noise{
        0x12,
        0x34,
        0x56,
        0x78
    };

    FrameStreamParser parser;

    parser.push(noise);

    parser.push(encoded);

    std::vector<std::uint8_t> received;

    ASSERT_TRUE(
        parser.next_frame(received)
    );

    EXPECT_EQ(
        received,
        encoded
    );

    EXPECT_EQ(
        parser.buffered_bytes(),
        0U
    );
}

TEST(SimulationTest, FakeStartByteInsideNoiseDoesNotBlockValidFrame) {

    Frame frame{
        FrameType::Response,
        {0xA1, 0xB2},
        0
    };

    const auto encoded =
        encode_frame(frame);

    /*
     * AA is a fake START byte.
     *
     * 55 is not a valid frame type, so the parser
     * must reject this candidate and continue looking
     * for the real frame.
     */
    const std::vector<std::uint8_t> noise{
        0x99,
        0xAA,
        0x55,
        0x66
    };

    FrameStreamParser parser;

    parser.push(noise);

    parser.push(encoded);

    std::vector<std::uint8_t> received;

    ASSERT_TRUE(
        parser.next_frame(received)
    );

    EXPECT_EQ(
        received,
        encoded
    );
}

TEST(SimulationTest, FragmentedFrameIsReassembled) {

    Frame frame{
        FrameType::SensorData,
        {0x55, 0x66, 0x77, 0x88},
        0
    };

    const auto encoded =
        encode_frame(frame);

    /*
     * Split the frame into two chunks to simulate
     * UART data arriving across separate reads.
     */
    const std::vector<std::uint8_t> first_chunk(
        encoded.begin(),
        encoded.begin() + 4
    );

    const std::vector<std::uint8_t> second_chunk(
        encoded.begin() + 4,
        encoded.end()
    );

    FrameStreamParser parser;

    parser.push(first_chunk);

    std::vector<std::uint8_t> received;

    EXPECT_FALSE(
        parser.next_frame(received)
    );

    EXPECT_EQ(
        parser.buffered_bytes(),
        first_chunk.size()
    );

    parser.push(second_chunk);

    ASSERT_TRUE(
        parser.next_frame(received)
    );

    EXPECT_EQ(
        received,
        encoded
    );

    EXPECT_EQ(
        parser.buffered_bytes(),
        0U
    );
}

TEST(SimulationTest, MultipleFramesAreProcessedFromSingleStream) {

    Frame frame1{
        FrameType::SensorData,
        {0x10, 0x20},
        0
    };

    Frame frame2{
        FrameType::Command,
        {0x30, 0x40, 0x50},
        0
    };

    Frame frame3{
        FrameType::Response,
        {0x60},
        0
    };

    const auto encoded1 =
        encode_frame(frame1);

    const auto encoded2 =
        encode_frame(frame2);

    const auto encoded3 =
        encode_frame(frame3);

    std::vector<std::uint8_t> stream;

    stream.insert(
        stream.end(),
        encoded1.begin(),
        encoded1.end()
    );

    stream.insert(
        stream.end(),
        encoded2.begin(),
        encoded2.end()
    );

    stream.insert(
        stream.end(),
        encoded3.begin(),
        encoded3.end()
    );

    FrameStreamParser parser;

    parser.push(stream);

    std::vector<std::uint8_t> received;

    ASSERT_TRUE(
        parser.next_frame(received)
    );

    EXPECT_EQ(
        received,
        encoded1
    );

    ASSERT_TRUE(
        parser.next_frame(received)
    );

    EXPECT_EQ(
        received,
        encoded2
    );

    ASSERT_TRUE(
        parser.next_frame(received)
    );

    EXPECT_EQ(
        received,
        encoded3
    );

    EXPECT_FALSE(
        parser.next_frame(received)
    );

    EXPECT_EQ(
        parser.buffered_bytes(),
        0U
    );
}

TEST(SimulationTest, MixedStreamProducesExpectedDiagnostics) {

    Frame valid_frame{
        FrameType::SensorData,
        {0x10, 0x20, 0x30},
        0
    };

    Frame corrupted_frame{
        FrameType::Command,
        {0x01, 0x02},
        0
    };

    const auto encoded_valid =
        encode_frame(valid_frame);

    auto encoded_corrupted =
        encode_frame(corrupted_frame);

    /*
     * Corrupt payload while leaving the original CRC.
     */
    encoded_corrupted[4] ^= 0xFF;

    FrameStreamParser parser;

    /*
     * Put both frames into one simulated UART stream.
     */
    parser.push(encoded_valid);

    parser.push(encoded_corrupted);

    Decoder decoder;

    std::vector<std::uint8_t> received;

    /*
     * First frame must be valid.
     */
    ASSERT_TRUE(
        parser.next_frame(received)
    );

    Frame decoded1{};

    const auto result1 =
        decoder.decode(
            received,
            decoded1
        );

    EXPECT_EQ(
        result1.status,
        DiagnosticStatus::Valid
    );

    /*
     * Second frame must report CRC failure.
     */
    ASSERT_TRUE(
        parser.next_frame(received)
    );

    Frame decoded2{};

    const auto result2 =
        decoder.decode(
            received,
            decoded2
        );

    EXPECT_EQ(
        result2.status,
        DiagnosticStatus::CrcError
    );

    /*
     * No more frames should remain.
     */
    EXPECT_FALSE(
        parser.next_frame(received)
    );
}

} // namespace