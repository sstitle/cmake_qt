cmake_minimum_required(VERSION 3.15)
project(CmakeQt VERSION 1.0 LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_subdirectory(hello_gl)
add_subdirectory(ascii_play)
add_subdirectory(tic_tac_toe)
add_subdirectory(cube_gl)
