// © 2019 Joseph Cameron - All Rights Reserved

#ifndef GDK_CONTROLS_H
#define GDK_CONTROLS_H

#include <map>
#include <memory>
#include <set>
#include <string>
#include <string_view>

#include <gdk/gamepad.h>
#include <gdk/keyboard.h>
#include <gdk/mouse.h>

namespace gdk
{
    /// \brief provides a way to map raw input from a gamepad, mouse, and keyboard behind a single logical input
    /// e.g: create a "Jump" binding that is non-zero when spacebar or gamepad button zero is pressed
    /// e.g: a "Look up" binding that is non-zero when up arrow or mouse axis y or gamepad axis 0 is interacted with.
    class controls
    {
    public:
        //! construct an empty instance
        static std::unique_ptr<controls> make(std::shared_ptr<keyboard> aKeyboard = nullptr, 
            std::shared_ptr<mouse> aMouse = nullptr, 
            std::shared_ptr<gamepad> aGamepad = nullptr);
        
        //! construct an instance from a string containing a JSON serialized controls instance
        static std::unique_ptr<controls> make_from_json(const std::string &json,
            std::shared_ptr<keyboard> aKeyboard = nullptr, 
            std::shared_ptr<mouse> aMouse = nullptr, 
            std::shared_ptr<gamepad> aGamepad = nullptr);

        //make_from_message_pack

        //! adds bindings from a string containing JSON data
        //virtual void addMappingsFromJSON(const std::string &aJSONData) = 0;

        ////virtual void deserializeFromJSON(const std::string &json) = 0;
        virtual std::string serializeToJSON() = 0;

        //! get value of an input
        virtual double get(const std::string &aName) const = 0;

        //foreach input... 

        //! [re]sets keyboard pointer
        virtual void setKeyboard(std::shared_ptr<keyboard> aKeyboard) = 0;

        //! [re]sets mouse pointer
        virtual void setMouse(std::shared_ptr<mouse> aMouse) = 0;
        
        //! [re]sets gamepad pointer
        virtual void setGamepad(std::shared_ptr<gamepad> aGamepad) = 0;

        //! adds a key to a mapping, creating a new mapping if it did not already exist.
        virtual void addKeyToMapping(const std::string &aMappingName, const keyboard::Key aKey) = 0;

        //! adds a mouse button to a mapping, creating a new mapping if it did not already exist.
        virtual void addMouseButtonToMapping(const std::string &aMappingName, const mouse::Button aButton) = 0;
        
        //! adds a mouse axis to a mapping, creating a new mapping if it did not already exist.
        virtual void addMouseAxisToMapping(const std::string &aMappingName, const mouse::Axis aAxis, const double scaleAndDirection) = 0;

        //! adds a gamepad button to a mapping for a specific gamepad type, creating a new mapping if it did not already exist.
        virtual void addGamepadButtonToMapping(const std::string &aMappingName, const std::string &aGamepadName, const int aButtonIndex) = 0;
        
        //! adds a gamepad axis to a mapping for a specific gamepad type, creating a new mapping if it did not already exist.
        virtual void addGamepadAxisToMapping(const std::string &aMappingName, const std::string &aGamepadName, const int aAxisIndex, const float aMinimum) = 0;
       
        //! adds a gamepad hat to a mapping for a specific gamepad type, creating a new mapping if it did not already exist.
        virtual void addGamepadHatToMapping(const std::string &aMappingName, const std::string &aGamepadName, const int aHatIndex, const gamepad::hat_state_type aHatState) = 0;
        
        virtual ~controls() = default;
    };
}

#endif

