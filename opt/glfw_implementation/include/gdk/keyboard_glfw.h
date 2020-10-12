// © 2019 Joseph Cameron - All Rights Reserved

#ifndef GDK_INPUT_KEYBOARD_GLFW_H
#define GDK_INPUT_KEYBOARD_GLFW_H

#include <memory>

#include <gdk/keyboard.h>

#include <GLFW/glfw3.h>

#include <unordered_map>

struct GLFWwindow;

namespace gdk
{
    /// \brief glfw based implementation for keyboards
    /// \attention glfwPollEvents() must be called periodically in order to update the keyboard state
    class keyboard_glfw : public keyboard
    {
        GLFWwindow *const m_pWindow;

		std::unordered_map<decltype(GLFW_KEY_Q), decltype(GLFW_PRESS)> m_KeyboardLastState;

		std::unordered_map<decltype(GLFW_KEY_Q), keyboard::Keystate> m_CurrentState;

    public:
		virtual std::optional<keyboard::Key> getAnyKeyDown() const override;

        virtual bool getKeyDown(const keyboard::Key &aKeyCode) const override;

		virtual bool getKeyJustDown(const keyboard::Key& aKeyCode) const override;

		virtual bool getKeyJustReleased(const keyboard::Key& aKeyCode) const override;

		void update();

		keyboard_glfw(decltype(m_pWindow) pWindow);
    };
}

#endif

