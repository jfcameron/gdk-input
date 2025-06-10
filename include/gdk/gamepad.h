// © Joseph Cameron - All Rights Reserved

#ifndef GDK_GAMEPAD_H
#define GDK_GAMEPAD_H

#include <memory>
#include <optional>
#include <string>
#include <vector>

namespace gdk {
    /// \brief gamepad interface
    class gamepad {
    public:
		using index_type = short unsigned int;
		using button_collection_type = std::vector<bool>;
		using axis_value_type = float;

        /// \brief check if the gamepad is connected
        [[nodiscard]] virtual bool connected() const = 0;

        /// \brief human readable device name
        [[nodiscard]] virtual std::string_view name() const = 0;

		/// \brief normalized magnitude of axis. 0 = no input, 1 = full input
		[[nodiscard]] virtual axis_value_type axis(const index_type index, const axis_value_type threshold = 0.1f) const = 0;

		/// \brief true if just exceeded threshold
		[[nodiscard]] virtual bool axis_just_exceeded_threshold(const index_type index, const axis_value_type threshold = 0.1f) const = 0;

		/// \brief true if just dropped below threshold
		[[nodiscard]] virtual bool axis_just_dropped_below_threshold(const index_type index, const axis_value_type threshold = 0.1f) const = 0;

		/// \brief returns an axis if one is down, otherwise the optional will be null
		/// \warning If multiple axes are down, the first found is returned. Order should be considered arbitrary
		[[nodiscard]] virtual std::optional<std::pair<index_type, axis_value_type>> any_axis_down(axis_value_type threshold = 0.1f) const = 0;

        /// \brief whether or not the button is being held down by the user
        [[nodiscard]] virtual bool button_down(const index_type index) const = 0;

		/// \brief whether or not the button was just pressed this frame
		[[nodiscard]] virtual bool button_just_pressed(const index_type index) const = 0;

		/// \brief whether or not the button was just released this frame
		[[nodiscard]] virtual bool button_just_released(const index_type index) const = 0;

		/// \brief returns a button if any button is down, otherwise the optional will be null
		/// \warning If multiple buttons are down, the first found is returned. Order should be considered arbitrary
		[[nodiscard]] virtual std::optional<button_collection_type::size_type> any_button_down() const = 0;

        virtual ~gamepad() = default;
    };

}

#endif
