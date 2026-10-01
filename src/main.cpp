#include "uartdiag/decoder.hpp"
#include "uartdiag/frame.hpp"

#include <iomanip>
#include <iostream>

int main() {

    uartdiag::Frame original{
        uartdiag::FrameType::SensorData,
        {0x10, 0x20, 0x30, 0x40},
        0
    };

    // Encode the frame
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

    std::cout << "\n\n";

    // Decode the frame
    uartdiag::Decoder decoder;
    uartdiag::Frame decoded{};

    const bool valid =
        decoder.decode(encoded, decoded);

    std::cout
        << "Decode result: "
        << (valid ? "VALID" : "INVALID")
        << '\n';

    std::cout
        << "Payload bytes: "
        << std::dec
        << decoded.payload.size()
        << '\n';

    // Corrupt one payload byte
    encoded[4] ^= 0xFF;

    uartdiag::Frame corrupted{};

    const bool corrupted_result =
        decoder.decode(encoded, corrupted);

    std::cout
        << "Corrupted frame: "
        << (corrupted_result ? "VALID" : "REJECTED")
        << '\n';

    return 0;
}