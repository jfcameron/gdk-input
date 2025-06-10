// © Joseph Cameron - All Rights Reserved

#ifndef GDK_INPUT_MOUSE_GLFW_H
#define GDK_INPUT_MOUSE_GLFW_H

#include <gdk/input_context.h>

#include <GLFW/glfw3.h>

#include <memory>
#include <unordered_map>

struct GLFWwindow;

namespace gdk {
    class mouse_glfw final {
    public:
		/// \attention must be called for proper behaviour of getDelta
		void update();

		bool button_down(const mouse::button aKeyCode) const;
		bool button_just_down(const mouse::button aKeyCode) const;
		bool button_just_released(const mouse::button aKeyCode) const;
		mouse::cursor_2d_type cursor_position() const;
		mouse::cursor_2d_type delta() const;
		void set_cursor_mode(const mouse::cursor_mode acursor_mode);
        mouse::cursor_mode get_cursor_mode() const;

        mouse_glfw(std::shared_ptr<GLFWwindow> pWindow);

    private:
		std::shared_ptr<GLFWwindow> m_pWindow;
        mouse::cursor_mode m_cursor_mode = mouse::cursor_mode::normal;
        mouse::cursor_2d_type m_LastDeltaCallCursorPosition = {0, 0};
		mouse::cursor_2d_type m_Delta = {0, 0};
		std::unordered_map<decltype(GLFW_MOUSE_BUTTON_1), decltype(GLFW_PRESS)> m_MouseButtonLastState;
		std::unordered_map<decltype(GLFW_MOUSE_BUTTON_1), mouse::button_state> m_CurrentState;
    };
}

#endif

