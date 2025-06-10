// © Joseph Cameron - All Rights Reserved

#ifndef GDK_GAMEPAD_GLFW_H
#define GDK_GAMEPAD_GLFW_H

#include <gdk/gamepad.h>

#include <GLFW/glfw3.h>

namespace gdk {
    /// \brief gamepad implementation for GLFW3
    /// \attention update method be called to update gamepad state. This is not handled by glfwPollEvents
    class gamepad_glfw final : public gamepad {
    public:
        /// \brief updates the state of this gamepad
        /// \attention must be called in a game loop
        void update();

        virtual bool connected() const override;
        virtual axis_value_type axis(index_type index, axis_value_type threshold) const override;
		virtual bool axis_just_exceeded_threshold(const index_type index, const axis_value_type threshold) const override;
		virtual bool axis_just_dropped_below_threshold(const index_type index, const axis_value_type threshold) const override;
		virtual std::optional<std::pair<index_type, axis_value_type>> any_axis_down(axis_value_type threshold) const override;
        virtual bool button_down(const index_type index) const override;
		virtual bool button_just_pressed(const index_type index) const override;
		virtual bool button_just_released(const index_type index) const override;
        virtual std::string_view name() const override;
		virtual std::optional<button_collection_type::size_type> any_button_down() const override;

        gamepad_glfw(const int joystickID);

    private:
		enum class button_state {
			UP,
			JUST_PRESSED,
			JUST_RELEASED,
			HELD_DOWN,
		};

		index_type m_JoystickIndex;
        std::string m_Name = "undefined";
        std::vector<button_state> m_Buttons;
        std::vector<axis_value_type> m_Axes;
		std::vector<axis_value_type> m_LastAxes;
    };
}

#endif

