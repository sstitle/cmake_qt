find_package(Qt6 REQUIRED COMPONENTS Widgets OpenGLWidgets)
add_executable(tic_tac_toe main.cpp ${RESOURCES})
target_link_libraries(tic_tac_toe Qt6::Widgets Qt6::OpenGLWidgets)
