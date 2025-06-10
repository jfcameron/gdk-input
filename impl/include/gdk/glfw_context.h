// © Joseph Cameron - All Rights Reserved

#ifndef GDK_GLFW_INPUT_CONTEXT_H
#define GDK_GLFW_INPUT_CONTEXT_H

#include <gdk/glfw_gamepad.h>
#include <gdk/glfw_keyboard.h>
#include <gdk/glfw_mouse.h>
#include <gdk/input_context.h>

#include <array>

namespace gdk::input {
    class glfw_context final : public gdk::input::context {
		using glfw_gamepad_ptr = std::shared_ptr<gamepad_glfw>;
        using glfw_window_ptr = std::shared_ptr<GLFWwindow>;
    public:
		/// \brief create a context from a glfw window
		/// \warn the implementation does not have any ownership over the pointer. It is up to the user to guarantee
		/// the glfw window's lifetime exceeds the context
		[[nodiscard]] static context_ptr_type make(std::shared_ptr<GLFWwindow> apGLFWWindow);
		/// \attention must be called in your update loop
		void update(); 

		[[nodiscard]] virtual std::optional<keyboard::key> any_key_down() const override;
		[[nodiscard]] virtual bool key_down(const keyboard::key& aKeyCode) const override;
		[[nodiscard]] virtual bool key_just_pressed(const keyboard::key &aKeyCode) const override;
		[[nodiscard]] virtual bool key_just_released(const keyboard::key &aKeyCode) const override;

		[[nodiscard]] virtual bool mouse_button_down(const mouse::button &aButton) const override;
		[[nodiscard]] virtual bool mouse_button_just_pressed(const mouse::button &aButton) const override;
		[[nodiscard]] virtual bool mouse_button_just_released(const mouse::button &aButton) const override;
		[[nodiscard]] virtual std::optional<mouse::button> any_mouse_button_down() const override;
		[[nodiscard]] virtual std::optional<mouse::axis> any_mouse_axis_down(float threshold) const override;
		[[nodiscard]] virtual mouse::cursor_2d_type mouse_cursor_position() const override;
		[[nodiscard]] virtual mouse::cursor_2d_type mouse_delta() const override;
		[[nodiscard]] virtual mouse::cursor_mode mouse_cursor_mode() const override;
		virtual void set_mouse_cursor_mode(mouse::cursor_mode mode) override;

		[[nodiscard]] virtual gamepad_ptr get_gamepad(const size_t index) override;
		[[nodiscard]] virtual gamepad_collection_type gamepads() override;

		glfw_context(std::shared_ptr<GLFWwindow> apGLFWWindow);
		virtual ~glfw_context() override = default;
    private:
		keyboard_glfw m_Keyboard;
		mouse_glfw m_Mouse;
		std::array<glfw_gamepad_ptr, GLFW_JOYSTICK_LAST + 1> m_Gamepads;
    };
};

#endif

