# gdk-input
Library for keyboard, mouse and gamepad input handling for games. Provides a null and glfw implemenation.

## Getting Started
Install cmake 3.21 or greater, and a c++20 compatible compiler 
```
# initialize the tree
git submodule update --init --recursive

# generate the script to build the demo programs on linux using gcc
cmake --preset linux-gcc-demo

# build the library and demo programs
cmake --build --preset linux-gcc-demo
```
See `CMakePresets.json` for the full list of supported platforms and compilers. See `.github/workflows/ci.yml` to see exactly how CI runners build it on different platforms.
