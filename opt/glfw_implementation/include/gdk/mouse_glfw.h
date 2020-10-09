// © 2019 Joseph Cameron - All Rights Reserved

#ifndef GDK_INPUT_MOUSE_GLFW_H
#define GDK_INPUT_MOUSE_GLFW_H

#include <memory>
#include <unordered_map>

#include <gdk/mouse.h>

#include <GLFW/glfw3.h>

struct GLFWwindow;

namespace gdk
{
    /// \brief mouse using glfw for implementation
    /// \attention glfwPollEvents() must be called periodically in order to update the mouse state
    /// \attention update method be called to update gamepad state. This is not handled by glfwPollEvents
    class mouse_glfw : public mouse
    {
        /// \brief ptr to the glfw window
		/// \attention mouse does not own this ptr.
		GLFWwindow* const m_pWindow;

        /// \brief state of the cursor
        mouse::CursorMode m_CursorMode = CursorMode::Normal;

        /// \brief buffer used to calculate per-frame cursor delta
        mouse::cursor_2d_type m_LastDeltaCallCursorPosition = {0, 0};

        /// \brief cursor delta buffer
		mouse::cursor_2d_type m_Delta = {0, 0};

		/// \brief mouse's button state as of last update() call
		std::unordered_map<decltype(GLFW_MOUSE_BUTTON_1), decltype(GLFW_PRESS)> m_MouseButtonLastState;

		/// \brief mouse's button state as of this update() call
		std::unordered_map<decltype(GLFW_MOUSE_BUTTON_1), mouse::ButtonState> m_CurrentState;

		double m_DeltaSensitivity = 1.0;

    public:
		/// \brief Check if the key is being held down
		virtual bool getButtonDown(const Button aKeyCode) const override;

		/// \brief check if key was just pressed down
		virtual bool getButtonJustDown(const Button aKeyCode) const override;

		/// \brief check if the key was just released
		virtual bool getButtonJustReleased(const Button aKeyCode) const override;


		/// \brief 
		virtual cursor_2d_type getCursorPosition() const override;

		virtual cursor_2d_type getDelta() const override;

		virtual void setDeltaSensitivity(double sens) override;



		virtual void setCursorMode(const CursorMode aCursorMode) override;

        

        virtual mouse::CursorMode getCursorMode() const override;


		/// \attention must be called for proper behaviour of getDelta
		void update();

        mouse_glfw(decltype(m_pWindow) pWindow);
    };
}

#endif
