#include "uartdiag/stream.hpp"
#include "uartdiag/frame.hpp"

#include <algorithm>
#include <cstddef>

namespace uartdiag {

void FrameStreamParser::push(
    const std::vector<std::uint8_t>& data
) {
    if (data.empty()) {
        return;
    }

    buffer_.insert(
        buffer_.end(),
        data.begin(),
        data.end()
    );

    /*
     * Prevent unbounded growth when the UART stream
     * contains continuous noise.
     */
    if (buffer_.size() > MAX_BUFFER_SIZE) {

        buffer_.erase(
            buffer_.begin(),
            buffer_.end() -
                static_cast<std::ptrdiff_t>(
                    MAX_BUFFER_SIZE
                )
        );
    }

    /*
     * Remove noise before the first possible frame start.
     */
    const auto start =
        std::find(
            buffer_.begin(),
            buffer_.end(),
            FRAME_START
        );

    if (
        start != buffer_.begin() &&
        start != buffer_.end()
    ) {

        buffer_.erase(
            buffer_.begin(),
            start
        );
    }
}

bool FrameStreamParser::next_frame(
    std::vector<std::uint8_t>& frame
) {
    frame.clear();

    while (true) {

        /*
         * Find the next possible frame start byte.
         */
        const auto start =
            std::find(
                buffer_.begin(),
                buffer_.end(),
                FRAME_START
            );

        /*
         * No START byte means the buffer contains
         * only noise.
         */
        if (start == buffer_.end()) {

            buffer_.clear();

            return false;
        }

        /*
         * Discard bytes before START.
         */
        if (start != buffer_.begin()) {

            buffer_.erase(
                buffer_.begin(),
                start
            );
        }

        /*
         * Need START + TYPE before we can validate
         * the candidate frame.
         */
        if (buffer_.size() < 2) {
            return false;
        }

        /*
         * Validate the frame type before trusting the
         * payload length.
         *
         * This is important for stream resynchronization.
         *
         * Example:
         *
         * AA 55 66 ...
         *
         * 55 is not a valid frame type, so this AA
         * cannot be the beginning of a valid frame.
         */
        const auto type =
            static_cast<FrameType>(
                buffer_[1]
            );

        if (
            type != FrameType::SensorData &&
            type != FrameType::Command &&
            type != FrameType::Response
        ) {

            /*
             * Discard only the invalid START byte.
             *
             * There may be another AA later in the
             * buffer that is the real frame start.
             */
            buffer_.erase(
                buffer_.begin()
            );

            continue;
        }

        /*
         * Need START + TYPE + LENGTH before we can
         * determine the complete frame size.
         */
        if (buffer_.size() < 3) {
            return false;
        }

        const std::size_t payload_length =
            static_cast<std::size_t>(
                buffer_[2]
            );

        /*
         * Frame layout:
         *
         * START
         * TYPE
         * LENGTH
         * PAYLOAD
         * CRC
         *
         * Total = payload_length + 4
         */
        const std::size_t frame_length =
            payload_length + 4;

        /*
         * Wait for the rest of the frame if the
         * current serial read contains only part of it.
         */
        if (buffer_.size() < frame_length) {
            return false;
        }

        /*
         * Extract complete candidate frame.
         */
        frame.assign(
            buffer_.begin(),
            buffer_.begin() +
                static_cast<std::ptrdiff_t>(
                    frame_length
                )
        );

        /*
         * Remove the extracted frame from the buffer.
         */
        buffer_.erase(
            buffer_.begin(),
            buffer_.begin() +
                static_cast<std::ptrdiff_t>(
                    frame_length
                )
        );

        return true;
    }
}

void FrameStreamParser::clear() {
    buffer_.clear();
}

std::size_t FrameStreamParser::buffered_bytes() const {
    return buffer_.size();
}

} // namespace uartdiag