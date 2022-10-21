cmake_minimum_required(VERSION 3.22)

if (_JOSRT_ISPC)
    set(ENABLED_LANGUAGES C ISPC)
elseif()
    set(ENABLED_LANGUAGES C)
endif()

project (josrt ${ENABLED_LANGUAGES})

include(cmake/josrt.cmake)

# TESTING
josrt_add_executable(test_printf tests/test_printf.c tests/syscall_driver.asm)
