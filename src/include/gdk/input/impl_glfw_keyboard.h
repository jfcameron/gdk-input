// © Joseph Cameron - All Rights Reserved

#ifndef GDK_INPUT_KEYBOARD_GLFW_H
#define GDK_INPUT_KEYBOARD_GLFW_H

#include <gdk/input/context.h>
#include <gdk/input/impl_text_input.h>

#include <GLFW/glfw3.h>

#include <memory>
#include <set>
#include <unordered_map>

struct GLFWwindow;

namespace gdk::input {
    /// \brief glfw based implementation for keyboards
    /// \attention glfwPollEvents() must be called periodically in order to update the keyboard state
    class keyboard_glfw final {
    public:
		/// \param aTextFocus whether a text field has the keyboard
		/// \param aPressed the glfw keys pressed since the last update
		void update(const bool aTextFocus, const std::set<int> &aPressed = {});

        bool key_down(const keyboard::key &aKeyCode) const;
		bool key_just_down(const keyboard::key &aKeyCode) const;
		bool key_just_released(const keyboard::key &aKeyCode) const;

		keyboard_glfw(std::shared_ptr<GLFWwindow> pWindow);

    private:
        std::shared_ptr<GLFWwindow> m_pWindow;
		std::unordered_map<decltype(GLFW_KEY_Q), withheld_key> m_Keys;
		std::unordered_map<decltype(GLFW_KEY_Q), keyboard::key_state> m_CurrentState;
    };
}

#endif

