#include "uartdiag/decoder.hpp"
#include "uartdiag/diagnostics.hpp"
#include "uartdiag/frame.hpp"
#include "uartdiag/hex.hpp"

#include <iomanip>
#include <iostream>
#include <string>
#include <vector>

namespace {

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

    std::cout << std::dec << '\n';
}

const char* frame_type_to_string(
    uartdiag::FrameType type
) {
    switch (type) {

        case uartdiag::FrameType::SensorData:
            return "SENSOR_DATA";

        case uartdiag::FrameType::Command:
            return "COMMAND";

        case uartdiag::FrameType::Response:
            return "RESPONSE";
    }

    return "UNKNOWN";
}

void print_usage() {

    std::cout
        << "UARTDiag - UART Protocol Diagnostic Tool\n\n"
        << "Usage:\n"
        << "  uartdiag --test <type>\n"
        << "  uartdiag --decode \"AA 01 04 10 20 30 40 BC\"\n\n"
        << "Tests:\n"
        << "  valid     Validate a correct frame\n"
        << "  crc       Inject payload corruption\n"
        << "  start     Inject invalid start byte\n"
        << "  length    Inject invalid payload length\n"
        << "  help      Show this help message\n";
}

void print_crc(
    const uartdiag::DiagnosticResult& result
) {
    std::cout
        << "\nCRC\n"
        << "---\n"
        << "Expected: 0x"
        << std::uppercase
        << std::hex
        << std::setw(2)
        << std::setfill('0')
        << static_cast<int>(result.expected_crc)
        << '\n'
        << "Received: 0x"
        << std::uppercase
        << std::hex
        << std::setw(2)
        << std::setfill('0')
        << static_cast<int>(result.received_crc)
        << std::dec
        << '\n';
}

void print_result(
    const uartdiag::DiagnosticResult& result
) {
    std::cout
        << "\nStatus:  "
        << uartdiag::to_string(result.status)
        << '\n'
        << "Message: "
        << result.message
        << '\n';

    if (result.status ==
            uartdiag::DiagnosticStatus::CrcError) {
        print_crc(result);
    }
}

int run_decode(
    const std::string& input
) {
    try {

        const auto raw_data =
            uartdiag::parse_hex(input);

        uartdiag::Decoder decoder;
        uartdiag::Frame decoded{};

        const auto result =
            decoder.decode(raw_data, decoded);

        std::cout
            << "\nUARTDiag\n"
            << "========================================\n"
            << "Input:\n";

        print_frame(raw_data);

        std::cout
            << "\nFrame Analysis\n"
            << "--------------\n";

        if (raw_data.size() >= 2) {

            const auto type =
                static_cast<uartdiag::FrameType>(
                    raw_data[1]
                );

            std::cout
                << "Frame Type: "
                << frame_type_to_string(type)
                << '\n';
        }

        if (raw_data.size() >= 3) {

            std::cout
                << "Payload Length: "
                << static_cast<int>(raw_data[2])
                << '\n';
        }

        if (result.status ==
            uartdiag::DiagnosticStatus::Valid) {

            std::cout
                << "Payload: ";

            print_frame(decoded.payload);

            print_crc(result);
        }
        else if (
            result.status ==
            uartdiag::DiagnosticStatus::CrcError) {

            if (raw_data.size() >= 4) {

                const auto payload_length =
                    static_cast<std::size_t>(
                        raw_data[2]
                    );

                if (
                    raw_data.size() ==
                    payload_length + 4
                ) {

                    std::vector<std::uint8_t> payload(
                        raw_data.begin() + 3,
                        raw_data.end() - 1
                    );

                    std::cout
                        << "Payload: ";

                    print_frame(payload);
                }
            }

            print_crc(result);
        }

        print_result(result);

        std::cout
            << "========================================\n";

        return result.is_valid() ? 0 : 1;

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
        decoder.decode(encoded, decoded);

    print_result(result);

    std::cout
        << "========================================\n";

    return result.is_valid() ? 0 : 1;
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

    const std::string command = argv[1];

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

        return run_test(argv[2]);
    }

    if (command == "--decode") {

        if (argc < 3) {

            std::cerr
                << "Error: missing hexadecimal frame.\n\n";

            print_usage();

            return 1;
        }

        return run_decode(argv[2]);
    }

    std::cerr
        << "Unknown command: "
        << command
        << "\n\n";

    print_usage();

    return 1;
}