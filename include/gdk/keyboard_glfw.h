// © 2019 Joseph Cameron - All Rights Reserved

#ifndef GDK_INPUT_KEYBOARD_GLFW_H
#define GDK_INPUT_KEYBOARD_GLFW_H

#include <memory>

#include <gdk/keyboard.h>

struct GLFWwindow;

namespace gdk
{
    class keyboard_glfw : public keyboard
    {
        std::shared_ptr<GLFWwindow> m_pWindow;

    public:

        virtual bool getKeyDown(const keyboard::Key &aKeyCode) override;

        keyboard_glfw(decltype(m_pWindow) pWindow)
        : m_pWindow(pWindow)
        {}
    };
}

#endif

