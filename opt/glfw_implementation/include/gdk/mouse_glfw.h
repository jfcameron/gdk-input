// © 2019 Joseph Cameron - All Rights Reserved

#ifndef GDK_INPUT_MOUSE_GLFW_H
#define GDK_INPUT_MOUSE_GLFW_H

#include <memory>

#include <gdk/mouse.h>

struct GLFWwindow;

namespace gdk
{
    /// \brief mouse using glfw for implementation
    /// \attention glfwPollEvents() must be called periodically in order to update the mouse state
    class mouse_glfw : public mouse
    {
        //! ptr to the glfw window
        std::shared_ptr<GLFWwindow> m_pWindow;

        //! buffer containing the cursor's position when delta was last called. used to calculate the delta.
        cursor_2d_type m_LastDeltaCallCursorPosition;

    public:
        virtual bool getButtonDown(const mouse::Button aButtonCode) override;

        virtual void setCursorMode(const CursorMode aCursorMode) override;

        virtual cursor_2d_type getCursorPosition() override;

        virtual cursor_2d_type getDelta() override;

        mouse_glfw(decltype(m_pWindow) pWindow)
        : m_pWindow(pWindow)
        {}
    };
}

#endif