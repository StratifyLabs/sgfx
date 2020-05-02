cmake_minimum_required (VERSION 3.6)

if( ${CMAKE_HOST_SYSTEM_NAME} STREQUAL "Darwin" )
  set(SOS_TOOLCHAIN_CMAKE_PATH /Applications/StratifyLabs-SDK/Tools/gcc/arm-none-eabi/cmake)
endif()
if( ${CMAKE_HOST_SYSTEM_NAME} STREQUAL "Windows" )
  set(SOS_TOOLCHAIN_CMAKE_PATH C:/StratifyLabs-SDK/Tools/gcc/arm-none-eabi/cmake)
endif()
if( ${CMAKE_HOST_SYSTEM_NAME} STREQUAL "Linux" )
	set(SOS_TOOLCHAIN_CMAKE_PATH /StratifyLabs-SDK/Tools/gcc/arm-none-eabi/cmake)
endif()
set(CMAKE_TOOLCHAIN_FILE ${SOS_TOOLCHAIN_CMAKE_PATH}/sos-lib-toolchain.cmake)
get_filename_component(SOS_NAME ${CMAKE_SOURCE_DIR} NAME)

include(${SOS_TOOLCHAIN_CMAKE_PATH}/sos-variables.cmake)

if( ${SOS_BUILD_CONFIG} STREQUAL arm )
	set(CMAKE_TOOLCHAIN_FILE ${SOS_TOOLCHAIN_CMAKE_PATH}/sos-lib-toolchain.cmake)
elseif( ${SOS_BUILD_CONFIG} STREQUAL link )
	set(CMAKE_TOOLCHAIN_FILE ${SOS_TOOLCHAIN_CMAKE_PATH}/link-toolchain.cmake)
endif()

set(SOS_NAME sgfx)
project(${SOS_NAME} CXX C)

install(DIRECTORY include/ DESTINATION include/sapi)
set(SOS_INCLUDE_DIRECTORIES include)


include( ${CMAKE_SOURCE_DIR}/${SOS_BUILD_CONFIG}.cmake )



