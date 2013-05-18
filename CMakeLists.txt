cmake_minimum_required (VERSION 2.6)

project (SimpleBrainfuck)

SET(CMAKE_CXX_FLAGS "-g -Wall -Wextra -pedantic -std=c++0x")
SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)

add_executable(sbf main.cpp BfParser.cpp BfInterpreter.cpp BfCommands.cpp)

