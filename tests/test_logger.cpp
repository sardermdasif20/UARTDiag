#include "uartdiag/decoder.hpp"
#include "uartdiag/frame.hpp"
#include "uartdiag/logger.hpp"
#include "uartdiag/report.hpp"

#include <cstdint>
#include <filesystem>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>

#include <gtest/gtest.h>

namespace {

std::string read_file(
    const std::filesystem::path& path
) {
    std::ifstream file(path);

    std::ostringstream contents;

    contents << file.rdbuf();

    return contents.str();
}

uartdiag::DiagnosticReport make_valid_report() {

    uartdiag::Frame frame{
        uartdiag::FrameType::SensorData,
        {0x10, 0x20, 0x30, 0x40},
        0
    };

    const auto encoded =
        uartdiag::encode_frame(frame);

    uartdiag::Decoder decoder;

    uartdiag::Frame decoded{};

    uartdiag::DiagnosticReport report;

    report.raw_data = encoded;

    report.has_frame_type = true;
    report.frame_type = frame.type;

    report.has_payload_length = true;
    report.payload_length =
        frame.payload.size();

    report.result =
        decoder.decode(
            encoded,
            decoded
        );

    report.payload =
        decoded.payload;

    report.expected_crc =
        report.result.expected_crc;

    report.received_crc =
        report.result.received_crc;

    report.has_frame_number = true;
    report.frame_number = 7;

    report.has_timestamp = true;
    report.timestamp =
        "2026-10-01 15:30:00";

    return report;
}

TEST(LoggerTest, OpensAndWritesCsvFile) {

    const auto path =
        std::filesystem::temp_directory_path()
        / "uartdiag_logger_test.csv";

    std::filesystem::remove(path);

    uartdiag::DiagnosticLogger logger;

    std::string error;

    ASSERT_TRUE(
        logger.open(
            path.string(),
            error
        )
    ) << error;

    EXPECT_TRUE(
        logger.is_open()
    );

    const auto report =
        make_valid_report();

    ASSERT_TRUE(
        logger.write(
            report,
            error
        )
    ) << error;

    logger.close();

    ASSERT_FALSE(
        logger.is_open()
    );

    const auto contents =
        read_file(path);

    EXPECT_NE(
        contents.find(
            "timestamp,frame_number,type,payload_length,"
            "raw_data,crc_expected,crc_received,status,"
            "severity,message"
        ),
        std::string::npos
    );

    EXPECT_NE(
        contents.find(
            "2026-10-01 15:30:00"
        ),
        std::string::npos
    );

    EXPECT_NE(
        contents.find(
            "7"
        ),
        std::string::npos
    );

    EXPECT_NE(
        contents.find(
            "SENSOR_DATA"
        ),
        std::string::npos
    );

    EXPECT_NE(
        contents.find(
            "AA 01 04 10 20 30 40"
        ),
        std::string::npos
    );

    EXPECT_NE(
        contents.find(
            "VALID"
        ),
        std::string::npos
    );

    EXPECT_NE(
        contents.find(
            "INFO"
        ),
        std::string::npos
    );

    std::filesystem::remove(path);
}

TEST(LoggerTest, WriteFailsWhenLoggerIsNotOpen) {

    uartdiag::DiagnosticLogger logger;

    std::string error;

    const auto report =
        make_valid_report();

    EXPECT_FALSE(
        logger.write(
            report,
            error
        )
    );

    EXPECT_EQ(
        error,
        "Diagnostic log is not open."
    );
}

TEST(LoggerTest, EmptyFilenameIsRejected) {

    uartdiag::DiagnosticLogger logger;

    std::string error;

    EXPECT_FALSE(
        logger.open(
            "",
            error
        )
    );

    EXPECT_EQ(
        error,
        "Log filename cannot be empty."
    );

    EXPECT_FALSE(
        logger.is_open()
    );
}

TEST(LoggerTest, MultipleReportsAreAppended) {

    const auto path =
        std::filesystem::temp_directory_path()
        / "uartdiag_logger_append_test.csv";

    std::filesystem::remove(path);

    const auto report =
        make_valid_report();

    {
        uartdiag::DiagnosticLogger logger;

        std::string error;

        ASSERT_TRUE(
            logger.open(
                path.string(),
                error
            )
        ) << error;

        ASSERT_TRUE(
            logger.write(
                report,
                error
            )
        ) << error;

        logger.close();
    }

    {
        uartdiag::DiagnosticLogger logger;

        std::string error;

        ASSERT_TRUE(
            logger.open(
                path.string(),
                error
            )
        ) << error;

        ASSERT_TRUE(
            logger.write(
                report,
                error
            )
        ) << error;

        logger.close();
    }

    const auto contents =
        read_file(path);

    const std::string header =
        "timestamp,frame_number,type,payload_length,"
        "raw_data,crc_expected,crc_received,status,"
        "severity,message";

    const auto first_header =
        contents.find(header);

    ASSERT_NE(
        first_header,
        std::string::npos
    );

    const auto second_header =
        contents.find(
            header,
            first_header + header.size()
        );

    EXPECT_EQ(
        second_header,
        std::string::npos
    );

    const auto first_entry =
        contents.find(
            "2026-10-01 15:30:00"
        );

    ASSERT_NE(
        first_entry,
        std::string::npos
    );

    const auto second_entry =
        contents.find(
            "2026-10-01 15:30:00",
            first_entry + 1
        );

    EXPECT_NE(
        second_entry,
        std::string::npos
    );

    std::filesystem::remove(path);
}

} // namespace
