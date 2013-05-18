cmake_minimum_required (VERSION 2.6)

project (SimpleBrainfuck)

SET(CMAKE_CXX_FLAGS "-g -Wall -Wextra -pedantic -std=c++0x")
SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)

file(GLOB_RECURSE sources_files src/*)

add_executable(sbf ${sources_files})

