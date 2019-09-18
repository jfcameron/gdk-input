// © 2019 Joseph Cameron - All Rights Reserved

#include <cassert>
#include <cstdio>
#include <cstring>
#include <iostream>
#include <map>
#include <vector>
#include <iostream>

#include <GLFW/glfw3.h>

#include <gdk/gamepad_glfw.h>

static constexpr char TAG[] = "gamepad_glfw";

static constexpr char SDL_GAMEPAD_NAME[] = "sdl_gamepad";

namespace gdk
{
    gamepad_glfw::gamepad_glfw(const int aJoystickIndex)
    : m_JoystickIndex(aJoystickIndex)
    , m_Name([aJoystickIndex]()
    {
        const char *name = glfwJoystickIsGamepad(aJoystickIndex)
            ? SDL_GAMEPAD_NAME 
            : glfwGetJoystickName(aJoystickIndex);

        if (!name) throw std::invalid_argument(std::string(TAG).append(": no gamepad at index: ").append(std::to_string(aJoystickIndex)));

        return name; 
    }())
    {}

    float gamepad_glfw::getAxis(int index) const 
    {
        return m_Axes[index];
    }

    gamepad::size_type gamepad_glfw::getAxisCount() const
    {
       return m_Axes.size(); 
    }

    gamepad::button_state_type gamepad_glfw::getButtonDown(int index) const 
    {
        return m_Buttons[index];
    }

    gamepad::size_type gamepad_glfw::getButtonCount() const
    {
       return m_Buttons.size(); 
    }

    gamepad::hat_state_type gamepad_glfw::getHat(int index) const
    {
        const auto hat_state_glfw = m_Hats[index];
    
        if (hat_state_glfw == GLFW_HAT_CENTERED) return {hat_state_type::horizontal_direction::Center, hat_state_type::vertical_direction::Center};

        return {
            hat_state_glfw & GLFW_HAT_RIGHT 
                ? hat_state_type::horizontal_direction::Right
                : hat_state_glfw & GLFW_HAT_LEFT 
                    ? hat_state_type::horizontal_direction::Left
                    : hat_state_type::horizontal_direction::Center, 

            hat_state_glfw & GLFW_HAT_UP 
                ? hat_state_type::vertical_direction::Up
                : hat_state_glfw & GLFW_HAT_DOWN 
                    ? hat_state_type::vertical_direction::Down
                    : hat_state_type::vertical_direction::Center
        };
    }

    gamepad::size_type gamepad_glfw::getHatCount() const
    {
        return m_Hats.size();
    }

    std::string_view gamepad_glfw::getName() const 
    {
        return m_Name;
    }

    void gamepad_glfw::update()
    {
        //update is a bit silly because I am not dealing with disconnects properly. 
        //this leads to a question every update: does this instance still point to the same hardware? To let these vary, I constantly reassign the name and check if its a joystick or gamepad. Think about this. Is a more elegant implementation of disconnect/reconnects possible with the glfw library? https://www.glfw.org/docs/latest/input_guide.html
        if (glfwJoystickIsGamepad(m_JoystickIndex))
        {
            m_Name = SDL_GAMEPAD_NAME;
            
            GLFWgamepadstate state; 
            glfwGetGamepadState(m_JoystickIndex, &state);

            m_Axes = decltype(m_Axes)(state.axes, state.axes + (sizeof(state.axes) / sizeof(state.axes[0])));

            // Hats are appended to the back of the button array. They are also in fixed positions
            // https://www.glfw.org/docs/latest/input_guide.html#gamepad "button indicies"
            m_Buttons = decltype(m_Buttons)(state.buttons, state.buttons + GLFW_GAMEPAD_BUTTON_DPAD_UP - 1); //TODO test for off by one error.

            m_Hats = { static_cast<decltype(m_Hats)::value_type>(
                state.buttons[GLFW_GAMEPAD_BUTTON_DPAD_RIGHT]
                    ? state.buttons[GLFW_GAMEPAD_BUTTON_DPAD_UP]
                        ? GLFW_HAT_RIGHT_UP
                        : state.buttons[GLFW_GAMEPAD_BUTTON_DPAD_DOWN]
                            ? GLFW_HAT_RIGHT_DOWN
                            : GLFW_HAT_CENTERED
                    : state.buttons[GLFW_GAMEPAD_BUTTON_DPAD_LEFT]
                        ? state.buttons[GLFW_GAMEPAD_BUTTON_DPAD_UP]
                            ? GLFW_HAT_LEFT_UP
                            : state.buttons[GLFW_GAMEPAD_BUTTON_DPAD_DOWN]
                                ? GLFW_HAT_LEFT_DOWN
                                : GLFW_HAT_CENTERED
                        : state.buttons[GLFW_GAMEPAD_BUTTON_DPAD_UP]
                            ? GLFW_HAT_UP
                            : state.buttons[GLFW_GAMEPAD_BUTTON_DPAD_DOWN]
                                ? GLFW_HAT_DOWN
                                : GLFW_HAT_CENTERED)
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

