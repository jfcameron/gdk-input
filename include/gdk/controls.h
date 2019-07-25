// © 2019 Joseph Cameron - All Rights Reserved

#ifndef GDK_CONTROLS_H
#define GDK_CONTROLS_H

#include <map>
#include <set>
#include <string>
#include <string_view>

#include <gdk/gamepads.h>
#include <gdk/keyboard.h>
#include <gdk/mouse.h>

namespace gdk
{
    /// \brief user input mapping abstraction
    /// abstraction on top of gamepads, mouse, keyboards.
    class controls
    {
        //TODO: mouse button, axis
        //TODO: gamepad hat, axis
        using key_collection_type = std::set<keyboard::Key>;
        using gamepad_button_collection_type = std::set<int>; //TODO: int? gamepad::button_index_type?

        struct gamepad_bindings
        {
            std::string name;

            gamepad_button_collection_type buttons;
        };

        struct bindings
        {
            key_collection_type keys;

            //std::set<gamepad_bindings> gamepads; //!< bindings for supported gamepads
            std::map<std::string, gamepad_bindings> gamepads;
        };

        std::map<std::string, bindings> m_Inputs;

        std::shared_ptr<keyboard> m_Keyboard;
        std::shared_ptr<gamepad> m_Gamepad;

    public:
        //! get value of an input
        float get(const std::string &aName) const;

        //! [re]sets keyboard pointer
        void setKeyboard(std::shared_ptr<keyboard> aKeyboard);

        //! [re]sets mouse pointer
        //void setMouse(std::shared_ptr<mouse> aMouse);
        
        //! [re]sets gamepad pointer
        void setGamepad(std::shared_ptr<gamepad> aGamepad);

        //! adds a mapping to an existing input OR creates a new input and adds to that
        void addKeyMapping(const std::string &aName, const keyboard::Key aKey);
    
        void addGamepadButtonMapping(const std::string &aInputName, const std::string &aGamepadName, const int aButtonIndex);

        //controls() = delete;

        controls(std::shared_ptr<keyboard> aKeyboard = nullptr, 
            //std::shared_ptr<mouse> aMouse = nullptr,
            std::shared_ptr<gamepad> aGamepad = nullptr);//,  //TODO: support multiple gamepads? would add complexity for a very niche feature..
            //const std::vector<std::string> &aInputNames);
    };
}

#endif

