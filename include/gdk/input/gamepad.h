// © Joseph Cameron - All Rights Reserved

#ifndef GDK_INPUT_GAMEPAD_H
#define GDK_INPUT_GAMEPAD_H

#include <cstddef>
#include <memory>
#include <iosfwd>
#include <optional>
#include <string>
#include <vector>

namespace gdk::input {
    /// \brief gamepad interface
    class gamepad {
    public:
		using index_type = short unsigned int;
		using button_collection_type = std::vector<bool>;
		using axis_value_type = float;

        /// \brief a control on the standard gamepad layout
        enum class button {
            a, b, x, y,
            left_bumper, right_bumper,
            back, start, guide,
            left_thumb, right_thumb,
            dpad_up, dpad_right, dpad_down, dpad_left
        };

        /// \brief a continuous control on the standard gamepad layout
        enum class axis {
            left_x, left_y,
            right_x, right_y,
            left_trigger, right_trigger
        };

        /// \brief where a hat is pointing
        struct hat_state_type final {
            enum class horizontal { left, centre, right };
            enum class vertical { up, centre, down };

            horizontal x{horizontal::centre};
            vertical y{vertical::centre};

            [[nodiscard]] bool centred() const {
                return x == horizontal::centre && y == vertical::centre;
            }
        };

        /// \brief check if the gamepad is connected
        [[nodiscard]] virtual bool connected() const = 0;

        /// \brief whether this device's buttons and axes are in the standard gamepad layout
        [[nodiscard]] virtual bool has_standard_mapping() const = 0;

        /// \brief how many hats the device reports; always zero when \ref has_standard_mapping
        [[nodiscard]] virtual std::size_t button_count() const = 0;

        //! how many axes this device has. \see button_count
        [[nodiscard]] virtual std::size_t axis_count() const = 0;

        [[nodiscard]] virtual std::size_t hat_count() const = 0;

        /// \brief where a hat is pointing; centred for an index the device does not have
        [[nodiscard]] virtual hat_state_type hat(const index_type index) const = 0;

        /// \brief human readable device name
        [[nodiscard]] virtual std::string_view name() const = 0;

        /// \brief identity of the attached device; empty when nothing is attached
        [[nodiscard]] virtual std::string_view guid() const = 0;

        /// \brief a device was attached to this player during the last update
        [[nodiscard]] virtual bool just_connected() const = 0;

        /// \brief the device attached to this player went away during the last update
        [[nodiscard]] virtual bool just_disconnected() const = 0;

		/// \brief normalized magnitude of axis. 0 = no input, 1 = full input
		[[nodiscard]] virtual axis_value_type axis_value(const index_type index, const axis_value_type threshold = 0.1f) const = 0;

		/// \brief the axis left its dead zone during the last update
		[[nodiscard]] virtual bool axis_just_exceeded_threshold(const index_type index, const axis_value_type threshold = 0.1f) const = 0;

		/// \brief the axis returned to its dead zone during the last update
		[[nodiscard]] virtual bool axis_just_dropped_below_threshold(const index_type index, const axis_value_type threshold = 0.1f) const = 0;

		/// \brief what \ref axis_value read before the last update
		[[nodiscard]] virtual axis_value_type previous_axis_value(const index_type index, const axis_value_type threshold = 0.1f) const = 0;

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

        [[nodiscard]] bool button_down(const button aButton) const {
            return has_standard_mapping() && button_down(static_cast<index_type>(aButton));
        }

        [[nodiscard]] bool button_just_pressed(const button aButton) const {
            return has_standard_mapping() && button_just_pressed(static_cast<index_type>(aButton));
        }

        [[nodiscard]] bool button_just_released(const button aButton) const {
            return has_standard_mapping() && button_just_released(static_cast<index_type>(aButton));
        }

        [[nodiscard]] axis_value_type previous_axis_value(const axis aAxis,
            const axis_value_type threshold = 0.1f) const {
            return previous_axis_value(static_cast<index_type>(aAxis), threshold);
        }

        [[nodiscard]] axis_value_type axis_value(const axis aAxis,
            const axis_value_type threshold = 0.1f) const {
            return has_standard_mapping()
                ? axis_value(static_cast<index_type>(aAxis), threshold)
                : 0;
        }

        [[nodiscard]] bool axis_just_exceeded_threshold(const axis aAxis,
            const axis_value_type threshold = 0.1f) const {
            return has_standard_mapping()
                && axis_just_exceeded_threshold(static_cast<index_type>(aAxis), threshold);
        }

        [[nodiscard]] bool axis_just_dropped_below_threshold(const axis aAxis,
            const axis_value_type threshold = 0.1f) const {
            return has_standard_mapping()
                && axis_just_dropped_below_threshold(static_cast<index_type>(aAxis), threshold);
        }

        virtual ~gamepad() = default;
    };

}

std::ostream& operator<<(std::ostream&, const gdk::input::gamepad::button);
std::ostream& operator<<(std::ostream&, const gdk::input::gamepad::axis);

#endif
