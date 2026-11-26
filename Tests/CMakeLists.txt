# Host unit tests for the pure-C parts of App/ (formatter, CRC-32, B0/B2
# frames, downlink parsing, SGM58031 driver). Built with the host C compiler,
# not with the firmware toolchain:
#
#   cmake --preset host        (from this directory)
#   cmake --build --preset host
#   ctest --preset host
#
# or simply: python Tools/host_tests.py  (from the repository root)
cmake_minimum_required(VERSION 3.22)
project(collector_host_tests C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

set(APP_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../App)

add_executable(collector_tests
    main.c
    test_crc32.c
    test_downlink.c
    test_fmt.c
    test_misc.c
    test_sgm58031.c
    test_uplink.c
    ${APP_DIR}/drivers/sgm58031.c
    ${APP_DIR}/protocol/crc32_sw.c
    ${APP_DIR}/protocol/downlink.c
    ${APP_DIR}/protocol/uplink.c
    ${APP_DIR}/util/fmt.c
)

target_include_directories(collector_tests PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${APP_DIR}/bsp
    ${APP_DIR}/drivers
    ${APP_DIR}/protocol
    ${APP_DIR}/util
)

if(MSVC)
    target_compile_options(collector_tests PRIVATE /W4 /WX)
else()
    target_compile_options(collector_tests PRIVATE -Wall -Wextra -Werror -Wno-unused-parameter)
endif()

enable_testing()
add_test(NAME unit COMMAND collector_tests)

# Optional: decode the frames with the 4G firmware's own parser.
#   -DQUECPYTHON_DIR=<path to a Collector-QuecPython checkout>
set(QUECPYTHON_DIR "" CACHE PATH "Collector-QuecPython checkout for the frame cross-check")
if(QUECPYTHON_DIR)
    find_package(Python3 COMPONENTS Interpreter REQUIRED)
    add_test(NAME crosscheck
        COMMAND ${Python3_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/../Tools/crosscheck_frames.py
            --tests $<TARGET_FILE:collector_tests>
            --quecpython ${QUECPYTHON_DIR}
            --work ${CMAKE_CURRENT_BINARY_DIR})
endif()
