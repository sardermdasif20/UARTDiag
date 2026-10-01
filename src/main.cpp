#include "uartdiag/decoder.hpp"
#include "uartdiag/diagnostics.hpp"
#include "uartdiag/frame.hpp"
#include "uartdiag/hex.hpp"
#include "uartdiag/report.hpp"
#include "uartdiag/serial.hpp"
#include "uartdiag/stream.hpp"

#include <atomic>
#include <chrono>
#include <ctime>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#ifdef _WIN32
#include <windows.h>
#endif

namespace {

std::atomic<bool> g_running{true};

struct SerialStatistics {
    std::size_t bytes_received{0};
    std::size_t frames_received{0};
    std::size_t valid_frames{0};
    std::size_t crc_errors{0};
    std::size_t invalid_frames{0};
};

#ifdef _WIN32

BOOL WINAPI console_ctrl_handler(
    DWORD signal
) {
    switch (signal) {

        case CTRL_C_EVENT:
        case CTRL_BREAK_EVENT:
        case CTRL_CLOSE_EVENT:
        case CTRL_LOGOFF_EVENT:
        case CTRL_SHUTDOWN_EVENT:

            g_running = false;
            return TRUE;

        default:
            return FALSE;
    }
}

bool install_console_handler() {
    return SetConsoleCtrlHandler(
        console_ctrl_handler,
        TRUE
    ) != 0;
}

#endif

std::string current_timestamp() {

    const auto now =
        std::chrono::system_clock::now();

    const std::time_t current_time =
        std::chrono::system_clock::to_time_t(now);

    const std::tm* local_time =
        std::localtime(&current_time);

    if (local_time == nullptr) {
        return "UNKNOWN";
    }

    std::ostringstream output;

    output
        << std::put_time(
            local_time,
            "%Y-%m-%d %H:%M:%S"
        );

    return output.str();
}

void print_frame(
    const std::vector<std::uint8_t>& frame
) {
    for (const auto byte : frame) {

        std::cout
            << std::uppercase
            << std::hex
            << std::setw(2)
            << std::setfill('0')
            << static_cast<int>(byte)
            << ' ';
    }

    std::cout
        << std::dec
        << '\n';
}

void print_usage() {

    std::cout
        << "UARTDiag - UART Protocol Diagnostic Tool\n\n"

        << "Usage:\n"
        << "  uartdiag --test <type>\n"
        << "  uartdiag --decode \"AA 01 04 10 20 30 40 BC\"\n"
        << "  uartdiag --serial <port> [baud]\n"
        << "  uartdiag --simulate [scenario]\n\n"

        << "Tests:\n"
        << "  valid     Validate a correct frame\n"
        << "  crc       Inject payload corruption\n"
        << "  start     Inject invalid start byte\n"
        << "  length    Inject invalid payload length\n\n"

        << "Serial:\n"
        << "  port      COM port, for example COM3\n"
        << "  baud      Baud rate, default is 115200\n\n"

        << "Simulation:\n"
        << "  all         Run all simulation scenarios\n"
        << "  valid       Valid frame\n"
        << "  crc         CRC corruption\n"
        << "  noise       Noise and resynchronization\n"
        << "  fragmented  Fragmented UART frame\n\n"

        << "Examples:\n"
        << "  uartdiag --serial COM3\n"
        << "  uartdiag --serial COM3 115200\n"
        << "  uartdiag --simulate\n"
        << "  uartdiag --simulate valid\n"
        << "  uartdiag --simulate crc\n"
        << "  uartdiag --simulate noise\n"
        << "  uartdiag --simulate fragmented\n";
}

uartdiag::DiagnosticReport build_report(
    const std::vector<std::uint8_t>& raw_data
) {
    uartdiag::DiagnosticReport report;

    report.raw_data = raw_data;

    if (raw_data.size() >= 2) {

        const auto type =
            static_cast<uartdiag::FrameType>(
                raw_data[1]
            );

        if (
            type == uartdiag::FrameType::SensorData ||
            type == uartdiag::FrameType::Command ||
            type == uartdiag::FrameType::Response
        ) {
            report.has_frame_type = true;
            report.frame_type = type;
        }
    }

    if (raw_data.size() >= 3) {

        report.has_payload_length = true;

        report.payload_length =
            static_cast<std::size_t>(
                raw_data[2]
            );
    }

    uartdiag::Decoder decoder;
    uartdiag::Frame decoded{};

    report.result =
        decoder.decode(
            raw_data,
            decoded
        );

    if (
        report.result.status ==
            uartdiag::DiagnosticStatus::Valid ||
        report.result.status ==
            uartdiag::DiagnosticStatus::CrcError
    ) {

        report.payload =
            decoded.payload;

        report.expected_crc =
            report.result.expected_crc;

        report.received_crc =
            report.result.received_crc;
    }

    return report;
}

int run_decode(
    const std::string& input
) {
    try {

        const auto raw_data =
            uartdiag::parse_hex(input);

        const auto report =
            build_report(raw_data);

        std::cout
            << uartdiag::format_report(
                report
            );

        return report.result.is_valid()
            ? 0
            : 1;
    }
    catch (const std::exception& error) {

        std::cerr
            << "Error: "
            << error.what()
            << '\n';

        return 1;
    }
}

int run_test(
    const std::string& test_name
) {
    uartdiag::Frame frame{
        uartdiag::FrameType::SensorData,
        {0x10, 0x20, 0x30, 0x40},
        0
    };

    auto encoded =
        uartdiag::encode_frame(frame);

    std::cout
        << "\nUARTDiag\n"
        << "========================================\n"
        << "Test: "
        << test_name
        << "\n\n";

    if (test_name == "crc") {

        std::cout
            << "Fault injected: payload corruption\n";

        encoded[4] ^= 0xFF;
    }
    else if (test_name == "start") {

        std::cout
            << "Fault injected: invalid start byte\n";

        encoded[0] = 0x55;
    }
    else if (test_name == "length") {

        std::cout
            << "Fault injected: invalid payload length\n";

        encoded[2] = 0x20;
    }
    else if (test_name != "valid") {

        std::cerr
            << "Unknown test: "
            << test_name
            << "\n\n";

        print_usage();

        return 1;
    }

    std::cout
        << "Frame:\n";

    print_frame(encoded);

    uartdiag::Decoder decoder;
    uartdiag::Frame decoded{};

    const auto result =
        decoder.decode(
            encoded,
            decoded
        );

    std::cout
        << "\nDiagnostic Result\n"
        << "-----------------\n"
        << "Status:  "
        << uartdiag::to_string(
            result.status
        )
        << '\n'
        << "Message: "
        << result.message
        << '\n';

    if (
        result.status ==
        uartdiag::DiagnosticStatus::CrcError
    ) {

        std::cout
            << "Expected CRC: 0x"
            << std::uppercase
            << std::hex
            << std::setw(2)
            << std::setfill('0')
            << static_cast<int>(
                result.expected_crc
            )
            << '\n';

        std::cout
            << "Received CRC: 0x"
            << std::uppercase
            << std::hex
            << std::setw(2)
            << std::setfill('0')
            << static_cast<int>(
                result.received_crc
            )
            << std::dec
            << '\n';
    }

    std::cout
        << "========================================\n";

    return result.is_valid()
        ? 0
        : 1;
}

bool parse_baud_rate(
    const std::string& input,
    unsigned int& baud_rate
) {
    try {

        const unsigned long value =
            std::stoul(input);

        if (
            value >
            static_cast<unsigned long>(
                0xFFFFFFFFUL
            )
        ) {
            return false;
        }

        baud_rate =
            static_cast<unsigned int>(
                value
            );

        return true;
    }
    catch (const std::exception&) {
        return false;
    }
}

void update_statistics(
    const uartdiag::DiagnosticResult& result,
    SerialStatistics& statistics
) {
    ++statistics.frames_received;

    if (result.is_valid()) {

        ++statistics.valid_frames;

        return;
    }

    if (
        result.status ==
        uartdiag::DiagnosticStatus::CrcError
    ) {

        ++statistics.crc_errors;

        return;
    }

    ++statistics.invalid_frames;
}

void print_statistics(
    const SerialStatistics& statistics
) {
    std::cout
        << "\nSession Statistics\n"
        << "------------------\n"

        << "Bytes received:  "
        << statistics.bytes_received
        << '\n'

        << "Frames received: "
        << statistics.frames_received
        << '\n'

        << "Valid frames:    "
        << statistics.valid_frames
        << '\n'

        << "CRC errors:      "
        << statistics.crc_errors
        << '\n'

        << "Invalid frames:  "
        << statistics.invalid_frames
        << '\n';
}

void process_stream(
    uartdiag::FrameStreamParser& parser,
    SerialStatistics& statistics,
    std::size_t& frame_number
) {
    std::vector<std::uint8_t> frame;

    while (
        g_running &&
        parser.next_frame(frame)
    ) {

        ++frame_number;

        const std::string timestamp =
            current_timestamp();

        std::cout
            << "\n----------------------------------------\n"
            << "Received frame #"
            << frame_number
            << " at "
            << timestamp
            << ":\n";

        print_frame(frame);

        auto report =
            build_report(frame);

        report.has_frame_number = true;
        report.frame_number =
            frame_number;

        report.has_timestamp = true;
        report.timestamp =
            timestamp;

        update_statistics(
            report.result,
            statistics
        );

        std::cout
            << uartdiag::format_report(
                report
            );

        std::cout
            << "----------------------------------------\n";
    }
}

void push_simulated_data(
    uartdiag::FrameStreamParser& parser,
    SerialStatistics& statistics,
    std::size_t& frame_number,
    const std::vector<std::uint8_t>& data,
    const std::string& description
) {
    std::cout
        << "\n[SIM] "
        << description
        << '\n';

    parser.push(data);

    statistics.bytes_received +=
        data.size();

    process_stream(
        parser,
        statistics,
        frame_number
    );
}

int run_serial(
    const std::string& port,
    unsigned int baud_rate
) {
    uartdiag::SerialConfig config;

    config.port = port;
    config.baud_rate = baud_rate;

    uartdiag::SerialPort serial;

    std::string error;

    if (!serial.open(
            config,
            error
        )) {

        std::cerr
            << "Serial error: "
            << error
            << '\n';

        return 1;
    }

#ifdef _WIN32

    if (!install_console_handler()) {

        std::cerr
            << "Warning: could not install "
            << "console shutdown handler.\n";
    }

#endif

    std::cout
        << "\nUARTDiag Serial Monitor\n"
        << "========================================\n"

        << "Port: "
        << config.port
        << '\n'

        << "Baud: "
        << config.baud_rate
        << "\n\n"

        << "Listening for UART frames...\n"
        << "Press Ctrl+C to stop.\n"

        << "========================================\n";

    uartdiag::FrameStreamParser parser;

    SerialStatistics statistics;

    std::size_t frame_number = 0;

    while (g_running) {

        std::vector<std::uint8_t> incoming;

        if (!serial.read(
                incoming,
                256,
                error
            )) {

            if (!g_running) {
                break;
            }

            std::cerr
                << "\nSerial read error: "
                << error
                << '\n';

            serial.close();

            return 1;
        }

        statistics.bytes_received +=
            incoming.size();

        if (!incoming.empty()) {
            parser.push(incoming);
        }

        process_stream(
            parser,
            statistics,
            frame_number
        );
    }

    serial.close();

#ifdef _WIN32

    SetConsoleCtrlHandler(
        console_ctrl_handler,
        FALSE
    );

#endif

    std::cout
        << "\nUARTDiag serial monitor stopped.\n";

    print_statistics(
        statistics
    );

    return 0;
}

void print_simulation_header(
    const std::string& scenario
) {
    std::cout
        << "\n========================================\n"
        << "UARTDiag Simulation: "
        << scenario
        << "\n"
        << "========================================\n";
}

void simulate_valid() {

    print_simulation_header(
        "VALID FRAME"
    );

    uartdiag::FrameStreamParser parser;
    SerialStatistics statistics;
    std::size_t frame_number = 0;

    uartdiag::Frame frame{
        uartdiag::FrameType::SensorData,
        {0x10, 0x20, 0x30, 0x40},
        0
    };

    const auto encoded =
        uartdiag::encode_frame(frame);

    push_simulated_data(
        parser,
        statistics,
        frame_number,
        encoded,
        "Sending valid sensor frame..."
    );

    print_statistics(statistics);
}

void simulate_crc() {

    print_simulation_header(
        "CRC CORRUPTION"
    );

    uartdiag::FrameStreamParser parser;
    SerialStatistics statistics;
    std::size_t frame_number = 0;

    uartdiag::Frame frame{
        uartdiag::FrameType::Command,
        {0x01, 0x02, 0x03},
        0
    };

    auto encoded =
        uartdiag::encode_frame(frame);

    encoded[4] ^= 0xFF;

    push_simulated_data(
        parser,
        statistics,
        frame_number,
        encoded,
        "Sending CRC-corrupted command frame..."
    );

    print_statistics(statistics);
}

void simulate_noise() {

    print_simulation_header(
        "NOISE AND RESYNCHRONIZATION"
    );

    uartdiag::FrameStreamParser parser;
    SerialStatistics statistics;
    std::size_t frame_number = 0;

    const std::vector<std::uint8_t> noise{
        0x12,
        0x34,
        0xAA,
        0x55,
        0x66,
        0x78
    };

    uartdiag::Frame frame{
        uartdiag::FrameType::Response,
        {0xA1, 0xB2},
        0
    };

    const auto encoded =
        uartdiag::encode_frame(frame);

    push_simulated_data(
        parser,
        statistics,
        frame_number,
        noise,
        "Sending noise containing a fake START byte..."
    );

    push_simulated_data(
        parser,
        statistics,
        frame_number,
        encoded,
        "Sending valid response frame..."
    );

    print_statistics(statistics);
}

void simulate_fragmented() {

    print_simulation_header(
        "FRAGMENTED FRAME"
    );

    uartdiag::FrameStreamParser parser;
    SerialStatistics statistics;
    std::size_t frame_number = 0;

    uartdiag::Frame frame{
        uartdiag::FrameType::SensorData,
        {0x55, 0x66, 0x77, 0x88},
        0
    };

    const auto encoded =
        uartdiag::encode_frame(frame);

    const std::vector<std::uint8_t> first_chunk(
        encoded.begin(),
        encoded.begin() + 4
    );

    const std::vector<std::uint8_t> second_chunk(
        encoded.begin() + 4,
        encoded.end()
    );

    push_simulated_data(
        parser,
        statistics,
        frame_number,
        first_chunk,
        "Sending frame chunk 1..."
    );

    push_simulated_data(
        parser,
        statistics,
        frame_number,
        second_chunk,
        "Sending frame chunk 2..."
    );

    print_statistics(statistics);
}

int run_simulation(
    const std::string& scenario
) {
    if (
        scenario == "all" ||
        scenario.empty()
    ) {

        std::cout
            << "\nUARTDiag UART Simulation Suite\n"
            << "========================================\n"
            << "Running all simulation scenarios.\n";

        simulate_valid();
        simulate_crc();
        simulate_noise();
        simulate_fragmented();

        std::cout
            << "\nSimulation suite complete.\n";

        return 0;
    }

    if (scenario == "valid") {
        simulate_valid();
        return 0;
    }

    if (scenario == "crc") {
        simulate_crc();
        return 0;
    }

    if (scenario == "noise") {
        simulate_noise();
        return 0;
    }

    if (scenario == "fragmented") {
        simulate_fragmented();
        return 0;
    }

    std::cerr
        << "Unknown simulation scenario: "
        << scenario
        << "\n\n";

    print_usage();

    return 1;
}

} // namespace

int main(
    int argc,
    char* argv[]
) {
    if (argc < 2) {

        print_usage();

        return 1;
    }

    const std::string command =
        argv[1];

    if (
        command == "--help" ||
        command == "-h" ||
        command == "help"
    ) {

        print_usage();

        return 0;
    }

    if (command == "--test") {

        if (argc < 3) {

            std::cerr
                << "Error: missing test type.\n\n";

            print_usage();

            return 1;
        }

        return run_test(
            argv[2]
        );
    }

    if (command == "--decode") {

        if (argc < 3) {

            std::cerr
                << "Error: missing hexadecimal frame.\n\n";

            print_usage();

            return 1;
        }

        return run_decode(
            argv[2]
        );
    }

    if (command == "--serial") {

        if (argc < 3) {

            std::cerr
                << "Error: missing serial port.\n\n";

            print_usage();

            return 1;
        }

        const std::string port =
            argv[2];

        unsigned int baud_rate =
            115200;

        if (argc >= 4) {

            if (!parse_baud_rate(
                    argv[3],
                    baud_rate
                )) {

                std::cerr
                    << "Error: invalid baud rate: "
                    << argv[3]
                    << '\n';

                return 1;
            }
        }

        return run_serial(
            port,
            baud_rate
        );
    }

    if (command == "--simulate") {

        const std::string scenario =
            argc >= 3
                ? argv[2]
                : "all";

        return run_simulation(
            scenario
        );
    }

    std::cerr
        << "Unknown command: "
        << command
        << "\n\n";

    print_usage();

    return 1;
}