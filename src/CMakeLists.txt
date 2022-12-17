
target_sources(josrt PRIVATE    
    "${CMAKE_CURRENT_SOURCE_DIR}/string/strlen.c"    
    "${CMAKE_CURRENT_SOURCE_DIR}/string/strcmp.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/stdio/print.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/extensions/slices.c"  
    "${CMAKE_CURRENT_SOURCE_DIR}/extensions/base64.c"  
    "${CMAKE_CURRENT_SOURCE_DIR}/extensions/hex_dump.c"  
    "${CMAKE_CURRENT_SOURCE_DIR}/internal/_vprint.c"    
    "${CMAKE_CURRENT_SOURCE_DIR}/internal/_wchar.c"    
    "${CMAKE_CURRENT_SOURCE_DIR}/math/pow.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/math/pow_data.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/math/exp_data.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/math/exp2f_data.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/math/ceil.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/math/floor.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/math/fabs.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/math/fabsf.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/stdlib/rand.c"    
    "${CMAKE_CURRENT_SOURCE_DIR}/internal/_toupper.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/internal/_tolower.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/internal/_isupper.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/internal/_islower.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/crt/libc_main.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/string/memcmp.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/string/memcpy.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/string/memmove.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/string/memset.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/assert.c"
)

target_include_directories(josrt PRIVATE 
"${CMAKE_CURRENT_SOURCE_DIR}/internal/include"
"${CMAKE_CURRENT_SOURCE_DIR}/include"
"${CMAKE_SYSTEM_PREFIX_PATH}"
"${CMAKE_SYSTEM_PREFIX_PATH}/toolchain/llvmintrin"
${jobase_SOURCE_DIR}
)


add_library(crt STATIC 
    "${CMAKE_CURRENT_SOURCE_DIR}/crt/crt0.asm"
    "${CMAKE_CURRENT_SOURCE_DIR}/crt/libc_main.c"
)

target_include_directories(crt PRIVATE 
"${CMAKE_CURRENT_SOURCE_DIR}/internal/include"
"${CMAKE_SOURCE_DIR}/include"
${jobase_SOURCE_DIR}
)
