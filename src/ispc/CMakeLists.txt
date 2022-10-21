
enable_language(ISPC)

set (TARGET_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/memcpy.ispc
)

target_compile_options(josrt_ispc PRIVATE -O2 --pic --target=sse4-i32x4 --arch=x86-64)
target_sources(josrt_ispc PRIVATE ${TARGET_SOURCES})
