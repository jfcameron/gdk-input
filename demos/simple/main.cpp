// © 2019 Joseph Cameron - All Rights Reserved

#include <demo/glfw_init.h>

#include <gdk/input_context.h>
#include <gdk/controls.h>
#include <gdk/keyboard_glfw.h>
#include <gdk/mouse_glfw.h>
#include <gdk/gamepad_glfw.h>

#include <cstdlib>
#include <vector>
#include <iostream>

const std::string JSONControlsMappingString = R"(
{
    "Jump": {
        "gamepads": {
                "8bitdo": {
                    "buttons": [0]
                },
                "sdl_gamepad": {
                    "buttons": [1]
                }
        },
        "keys": ["Q", "Space", "LeftControl"],
        "mouse": {
            "buttons": []
        }
    }
}
)";

using namespace gdk;

int main(int argc, char **argv)
{
    auto pWindow = initGLFW();

	std::shared_ptr<input::context> pInput = std::move(input::context::make(pWindow.get()));

	auto pControl = controls::make_from_json(pInput);

	//TODO: parameterize
	const std::set<std::string> bindings{
		"Up",
		"Down",
		"Left",
		"Right",
		"A",
		"B",
	};

	//Iterate binding names, bind real inputs to the names
	for (const auto& binding : bindings)
	{
		glfwPollEvents();

		pInput->update();

		std::cout << "binding: " << binding << "\n";

		//for keys, for buttons, for hats, ....
		{
			//	if a > threshold
				//pControl->addKeyToMapping(binding, keyboard::Key::C);
		}
	}
		
	/*while (!pInput->get_key_just_released(keyboard::Key::Escape))
	{
		glfwPollEvents();

		pInput->update();
	}*/

	std::cout << pControl->serializeToJSON() << "\n";

    return EXIT_SUCCESS;
}
