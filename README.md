# CMake Qt

Here are some graphics examples using CMake and Qt.

## Setup Dependencies

### Install Qt6

Follow the instructions to install a recent version of Qt6, I was using 6.7.3 at the time of writing.

https://doc.qt.io/qt-6/get-and-install-qt.html

### Install CMake

Follow the instructions to install CMake, a cross-platform tool for building C++ programs.

## Build

```bash
make build-cpp
```

## Run

There are a few executables you can run:

### Tic Tac Toe

Example C++ implementation of Tic-Tac-Toe using the state and reducer pattern. Inspired by the React tutorial here:

https://react.dev/learn/tutorial-tic-tac-toe

```bash
make run-tic-tac-toe
```

### Ascii Play

Simple ASCII art showing randomized colors and letters. Inspired by (but nowhere as good as): https://ertdfgcvb.xyz/

```bash
make run-ascii-play
```

### Hello GL

Based on code examples from:

https://duriansoftware.com/joe/an-intro-to-modern-opengl
https://github.com/jckarter/hello-gl

```bash
make run-hello-gl
```


### Cube GL

Simple rotating cube example:

```bash
make run-cube-gl
```
