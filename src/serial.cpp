#include "uartdiag/serial.hpp"

#include <windows.h>

#include <sstream>

namespace uartdiag {

namespace {

DWORD baud_rate_to_constant(
    unsigned int baud_rate
) {
    switch (baud_rate) {

        case 9600:
            return CBR_9600;

        case 19200:
            return CBR_19200;

        case 38400:
            return CBR_38400;

        case 57600:
            return CBR_57600;

        case 115200:
            return CBR_115200;

        default:
            return 0;
    }
}

std::string windows_error_message(
    const char* operation
) {
    const DWORD error_code = GetLastError();

    std::ostringstream message;

    message
        << operation
        << " failed. Windows error code: "
        << error_code;

    return message.str();
}

} // namespace

SerialPort::~SerialPort() {
    close();
}

bool SerialPort::open(
    const SerialConfig& config,
    std::string& error
) {
    close();

    const DWORD baud_rate =
        baud_rate_to_constant(config.baud_rate);

    if (baud_rate == 0) {
        error =
            "Unsupported baud rate: " +
            std::to_string(config.baud_rate);

        return false;
    }

    if (config.port.empty()) {
        error = "Serial port name cannot be empty.";
        return false;
    }

    std::string port_name = config.port;

    /*
     * Windows requires the special device namespace for
     * COM ports above COM9. It is harmless and useful for
     * ordinary ports as well.
     */
    if (
        port_name.rfind("\\\\.\\", 0) != 0
    ) {
        port_name = "\\\\.\\" + port_name;
    }

    HANDLE handle = CreateFileA(
        port_name.c_str(),
        GENERIC_READ | GENERIC_WRITE,
        0,
        nullptr,
        OPEN_EXISTING,
        0,
        nullptr
    );

    if (handle == INVALID_HANDLE_VALUE) {
        error = windows_error_message(
            "Opening serial port"
        );

        return false;
    }

    DCB dcb{};

    dcb.DCBlength = sizeof(DCB);

    if (!GetCommState(handle, &dcb)) {
        error = windows_error_message(
            "Reading serial port configuration"
        );

        CloseHandle(handle);

        return false;
    }

    /*
     * Configure standard UART 8N1:
     *
     * 8 data bits
     * no parity
     * 1 stop bit
     */
    dcb.BaudRate = baud_rate;
    dcb.ByteSize = 8;
    dcb.StopBits = ONESTOPBIT;
    dcb.Parity = NOPARITY;

    dcb.fBinary = TRUE;
    dcb.fParity = FALSE;

    /*
     * Disable hardware/software flow control.
     *
     * UARTDiag currently expects a simple raw UART stream.
     */
    dcb.fOutxCtsFlow = FALSE;
    dcb.fOutxDsrFlow = FALSE;
    dcb.fDtrControl = DTR_CONTROL_DISABLE;
    dcb.fDsrSensitivity = FALSE;
    dcb.fRtsControl = RTS_CONTROL_DISABLE;
    dcb.fOutX = FALSE;
    dcb.fInX = FALSE;

    if (!SetCommState(handle, &dcb)) {
        error = windows_error_message(
            "Configuring serial port"
        );

        CloseHandle(handle);

        return false;
    }

    COMMTIMEOUTS timeouts{};

    /*
     * ReadFile returns periodically instead of blocking
     * indefinitely. This is important because the monitor
     * needs to respond to Ctrl+C.
     */
    timeouts.ReadIntervalTimeout = 50;
    timeouts.ReadTotalTimeoutConstant = 100;
    timeouts.ReadTotalTimeoutMultiplier = 10;

    timeouts.WriteTotalTimeoutConstant = 100;
    timeouts.WriteTotalTimeoutMultiplier = 10;

    if (!SetCommTimeouts(handle, &timeouts)) {
        error = windows_error_message(
            "Configuring serial port timeouts"
        );

        CloseHandle(handle);

        return false;
    }

    /*
     * Clear stale data that may already exist in the
     * Windows driver buffers.
     */
    if (!PurgeComm(
            handle,
            PURGE_RXCLEAR |
            PURGE_TXCLEAR
        )) {

        error = windows_error_message(
            "Clearing serial port buffers"
        );

        CloseHandle(handle);

        return false;
    }

    handle_ = handle;

    return true;
}

void SerialPort::close() {

    if (handle_ != nullptr) {

        HANDLE handle =
            static_cast<HANDLE>(handle_);

        CloseHandle(handle);

        handle_ = nullptr;
    }
}

bool SerialPort::is_open() const {
    return handle_ != nullptr;
}

bool SerialPort::read(
    std::vector<std::uint8_t>& data,
    std::size_t max_bytes,
    std::string& error
) {
    data.clear();

    if (!is_open()) {
        error = "Serial port is not open.";
        return false;
    }

    if (max_bytes == 0) {
        return true;
    }

    if (
        max_bytes >
        static_cast<std::size_t>(MAXDWORD)
    ) {
        error = "Requested read size is too large.";
        return false;
    }

    data.resize(max_bytes);

    DWORD bytes_read = 0;

    HANDLE handle =
        static_cast<HANDLE>(handle_);

    if (!ReadFile(
            handle,
            data.data(),
            static_cast<DWORD>(max_bytes),
            &bytes_read,
            nullptr
        )) {

        error = windows_error_message(
            "Reading from serial port"
        );

        data.clear();

        return false;
    }

    data.resize(bytes_read);

    return true;
}

} // namespace uartdiag