// © 2020 Joseph Cameron - All Rights Reserved

#include <demo/glfw_init.h>

#include <gdk/input_context.h>
#include <gdk/controls.h>
#include <gdk/keyboard_glfw.h>
#include <gdk/mouse_glfw.h>
#include <gdk/gamepad_glfw.h>
#include <gdk/configurator.h>

#include <jfc/ring_buffer.h>

#include <cstdlib>
#include <vector>
#include <iostream>
#include <array>
#include <sstream>

using namespace gdk;

int main(int argc, char **argv)
{
    auto pWindow = initGLFW();

	std::shared_ptr<input::context> pInput = std::move(input::context::make(pWindow.get()));

	auto pControl = std::shared_ptr<controls>(controls::make(pInput, true, true, 0));

	configurator c(pInput, pControl, {
		"Up",
		"Down",
		"Left",
		"Right",
		"A",
		"B",
	});
	
	jfc::ring_buffer<int, 60> ring({});
	
	auto iter = ring.get();
	
	for (int i = 1; i < 61; i++)
	{
		*iter = i;
		iter--;
	}

	auto citer = ring.cget();

	for (int i = 1; i < 61; i++)
	{
		std::cout << *citer << "\n";
		citer--;
	}

	for (bool bShouldExit(false);!bShouldExit;)
	{
		glfwPollEvents();

		pInput->update();

		c.update();

		if (pInput->get_key_just_released(keyboard::Key::Escape)) bShouldExit = true;
	}

	std::cout << pControl->serialize_to_json() << "\n";

    return EXIT_SUCCESS;
}
