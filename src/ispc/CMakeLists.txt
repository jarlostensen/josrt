
enable_language(ISPC)

set (TARGET_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/memcpy.ispc
)

set( ISPC_ARCH "x86-64")
set (ISPC_FLAGS "-O2 --pic")
set (ISPC_IA_TARGETS "sse4")
set(ISPC_TARGETS ${ISPC_IA_TARGETS})
#TODO: this doesn't work
set(CMAKE_ISPC_FLAGS "${ISPC_FLAGS} --target=${ISPC_TARGETS} --arch=${ISPC_ARCH}")

target_sources(josrt_ispc PRIVATE ${TARGET_SOURCES})
