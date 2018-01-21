cmake_minimum_required (VERSION 3.6)


#Add sources to the project
set(SOURCES_PREFIX ${CMAKE_SOURCE_DIR}/src CACHE INTERNAL "src prefix")
add_subdirectory(src)
list(APPEND SOS_LIB_SOURCELIST ${SOURCES})

if( ${CMAKE_HOST_SYSTEM_NAME} STREQUAL "Darwin" )
  set(SOS_TOOLCHAIN_CMAKE_PATH /Applications/StratifyLabs-SDK/Tools/gcc/arm-none-eabi/lib/ldscripts CACHE INTERNAL "sos toolchain path")
  set(SOS_TOOLCHAIN_CMAKE_PATH /Users/tgil/git/StratifyOS/cmake CACHE INTERNAL "sos toolchain path")
endif()
if( ${CMAKE_HOST_SYSTEM_NAME} STREQUAL "Windows" )
  set(SOS_TOOLCHAIN_CMAKE_PATH C:/StratifyLabs-SDK/Tools/gcc/arm-none-eabi/lib/ldscripts CACHE INTERNAL "sos toolchain path")
endif()
set(CMAKE_TOOLCHAIN_FILE ${SOS_TOOLCHAIN_CMAKE_PATH}/sos-lib-toolchain.cmake CACHE INTERNAL "sos toolchain file")
get_filename_component(SOS_LIB_NAME ${CMAKE_SOURCE_DIR} NAME)
project(${SOS_LIB_NAME} CXX C)

install(DIRECTORY include/ DESTINATION include/sapi)

set(SOS_LIB_INCLUDE_DIRECTORIES include CACHE INTERNAL "sos lib include directories")


set(SOS_LIB_ARCH armv7-m CACHE INTERNAL "sos build ")

set(SOS_LIB_CONFIG kernel_1bpp CACHE INTERNAL "sos build config release")
set(SOS_LIB_BUILD_FLAGS -DSG_BITS_PER_PIXEL=1 CACHE INTERNAL "sos lib build flags")
include(${SOS_TOOLCHAIN_CMAKE_PATH}/sos-lib.cmake)

set(SOS_LIB_CONFIG kernel_2bpp CACHE INTERNAL "sos build config release")
set(SOS_LIB_BUILD_FLAGS -DSG_BITS_PER_PIXEL=2 CACHE INTERNAL "sos lib build flags")
include(${SOS_TOOLCHAIN_CMAKE_PATH}/sos-lib.cmake)

set(SOS_LIB_CONFIG kernel_4bpp CACHE INTERNAL "sos build config release")
set(SOS_LIB_BUILD_FLAGS -DSG_BITS_PER_PIXEL=4 CACHE INTERNAL "sos lib build flags")
include(${SOS_TOOLCHAIN_CMAKE_PATH}/sos-lib.cmake)

set(SOS_LIB_CONFIG kernel_8bpp CACHE INTERNAL "sos build config release")
set(SOS_LIB_BUILD_FLAGS -DSG_BITS_PER_PIXEL=8 CACHE INTERNAL "sos lib build flags")
include(${SOS_TOOLCHAIN_CMAKE_PATH}/sos-lib.cmake)

set(SOS_LIB_ARCH armv7e-m CACHE INTERNAL "sos build ")

set(SOS_LIB_CONFIG kernel_1bpp CACHE INTERNAL "sos build config release")
set(SOS_LIB_BUILD_FLAGS -DSG_BITS_PER_PIXEL=1 CACHE INTERNAL "sos lib build flags")
include(${SOS_TOOLCHAIN_CMAKE_PATH}/sos-lib.cmake)

set(SOS_LIB_CONFIG kernel_2bpp CACHE INTERNAL "sos build config release")
set(SOS_LIB_BUILD_FLAGS -DSG_BITS_PER_PIXEL=2 CACHE INTERNAL "sos lib build flags")
include(${SOS_TOOLCHAIN_CMAKE_PATH}/sos-lib.cmake)

set(SOS_LIB_CONFIG kernel_4bpp CACHE INTERNAL "sos build config release")
set(SOS_LIB_BUILD_FLAGS -DSG_BITS_PER_PIXEL=4 CACHE INTERNAL "sos lib build flags")
include(${SOS_TOOLCHAIN_CMAKE_PATH}/sos-lib.cmake)

set(SOS_LIB_CONFIG kernel_8bpp CACHE INTERNAL "sos build config release")
set(SOS_LIB_BUILD_FLAGS -DSG_BITS_PER_PIXEL=8 CACHE INTERNAL "sos lib build flags")
include(${SOS_TOOLCHAIN_CMAKE_PATH}/sos-lib.cmake)

message( STATUS "DIR:" ${PROJECT_BINARY_DIR} )
