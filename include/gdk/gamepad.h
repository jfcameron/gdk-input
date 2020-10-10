// © 2019 Joseph Cameron - All Rights Reserved

#ifndef GDK_GAMEPAD_H
#define GDK_GAMEPAD_H

#include <memory>
#include <string>
#include <vector>

namespace gdk
{
    /// \brief gamepad interface
    class gamepad
    {
    public:
        using size_type = size_t; //TODO: delete?

        using button_state_type = bool;

		using index_type = size_t;

		using button_collection_type = std::vector<button_state_type>;
        
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
        virtual float get_axis(index_type index) const = 0;

        //! whether or not the button is being held down by the user
        virtual button_state_type get_button_down(index_type index) const = 0;

        //! value of a hat direction by index
        virtual hat_state_type get_hat(index_type index) const = 0;

		//! state of all buttons
		virtual button_collection_type get_buttons() const = 0;

        virtual ~gamepad() = default;
    };
}

#endif
