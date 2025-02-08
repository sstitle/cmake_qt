# Define the target to build the C++ project using CMake and Ninja
build-cpp:
	cmake -S . -B build
	cmake --build build

# Define the target to run the C++ application
run-cpp: build-cpp
	./build/MyApp
