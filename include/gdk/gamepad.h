// © 2019 Joseph Cameron - All Rights Reserved

#ifndef GDK_GAMEPAD_H
#define GDK_GAMEPAD_H

#include <memory>
#include <string>
#include <vector>
#include <optional>

namespace gdk
{
    /// \brief gamepad interface
    class gamepad
    {
    public:
        using button_state_type = bool;

		using index_type = short unsigned int;

		using button_collection_type = std::vector<button_state_type>;

		using axis_value_type = float;
        
        //! hat direction
        /// left -1, right +1, up +1, down -1 TODO: I have changed my mind. dont like it. enum
        struct hat_state_type
        {
            enum class vertical_direction
            {
                Up, Down, Center
            };

            enum class horizontal_direction
            {
                Left, Right, Center
            };

            horizontal_direction x;
            vertical_direction y;
        };

        //! human readable device name
        virtual std::string_view get_name() const = 0;
        
        //! normalized magnitude of axis. 0 = no input, 1 = full input
        virtual axis_value_type get_axis(index_type index, axis_value_type threshold = 0.1f) const = 0;

        //! whether or not the button is being held down by the user
        virtual button_state_type get_button_down(index_type index) const = 0;

        //! value of a hat direction by index
        virtual hat_state_type get_hat(index_type index) const = 0;

		//! returns a button if any button is down, otherwise the optional will be null
		/// \warning If multiple buttons are down, the first found is returned. Order should be considered arbitrary
		virtual std::optional<button_collection_type::size_type> get_any_button_down() const = 0;

		//! returns a hat and direction if a hat is down, otherwise the optional will be null
		/// \warning If multiple hats are down, the first found is returned. Order should be considered arbitrary
		virtual std::optional<std::pair<index_type, hat_state_type>> get_any_hat_down() const = 0;

		//! returns an axis if one is down, otherwise the optional will be null
		/// \warning If multiple axes are down, the first found is returned. Order should be considered arbitrary
		virtual std::optional<std::pair<index_type, axis_value_type>> get_any_axis_down(axis_value_type threshold = 0.1f) const = 0;

		//! return an axis if any are 

        virtual ~gamepad() = default;
    };

}

std::ostream& operator<<(std::ostream&, const gdk::gamepad::hat_state_type::vertical_direction);
std::ostream& operator<<(std::ostream&, const gdk::gamepad::hat_state_type::horizontal_direction);

#endif
