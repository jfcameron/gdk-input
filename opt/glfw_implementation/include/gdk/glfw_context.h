// © 2020 Joseph Cameron - All Rights Reserved

#ifndef GDK_GLFW_INPUT_CONTEXT_H
#define GDK_GLFW_INPUT_CONTEXT_H

#include <gdk/input_context.h>

#include <gdk/keyboard_glfw.h>
#include <gdk/mouse_glfw.h>

namespace gdk::input
{
    /// \brief intermediary between the user and the underlying implementation.
    /// provides common interface and manages state
    class glfw_context final : public gdk::input::context
    {
		GLFWwindow *const m_pGLFWWindow;

		keyboard_glfw m_Keyboard;

    public:
		virtual bool get_key_down(const keyboard::Key& aKeyCode) const override;

		virtual bool get_key_just_pressed(const keyboard::Key& aKeyCode) const override;

	 	glfw_context(GLFWwindow *const apGLFWWindow);

		virtual void update() override;
		
		//! default dtor
		virtual ~glfw_context() override = default;
    };
};

#endif

