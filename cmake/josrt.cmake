set(CMAKE_C_STANDARD 17)

include(FetchContent)
FetchContent_Declare(joBase
    GIT_REPOSITORY https://github.com/jarlostensen/joBase
    GIT_TAG 1.0.0-rc1
    CONFIGURE_COMMAND ""
    BUILD_COMMAND ""
)
FetchContent_MakeAvailable(jobase)

# the only one supported for now
# TODO: this is now set where this is being used set(ARCH "x86_64")
if (NOT DEFINED ARCH)
  set(ARCH "x86_64" CACHE STRING "Architecture")
endif()

add_definitions(-DARCH_${ARCH})
add_definitions(-DARCH="${ARCH}")
set(ELF ${ARCH})

message(STATUS "Target CPU ${ARCH}")
set(TRIPLE "${ARCH}-pc-linux-elf")
set(CMAKE_C_COMPILER_TARGET ${TRIPLE})
message(STATUS "Target triple ${TRIPLE}")
set(CMAKE_SKIP_RPATH ON)
set(BUILD_SHARED_LIBRARIES OFF)
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -static")
set(CAPABS "${CAPABS} -g -fstack-protector-strong -ffunction-sections -fdata-sections -nostdlib -nostdinc")
set(WARNS "-Wall -Wextra")

set(ISYSROOT ${CMAKE_SOURCE_DIR}/include)

set(CMAKE_CROSSCOMPILING TRUE)
set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_PREFIX_PATH  "${ISYSROOT}")
# C compiler flags
# 64 bit
# large memory model to allow high-RAM relocation (if we want)
# 16 bit wchar
# no red zone in order to safely support kernels
set(COMPILER_CODEGEN_FLAGS "-m64 -mcmodel=large -fshort-wchar -mno-red-zone -fno-builtin-setjmp -fno-builtin-longjmp -fomit-frame-pointer -fno-delete-null-pointer-checks -fno-common -fno-zero-initialized-in-bss -fno-exceptions -fno-stack-protector -fno-stack-check -fno-strict-aliasing -fno-merge-all-constants --std=c11 -Wall")

# configure active compiler
if(CMAKE_C_COMPILER_ID STREQUAL "Clang")
  set(COMPILER_FLAGS "${COMPILER_CODEGEN_FLAGS} -target ${TRIPLE} -ffreestanding -isystem=${ISYSROOT}")  
else()
  message(FATAL_ERROR "unsupported compiler ${CMAKE_C_COMPILER_ID}")
endif()

add_library(josrt STATIC "")
target_include_directories(josrt PRIVATE
  "${CMAKE_SYSTEM_PREFIX_PATH}"
  ${jobase_SOURCE_DIR}
  )
add_subdirectory(${CMAKE_SOURCE_DIR}/src)

function(ik_add_executable NAME)
  set(ELF_TARGET_NAME ${NAME})
  add_executable(${ELF_TARGET_NAME} ${ARGN})
  target_include_directories(${ELF_TARGET_NAME} PRIVATE
  "${CMAKE_SYSTEM_PREFIX_PATH}"
  )
  
  target_link_libraries(${ELF_TARGET_NAME} PRIVATE josrt)
  target_compile_options(${ELF_TARGET_NAME} PRIVATE -Wall -Wextra -fstack-protector -nostdlib -nostdinc)
  target_compile_options(${ELF_TARGET_NAME} PRIVATE -ffunction-sections -fdata-sections)  
endfunction()
