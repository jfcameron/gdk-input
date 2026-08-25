// © Joseph Cameron - All Rights Reserved

#ifndef GDK_GAMEPAD_GLFW_H
#define GDK_GAMEPAD_GLFW_H

#include <gdk/input/gamepad.h>

#include <GLFW/glfw3.h>

#include <optional>
#include <string>

namespace gdk::input {
    /// \brief gamepad implementation for GLFW3
    /// \attention update method be called to update gamepad state. This is not handled by glfwPollEvents
    class gamepad_glfw final : public gamepad {
    public:
        /// \brief updates the state of this gamepad
        /// \attention must be called in a game loop
        void update();

        virtual bool connected() const override;
        virtual std::string_view guid() const override;
        virtual bool just_connected() const override;
        virtual bool just_disconnected() const override;
        virtual bool has_standard_mapping() const override;
        virtual std::size_t button_count() const override;
        virtual std::size_t axis_count() const override;
        virtual std::size_t hat_count() const override;
        virtual hat_state_type hat(const index_type index) const override;
        virtual axis_value_type axis_value(index_type index, axis_value_type threshold) const override;
		virtual bool axis_just_exceeded_threshold(const index_type index, const axis_value_type threshold) const override;
		virtual bool axis_just_dropped_below_threshold(const index_type index, const axis_value_type threshold) const override;
		virtual axis_value_type previous_axis_value(const index_type index, const axis_value_type threshold) const override;
		virtual std::optional<std::pair<index_type, axis_value_type>> any_axis_down(axis_value_type threshold) const override;
        virtual bool button_down(const index_type index) const override;
		virtual bool button_just_pressed(const index_type index) const override;
		virtual bool button_just_released(const index_type index) const override;
        virtual std::string_view name() const override;
		virtual std::optional<button_collection_type::size_type> any_button_down() const override;

        /// \brief a player's gamepad, holding no device until one is attached
        gamepad_glfw();

        //! a device appeared at this joystick id and belongs to this player
        void attach(const int aJoystickIndex, const std::string &aGuid);

        //! the device went away; the guid stays so the player can be found again
        void detach();

        //! the joystick id currently feeding this player, if any
        [[nodiscard]] std::optional<int> joystick_index() const;

        //! the joystick id this player's device last arrived on, kept across a disconnect
        [[nodiscard]] std::optional<int> last_joystick_index() const;

    private:
		enum class button_state {
			UP,
			JUST_PRESSED,
			JUST_RELEASED,
			HELD_DOWN,
		};

		std::optional<int> m_JoystickIndex;
		std::optional<int> m_LastJoystickIndex;
		std::string m_Guid;
		bool m_JustConnected{false};
		bool m_JustDisconnected{false};
        std::string m_Name = "undefined";
        std::vector<button_state> m_Buttons;
        std::vector<axis_value_type> m_Axes;
		std::vector<axis_value_type> m_LastAxes;
		std::vector<hat_state_type> m_Hats;
		bool m_HasStandardMapping{false};
    };
}

#endif

