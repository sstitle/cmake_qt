find_package(Qt6 REQUIRED COMPONENTS Widgets OpenGLWidgets)
qt6_add_resources(RESOURCES resources.qrc)
add_executable(hello_gl main.cpp ${RESOURCES})
target_link_libraries(hello_gl Qt6::Widgets Qt6::OpenGLWidgets)
