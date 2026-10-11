cmake_minimum_required(VERSION 3.15.0)
project(SymChro VERSION 0.1.0 LANGUAGES C CXX)

include_directories(src)

set(PYBIND11_FINDPYTHON ON)
find_package(pybind11 CONFIG REQUIRED)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_library(SymChro  src/symchro.cpp)

add_executable(SymChroMain main.cpp)

target_link_libraries(SymChroMain PUBLIC SymChro )

pybind11_add_module(pysymchro src/bindings.cpp src/symchro.cpp)

install(TARGETS pysymchro DESTINATION ${CMAKE_INSTALL_PREFIX})