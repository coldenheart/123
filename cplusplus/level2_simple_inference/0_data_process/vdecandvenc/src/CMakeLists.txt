# Copyright (c) Huawei Technologies Co., Ltd. 2019. All rights reserved.

# CMake lowest version requirement
cmake_minimum_required(VERSION 3.5.1)

# project information
project(classification)

# Compile options
add_compile_options(-std=c++11)


set(CMAKE_RUNTIME_OUTPUT_DIRECTORY  "../../../out")
set(CMAKE_CXX_FLAGS_DEBUG "-fPIC -O0 -g -Wall")
set(CMAKE_CXX_FLAGS_RELEASE "-fPIC -O2 -Wall")

add_definitions(-DENABLE_DVPP_INTERFACE)

file(GLOB_RECURSE COMMON_SRC_ALL "common/src/*.cpp")
list(APPEND COMMON_DEPEND_LIB avcodec avformat avdevice avutil swresample avfilter swscale)
if (${CMAKE_CXX_COMPILER} MATCHES "aarch")
    set(ARCH "arm")
    set(COMMON_SRC "${COMMON_SRC_ALL}")
    list(APPEND COMMON_DEPEND_LIB media_mini)
    add_definitions(-DENABLE_BOARD_CAMARE)
else ()
    set(ARCH "x86")
    string(REGEX REPLACE "common/src/camera.cpp" "" COMMON_SRC "${COMMON_SRC_ALL}")
endif()

set(INC_PATH $ENV{DDK_PATH})

if (NOT DEFINED ENV{DDK_PATH})
    set(INC_PATH "/usr/local/Ascend")
    message(STATUS "set default INC_PATH: ${INC_PATH}")
else ()
    message(STATUS "env INC_PATH: ${INC_PATH}")
endif()

set(LIB_PATH $ENV{DDK_PATH}/acllib/lib64/stub)

if (NOT DEFINED ENV{NPU_HOST_LIB})
    set(LIB_PATH "/usr/local/Ascend/acllib/lib64/stub/")
    message(STATUS "set default LIB_PATH: ${LIB_PATH}")
else ()
    message(STATUS "env LIB_PATH: ${LIB_PATH}")
endif()

# Header path
include_directories(
    ../inc/
    common/inc/
    $ENV{HOME}/ascend_ddk/${ARCH}/include/
    $ENV{HOME}/ascend_ddk/${ARCH}/include/ascenddk
    ${INC_PATH}/acllib/include/
    $ENV{HOME}/Ascend/driver
)

if(target STREQUAL "Simulator_Function")
    add_compile_options(-DFUNC_SIM)
endif()

# add host lib path
link_directories(
    /usr/local/lib/
    $ENV{NPU_HOST_LIB}
    $ENV{HOME}/ascend_ddk/${ARCH}/lib/
    ${INC_PATH}/atc/lib64
    ${INC_PATH}/aclib/lib64/stub/
    $ENV{HOME}/Ascend/
    $ENV{HOME}/Ascend/driver/
)

add_executable(main
        ${COMMON_SRC}
        utils.cpp
        video_encode.cpp
        venc_process.cpp
        object_detect.cpp
        main.cpp)

if(target STREQUAL "Simulator_Function")
    target_link_libraries(main funcsim)
else()
    target_link_libraries(main ascendcl acl_dvpp stdc++ pthread ${COMMON_DEPEND_LIB} c_sec slog dl rt)
endif()

install(TARGETS main DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
