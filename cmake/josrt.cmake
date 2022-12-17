cmake_minimum_required(VERSION 3.22)
set(CMAKE_C_STANDARD 17)

include(FetchContent)
FetchContent_Declare(joBase
    GIT_REPOSITORY https://github.com/jarlostensen/joBase
    GIT_TAG 1.0.0-rc2
    CONFIGURE_COMMAND ""
    BUILD_COMMAND ""
)
FetchContent_MakeAvailable(jobase)

# the only one supported for now
# TODO: this is now set where this is being used set(ARCH "x86_64")
if (NOT DEFINED ARCH)
  set(ARCH "x86_64" CACHE STRING "Architecture")
endif()

add_definitions(-DARCH=${ARCH})
set(ELF ${ARCH})

message(STATUS "Target CPU ${ARCH}")
set(TRIPLE "${ARCH}-pc-linux-elf")
set(CMAKE_C_COMPILER_TARGET ${TRIPLE})
message(STATUS "Target triple ${TRIPLE}")


#############################################################################
# NASM 
enable_language(ASM_NASM)
if(CMAKE_ASM_NASM_COMPILER_LOADED)
  set(CMAKE_ASM_NASM_SOURCE_FILE_EXTENSIONS "asm;nasm;S")
  set(CAN_USE_ASSEMBLER TRUE)
  set(CMAKE_ASM_NASM_OBJECT_FORMAT elf64)
  set(CMAKE_ASM_NASM_COMPILE_OBJECT "<CMAKE_ASM_NASM_COMPILER> -f ${CMAKE_ASM_NASM_OBJECT_FORMAT} -o <OBJECT> <SOURCE>")
endif(CMAKE_ASM_NASM_COMPILER_LOADED)

set(CMAKE_SKIP_RPATH ON)
set(BUILD_SHARED_LIBRARIES OFF)
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -static")
set(ISYSROOT ${CMAKE_SOURCE_DIR}/include)

set(CMAKE_CROSSCOMPILING TRUE)
set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_PREFIX_PATH  "${ISYSROOT}")
set(COMPILER_CODEGEN_FLAGS -m64 -march=native ${WARNS})

set(LINKER_FLAGS 
      -nostdlib
      -target ${TRIPLE}
  )

# configure active compiler
if(CMAKE_C_COMPILER_ID STREQUAL "Clang")
  set(COMPILER_FLAGS ${COMPILER_CODEGEN_FLAGS} -nostdinc -ffreestanding -isystem ${ISYSROOT})
else()
  set(COMPILER_FLAGS "")
  message(FATAL_ERROR "(currently) unsupported toolchain ${CMAKE_C_COMPILER_ID}")
endif()

message("-- Compiling using ${CMAKE_C_COMPILER_ID} with: ${COMPILER_FLAGS}")

add_library(josrt STATIC "")
target_compile_options(josrt PRIVATE 
  ${COMPILER_FLAGS}
  "$<IF:$<CONFIG:RELEASE>,-Ofast,-ggdb3>"
)
add_subdirectory(${CMAKE_SOURCE_DIR}/src)

# until we support the full whack we don't...
add_compile_definitions(__STDC_HOSTED__=0)

if (_JOSRT_ISPC)
  add_library(josrt_ispc STATIC "")
  add_subdirectory(src/ispc)
endif()

################################################# TESTING

function(josrt_add_elf NAME WITH_IO WITH_FILE)

  if(CMAKE_BUILD_TYPE MATCHES Debug)
    set(ELF_TARGET_NAME ${NAME}d)
  else()
    set(ELF_TARGET_NAME ${NAME})
  endif()

  #NOTE: when creating an executable we need the full start->libc_main loop and this one only supports ELF
  target_compile_definitions(crt PRIVATE _JOSRT_HOSTED_ELF=1)
  target_compile_definitions(josrt PRIVATE
    $<${WITH_IO}:_JOSRT_REQUIRES_IO=1>
    $<${WITH_FILE}:_JOSRT_REQUIRES_FILE=1>
  )
    
  add_executable(${ELF_TARGET_NAME} ${ARGN})
  target_include_directories(${ELF_TARGET_NAME} PRIVATE
  "${CMAKE_SYSTEM_PREFIX_PATH}"
  "${CMAKE_SYSTEM_PREFIX_PATH}/toolchain/llvmintrin"
  ${jobase_SOURCE_DIR}
  )
  
  if(_JOSRT_ISPC)
    target_link_libraries(${ELF_TARGET_NAME} PRIVATE josrt crt josrt_ispc)
  else()
    target_link_libraries(${ELF_TARGET_NAME} PRIVATE josrt crt)
  endif()
  target_compile_options(${ELF_TARGET_NAME} PRIVATE 
    ${COMPILER_FLAGS}
    "$<IF:$<CONFIG:RELEASE>,-Ofast,-ggdb3>"
    ) 
    target_compile_definitions(${ELF_TARGET_NAME} PRIVATE
    $<${WITH_IO}:_JOSRT_REQUIRES_IO=1>
    $<${WITH_FILE}:_JOSRT_REQUIRES_FILE=1>
  )
  target_link_options(${ELF_TARGET_NAME} PRIVATE "${LINKER_FLAGS}")  
endfunction()
