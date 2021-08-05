cmake_minimum_required(VERSION 3.17)
project(rlpbr)

set(CMAKE_C_STANDARD 11)

if (CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)
    add_subdirectory(raylib)
else()
    target_compile_definitions(rlpbr BUNDLE_SHADERS)
endif ()

add_library(rlpbr rlpbr.c rlpbr.h shaders.h)
target_include_directories(rlpbr PUBLIC .)
target_link_libraries(rlpbr raylib)

add_executable(test test.c)
target_link_libraries(test rlpbr)

if (NOT CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)
    target_compile_definitions(rlpbr PRIVATE BUNDLE_SHADERS)
endif ()
