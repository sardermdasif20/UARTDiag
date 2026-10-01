#pragma once

#include <cstddef>
#include <cstdint>
#include <vector>

namespace uartdiag {

class FrameStreamParser {
public:
    /*
     * Maximum number of bytes allowed to remain buffered.
     *
     * Largest valid frame:
     *
     * START + TYPE + LENGTH + 255-byte PAYLOAD + CRC
     *
     * = 259 bytes.
     */
    static constexpr std::size_t MAX_BUFFER_SIZE = 1024;

    void push(
        const std::vector<std::uint8_t>& data
    );

    bool next_frame(
        std::vector<std::uint8_t>& frame
    );

    void clear();

    std::size_t buffered_bytes() const;

private:
    void enforce_buffer_limit();

    std::vector<std::uint8_t> buffer_;
};

} // namespace uartdiag