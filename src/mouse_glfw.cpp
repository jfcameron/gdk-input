// © 2019 Joseph Cameron - All Rights Reserved

#include <gdk/mouse_glfw.h>

#include <GLFW/glfw3.h>

#include <iostream>
#include <stdexcept>

static constexpr char TAG[] = "mouse";

static inline int glfwmouseButtonFromButton(const gdk::mouse::Button &a)
{
    switch(a)
    {
        case gdk::mouse::Button::Left:   return GLFW_MOUSE_BUTTON_LEFT;
        case gdk::mouse::Button::Right:  return GLFW_MOUSE_BUTTON_RIGHT;
        case gdk::mouse::Button::Middle: return GLFW_MOUSE_BUTTON_MIDDLE;
        case gdk::mouse::Button::Four:   return GLFW_MOUSE_BUTTON_4;
        case gdk::mouse::Button::Five:   return GLFW_MOUSE_BUTTON_5;
        case gdk::mouse::Button::Six:    return GLFW_MOUSE_BUTTON_6;
        case gdk::mouse::Button::Seven:  return GLFW_MOUSE_BUTTON_7;
        case gdk::mouse::Button::Eight:  return GLFW_MOUSE_BUTTON_8;
    }
}

namespace gdk
{
    bool mouse_glfw::getButtonDown(const mouse::Button aButton)
    {
        return static_cast<bool>(glfwGetMouseButton(m_pWindow.get(), glfwmouseButtonFromButton(aButton)));
    }

    /*Vector2 getCursorPosition()
    {
        //return glfw::GetCursorPos();

        double x,y;
        glfwGetCursorPos(gdk::input::PRIVATE::pGLFWwindow, &x, &y);
        
        return Vector2(x, y);
    }*/

    /*bool getButton(const Button &aKeyCode)
    {
        throw std::runtime_error(std::string(TAG).append("/mouse::getButton(const Button &aKeyCode) not implemented"));
    }*/

    /*Vector2 getDelta()
    {
        throw std::runtime_error(std::string(TAG).append("/mouse::getDelta not implemented"));
    }*/
}

