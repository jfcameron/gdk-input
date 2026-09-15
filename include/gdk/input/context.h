// © Joseph Cameron - All Rights Reserved

#ifndef GDK_INPUT_CONTEXT_H
#define GDK_INPUT_CONTEXT_H

#include <gdk/input/gamepad.h>
#include <gdk/input/text.h>
#include <gdk/input/types.h>

#include <memory>
#include <cstddef>
#include <optional>
#include <vector>
#include <string>

namespace gdk::input {

    class context {
    public:
		using gamepad_ptr = gamepad_ptr_type;
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

		/// \brief every key currently down
		[[nodiscard]] std::vector<keyboard::key> keys_down() const;

		/// \brief returns a key if one is down
		[[nodiscard]] std::optional<keyboard::key> any_key_down() const;
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

		//! every mouse button currently down. \see keys_down
		[[nodiscard]] std::vector<mouse::button> mouse_buttons_down() const;

		/// \brief returns a mouse button if one is down
		[[nodiscard]] std::optional<mouse::button> any_mouse_button_down() const;

		/// \brief get the cursor coordinates
		/// 
		/// cursor coordinates are normalized across the size of the window.
		/// 0,0 is bottom left, 1,1 is top right
		[[nodiscard]] virtual mouse::cursor_2d_type mouse_cursor_position() const = 0;

		/// \brief get the distance the cursor has moved since the last update
		[[nodiscard]] virtual mouse::cursor_2d_type mouse_delta() const = 0;

		/// \brief get the scroll wheel movement since the last update
		[[nodiscard]] virtual mouse::scroll_2d_type mouse_scroll_delta() const = 0;

		/// \brief returns a mouse axis if it moved further than aThreshold since the last update
		///
		/// Cursor motion is reported ahead of the wheel, and x ahead of y within each.
		[[nodiscard]] virtual std::optional<mouse::axis> any_mouse_axis_down(float threshold) const = 0;

		/// \brief gets the cursor mode
		[[nodiscard]] virtual mouse::cursor_mode mouse_cursor_mode() const = 0;

		/// \brief affects cursor behaviour
		virtual void set_mouse_cursor_mode(mouse::cursor_mode mode) = 0;
    ///@}

    /// \name text input
    /// Typing into a text field: what was typed, what an input method is composing, and the field
    /// telling the context it has the keyboard. \see text.h
    ///@{
    //
		/// \brief what was typed between the last two updates: text committed and editing keys, in order
		///
		/// Collected whether or not a field has focus; a field reads them while it does.
		[[nodiscard]] virtual const std::vector<text::event> &text_events() const = 0;

		/// \brief what an input method is composing now: empty when it is composing nothing
		[[nodiscard]] virtual text::composition text_composition() const = 0;

		/// \brief whether a text field has the keyboard
		[[nodiscard]] virtual bool text_input_focus() const = 0;

		/// \brief give the keyboard to a text field or hand it back
		///
		/// **While a field has it, every key reads as up** to everything but \ref text_events 
		///
		/// Handing the keyboard back abandons any composition in progress.
		virtual void set_text_input_focus(const bool aFocus) = 0;

		/// \brief tell the input method where the field's caret is, so its windows open beside it
		virtual void set_text_input_caret(const text::caret &aCaret) = 0;
    ///@}

    /// \name gamepad methods
    ///@{
    //
		/// \brief returns a ptr to a gamepad
		[[nodiscard]] virtual gamepad_ptr get_gamepad(const size_t index) = 0;

		/// \brief returns array of all gamepads
		[[nodiscard]] virtual gamepad_collection_type gamepads() = 0;

		/// \brief exchange the devices two players hold
		virtual void swap_players(const std::size_t aLeft, const std::size_t aRight) = 0;

		/// \brief teach the backend about controllers it does not recognise
		virtual std::size_t add_gamepad_mappings(const std::string &aMappings) = 0;
    ///@}

        virtual ~context() = default;

    protected:
        context() = default;
    };
};


#endif

