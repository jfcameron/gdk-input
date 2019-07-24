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
        const char *name = glfwGetJoystickName(aJoystickIndex); //the returned char* is non-owning.

        if (!name) 
            throw std::invalid_argument(std::string(TAG).append(": no gamepad at index: ").append(std::to_string(aJoystickIndex)));

        return name; 
    }())
    {}

    gamepad::size_type gamepad_glfw::getAxisCount() const
    {
       return m_Axes.size(); 
    }

    gamepad::size_type gamepad_glfw::getButtonCount() const
    {
       return m_Buttons.size(); 
    }

    gamepad::size_type gamepad_glfw::getHatCount() const
    {
        std::runtime_error("Unimplemented!!");

        return 0;
    }

    float gamepad_glfw::getAxis(int index) const 
    {
        return m_Axes[index];
    }

    bool gamepad_glfw::getButtonDown(int index) const 
    {
        return m_Buttons[index];
    }

    gamepad::hat_type gamepad_glfw::getHat(int index) const
    {
        std::runtime_error("Unimplemented!!");

        return {};
    }

    std::string_view gamepad_glfw::getName() const 
    {
        return "balr";
    }

    void gamepad_glfw::update()
   {
        if (GLFW_TRUE == glfwJoystickPresent(m_JoystickIndex))
        {
            int button_count;
            const unsigned char *buttons = glfwGetJoystickButtons(m_JoystickIndex, &button_count);
            
            m_Buttons = decltype(m_Buttons)(buttons, buttons + button_count);

            int axes_count;
            const float *axes = glfwGetJoystickAxes(m_JoystickIndex, &axes_count);

            m_Axes = decltype(m_Axes)(axes, axes + axes_count);

            //TODO: hats. also hat hint at initialization
        }
        //TODO: is throw appropriate? No. a disconnected flag probably better. How to do reconnect logic? uncertain.
        // static vector that shares used indicies? When I am disconnected & update is called, check unused indicies, check if it has a name == to my name?
        // https://github.com/jfcameron/gdk-input/issues/1
        else throw std::invalid_argument(std::string(TAG).append(": no gamepad at index: ")
            .append(std::to_string(m_JoystickIndex)));
    }
}

