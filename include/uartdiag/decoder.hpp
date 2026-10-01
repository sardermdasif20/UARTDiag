#pragma once

#include "uartdiag/diagnostics.hpp"
#include "uartdiag/frame.hpp"

#include <cstdint>
#include <vector>

namespace uartdiag {

class Decoder {
public:

    DiagnosticResult decode(
        const std::vector<std::uint8_t>& raw_data,
        Frame& frame
    );
};

} // namespace uartdiag