#include "uartdiag/decoder.hpp"
#include "uartdiag/diagnostics.hpp"
#include "uartdiag/frame.hpp"

#include <iomanip>
#include <iostream>

void print_diagnostic(
    const uartdiag::DiagnosticResult& result
) {
    std::cout
        << "\n========================================\n"
        << "           UART DIAGNOSTIC\n"
        << "========================================\n";

    std::cout
        << "Status:        "
        << uartdiag::to_string(result.status)
        << '\n';

    std::cout
        << "Message:       "
        << result.message
        << '\n';

    if (result.status == uartdiag::DiagnosticStatus::CrcError) {
        std::cout
            << "Expected CRC:  0x"
            << std::uppercase
            << std::hex
            << std::setw(2)
            << std::setfill('0')
            << static_cast<int>(result.expected_crc)
            << '\n';

        std::cout
            << "Received CRC:  0x"
            << std::uppercase
            << std::hex
            << std::setw(2)
            << std::setfill('0')
            << static_cast<int>(result.received_crc)
            << '\n';
    }

    std::cout
        << "========================================\n";
}

int main() {

    // TEST 1: VALID FRAME
    uartdiag::Frame original{
        uartdiag::FrameType::SensorData,
        {0x10, 0x20, 0x30, 0x40},
        0
    };

    auto encoded = uartdiag::encode_frame(original);

    std::cout << "Encoded frame:\n";

    for (const auto byte : encoded) {
        std::cout
            << std::uppercase
            << std::hex
            << std::setw(2)
            << std::setfill('0')
            << static_cast<int>(byte)
            << ' ';
    }

    std::cout << '\n';

    uartdiag::Decoder decoder;
    uartdiag::Frame decoded{};

    auto result = decoder.decode(encoded, decoded);

    print_diagnostic(result);


    // TEST 2: CRC CORRUPTION
    std::cout << "\nTesting CRC corruption...\n";

    auto corrupted_data = encoded;
    corrupted_data[4] ^= 0xFF;

    uartdiag::Frame corrupted{};

    auto corrupted_result =
        decoder.decode(corrupted_data, corrupted);

    print_diagnostic(corrupted_result);


    // TEST 3: INVALID START BYTE
    std::cout << "\nTesting invalid start byte...\n";

    auto invalid_start = encoded;
    invalid_start[0] = 0x55;

    uartdiag::Frame invalid_start_frame{};

    auto invalid_start_result =
        decoder.decode(
            invalid_start,
            invalid_start_frame
        );

    print_diagnostic(invalid_start_result);


    // TEST 4: INVALID LENGTH
    std::cout << "\nTesting invalid length...\n";

    auto invalid_length = encoded;
    invalid_length[2] = 0x20;

    uartdiag::Frame invalid_length_frame{};

    auto invalid_length_result =
        decoder.decode(
            invalid_length,
            invalid_length_frame
        );

    print_diagnostic(invalid_length_result);

    return 0;
}