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
    /// unify gamepad, mouse, keyboard input behind a single binding
    /// e.g: a "Jump" binding that is non-zero when spacebar or gamepad button zero is pressed
    /// e.g: a "Look up" binding that is non-zero when up arrow or mouse axis y or gamepad axis 0 etc.
    class controls
    {
        //TODO: serialize, deserialize methods. 
        using key_collection_type = std::set<keyboard::Key>;

        using mouse_button_collection_type = std::set<mouse::Button>;
        using mouse_axis_collection_type = std::set<std::pair<mouse::Axis, /*scale and direction*/double>>;
        
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

                mouse_axis_collection_type axes;
            } mouse;

            std::map<std::string, gamepad_bindings> gamepads; //!< bindings for supported gamepads
        };

        std::map<std::string, bindings> m_Inputs; //TODO: rename to input map?

        std::shared_ptr<keyboard> m_pKeyboard;
        std::shared_ptr<mouse>    m_pMouse;
        std::shared_ptr<gamepad>  m_pGamepad;

    public:
        //! get value of an input
        double get(const std::string &aName) const;

        //! [re]sets keyboard pointer
        void setKeyboard(std::shared_ptr<keyboard> aKeyboard);

        //! [re]sets mouse pointer
        void setMouse(std::shared_ptr<mouse> aMouse);
        
        //! [re]sets gamepad pointer
        void setGamepad(std::shared_ptr<gamepad> aGamepad);

        //! adds a key to a mapping, creating a new mapping if it did not already exist.
        void addKeyToMapping(const std::string &aMappingName, const keyboard::Key aKey);

        //! adds a mouse button to a mapping, creating a new mapping if it did not already exist.
        void addMouseButtonToMapping(const std::string &aMappingName, const mouse::Button aButton);
        
        //! adds a mouse axis to a mapping, creating a new mapping if it did not already exist.
        void addMouseAxisToMapping(const std::string &aMappingName, const mouse::Axis aAxis, const double scaleAndDirection);

        //! adds a gamepad button to a mapping for a specific gamepad type, creating a new mapping if it did not already exist.
        void addGamepadButtonToMapping(const std::string &aMappingName, const std::string &aGamepadName, const int aButtonIndex);
        
        //! adds a gamepad axis to a mapping for a specific gamepad type, creating a new mapping if it did not already exist.
        void addGamepadAxisToMapping(const std::string &aMappingName, const std::string &aGamepadName, const int aAxisIndex, const float aMinimum);
       
        //! adds a gamepad hat to a mapping for a specific gamepad type, creating a new mapping if it did not already exist.
        void addGamepadHatToMapping(const std::string &aMappingName, const std::string &aGamepadName, const int aHatIndex, const gamepad::hat_state_type aHatState);

        //! adds bindings from a string containing JSON data
        void addMappingsFromJSON(const std::string &aJSONData);

        controls(std::shared_ptr<keyboard> aKeyboard = nullptr, std::shared_ptr<mouse> aMouse = nullptr, std::shared_ptr<gamepad> aGamepad = nullptr);
    };
}

#endif

