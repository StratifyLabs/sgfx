

#Add sources to the project
set(SOURCES_PREFIX ${CMAKE_SOURCE_DIR}/src)
add_subdirectory(src)
file(GLOB_RECURSE HEADER_SOURCES ${CMAKE_SOURCE_DIR}/include/*)
list(APPEND SOS_SOURCELIST ${SOURCES} ${HEADER_SOURCES})

set(SOS_CONFIG release)
set(SOS_OPTION kernel)
set(SOS_DEFINITIONS SG_BITS_PER_PIXEL=0)
include(${SOS_TOOLCHAIN_CMAKE_PATH}/sos-lib-std.cmake)

set(SOS_CONFIG release)
set(SOS_OPTION kernel_1bpp)
set(SOS_DEFINITIONS SG_BITS_PER_PIXEL=1)
include(${SOS_TOOLCHAIN_CMAKE_PATH}/sos-lib-std.cmake)

set(SOS_OPTION kernel_2bpp)
set(SOS_DEFINITIONS SG_BITS_PER_PIXEL=2)
include(${SOS_TOOLCHAIN_CMAKE_PATH}/sos-lib-std.cmake)

set(SOS_OPTION kernel_4bpp)
set(SOS_DEFINITIONS SG_BITS_PER_PIXEL=4)
include(${SOS_TOOLCHAIN_CMAKE_PATH}/sos-lib-std.cmake)

set(SOS_OPTION kernel_8bpp)
set(SOS_DEFINITIONS SG_BITS_PER_PIXEL=8)
include(${SOS_TOOLCHAIN_CMAKE_PATH}/sos-lib-std.cmake)

set(SOS_OPTION kernel_16bpp)
set(SOS_DEFINITIONS SG_BITS_PER_PIXEL=16)
include(${SOS_TOOLCHAIN_CMAKE_PATH}/sos-lib-std.cmake)
