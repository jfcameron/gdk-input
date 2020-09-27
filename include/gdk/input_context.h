// © 2020 Joseph Cameron - All Rights Reserved

#ifndef GDK_INPUT_CONTEXT_H
#define GDK_INPUT_CONTEXT_H

#include <memory>

#include <gdk/keyboard.h>
#include <gdk/mouse.h>

struct GLFWwindow;

namespace gdk::input
{
    /// \brief intermediary between the user and the underlying implementation.
    /// provides common interface and manages state
    class context 
    {
    public:
		//! ptr type returned by factory method
		using context_ptr_type = std::unique_ptr<gdk::input::context>;

		//! shared ptr to a context
		using context_shared_ptr_type = std::shared_ptr<gdk::input::context>;

		//! create a context from a glfw window
		/// \warn the implementation does not have any ownership over the pointer. It is up to the user to guarantee
		/// the glfw window's lifetime exceeds the context
		static context_ptr_type make(GLFWwindow* const apGLFWWindow);

		//! check if a key is down (held or just pressed)
		virtual bool get_key_down(const keyboard::Key& aKeyCode) const = 0;

		//! check if a key was just pressed
		virtual bool get_key_just_pressed(const keyboard::Key& aKeyCode) const = 0;

		// check if the key was just released
		virtual bool get_key_just_released(const keyboard::Key& aKeyCode) const = 0;

		//! must be called in your loop
		virtual void update() = 0;

        //! virtual destructor
        virtual ~context() = default;

    protected:
        //! prevent instantiation
        context() = default;
    };
};

#endif
