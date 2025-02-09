cmake_minimum_required(VERSION 3.15)
project(MyApp VERSION 1.0 LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)


find_package(Qt6 REQUIRED COMPONENTS Widgets OpenGLWidgets)
qt6_add_resources(RESOURCES resources.qrc)
add_executable(MyApp main.cpp ${RESOURCES})
target_link_libraries(MyApp Qt6::Widgets Qt6::OpenGLWidgets)
