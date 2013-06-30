cmake_minimum_required (VERSION 2.6)

project (SimpleBrainfuck)

SET(CMAKE_CXX_FLAGS "-O3 -s -march=native -Wall -Wextra -pedantic -std=c++0x")
SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)
SET(CMAKE_EXPORT_COMPILE_COMMANDS "ON")

file(GLOB_RECURSE sources_files src/*)

add_executable(sbf ${sources_files})

