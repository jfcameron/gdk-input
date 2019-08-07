// © 2019 Joseph Cameron - All Rights Reserved

#include <gdk/mouse_glfw.h>

#include <GLFW/glfw3.h>

#include <iostream>
#include <stdexcept>

static constexpr char TAG[] = "mouse";

static inline int glfwmouseButtonFromButton(const gdk::mouse::Button a)
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
    
    throw std::invalid_argument(std::string("Unable to convert mouse button \"").append(std::to_string(static_cast< std::underlying_type< decltype(a)>::type>(a))).append("\" to GLFW_MOUSE_BUTTON")); 
}

namespace gdk
{
    bool mouse_glfw::getButtonDown(const mouse::Button aButton)
    {
        return static_cast<bool>(glfwGetMouseButton(m_pWindow.get(), glfwmouseButtonFromButton(aButton)));
    }

    void mouse_glfw::setCursorMode(const CursorMode aCursorMode)
    {
        decltype(GLFW_CURSOR_NORMAL) cursorModeBuffer;

        switch (aCursorMode)
        {
            case CursorMode::Normal:
            {
                cursorModeBuffer = GLFW_CURSOR_NORMAL;
            } break;

            case CursorMode::Hidden:
            {
                cursorModeBuffer = GLFW_CURSOR_HIDDEN;
            } break;

            case CursorMode::Locked:
            {
                cursorModeBuffer = GLFW_CURSOR_DISABLED;
            } break;

            default: throw std::invalid_argument(std::string("Unhandled CursorMode: ").append(std::to_string(static_cast<std::underlying_type<decltype(aCursorMode)>::type>(aCursorMode))));
        }

        glfwSetInputMode(m_pWindow.get(), GLFW_CURSOR, cursorModeBuffer);

        m_CursorMode = aCursorMode;
    }

    mouse::CursorMode mouse_glfw::getCursorMode()
    {
        return m_CursorMode;    
    }

    mouse::cursor_2d_type mouse_glfw::getCursorPosition()
    {
        double x,y;
        glfwGetCursorPos(m_pWindow.get(), &x, &y);

        return {x, y};
    }

    /*mouse::cursor_2d_type mouse_glfw::getDelta()
    {
        auto currentCursorPosition = getCursorPosition();
       
        mouse::cursor_2d_type deltaBuffer;

        deltaBuffer.x = currentCursorPosition.x - m_LastDeltaCallCursorPosition.x;
        deltaBuffer.y = currentCursorPosition.y - m_LastDeltaCallCursorPosition.y;

        //m_LastDeltaCallCursorPosition = currentCursorPosition;
        
        return deltaBuffer;
    }*/
}

