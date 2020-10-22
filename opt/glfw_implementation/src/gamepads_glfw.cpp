// © 2019, 2020 Joseph Cameron - All Rights Reserved

#include <cassert>
#include <cmath>
#include <cstdio>
#include <cstring>
#include <iostream>
#include <iostream>
#include <map>
#include <vector>

#include <gdk/gamepad_glfw.h>

static constexpr char TAG[] = "gamepad_glfw";

static constexpr char SDL_GAMEPAD_NAME[] = "standard_gamepad";

namespace gdk
{
    gamepad_glfw::gamepad_glfw(const int aJoystickIndex)
    : m_JoystickIndex(aJoystickIndex)
    , m_Name(/*[aJoystickIndex]()
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
    }()*/)
    {}

    float gamepad_glfw::get_axis(gamepad::index_type index, axis_value_type threshold) const
    {
		if (index >= m_Axes.size()) return 0;

		auto value = m_Axes[index];

		if (std::abs(value) < threshold) return 0;

		// glfw gamepad triggers are -1 when untouched.
		// This moves the range from [-1 - +1], to [0 - 1]
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

	bool gamepad_glfw::get_axis_just_exceeded_threshold(const index_type index, const axis_value_type threshold) const
	{
		if (index >= m_Axes.size()) return false;

		const auto currentValue = m_Axes[index];

		const auto lastValue = index < m_LastAxes.size()
			? m_LastAxes[index]
			: 0;

		if (currentValue >= threshold && lastValue < threshold) return true;
		
		return false;
	}

	bool gamepad_glfw::get_axis_just_dropped_below_threshold(const index_type index, const axis_value_type threshold) const
	{
		if (index >= m_Axes.size()) return false;

		const auto currentValue = m_Axes[index];

		const auto lastValue = index < m_LastAxes.size()
			? m_LastAxes[index]
			: 0;

		if (currentValue <= threshold && lastValue > threshold) return true;

		return false;
	}

	bool gamepad_glfw::get_button_down(const gamepad::index_type index) const
    {
		if (index >= m_Buttons.size()) return false;

		return m_Buttons[index] != button_state::UP;
    }

	bool gamepad_glfw::get_button_just_pressed(const index_type index) const
	{
		if (index >= m_Buttons.size()) return false;

		return m_Buttons[index] == button_state::JUST_PRESSED;
	}

	bool gamepad_glfw::get_button_just_released(const index_type index) const
	{
		if (index >= m_Buttons.size()) return false;

		return m_Buttons[index] == button_state::JUST_RELEASED;
	}

	std::optional<gamepad::button_collection_type::size_type> gamepad_glfw::get_any_button_down() const
	{
		for (decltype(m_Buttons)::size_type i(0); i < m_Buttons.size(); ++i)
			if (m_Buttons[i] != button_state::UP) return i;

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
            
			// Axes
            GLFWgamepadstate state; 
            glfwGetGamepadState(m_JoystickIndex, &state);

			m_LastAxes = m_Axes;
            m_Axes = decltype(m_Axes)(state.axes, state.axes + (sizeof(state.axes) / sizeof(state.axes[0])));

			// Buttons & hats
            // Hats are appended to the back of the button array. They are also in fixed positions
            // https://www.glfw.org/docs/latest/input_guide.html#gamepad "button indicies"
			std::vector<decltype(GLFW_PRESS)> buttons(state.buttons, state.buttons + GLFW_GAMEPAD_BUTTON_LAST + 1);
			
			if (m_Buttons.size() == buttons.size())
			{
				for (size_t i(0); i < buttons.size(); ++i)
				{
					switch (m_Buttons[i])
					{
					case button_state::HELD_DOWN: 
						m_Buttons[i] = buttons[i]
							? button_state::HELD_DOWN
							: button_state::JUST_RELEASED;
					break;

					case button_state::UP: 
						m_Buttons[i] = buttons[i]
							? button_state::JUST_PRESSED
							: button_state::UP;
					break;
					
					case button_state::JUST_PRESSED: 
						m_Buttons[i] = buttons[i]
							? button_state::HELD_DOWN
							: button_state::JUST_RELEASED;
					break;
					
					case button_state::JUST_RELEASED: 
						m_Buttons[i] = buttons[i]
							? button_state::JUST_PRESSED
							: button_state::UP;
					break;
					}
				}
			}
			else // This occurs if a joystick is swapped out & first update call
			{
				m_Buttons.clear();
				m_Buttons.reserve(buttons.size());

				for (const auto& b : buttons)
					m_Buttons.push_back(b
						? button_state::JUST_PRESSED
						: button_state::UP);
			}
        }
        /*else if (glfwJoystickPresent(m_JoystickIndex)) 
        {
            m_Name = glfwGetJoystickName(m_JoystickIndex);

            int count;
            
            const unsigned char *buttons = glfwGetJoystickButtons(m_JoystickIndex, &count);

            m_Buttons = decltype(m_Buttons)(buttons, buttons + count);

            const float *axes = glfwGetJoystickAxes(m_JoystickIndex, &count);

            m_Axes = decltype(m_Axes)(axes, axes + count);

            const unsigned char *hats = glfwGetJoystickHats(m_JoystickIndex, &count);
			//todo: convert hats to buttons. just a big switch.

            m_Hats = decltype(m_Hats)(hats, hats + count);
        }*/
        else //Disconnected
        {
            m_Buttons.clear();
            m_Axes.clear();
        }
    }
}
