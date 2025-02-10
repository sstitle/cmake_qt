# Define the target to build the C++ project using CMake and Ninja
build-cpp:
	cmake -S . -B build
	cmake --build build

# Define the target to run the hello_gl application
run-hello-gl: build-cpp
	./build/hello_gl/hello_gl

# Define the target to run the ascii_play application
run-ascii-play: build-cpp
	./build/ascii_play/ascii_play

# Define the target to run the tic_tac_toe application
run-tic-tac-toe: build-cpp
	./build/tic_tac_toe/tic_tac_toe
