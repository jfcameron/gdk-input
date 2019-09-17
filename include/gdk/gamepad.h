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
        using size_type = size_t;
        using button_state_type = bool;
        
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
        virtual std::string_view getName() const = 0;
        
        //! normalized magnitude of axis. 0 = no input, 1 = full input
        virtual float getAxis(int index) const = 0;

        //! number of axes
        virtual size_type getAxisCount() const = 0;
        
        //! whether or not the button is being held down by the user
        virtual button_state_type getButtonDown(int index) const = 0;

        //! number of buttons
        virtual size_type getButtonCount() const = 0;

        //! value of a hat direction by index
        virtual hat_state_type getHat(int index) const = 0;

        //! number of hats
        virtual size_type getHatCount() const = 0;

        virtual ~gamepad() = default;
    };
}

#endif
