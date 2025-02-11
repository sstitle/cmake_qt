# Define the target to build the C++ project using CMake
build-cpp:
	cmake -S . -B build
	cmake --build build

run-hello-gl: build-cpp
	./build/hello_gl/hello_gl

run-ascii-play: build-cpp
	./build/ascii_play/ascii_play

run-tic-tac-toe: build-cpp
	./build/tic_tac_toe/tic_tac_toe

run-cube-gl: build-cpp
	./build/cube_gl/cube_gl

run-snake: build-cpp
	./build/snake/snake
