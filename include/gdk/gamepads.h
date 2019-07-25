// © 2019 Joseph Cameron - All Rights Reserved

#ifndef GDK_GAMEPADS_H
#define GDK_GAMEPADS_H

#include <memory>
#include <string>
#include <vector>

namespace gdk
{
    /// \brief gamepad interface type
    class gamepad
    {
    public:
        using size_type = size_t;
        using button_state_type = bool;
        
        //! hat direction
        /// left -1, right +1, up +1, down -1
        struct hat_state_type
        {
            short int x, y;
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

        // TODO: think about how to hide this. the user does nnot need access to this. It will also be a no-op on some implementations, which is silly. solution: hierarchy of interfaces? an internal interface, an external interface etc. Think about it.
        //! opportunity to update inputs in the case of non-callback based implementations TODO: should hide from users?
        virtual void update() = 0;

        virtual ~gamepad() = default;
    };
    
    namespace Gamepads
    {
        //! Get a gamepad by index. Index value is based on order in
        /// which the device was connected to the system.
        //std::weak_ptr<Gamepad> get(const int aIndex);

        //! Get a gamepad by device name
        //std::weak_ptr<Gamepad> get(const std::string &aName);

        // extern vector<gamepad> pads? maybe? expose iterators st for (gamepad : gamepads) can be done
    }
}

#endif
