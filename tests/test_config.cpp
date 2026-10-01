#include "uartdiag/config.hpp"

#include <fstream>
#include <string>

#include <gtest/gtest.h>

namespace {

std::string test_filename(
    const std::string& name
) {
    return "uartdiag_test_" + name + ".conf";
}

TEST(ConfigTest, DefaultConfigurationIsValid) {
    const uartdiag::DiagnosticConfig config;

    EXPECT_TRUE(config.is_valid());
}

TEST(ConfigTest, DefaultBaudRateIs115200) {
    const uartdiag::DiagnosticConfig config;

    EXPECT_EQ(
        config.default_baud_rate,
        115200U
    );
}

TEST(ConfigTest, DefaultSerialReadSizeIs256) {
    const uartdiag::DiagnosticConfig config;

    EXPECT_EQ(
        config.serial_read_size,
        256U
    );
}

TEST(ConfigTest, DefaultSimulationLogFilenameIsCorrect) {
    const uartdiag::DiagnosticConfig config;

    EXPECT_EQ(
        config.simulation_log_filename,
        "uartdiag_simulation.csv"
    );
}

TEST(ConfigTest, DefaultSerialLogFilenameIsCorrect) {
    const uartdiag::DiagnosticConfig config;

    EXPECT_EQ(
        config.serial_log_filename,
        "uartdiag_serial.csv"
    );
}

TEST(ConfigTest, ZeroBaudRateIsInvalid) {
    uartdiag::DiagnosticConfig config;

    config.default_baud_rate = 0;

    EXPECT_FALSE(config.is_valid());
}

TEST(ConfigTest, ZeroSerialReadSizeIsInvalid) {
    uartdiag::DiagnosticConfig config;

    config.serial_read_size = 0;

    EXPECT_FALSE(config.is_valid());
}

TEST(ConfigTest, EmptySimulationLogFilenameIsInvalid) {
    uartdiag::DiagnosticConfig config;

    config.simulation_log_filename.clear();

    EXPECT_FALSE(config.is_valid());
}

TEST(ConfigTest, EmptySerialLogFilenameIsInvalid) {
    uartdiag::DiagnosticConfig config;

    config.serial_log_filename.clear();

    EXPECT_FALSE(config.is_valid());
}

TEST(ConfigTest, LoadsValidConfigurationFile) {
    const std::string filename =
        test_filename("valid");

    {
        std::ofstream file(filename);

        ASSERT_TRUE(file.is_open());

        file
            << "# UARTDiag test configuration\n"
            << "baud_rate=57600\n"
            << "serial_read_size=512\n"
            << "simulation_log=simulation_test.csv\n"
            << "serial_log=serial_test.csv\n";
    }

    uartdiag::DiagnosticConfig config;
    std::string error;

    EXPECT_TRUE(
        uartdiag::load_config_file(
            filename,
            config,
            error
        )
    );

    EXPECT_TRUE(error.empty());

    EXPECT_EQ(
        config.default_baud_rate,
        57600U
    );

    EXPECT_EQ(
        config.serial_read_size,
        512U
    );

    EXPECT_EQ(
        config.simulation_log_filename,
        "simulation_test.csv"
    );

    EXPECT_EQ(
        config.serial_log_filename,
        "serial_test.csv"
    );

    std::remove(filename.c_str());
}

TEST(ConfigTest, IgnoresBlankLinesAndComments) {
    const std::string filename =
        test_filename("comments");

    {
        std::ofstream file(filename);

        ASSERT_TRUE(file.is_open());

        file
            << "\n"
            << "# comment\n"
            << "  baud_rate = 38400  \n"
            << "\n"
            << "# another comment\n"
            << "serial_read_size = 128\n";
    }

    uartdiag::DiagnosticConfig config;
    std::string error;

    EXPECT_TRUE(
        uartdiag::load_config_file(
            filename,
            config,
            error
        )
    );

    EXPECT_EQ(
        config.default_baud_rate,
        38400U
    );

    EXPECT_EQ(
        config.serial_read_size,
        128U
    );

    std::remove(filename.c_str());
}

TEST(ConfigTest, MissingConfigurationFileFails) {
    uartdiag::DiagnosticConfig config;
    std::string error;

    EXPECT_FALSE(
        uartdiag::load_config_file(
            "file_that_does_not_exist.conf",
            config,
            error
        )
    );

    EXPECT_FALSE(error.empty());
}

TEST(ConfigTest, InvalidLineFails) {
    const std::string filename =
        test_filename("invalid_line");

    {
        std::ofstream file(filename);

        ASSERT_TRUE(file.is_open());

        file << "baud_rate 115200\n";
    }

    uartdiag::DiagnosticConfig config;
    std::string error;

    EXPECT_FALSE(
        uartdiag::load_config_file(
            filename,
            config,
            error
        )
    );

    EXPECT_FALSE(error.empty());

    std::remove(filename.c_str());
}

TEST(ConfigTest, UnknownKeyFails) {
    const std::string filename =
        test_filename("unknown_key");

    {
        std::ofstream file(filename);

        ASSERT_TRUE(file.is_open());

        file
            << "baud_rate=115200\n"
            << "unknown_setting=123\n";
    }

    uartdiag::DiagnosticConfig config;
    std::string error;

    EXPECT_FALSE(
        uartdiag::load_config_file(
            filename,
            config,
            error
        )
    );

    EXPECT_FALSE(error.empty());

    std::remove(filename.c_str());
}

TEST(ConfigTest, InvalidBaudRateFails) {
    const std::string filename =
        test_filename("invalid_baud");

    {
        std::ofstream file(filename);

        ASSERT_TRUE(file.is_open());

        file << "baud_rate=abc\n";
    }

    uartdiag::DiagnosticConfig config;
    std::string error;

    EXPECT_FALSE(
        uartdiag::load_config_file(
            filename,
            config,
            error
        )
    );

    EXPECT_FALSE(error.empty());

    std::remove(filename.c_str());
}

TEST(ConfigTest, ZeroBaudRateFromFileFails) {
    const std::string filename =
        test_filename("zero_baud");

    {
        std::ofstream file(filename);

        ASSERT_TRUE(file.is_open());

        file << "baud_rate=0\n";
    }

    uartdiag::DiagnosticConfig config;
    std::string error;

    EXPECT_FALSE(
        uartdiag::load_config_file(
            filename,
            config,
            error
        )
    );

    EXPECT_FALSE(error.empty());

    std::remove(filename.c_str());
}

TEST(ConfigTest, ZeroReadSizeFromFileFails) {
    const std::string filename =
        test_filename("zero_read_size");

    {
        std::ofstream file(filename);

        ASSERT_TRUE(file.is_open());

        file << "serial_read_size=0\n";
    }

    uartdiag::DiagnosticConfig config;
    std::string error;

    EXPECT_FALSE(
        uartdiag::load_config_file(
            filename,
            config,
            error
        )
    );

    EXPECT_FALSE(error.empty());

    std::remove(filename.c_str());
}

TEST(ConfigTest, EmptySimulationLogFromFileFails) {
    const std::string filename =
        test_filename("empty_simulation_log");

    {
        std::ofstream file(filename);

        ASSERT_TRUE(file.is_open());

        file << "simulation_log=\n";
    }

    uartdiag::DiagnosticConfig config;
    std::string error;

    EXPECT_FALSE(
        uartdiag::load_config_file(
            filename,
            config,
            error
        )
    );

    EXPECT_FALSE(error.empty());

    std::remove(filename.c_str());
}

TEST(ConfigTest, EmptySerialLogFromFileFails) {
    const std::string filename =
        test_filename("empty_serial_log");

    {
        std::ofstream file(filename);

        ASSERT_TRUE(file.is_open());

        file << "serial_log=\n";
    }

    uartdiag::DiagnosticConfig config;
    std::string error;

    EXPECT_FALSE(
        uartdiag::load_config_file(
            filename,
            config,
            error
        )
    );

    EXPECT_FALSE(error.empty());

    std::remove(filename.c_str());
}

} // namespace