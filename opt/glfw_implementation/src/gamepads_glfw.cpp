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

namespace gdk
{
    gamepad_glfw::gamepad_glfw(const int aJoystickIndex)
    : m_JoystickIndex(aJoystickIndex)
    , m_Name([aJoystickIndex]()
    {
/*#if defined(GDK_INPUT_IMPLEMENTATION_GLFW)
        throw std::runtime_error("blarblarblarblar!");
#endif*/
        const char *name = glfwGetJoystickName(aJoystickIndex); //the returned char* is non-owning.

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
    
        if (hat_state_glfw == GLFW_HAT_CENTERED) return {0, 0};

        return {
            hat_state_glfw & GLFW_HAT_RIGHT ? short(1) : hat_state_glfw & GLFW_HAT_LEFT ? short(-1) : short(0), 
            hat_state_glfw & GLFW_HAT_UP    ? short(1) : hat_state_glfw & GLFW_HAT_DOWN ? short(-1) : short(0)
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
        if (GLFW_TRUE == glfwJoystickPresent(m_JoystickIndex))
        {
            m_Name = glfwGetJoystickName(m_JoystickIndex);

            int count;
            
            const unsigned char *buttons = glfwGetJoystickButtons(m_JoystickIndex, &count);

            m_Buttons = decltype(m_Buttons)(buttons, buttons + count);

            const float *axes = glfwGetJoystickAxes(m_JoystickIndex, &count);

            m_Axes = decltype(m_Axes)(axes, axes + count);

            const unsigned char* hats = glfwGetJoystickHats(m_JoystickIndex, &count);

            m_Hats = decltype(m_Hats)(hats, hats + count);
        }
        else // order can get confused if multiple controllers are disconnected at the same time.
        {
            m_Buttons.clear();
            m_Axes.clear();
            m_Hats.clear();
        }
    }
}

