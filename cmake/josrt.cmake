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


#############################################################################
# NASM 
enable_language(ASM_NASM)
if(CMAKE_ASM_NASM_COMPILER_LOADED)
  set(CMAKE_ASM_NASM_SOURCE_FILE_EXTENSIONS "asm;nasm;S")
  set(CAN_USE_ASSEMBLER TRUE)
  #NOTE: all of the below is needed to force CMake+NASM to output win64 COFF object files.
  #set(CMAKE_ASM_NASM_OBJECT_FORMAT win64)  
  set(CMAKE_ASM_NASM_COMPILE_OBJECT "<CMAKE_ASM_NASM_COMPILER> -f ${CMAKE_ASM_NASM_OBJECT_FORMAT} -o <OBJECT> <SOURCE>")
endif(CMAKE_ASM_NASM_COMPILER_LOADED)

set(CMAKE_SKIP_RPATH ON)
set(BUILD_SHARED_LIBRARIES OFF)
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -static")
set(ISYSROOT ${CMAKE_SOURCE_DIR}/include)

set(CMAKE_CROSSCOMPILING TRUE)
set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_PREFIX_PATH  "${ISYSROOT}")
set(COMPILER_CODEGEN_FLAGS -m64 ${WARNS})

set(LINKER_FLAGS 
      -nostdlib
      -target ${TRIPLE}
  )

# configure active compiler
if(CMAKE_C_COMPILER_ID STREQUAL "Clang")
  set(COMPILER_FLAGS "${COMPILER_CODEGEN_FLAGS}" -target "${TRIPLE}" -nostdinc -ffreestanding -isystem "${ISYSROOT}")
else()
  message(FATAL_ERROR "unsupported compiler ${CMAKE_C_COMPILER_ID}")
endif()


add_library(josrt STATIC "")
target_compile_options(josrt PRIVATE "${COMPILER_FLAGS}")
target_include_directories(josrt PRIVATE
  "${CMAKE_SYSTEM_PREFIX_PATH}"
  ${jobase_SOURCE_DIR}
  )
add_subdirectory(${CMAKE_SOURCE_DIR}/src)

if (_JOSRT_ISPC)
  add_library(josrt_ispc STATIC "")
  add_subdirectory(src/ispc)
endif()

################################################# TESTING

function(josrt_add_executable NAME)
  set(ELF_TARGET_NAME ${NAME})
  add_executable(${ELF_TARGET_NAME} ${ARGN})
  target_include_directories(${ELF_TARGET_NAME} PRIVATE
  "${CMAKE_SYSTEM_PREFIX_PATH}"
  ${jobase_SOURCE_DIR}
  )
  
  target_link_libraries(${ELF_TARGET_NAME} PRIVATE josrt crt josrt_ispc)
  target_compile_options(${ELF_TARGET_NAME} PRIVATE ${COMPILER_FLAGS} -g)
  target_link_options(${ELF_TARGET_NAME} PRIVATE "${LINKER_FLAGS}")
endfunction()
