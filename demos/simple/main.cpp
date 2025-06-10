// © Joseph Cameron - All Rights Reserved

#include <demo/glfw_init.h>

//#include <gdk/controls.h>
#include <gdk/gamepad_glfw.h>
#include <gdk/glfw_context.h>
#include <gdk/input_context.h>
#include <gdk/keyboard_glfw.h>
#include <gdk/mouse_glfw.h>

#include <cstdlib>
#include <vector>
#include <iostream>
#include <array>
#include <sstream>

using namespace gdk;

int main(int argc, char **argv)
{
    auto pWindow = initGLFW();

	auto pInput = input::glfw_context::make(pWindow.get());

	auto pControl = std::shared_ptr<controls>(controls::make(pInput, true, true, 0));

	/*configurator c(pInput, pControl, {
		"Up",
		"Down",
		"Left",
		"Right",
		"A",
		"B",
	});*/

	auto pGamepad = pInput->get_gamepad(0);

	for (bool bShouldExit(false);!bShouldExit;) {
		glfwPollEvents();
        std::static_pointer_cast<input::glfw_context>(pInput)->update(); 
		//c.update();
		if (pInput->key_just_released(keyboard::key::escape)) bShouldExit = true;
	}

	std::cout << pControl->serialize_to_json() << "\n";

    return EXIT_SUCCESS;
}
