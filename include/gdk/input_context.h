// © 2020 Joseph Cameron - All Rights Reserved

#ifndef GDK_INPUT_CONTEXT_H
#define GDK_INPUT_CONTEXT_H

#include <memory>
#include <optional>

#include <gdk/keyboard.h>
#include <gdk/mouse.h>
#include <gdk/gamepad.h>

struct GLFWwindow;

namespace gdk::input
{
    /// \brief intermediary between the user and the underlying implementation.
    /// provides common interface and manages state
    class context 
    {
    public:
		/// \brief ptr type returned by factory method
		using context_ptr_type = std::unique_ptr<gdk::input::context>;

		/// \brief shared ptr to a context
		using context_shared_ptr_type = std::shared_ptr<gdk::input::context>;

		/// \brief shared ptr to a gamepad
		using gamepad_ptr = std::shared_ptr<gamepad>;

		/// \brief collection of gamepads
		using gamepad_collection_type = std::vector<gamepad_ptr>;

		/// \brief create a context from a glfw window
		/// \warn the implementation does not have any ownership over the pointer. It is up to the user to guarantee
		/// the glfw window's lifetime exceeds the context
		[[nodiscard]] static context_ptr_type make(GLFWwindow* const apGLFWWindow);

		/// \brief check if a key is down (held or just pressed)
		[[nodiscard]] virtual bool get_key_down(const keyboard::Key& aKeyCode) const = 0;

		/// \brief check if a key was just pressed
		[[nodiscard]] virtual bool get_key_just_pressed(const keyboard::Key& aKeyCode) const = 0;

		/// \brief check if the key was just released
		[[nodiscard]] virtual bool get_key_just_released(const keyboard::Key& aKeyCode) const = 0;

		/// \brief returns a key if one is down
		[[nodiscard]] virtual std::optional<keyboard::Key> get_any_key_down() const = 0;

		/// \brief  check if a mouse button is down (held or just pressed)
		[[nodiscard]] virtual bool get_mouse_button_down(const mouse::Button& aButton) const = 0;

		/// \brief check if a mouse button was just pressed
		[[nodiscard]] virtual bool get_mouse_button_just_pressed(const mouse::Button& aButton) const = 0;

		/// \brief check if a mouse button was just released
		[[nodiscard]] virtual bool get_mouse_button_just_released(const mouse::Button& aButton) const = 0;

		/// \brief returns a mouse button if one is down
		[[nodiscard]] virtual std::optional<mouse::Button> get_any_mouse_button_down() const = 0;

		/// \brief get the cursor coordinates
		/// 
		/// cursor coordinates are normalized across the size of the window.
		/// 0,0 is bottom left, 1,1 is top right
		[[nodiscard]] virtual mouse::cursor_2d_type get_mouse_cursor_position() const = 0;

		/// \brief get the distance the cursor has moved since last delta in unnormalized screen coordinates
		/// this value is still reliable in cursor locked mode
		/// the value is multiplied by a scalar set via set_mouse_delta_sensitivity
		[[nodiscard]] virtual mouse::cursor_2d_type get_mouse_delta() const = 0;

		/// \brief returns a mouse axis if one is nonzero.
		[[nodiscard]] virtual std::optional<mouse::Axis> get_any_mouse_axis_down(float threshold) const = 0;

		/// \brief gets the cursor mdoe
		[[nodiscard]] virtual mouse::CursorMode get_mouse_cursor_mode() const = 0;

		/// \brief affects cursor behaviour
		virtual void set_mouse_cursor_mode(mouse::CursorMode mode) = 0;

		/// \brief affects the values returned by get_mouse_delta()
		virtual void set_mouse_delta_sensitivity(const double sens) = 0;

		/// \brief returns a ptr to a gamepad
		///
		/// the order of gamepads is the order in which they were connected to the device
		/// the index refers to a poisition in that order.
		/// the gamepad ptr is guaranteed not to be null, even if there are less than N gamepads connected,
		/// such a gamepad will simply return 0 for all input polling.
		[[nodiscard]] virtual gamepad_ptr get_gamepad(const size_t index) = 0;

		/// \brief returns array of all gamepads
		[[nodiscard]] virtual gamepad_collection_type get_gamepads() = 0;

		/// \brief returns maximum number of connected gamepads this library can handle
		//[[nodiscard]] virtual size_t get_gamepad_maximum_count() const = 0;

		/// \brief returns the number of gamepads currently connected to the device
		///
		//[[nodiscard]] virtual size_t get_gamepad_connected_count() = 0;

		/// \brief must be called in your loop
		virtual void update() = 0;

        /// \brief virtual destructor
        virtual ~context() = default;

    protected:
        /// \brief prevent instantiation
        context() = default;
    };
};

#endif
