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
#include <gdk/input_context.h>

namespace gdk
{
    /// \brief provides a way to map raw input from a gamepad, mouse, and keyboard behind a single logical input
    /// e.g: create a "Jump" binding that is non-zero when spacebar or gamepad button zero is pressed
    /// e.g: a "Look up" binding that is non-zero when up arrow or mouse axis y or gamepad axis 0 is interacted with.
	class controls
    {
    public:
        //! construct an instance from a string containing a JSON serialized controls instance
        static std::unique_ptr<controls> make(gdk::input::context::context_shared_ptr_type aInput,
			const std::string &json = "");

        //! serialize state to string of json for e.g: disk storage, network transmission, ...
        virtual std::string serialize_to_json() = 0;
		
        //! get value of an input
        virtual double get_down(const std::string &aName) const = 0;

		//! true if an input was just pressed
		virtual bool get_just_pressed(const std::string& aName) const = 0;
		
		//! true if an input was just released
		virtual bool get_just_released(const std::string& aName) const = 0;

        //! adds a key to a mapping, creating a new mapping if it did not already exist.
        virtual void bind(const std::string &aMappingName, 
			const keyboard::Key aKey) = 0;

        //! adds a mouse button to a mapping, creating a new mapping if it did not already exist.
        virtual void bind(const std::string &aMappingName,
			const mouse::Button aButton) = 0;
        
        //! adds a mouse axis to a mapping, creating a new mapping if it did not already exist.
        virtual void bind(const std::string &aMappingName,
			const mouse::Axis aAxis, 
			const double scaleAndDirection) = 0;

        //! adds a gamepad button to a mapping for a specific gamepad type, creating a new mapping if it did not already exist.
        virtual void bind(const std::string &aMappingName,
			const std::string &aGamepadName, 
			const int aButtonIndex) = 0;
        
        //! adds a gamepad axis to a mapping for a specific gamepad type, creating a new mapping if it did not already exist.
        virtual void bind(const std::string &aMappingName,
			const std::string &aGamepadName, 
			const int aAxisIndex, 
			const float aMinimum) = 0;
       
        //! adds a gamepad hat to a mapping for a specific gamepad type, creating a new mapping if it did not already exist.
        virtual void bind(const std::string &aMappingName,
			const std::string &aGamepadName, 
			const int aHatIndex, 
			const gamepad::hat_state_type aHatState) = 0;

		//! unbind a key
		virtual void unbind(const std::string& aMappingName,
			const keyboard::Key aKey) = 0;

		//! unbind a mouse buttton
		virtual void unbind(const std::string& aMappingName,
			const mouse::Button aButton) = 0;

		//! unbind a gamepad button
		virtual void unbind(const std::string& aMappingName,
			const std::string& aGamepadName,
			const int aButtonIndex) = 0;

		/*//! unbind an axis
		virtual void unbind(const std::string& aMappingName,
			const mouse::Axis aAxis,
			const double scaleAndDirection) = 0;

		virtual void unbind(const std::string& aMappingName,
			const std::string& aGamepadName,
			const int aAxisIndex,
			const float aMinimum) = 0;

		virtual void unbind(const std::string& aMappingName,
			const std::string& aGamepadName,
			const int aHatIndex,
			const gamepad::hat_state_type aHatState) = 0;*/

		// virtual void unbind(std::string name) = 0; //unbind everything from bind
		
		// void unbind_all() = 0; //unbind everything

		// void enable_mouse()
		// void disable_mouse()
		// bool is_mouse_enabled()

		// void enable_keyboard()
		// void disable_keyboard()
		// bool is_keyboard_enabled()

		// enable_gamepad(index)
		// disable_gamepad(index)
		// disable_gamepads()
        
        virtual ~controls() = default;
    };
}

#endif
