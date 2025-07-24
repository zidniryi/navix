cmake_minimum_required(VERSION 3.10)
project(cpp_assist)

set(CMAKE_CXX_STANDARD 17)

file(GLOB SOURCES "src/*.cpp")
add_executable(cpp-assist ${SOURCES})
