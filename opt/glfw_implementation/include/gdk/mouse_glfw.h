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
    /// \attention update method be called to update gamepad state. This is not handled by glfwPollEvents
    class mouse_glfw : public mouse
    {
        //! ptr to the glfw window
        std::shared_ptr<GLFWwindow> m_pWindow;

        //! state of the cursor
        mouse::CursorMode m_CursorMode = CursorMode::Normal;

        //! buffer used to calculate per-frame cursor delta
        mouse::cursor_2d_type m_LastDeltaCallCursorPosition = {0, 0};

        //! cursor delta buffer
        mouse::cursor_2d_type m_Delta;

    public:
        /// \attention must be called for proper behaviour of getDelta
        void update();

        virtual void setCursorMode(const CursorMode aCursorMode) override;

        virtual bool getButtonDown(const mouse::Button aButtonCode) const override;

        virtual cursor_2d_type getCursorPosition() const override;

        virtual cursor_2d_type getDelta() const override;

        virtual mouse::CursorMode getCursorMode() const override;

        mouse_glfw(decltype(m_pWindow) pWindow);
    };
}

#endif
