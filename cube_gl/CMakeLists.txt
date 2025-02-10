find_package(Qt6 REQUIRED COMPONENTS Widgets OpenGLWidgets)
add_executable(cube_gl main.cpp ${RESOURCES})
target_link_libraries(cube_gl Qt6::Widgets Qt6::OpenGLWidgets)
