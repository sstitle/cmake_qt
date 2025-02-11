find_package(Qt6 REQUIRED COMPONENTS Widgets OpenGLWidgets)
add_executable(snake main.cpp ${RESOURCES})
target_link_libraries(snake Qt6::Widgets Qt6::OpenGLWidgets)
