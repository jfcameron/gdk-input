// © Joseph Cameron - All Rights Reserved

#ifndef GDK_INPUT_CONTEXT_H
#define GDK_INPUT_CONTEXT_H

#include <gdk/gamepad.h>

#include <memory>
#include <optional>

struct GLFWwindow;

namespace gdk::mouse {
    enum class button { 
        left, 
        right,
        middle,
        four, 
        five,
        six,
        seven,
        eight
    };

    enum class button_state {
        up,
        just_pressed, 
        just_released,
        held_down 
    };

    enum class cursor_mode {
        normal,
        hidden,
        locked
    };

    enum class axis {
        x, y
    };
    
    struct cursor_2d_type {
        double x, y;
    };
}

namespace gdk::keyboard {
    enum class key_state {
        up,
        just_pressed, 
        just_released,
        held_down 
    };

    enum class key {
        escape, 
        f1, f2, f3, f4, f5, f6, f7, f8, f9, f10, f11, f12, 
        printscreen, scrolllock, pausebreak, 
        q, w, e, r, t, y, u, i, o, p, a, s, d, f, g, h, j, k, l, z, x, c, v, b, n, m, 
        one, two, three, four, five, six, seven, eight, nine, zero, 
        tilda, minus, equals, backspace, home, end, 
        tab, openbracket, closebracket, backslash, insert, pageup, 
        capslock, semicolon, quote, enter, deletekey, pagedown, 
        leftshift, comma, period, forwardslash, rightshift, 
        leftcontrol, leftalt, space, rightalt, rightcontrol,
        leftarrow, rightarrow, uparrow, downarrow, 
        numlock, numslash, numasterisk, numminus, 
        num7, num8, num9, numplus, 
        num4, num5, num6, 
        num1, num2, num3, numenter, 
        num0, numperiod
    };
}

namespace gdk::input {
    class context;
    using context_ptr_type = std::shared_ptr<gdk::input::context>;

    /// \brief intermediary between the user and the underlying implementation.
    /// provides common interface and manages state
    class context {
    public:
		using gamepad_ptr = std::shared_ptr<gamepad>;
		using gamepad_collection_type = std::vector<gamepad_ptr>;

    /// \name keyboard methods
    ///@{
    //
		/// \brief check if a key is down (held or just pressed)
		[[nodiscard]] virtual bool key_down(const keyboard::key& aKeyCode) const = 0;

		/// \brief check if a key was just pressed
		[[nodiscard]] virtual bool key_just_pressed(const keyboard::key& aKeyCode) const = 0;

		/// \brief check if the key was just released
		[[nodiscard]] virtual bool key_just_released(const keyboard::key& aKeyCode) const = 0;

		/// \brief returns a key if one is down
		[[nodiscard]] virtual std::optional<keyboard::key> any_key_down() const = 0;
    ///@}

    /// \name mouse methods
    ///@{
    //
		/// \brief  check if a mouse button is down (held or just pressed)
		[[nodiscard]] virtual bool mouse_button_down(const mouse::button &aButton) const = 0;

		/// \brief check if a mouse button was just pressed
		[[nodiscard]] virtual bool mouse_button_just_pressed(const mouse::button &aButton) const = 0;

		/// \brief check if a mouse button was just released
		[[nodiscard]] virtual bool mouse_button_just_released(const mouse::button &aButton) const = 0;

		/// \brief returns a mouse button if one is down
		[[nodiscard]] virtual std::optional<mouse::button> any_mouse_button_down() const = 0;

		/// \brief get the cursor coordinates
		/// 
		/// cursor coordinates are normalized across the size of the window.
		/// 0,0 is bottom left, 1,1 is top right
		[[nodiscard]] virtual mouse::cursor_2d_type mouse_cursor_position() const = 0;

		/// \brief get the distance the cursor has moved since last delta in unnormalized screen coordinates
		/// this value is still reliable in cursor locked mode
		[[nodiscard]] virtual mouse::cursor_2d_type mouse_delta() const = 0;

		/// \brief returns a mouse axis if one is nonzero.
		[[nodiscard]] virtual std::optional<mouse::axis> any_mouse_axis_down(float threshold) const = 0;

		/// \brief gets the cursor mode
		[[nodiscard]] virtual mouse::cursor_mode mouse_cursor_mode() const = 0;

		/// \brief affects cursor behaviour
		virtual void set_mouse_cursor_mode(mouse::cursor_mode mode) = 0;
    ///@}

    /// \name gamepad methods
    ///@{
    //
		/// \brief returns a ptr to a gamepad
		///
		/// the order of gamepads is the order in which they were connected to the device
		/// the index refers to a poisition in that order.
		/// the gamepad ptr is guaranteed not to be null, even if there are less than N gamepads connected,
		/// such a gamepad will simply return 0 for all input polling.
		[[nodiscard]] virtual gamepad_ptr get_gamepad(const size_t index) = 0;

		/// \brief returns array of all gamepads
		[[nodiscard]] virtual gamepad_collection_type gamepads() = 0;
    ///@}

        /// \brief virtual destructor
        virtual ~context() = default;

    protected:
        /// \brief prevent instantiation
        context() = default;
    };
};

std::ostream& operator<<(std::ostream&, const gdk::keyboard::key);
std::ostream& operator<<(std::ostream&, const gdk::keyboard::key_state);
std::ostream& operator<<(std::ostream&, const gdk::mouse::axis);
std::ostream& operator<<(std::ostream&, const gdk::mouse::button);
std::ostream& operator<<(std::ostream&, const gdk::mouse::button_state);
std::ostream& operator<<(std::ostream&, const gdk::mouse::cursor_mode);

#endif

