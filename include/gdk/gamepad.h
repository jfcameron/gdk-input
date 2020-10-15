// © 2019, 2020 Joseph Cameron - All Rights Reserved

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
		using index_type = short unsigned int;

		using button_collection_type = std::vector<bool>;

		using axis_value_type = float;

        //! human readable device name
        virtual std::string_view get_name() const = 0;
        
        //! normalized magnitude of axis. 0 = no input, 1 = full input
        virtual axis_value_type get_axis(index_type index, axis_value_type threshold = 0.1f) const = 0;

		//! true if just exceeded threshold
		//bool get_axis_just_exceeded_threshold(index, threshold)

		//! true if just dropped below threshold
		//bool get_axis_just_dropped_below_threshold(index, threshold)

        //! whether or not the button is being held down by the user
        [[nodiscard]] virtual bool get_button_down(const index_type index) const = 0;

		//! whether or not the button was just pressed this frame
		[[nodiscard]] virtual bool get_button_just_pressed(const index_type index) const = 0;

		//! whether or not the button was just released this frame
		[[nodiscard]] virtual bool get_button_just_released(const index_type index) const = 0;

		//! returns a button if any button is down, otherwise the optional will be null
		/// \warning If multiple buttons are down, the first found is returned. Order should be considered arbitrary
		virtual std::optional<button_collection_type::size_type> get_any_button_down() const = 0;

		//! returns a hat and direction if a hat is down, otherwise the optional will be null
		/// \warning If multiple hats are down, the first found is returned. Order should be considered arbitrary
		//virtual std::optional<std::pair<index_type, hat_state_type>> get_any_hat_down() const = 0;

		//! returns an axis if one is down, otherwise the optional will be null
		/// \warning If multiple axes are down, the first found is returned. Order should be considered arbitrary
		virtual std::optional<std::pair<index_type, axis_value_type>> get_any_axis_down(axis_value_type threshold = 0.1f) const = 0;

        virtual ~gamepad() = default;
    };

}

#endif
