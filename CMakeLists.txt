cmake_minimum_required(VERSION 3.18)

project (josrt C)

include(cmake/josrt.cmake)

# TESTING
josrt_add_executable(test_printf tests/test_printf.c tests/syscall_driver.asm)
