// © 2019 Joseph Cameron - All Rights Reserved

#ifndef GDK_CONTROLS_H
#define GDK_CONTROLS_H

#include <map>
#include <memory>
#include <set>
#include <string>
#include <string_view>
#include <optional>

#include <gdk/gamepad.h>
#include <gdk/keyboard.h>
#include <gdk/mouse.h>
#include <gdk/input_context.h>

namespace gdk
{
    /// \brief provides a way to bind input from a gamepad, mouse, keyboard to a single binding name
    /// e.g: create a "Jump" binding that is non-zero when spacebar or gamepad button zero is pressed
    /// e.g: a "Look up" binding that is non-zero when up arrow or mouse axis y or gamepad axis 0 is interacted with.
	/// This also allows a game to easily support many types of gamepads.
	class controls
    {
    public:
		//! keys bound to a binding name
		using key_collection_type = std::set<keyboard::Key>;

		//! mouse buttons bound to a binding name
		using mouse_button_collection_type = std::set<mouse::Button>;
		//! mouse axes bound to a binding name
		using mouse_axis_collection_type = std::set<std::pair<mouse::Axis, /*scale and direction*/double>>;

		//! gamepad buttons bound to a binding name
		using gamepad_button_collection_type = std::set</*index*/int>;
		//! gamepad axes bound to a binding name
		using gamepad_axis_collection_type = std::map</*index*/int, /*deadzone*/float>;
		//! gamepad hats bound to a binding name
		using gamepad_hat_collection_type = std::map</*index*/int, /*hat direction*/gamepad::hat_state_type>;

		//! collection of all inputs bound to a name
		struct bindings
		{
			//! bound keys
			key_collection_type keys;

			//! bound mouse buttons
			mouse_button_collection_type mouse_buttons;
			//! bound mouse axes
			mouse_axis_collection_type mouse_axes;

			//! collection of gamepad inputs
			struct gamepad
			{
				//! bound gamepad buttons
				gamepad_button_collection_type buttons;
				//! bound gamepad axes
				gamepad_axis_collection_type axes;
				//! bound gamepad hats
				gamepad_hat_collection_type hats;
			};

			//! bound gamepads
			std::vector<gamepad> gamepads;
		};

        //! construct an instance from a string containing a JSON serialized controls instance
        static std::unique_ptr<controls> make(gdk::input::context::context_shared_ptr_type aInput,
			bool bKeyboardActive, 
			bool bMouseActive, 
			std::optional<size_t> gamepadActive,
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

		//! unbind all inputs from a name
		virtual void unbind(const std::string& aMappingName) = 0;

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

		//! returns a group of collections, representing all the inputs bound to the given binding name
		virtual bindings get_bindings(const std::string& aBindingName) = 0;

		//! returns a group of collections, representing all the inputs bound to all binding names
		virtual std::map<std::string, bindings> get_bindings() = 0;

		//! controls will react to keyboard inputs
		virtual void activate_keyboard() = 0;
		
		//! controls will ignore keyboard inputs
		virtual void deactivate_keyboard() = 0;
		
		//! whether or not controls reacts to keyboard inputs
		virtual bool is_keyboard_active() = 0;

		//! controls will react to mouse inputs
		virtual void activate_mouse() = 0;

		//! controls will ignore mouse inputs
		virtual void deactivate_mouse() = 0;

		//! whether or not controls reacts to mouse inputs
		virtual bool is_mouse_active() = 0;

		//! controls will react to the specified gamepad
		/// \warn controls will only ever react to a single gamepad at a time
		virtual void activate_gamepad(size_t) = 0;

		//! controls will ignore gamepad input
		virtual void deactivate_gamepad() = 0;

		//! which (if any) gamepad the controls reacts to
		virtual std::optional<size_t> is_gamepad_active() = 0;

        virtual ~controls() = default;
    };
}

#endif
