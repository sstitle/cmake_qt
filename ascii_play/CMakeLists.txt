find_package(Qt6 REQUIRED COMPONENTS Widgets OpenGLWidgets)
add_executable(ascii_play main.cpp ${RESOURCES})
target_link_libraries(ascii_play Qt6::Widgets Qt6::OpenGLWidgets)
