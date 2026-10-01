#include "uartdiag/config.hpp"

#include <gtest/gtest.h>

namespace {

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

} // namespace