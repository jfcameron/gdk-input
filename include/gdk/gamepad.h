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
		/// \brief type used to index buttons, axes
		using index_type = short unsigned int;

		/// \brief type used to store a collection of buttons
		using button_collection_type = std::vector<bool>;

		/// \brief precision type used to store value of an axis
		using axis_value_type = float;

        /// \brief human readable device name
        virtual std::string_view get_name() const = 0;

		/// \brief normalized magnitude of axis. 0 = no input, 1 = full input
		[[nodiscard]] virtual axis_value_type get_axis(const index_type index, const axis_value_type threshold = 0.1f) const = 0;

		/// \brief true if just exceeded threshold
		[[nodiscard]] virtual bool get_axis_just_exceeded_threshold(const index_type index, const axis_value_type threshold = 0.1f) const = 0;

		/// \brief true if just dropped below threshold
		[[nodiscard]] virtual bool get_axis_just_dropped_below_threshold(const index_type index, const axis_value_type threshold = 0.1f) const = 0;

		/// \brief returns an axis if one is down, otherwise the optional will be null
		/// \warning If multiple axes are down, the first found is returned. Order should be considered arbitrary
		[[nodiscard]] virtual std::optional<std::pair<index_type, axis_value_type>> get_any_axis_down(axis_value_type threshold = 0.1f) const = 0;

        /// \brief whether or not the button is being held down by the user
        [[nodiscard]] virtual bool get_button_down(const index_type index) const = 0;

		/// \brief whether or not the button was just pressed this frame
		[[nodiscard]] virtual bool get_button_just_pressed(const index_type index) const = 0;

		/// \brief whether or not the button was just released this frame
		[[nodiscard]] virtual bool get_button_just_released(const index_type index) const = 0;

		/// \brief returns a button if any button is down, otherwise the optional will be null
		/// \warning If multiple buttons are down, the first found is returned. Order should be considered arbitrary
		[[nodiscard]] virtual std::optional<button_collection_type::size_type> get_any_button_down() const = 0;

        virtual ~gamepad() = default;
    };

}

#endif
