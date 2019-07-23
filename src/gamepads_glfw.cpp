// © 2019 Joseph Cameron - All Rights Reserved

#include <cassert>
#include <cstdio>
#include <cstring>
#include <iostream>
#include <map>
#include <vector>

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

    float gamepad_glfw::getAxis(int index) const 
    {
        return 0;
    }

    bool gamepad_glfw::getButtonDown(int index) const 
    {
        return false;
    }

    gamepad::hat_type gamepad_glfw::getHat(int index) const
    {
        return {};
    }

    std::string gamepad_glfw::getName() const 
    {
        return "balr";
    }

    void gamepad_glfw::update()
   {
        if (GLFW_TRUE == glfwJoystickPresent(m_JoystickIndex))
        {
            int button_count;
            const unsigned char *buttons = glfwGetJoystickButtons(m_JoystickIndex, &button_count);
            
            int axes_count;
            const float *axes = glfwGetJoystickAxes(m_JoystickIndex, &axes_count);

            //return std::vector<const unsigned char *>();// decltype(m_Buttons)(buttons, buttons + button_count);
            //return decltype(m_Buttons)(buttons, buttons + button_count);

            m_Buttons = decltype(m_Buttons)(buttons, buttons + 1);
            //std::vector<std::string>(argv, argv + argc);
        }
        //TODO: is throw appropriate? No. a disconnected flag probably better. How to do reconnect logic? uncertain.
        // static vector that shares used indicies? When I am disconnected & update is called, check unused indicies, check if it has a name == to my name?
        else throw std::invalid_argument(std::string(TAG).append(": no gamepad at index: ")
            .append(std::to_string(m_JoystickIndex)));
    }
}

