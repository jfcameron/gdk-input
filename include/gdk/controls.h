// © 2019 Joseph Cameron - All Rights Reserved

#ifndef GDK_CONTROLS_H
#define GDK_CONTROLS_H

#include <map>
#include <set>
#include <string>
#include <string_view>

#include <gdk/gamepad.h>
#include <gdk/keyboard.h>
#include <gdk/mouse.h>

namespace gdk
{
    /// \brief user input mapping abstraction
    /// abstraction on top of gamepads, mouse, keyboards.
    class controls
    {
        //TODO: mouse axis
        //TODO: serialize, deserialize methods. 
        using key_collection_type = std::set<keyboard::Key>;

        using mouse_button_collection_type = std::set<mouse::Button>;
        
        using gamepad_button_collection_type = std::set</*index*/int>;
        using gamepad_axis_collection_type = std::map</*index*/int, /*deadzone*/float>;
        using gamepad_hat_collection_type = std::map</*index*/int, /*hat direction*/gamepad::hat_state_type>;

        struct gamepad_bindings
        {
            gamepad_button_collection_type buttons;
            
            gamepad_axis_collection_type axes;

            gamepad_hat_collection_type hats;
        };

        struct bindings
        {
            key_collection_type keys;

            struct
            {
                mouse_button_collection_type buttons;

                //TODO mouse axes

            } mouse;

            std::map<std::string, gamepad_bindings> gamepads; //!< bindings for supported gamepads
        };

        std::map<std::string, bindings> m_Inputs; //TODO: rename to input map?

        std::shared_ptr<keyboard> m_Keyboard;
        std::shared_ptr<mouse>    m_Mouse;
        std::shared_ptr<gamepad>  m_Gamepad;

    public:
        //! get value of an input
        float get(const std::string &aName) const;

        //! [re]sets keyboard pointer
        void setKeyboard(std::shared_ptr<keyboard> aKeyboard);

        //! [re]sets mouse pointer
        void setMouse(std::shared_ptr<mouse> aMouse);
        
        //! [re]sets gamepad pointer
        void setGamepad(std::shared_ptr<gamepad> aGamepad);

        //! adds a mapping to an existing input OR creates a new input and adds to that
        void addKeyMapping(const std::string &aName, const keyboard::Key aKey);

        void addMouseButtonMapping(const std::string &aName, const mouse::Button aButton);
        //void addMouseAxis(...)

        void addGamepadButtonMapping(const std::string &aInputName, const std::string &aGamepadName, const int aButtonIndex);
        
        void addGamepadAxisMapping(const std::string &aInputName, const std::string &aGamepadName, const int aAxisIndex, const float aMinimum);
       
        void addGamepadHatMapping(const std::string &aInputName, const std::string &aGamepadName, const int aHatIndex, const gamepad::hat_state_type aHatState);

        controls(std::shared_ptr<keyboard> aKeyboard = nullptr, std::shared_ptr<mouse> aMouse = nullptr, std::shared_ptr<gamepad> aGamepad = nullptr);
    };
}

#endif

