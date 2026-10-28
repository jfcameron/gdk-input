# gdk-input
The gdk input interface: a `context` holding the keyboard, mouse, text input and gamepads as they stood at the last update, `controls` that name a game's actions and bind them to physical inputs, and `capture`, which waits for a press so a player can rebind one. It carries no implementation but a null one, for tests.

API documentation: https://jfcameron.github.io/gdk-input/

Implementations:
- [gdk-input_glfw](https://github.com/jfcameron/gdk-input_glfw): desktop and the browser, through GLFW
- [gdk-input_android](https://github.com/jfcameron/gdk-input_android): an Android activity's input events

## Getting Started
Install cmake 3.21 or greater, and a c++20 compatible compiler
```
# initialize the tree
git submodule update --init --recursive

# generate the script to build the library and its tests on linux using gcc
cmake --preset linux-gcc

# build the library and tests
cmake --build --preset linux-gcc

# run the tests
ctest --preset linux-gcc
```
See `CMakePresets.json` for the full list of supported platforms and compilers. See `.github/workflows/ci.yml` to see how CI runners build it on different platforms.

To use it from another project, add it with `add_subdirectory` and link `gdk::input`, or install it and `find_package(gdk-input)`.

`GDKINPUT_EXT_LUA` builds `gdk::input_ext_lua`, which writes a `controls`' bindings to a lua table and reads them back (`<gdk/input/ext/lua_bindings.h>`). It brings jfc-lua with it and is not installed.

## Usage
```cpp
#include <gdk/input/context.h>
#include <gdk/input/controls.h>

void play(const gdk::input::context_ptr_type &pInput) {
    using namespace gdk::input;

    const auto pControls = controls::make(pInput);

    pControls->bind("jump", keyboard::key::space);
    pControls->bind("jump", gamepad::button::a);
    pControls->bind("move_left", gamepad::axis::left_x, -1);
    pControls->bind("look_x", mouse::axis::x, 0.01f);

    // each frame, after the implementation's update:
    if (pControls->just_pressed("jump")) { /* jump */ }

    const auto left = pControls->get("move_left");
}
```

An action's edges are the action's: holding one key bound to it and pressing another doesn't press it again. The cursor's position is normalised across the window, 0,0 the bottom left; its delta is in pixels, y up, and the wheel's in notches, both for the last update only.

Whatever composes the program picks the implementation, for example `gdk::input::glfw_context::make(pGLFWWindow)`.

## Conformance
`gdkinput_conformance` is the suite every implementation passes. An implementation's test target links it and defines `gdk::input::conformance::make_driver()` and `implementation()` (see `test/conformance/conformance.h`): the driver plays the platform, pressing keys, moving the cursor and plugging in a pad, and the suite checks the context reports it the same way everywhere. The null implementation runs it here.

`gdkinput_implementation` gives an implementation the private helpers the null one uses too (UTF-8, keys withheld from the game while a text field has the keyboard, gamepad mappings, player slots). It is a build-time target and is never installed.
