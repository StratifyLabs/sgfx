cmake_minimum_required (VERSION 3.6)


#Add sources to the project
set(SOURCES_PREFIX ${CMAKE_SOURCE_DIR}/src)
add_subdirectory(src)
list(APPEND SOS_LIB_SOURCELIST ${SOURCES})

if( ${CMAKE_HOST_SYSTEM_NAME} STREQUAL "Darwin" )
  set(SOS_TOOLCHAIN_CMAKE_PATH /Applications/StratifyLabs-SDK/Tools/gcc/arm-none-eabi/cmake)
endif()
if( ${CMAKE_HOST_SYSTEM_NAME} STREQUAL "Windows" )
  set(SOS_TOOLCHAIN_CMAKE_PATH C:/StratifyLabs-SDK/Tools/gcc/arm-none-eabi/lib/ldscripts)
endif()
set(CMAKE_TOOLCHAIN_FILE ${SOS_TOOLCHAIN_CMAKE_PATH}/sos-lib-toolchain.cmake)
get_filename_component(SOS_LIB_NAME ${CMAKE_SOURCE_DIR} NAME)
project(${SOS_LIB_NAME} CXX C)

install(DIRECTORY include/ DESTINATION include/sapi)

set(SOS_LIB_INCLUDE_DIRECTORIES include)

set(SOS_LIB_TYPE release)

# ARMv7-m ----------------------------------------------
set(SOS_LIB_ARCH armv7-m)
set(SOS_LIB_TYPE release)
set(SOS_LIB_OPTION kernel_1bpp)
set(SOS_LIB_DEFINITIONS SG_BITS_PER_PIXEL=1)
include(${SOS_TOOLCHAIN_CMAKE_PATH}/sos-lib.cmake)

set(SOS_LIB_OPTION kernel_2bpp)
set(SOS_LIB_DEFINITIONS SG_BITS_PER_PIXEL=2)
include(${SOS_TOOLCHAIN_CMAKE_PATH}/sos-lib.cmake)

set(SOS_LIB_OPTION kernel_4bpp)
set(SOS_LIB_DEFINITIONS SG_BITS_PER_PIXEL=4)
include(${SOS_TOOLCHAIN_CMAKE_PATH}/sos-lib.cmake)

set(SOS_LIB_OPTION kernel_8bpp)
set(SOS_LIB_DEFINITIONS SG_BITS_PER_PIXEL=8)
include(${SOS_TOOLCHAIN_CMAKE_PATH}/sos-lib.cmake)

# ARMv7e-m (FPU) ----------------------------------------------
set(SOS_LIB_ARCH armv7e-m)

set(SOS_LIB_OPTION kernel_1bpp)
set(SOS_LIB_DEFINITIONS SG_BITS_PER_PIXEL=1)
include(${SOS_TOOLCHAIN_CMAKE_PATH}/sos-lib.cmake)

set(SOS_LIB_OPTION kernel_2bpp)
set(SOS_LIB_DEFINITIONS SG_BITS_PER_PIXEL=2)
include(${SOS_TOOLCHAIN_CMAKE_PATH}/sos-lib.cmake)

set(SOS_LIB_OPTION kernel_4bpp)
set(SOS_LIB_DEFINITIONS SG_BITS_PER_PIXEL=4)
include(${SOS_TOOLCHAIN_CMAKE_PATH}/sos-lib.cmake)

set(SOS_LIB_OPTION kernel_8bpp)
set(SOS_LIB_DEFINITIONS SG_BITS_PER_PIXEL=8)
include(${SOS_TOOLCHAIN_CMAKE_PATH}/sos-lib.cmake)
