cmake_minimum_required(VERSION 3.20)

project(UARTDiag
    VERSION 1.0.0
    DESCRIPTION "UART protocol diagnostics and fault analysis tool"
    LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_executable(uartdiag
    src/main.cpp
    src/frame.cpp
    src/crc.cpp
    src/decoder.cpp
    src/diagnostics.cpp
)

target_include_directories(uartdiag
    PRIVATE
        ${PROJECT_SOURCE_DIR}/include
)

if (MSVC)
    target_compile_options(uartdiag PRIVATE /W4)
else()
    target_compile_options(uartdiag PRIVATE
        -Wall
        -Wextra
        -Wpedantic
    )
endif()