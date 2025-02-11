find_package(Qt6 REQUIRED COMPONENTS Widgets OpenGLWidgets)
add_executable(snake main.cpp src/SnakeState.cpp src/SnakeWidget.cpp ${RESOURCES})
target_link_libraries(snake Qt6::Widgets Qt6::OpenGLWidgets)
target_include_directories(snake PRIVATE inc)
