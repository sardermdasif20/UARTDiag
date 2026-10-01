cmake_minimum_required(VERSION 3.20)

project(UARTDiag
    VERSION 1.0.0
    DESCRIPTION "UART protocol diagnostics and fault analysis tool"
    LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_library(uartdiag_lib
    src/frame.cpp
    src/crc.cpp
    src/decoder.cpp
    src/diagnostics.cpp
    src/hex.cpp
)

target_include_directories(uartdiag_lib
    PUBLIC
        ${PROJECT_SOURCE_DIR}/include
)

if (MSVC)
    target_compile_options(uartdiag_lib PRIVATE /W4)
else()
    target_compile_options(uartdiag_lib PRIVATE
        -Wall
        -Wextra
        -Wpedantic
    )
endif()

add_executable(uartdiag
    src/main.cpp
)

target_link_libraries(uartdiag
    PRIVATE uartdiag_lib
)

enable_testing()

include(FetchContent)

FetchContent_Declare(
    googletest
    URL https://github.com/google/googletest/archive/refs/tags/v1.15.2.zip
    DOWNLOAD_EXTRACT_TIMESTAMP TRUE
)

FetchContent_MakeAvailable(googletest)

add_executable(uartdiag_tests
    tests/test_frame.cpp
)

target_link_libraries(uartdiag_tests
    PRIVATE
        uartdiag_lib
        GTest::gtest_main
)

include(GoogleTest)

gtest_discover_tests(uartdiag_tests)