#include "uartdiag/hex.hpp"

#include <sstream>
#include <stdexcept>

namespace uartdiag {

std::vector<std::uint8_t> parse_hex(
    const std::string& input
) {
    std::vector<std::uint8_t> bytes;

    std::istringstream stream(input);
    std::string token;

    while (stream >> token) {

        if (token.size() != 2) {
            throw std::invalid_argument(
                "Each byte must contain exactly two hexadecimal digits."
            );
        }

        try {
            const auto value =
                std::stoul(token, nullptr, 16);

            if (value > 0xFF) {
                throw std::invalid_argument(
                    "Hex byte is out of range."
                );
            }

            bytes.push_back(
                static_cast<std::uint8_t>(value)
            );

        } catch (const std::exception&) {
            throw std::invalid_argument(
                "Invalid hexadecimal byte: " + token
            );
        }
    }

    if (bytes.empty()) {
        throw std::invalid_argument(
            "No hexadecimal bytes were provided."
        );
    }

    return bytes;
}

} // namespace uartdiag