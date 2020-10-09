// © 2020 Joseph Cameron - All Rights Reserved

#ifndef GDK_GLFW_INPUT_CONTEXT_H
#define GDK_GLFW_INPUT_CONTEXT_H

#include <gdk/input_context.h>
#include <gdk/keyboard_glfw.h>
#include <gdk/mouse_glfw.h>
#include<gdk/gamepad_glfw.h>

#include <array>

namespace gdk::input
{
    /// \brief intermediary between the user and the underlying implementation.
    /// provides common interface and manages state
    class glfw_context final : public gdk::input::context
    {
		using glfw_gamepad_ptr = std::shared_ptr<gamepad_glfw>;

		/// \brief non-owning ptr to a glfw window
		GLFWwindow *const m_pGLFWWindow;

		/// \brief keyboard instance
		keyboard_glfw m_Keyboard;

		/// \brief mouse instance
		mouse_glfw m_Mouse;

		/// \brief array of total possible gamepads connected to the device
		std::array<glfw_gamepad_ptr, 1 + GLFW_JOYSTICK_LAST - GLFW_JOYSTICK_1> m_Gamepads;

    public:
		[[nodiscard]] virtual bool get_key_down(const keyboard::Key& aKeyCode) const override;

		[[nodiscard]] virtual bool get_key_just_pressed(const keyboard::Key& aKeyCode) const override;

		[[nodiscard]] virtual bool get_key_just_released(const keyboard::Key& aKeyCode) const override;

		
		[[nodiscard]] virtual bool get_mouse_button_down(const mouse::Button& aButton) const override;

		[[nodiscard]] virtual bool get_mouse_button_just_pressed(const mouse::Button& aButton) const override;

		[[nodiscard]] virtual bool get_mouse_button_just_released(const mouse::Button& aButton) const override;

		[[nodiscard]] virtual mouse::cursor_2d_type get_mouse_cursor_position() const override;

		[[nodiscard]] virtual mouse::cursor_2d_type get_mouse_delta() const override;
		
		[[nodiscard]] virtual mouse::CursorMode get_mouse_cursor_mode() const override;

		virtual void set_mouse_cursor_mode(mouse::CursorMode mode) override;

		virtual void set_mouse_delta_sensitivity(const double sens) override;


		virtual gamepad_ptr get_gamepad(const size_t index) override;

		//virtual size_t get_gamepad_maximum_count() const override;

		/// \attention must be called in your update loop
		virtual void update() override;

		//! ctor
		glfw_context(GLFWwindow* const apGLFWWindow);

		//! default dtor
		virtual ~glfw_context() override = default;
    };
};

#endif

