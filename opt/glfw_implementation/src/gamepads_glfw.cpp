// © 2019 Joseph Cameron - All Rights Reserved

#include <cassert>
#include <cstdio>
#include <cstring>
#include <iostream>
#include <map>
#include <vector>
#include <iostream>

#include <gdk/gamepad_glfw.h>

static constexpr char TAG[] = "gamepad_glfw";

static constexpr char SDL_GAMEPAD_NAME[] = "standard_gamepad";

namespace gdk
{
    gamepad_glfw::gamepad_glfw(const int aJoystickIndex)
    : m_JoystickIndex(aJoystickIndex)
    , m_Name([aJoystickIndex]()
    {
		//NOTE: since glfw gamepads can shift in real time, no setup work should be done in the ctor.
		//see update. should add a private method that shares commonalities of setup, call here and there.
        const char *name = glfwJoystickIsGamepad(aJoystickIndex)
            ? SDL_GAMEPAD_NAME 
            : glfwGetJoystickName(aJoystickIndex);

		//likely should be a bool member not this stringy stuff
		// add a is_connected member & getmethod
		if (!name) name = "disconnected"; 

        return name; 
    }())
    {}

    float gamepad_glfw::get_axis(gamepad::index_type index, axis_value_type threshold) const
    {
		if (index >= m_Axes.size()) return 0;

		auto value = m_Axes[index];

		if (std::abs(value) < threshold) return 0;

		// Gamepad triggers are -1 when untouched.
		// This normalizes the value. instead of weird range of [-1 - +1], go to [0 - 1]
		// gdk always presents a value of 0 as unpressed
		if (m_Name == SDL_GAMEPAD_NAME)
		{
			if (index == GLFW_GAMEPAD_AXIS_LEFT_TRIGGER || index == GLFW_GAMEPAD_AXIS_RIGHT_TRIGGER)
			{
				value += 1.f;
				value /= 2.f;
			}
		}

        return value;
    }

    gamepad::button_state_type gamepad_glfw::get_button_down(gamepad::index_type index) const
    {
		if (index >= m_Buttons.size()) return 0;

		return m_Buttons[index];
    }

	std::optional<gamepad::button_collection_type::size_type> gamepad_glfw::get_any_button_down() const
	{
		for (decltype(m_Buttons)::size_type i(0); i < m_Buttons.size(); ++i)
		{
			if (m_Buttons[i]) return i;
		}

		return {};
	}

	std::optional<std::pair<gamepad::index_type, gamepad::hat_state_type>> gamepad_glfw::get_any_hat_down() const
	{
		for (decltype(m_Hats)::size_type i(0); i < m_Hats.size(); ++i)
		{
			if (m_Hats[i]) return 
			{{i, get_hat(i)}};
		}

		return {};
	}

	std::optional<std::pair<gamepad::index_type, gamepad_glfw::axis_value_type>> gamepad_glfw::get_any_axis_down(axis_value_type threshold) const
	{
		for (decltype(m_Axes)::size_type i(0); i < m_Axes.size(); ++i)
		{
			if (m_Axes[i])
			{
				if (auto val = get_axis(i, threshold)) return {{
					i,
					val
				}};
			}
		}

		return {};
	}

    gamepad::hat_state_type gamepad_glfw::get_hat(gamepad::index_type index) const
    {
		gamepad::hat_state_type hat {
			hat_state_type::horizontal_direction::Center,
			hat_state_type::vertical_direction::Center
		};

		if (index >= m_Hats.size()) return hat;

        const auto hat_state_glfw = m_Hats[index];

		if      (hat_state_glfw & GLFW_HAT_LEFT) hat.x  = hat_state_type::horizontal_direction::Left;
		if (hat_state_glfw & GLFW_HAT_RIGHT) hat.x = hat_state_type::horizontal_direction::Right;

		if      (hat_state_glfw & GLFW_HAT_UP) hat.y   = hat_state_type::vertical_direction::Up;
		if (hat_state_glfw & GLFW_HAT_DOWN) hat.y = hat_state_type::vertical_direction::Down;

		return hat;
    }

    std::string_view gamepad_glfw::get_name() const 
    {
        return m_Name;
    }

    void gamepad_glfw::update()
    {
        //update is a bit silly because I am not dealing with disconnects properly. 
        //this leads to a question every update: does this instance still point to the same hardware? 
		//To let these vary, I constantly reassign the name and check if its a joystick or gamepad. 
		//Think about this. Is a more elegant implementation of disconnect/reconnects possible with the glfw library? 
		//https://www.glfw.org/docs/latest/input_guide.html
		//
		// the hardware gamepad can change underneath the gamepad_glfw instance. After looking into it, I dont think there is
		// a way to assign a unique ID to gamepads within GLFW, which means this just will happen if players disconnect pads and reconnect them
		// in differing orders.
        if (glfwJoystickIsGamepad(m_JoystickIndex))
        {
            m_Name = SDL_GAMEPAD_NAME;
            
            GLFWgamepadstate state; 
            glfwGetGamepadState(m_JoystickIndex, &state);

            m_Axes = decltype(m_Axes)(state.axes, state.axes + (sizeof(state.axes) / sizeof(state.axes[0])));

            // Hats are appended to the back of the button array. They are also in fixed positions
            // https://www.glfw.org/docs/latest/input_guide.html#gamepad "button indicies"
            m_Buttons = decltype(m_Buttons)(state.buttons, state.buttons + GLFW_GAMEPAD_BUTTON_DPAD_UP - 1); //TODO test for off by one error.

			auto a(GLFW_HAT_CENTERED);
	
			if (state.buttons[GLFW_GAMEPAD_BUTTON_DPAD_RIGHT])
			{
				a = GLFW_HAT_RIGHT;

				if (state.buttons[GLFW_GAMEPAD_BUTTON_DPAD_UP]) a = GLFW_HAT_RIGHT_UP;
				if (state.buttons[GLFW_GAMEPAD_BUTTON_DPAD_DOWN]) a = GLFW_HAT_RIGHT_DOWN;
			}
			else if (state.buttons[GLFW_GAMEPAD_BUTTON_DPAD_LEFT])
			{
				a = GLFW_HAT_LEFT;

				if (state.buttons[GLFW_GAMEPAD_BUTTON_DPAD_UP]) a = GLFW_HAT_LEFT_UP;
				if (state.buttons[GLFW_GAMEPAD_BUTTON_DPAD_DOWN]) a = GLFW_HAT_LEFT_DOWN;
			}
			else if (state.buttons[GLFW_GAMEPAD_BUTTON_DPAD_UP]) a = GLFW_HAT_UP;
			else if (state.buttons[GLFW_GAMEPAD_BUTTON_DPAD_DOWN]) a = GLFW_HAT_DOWN;

			m_Hats = {
				static_cast<decltype(m_Hats)::value_type>(a)
			};
        }
        else if (glfwJoystickPresent(m_JoystickIndex)) 
        {
            m_Name = glfwGetJoystickName(m_JoystickIndex);

            int count;
            
            const unsigned char *buttons = glfwGetJoystickButtons(m_JoystickIndex, &count);

            m_Buttons = decltype(m_Buttons)(buttons, buttons + count);

            const float *axes = glfwGetJoystickAxes(m_JoystickIndex, &count);

            m_Axes = decltype(m_Axes)(axes, axes + count);

            const unsigned char *hats = glfwGetJoystickHats(m_JoystickIndex, &count);

            m_Hats = decltype(m_Hats)(hats, hats + count);
        }
        else //Disconnected
        {
            m_Buttons.clear();
            m_Axes.clear();
            m_Hats.clear();
        }
    }
}
