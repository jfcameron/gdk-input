// © Joseph Cameron - All Rights Reserved

#ifndef GDK_INPUT_KEYBOARD_GLFW_H
#define GDK_INPUT_KEYBOARD_GLFW_H

#include <gdk/input_context.h>

#include <GLFW/glfw3.h>

#include <memory>
#include <unordered_map>

struct GLFWwindow;

namespace gdk {
    /// \brief glfw based implementation for keyboards
    /// \attention glfwPollEvents() must be called periodically in order to update the keyboard state
    class keyboard_glfw final {
    public:
		void update();

		std::optional<keyboard::key> any_key_down() const;
        bool key_down(const keyboard::key &aKeyCode) const;
		bool key_just_down(const keyboard::key &aKeyCode) const;
		bool key_just_released(const keyboard::key &aKeyCode) const;

		keyboard_glfw(std::shared_ptr<GLFWwindow> pWindow);

    private:
        std::shared_ptr<GLFWwindow> m_pWindow;
		std::unordered_map<decltype(GLFW_KEY_Q), decltype(GLFW_PRESS)> m_KeyboardLastState;
		std::unordered_map<decltype(GLFW_KEY_Q), keyboard::key_state> m_CurrentState;
    };
}

#endif

