cmake_minimum_required(VERSION 3.10.0)
project(SymChro VERSION 0.1.0 LANGUAGES C CXX)

include_directories(src)

add_library(libSymChro src/symchro.cpp)
add_executable(SymChro main.cpp src/symchro.cpp)