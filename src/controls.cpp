// © 2019 Joseph Cameron - All Rights Reserved

#include <type_traits>
#include <iostream>

#include <jsoncons/json.hpp>
#include <magic_enum/magic_enum.hpp>

#include <gdk/controls.h>

///////////// ///////////// ///////////// /////////////
//TODO: move to separate repo. jscons_wrapper? jsoncons+util? jsoncons_package? Yes. Expand magic enum requirements.
#define JSONCONS_UTIL_ENUM_TRAITS_DECL_BASE(CharT, EnumType)  \
namespace jsoncons \
{ \
    template<typename Json> \
    struct json_type_traits<Json, EnumType,typename std::enable_if<std::is_same<typename Json::char_type,CharT>::value>::type> \
    { \
        static_assert(std::is_enum<EnumType>::value, # EnumType " must be an enum"); \
        typedef EnumType value_type; \
        typedef std::basic_string<CharT> string_type; \
        typedef basic_string_view<CharT> string_view_type; \
        typedef typename Json::allocator_type allocator_type; \
        typedef std::pair<EnumType,string_type> mapped_type; \
 \
        static std::pair<const mapped_type*,const mapped_type*> get_values()/*TODO: think about static array rather than vec. Think about expanding magic_enum implementatiosn, to remove dependency on that porject*/ \
        { \
            static const std::vector<mapped_type> v = []() \
            { \
                std::remove_const<decltype(v)>::type buffer; \
             \
                auto values = magic_enum::enum_values<EnumType>(); \
 \
                buffer.reserve(magic_enum::enum_count<EnumType>()); \
 \
                for (const auto &value : values) \
                { \
                    buffer.push_back({value, string_type(magic_enum::enum_name<EnumType>(value))}); \
                } \
 \
                return buffer; \
            }(); \
 \
            return std::make_pair(&(*v.begin()), &(*v.end())); \
        } \
 \
        static bool is(const Json& ajson) noexcept \
        { \
            if (!ajson.is_string()) return false; \
 \
            auto first = get_values().first; \
            auto last = get_values().second; \
 \
            const string_view_type s = ajson.template as<string_view_type>(); \
 \
            if (s.empty() && std::find_if(first, last, [](const mapped_type& item) -> bool \
                {  \
                    return item.first == value_type();  \
                }) == last) \
            { \
                return true; \
            } \
 \
            auto it = std::find_if(first, last, [&](const mapped_type& item) -> bool \
            {  \
                return item.second == s;  \
            }); \
 \
            return it != last; \
        } \
        static value_type as(const Json& ajson) \
        { \
            if (!ajson.is_string()) \
            { \
                JSONCONS_THROW(json_runtime_error<std::runtime_error>("Not an enum: must be a json string type!")); \
            } \
 \
            const string_view_type s = ajson.template as<string_view_type>(); \
 \
            auto first = get_values().first; \
 \
            auto last = get_values().second; \
 \
            if (s.empty() && std::find_if(first, last, [](const mapped_type& item) -> bool \
                {  \
                    return item.first == value_type();  \
                }) == last) \
            { \
                return value_type(); \
            } \
 \
            auto it = std::find_if(first, last, [&](const mapped_type& item) -> bool \
            {  \
                return item.second == s;  \
            }); \
 \
            if (it == last) \
            { \
                if (s.empty()) \
                { \
                    return value_type(); \
                } \
                else \
                { \
                    JSONCONS_THROW(json_runtime_error<std::runtime_error>(std::string("Not an enum: ") + ajson.template as<std::string>())); \
                } \
            } \
            return it->first; \
        } \
        static Json to_json(value_type aval, allocator_type allocator=allocator_type()) \
        { \
            static constexpr CharT empty_string[] = {0}; \
 \
            auto first = get_values().first; \
 \
            auto last = get_values().second; \
 \
            auto it = std::find_if(first, last, [aval](const mapped_type& item) -> bool \
            {  \
                return item.first == aval;  \
            }); \
 \
            if (it == last) \
            { \
                if (aval == value_type()) \
                { \
                    return Json(empty_string); \
                } \
                else \
                { \
                    JSONCONS_THROW(json_runtime_error<std::runtime_error>("Not an enum")); \
                } \
            } \
 \
            return Json(it->second,allocator); \
        } \
    }; \
} \

#define JSONCONS_UTIL_ENUM_TRAITS_DECL(EnumType)  \
    JSONCONS_UTIL_ENUM_TRAITS_DECL_BASE(char,EnumType) \
    //JSONCONS_UTIL_ENUM_TRAITS_DECL_BASE(wchar_t, EnumType) \ //magic_enum does nto output to wchar_t. Would have to modify to support type param for string_view

namespace gdk
{
    class controls_impl  : public controls
    {
    public:
        //using key_collection_type = std::set<keyboard::Key>;

        //using mouse_button_collection_type = std::set<mouse::Button>;
        //using mouse_axis_collection_type = std::set<std::pair<mouse::Axis, /*scale and direction*/double>>;
        
		// Note: strings as indicies is wrong. This has to do with jsoncons not supporting int keys in maps.
		// Check if the project has improved since I pulled it in (early 2019). 
		// If not, consider finding a different solution to json serialization.
        // using gamepad_button_collection_type = std::set</*index*/int>;
		// ideally just use controls::gamepad_axis_collection_type
        using gamepad_axis_collection_type_impl = std::map</*index*//*int*/std::string, /*deadzone*/float>;
        using gamepad_hat_collection_type_impl = std::map</*index*//*int*/std::string, /*hat direction*/gamepad::hat_state_type>;

		gamepad_hat_collection_type_impl hat_test;
		gamepad_axis_collection_type_impl axes_test;

        struct gamepad_bindings
        {
            gamepad_axis_collection_type_impl   axes;
            gamepad_button_collection_type buttons;
            gamepad_hat_collection_type_impl    hats;
        };

        struct bindings
        {
            key_collection_type keys;

            struct
            {
                mouse_axis_collection_type   axes;
                mouse_button_collection_type buttons;
            } mouse;

			//! bindings for supported gamepads
            std::map<std::string, gamepad_bindings> gamepads; 
        };

    private:
		gdk::input::context::context_shared_ptr_type m_pInput;

        std::map<std::string, bindings> m_Inputs;

		bool m_bKeyboardIsActive;

		bool m_bMouseIsActive;

		std::optional<size_t> m_ActiveGamepad;

    public:
        /// \brief data model for use storing/transmitting state
        using serial_data_model = decltype(controls_impl::m_Inputs);

        virtual double get_down(const std::string &aName) const override;

		virtual bool get_just_pressed(const std::string& aName) const override;

		virtual bool get_just_released(const std::string& aName) const override;

        virtual void bind(const std::string &aMappingName, const keyboard::Key aKey) override;

		virtual void unbind(const std::string& aMappingName) override;

		virtual void unbind(const std::string& aMappingName, const keyboard::Key aKey) override;

		virtual void unbind(const std::string& aMappingName, const mouse::Button aButton) override;

		virtual void unbind(const std::string& aMappingName, const std::string& aGamepadName, const int aButtonIndex) override;

        virtual void bind(const std::string &aMappingName, const mouse::Button aButton) override;
        
        virtual void bind(const std::string &aMappingName, const mouse::Axis aAxis, const double scaleAndDirection) override;

        virtual void bind(const std::string &aMappingName, const std::string &aGamepadName, const int aButtonIndex) override;
        
        virtual void bind(const std::string &aMappingName, const std::string &aGamepadName, const int aAxisIndex, const float aMinimum) override;
       
        virtual void bind(const std::string &aMappingName, const std::string &aGamepadName, const int aHatIndex, const gamepad::hat_state_type aHatState) override;

        virtual std::string serialize_to_json() override;

		virtual void activate_keyboard() override;

		virtual void deactivate_keyboard() override;

		virtual bool is_keyboard_active() override;

		virtual void activate_mouse() override;

		virtual void deactivate_mouse() override;

		virtual bool is_mouse_active() override;

		virtual void activate_gamepad(size_t) override;

		virtual void deactivate_gamepad() override;

		virtual std::optional<size_t> is_gamepad_active() override;

		virtual controls::bindings get_bindings(const std::string& aBindingName) override;

		virtual std::map<std::string, controls::bindings> get_bindings() override;
        
        controls_impl(const serial_data_model &aDataModel, 
			gdk::input::context::context_shared_ptr_type aInput,
			bool bKeyboardActive,
			bool bMouseActive,
			std::optional<size_t> gamepadActive);

        ~controls_impl() = default;
    };
    
    std::unique_ptr<controls> controls::make(gdk::input::context::context_shared_ptr_type aInput,
		bool bKeyboardActive,
		bool bMouseActive,
		std::optional<size_t> gamepadActive,
		const std::string& json)
    {
		const auto serialDataModel = json.size() 
			? jsoncons::decode_json<gdk::controls_impl::serial_data_model>(json) 
			: gdk::controls_impl::serial_data_model();

        return std::make_unique<controls_impl>(controls_impl(serialDataModel, aInput, bKeyboardActive, bMouseActive, gamepadActive));
    }
	    
    controls_impl::controls_impl(const controls_impl::serial_data_model &serial_data_model, 
		gdk::input::context::context_shared_ptr_type aInput,
		bool bKeyboardActive,
		bool bMouseActive,
		std::optional<size_t> gamepadActive)
		: m_pInput(aInput)
		, m_Inputs(serial_data_model) 
		, m_bKeyboardIsActive(bKeyboardActive)
		, m_bMouseIsActive(bMouseActive)
		, m_ActiveGamepad(gamepadActive)
    {}

	bool controls_impl::get_just_released(const std::string& aName) const
	{
		auto iter = m_Inputs.find(aName);

		if (iter == m_Inputs.end()) return 0;

		if (m_bKeyboardIsActive)
		{
			for (const auto& key : iter->second.keys)
				if (const auto value = m_pInput->get_key_just_released(key))
					return static_cast<double>(value);
		}

		if (m_bMouseIsActive)
		{
			for (const auto& button : iter->second.mouse.buttons)
				if (const auto value = m_pInput->get_mouse_button_just_released(button))
					return static_cast<float>(value);
		}

		//TODO: gamepad buttons
		//TODO: gamepad hats
		//TODO: gamepad sticks
		//if (m_ActiveGamepad)

		return false;
	}

	bool controls_impl::get_just_pressed(const std::string& aName) const
	{
		auto iter = m_Inputs.find(aName);

		if (iter == m_Inputs.end()) return 0;

		if (m_bKeyboardIsActive)
		{
			for (const auto& key : iter->second.keys)
				if (const auto value = m_pInput->get_key_just_pressed(key))
					return static_cast<double>(value);
		}

		if (m_bMouseIsActive)
		{
			for (const auto& button : iter->second.mouse.buttons)
				if (const auto value = m_pInput->get_mouse_button_just_pressed(button))
					return static_cast<float>(value);
		}

		//TODO: gamepad buttons
		//TODO: gamepad hats
		//TODO: gamepad sticks
		//if (m_ActiveGamepad)

		return false;
	}

    double controls_impl::get_down(const std::string &aName) const
    {
        auto iter = m_Inputs.find(aName); 

		if (iter == m_Inputs.end()) return 0;
		
		if (m_bKeyboardIsActive)
		{
			for (const auto& key : iter->second.keys)
				if (const auto value = m_pInput->get_key_down(key))
					return static_cast<double>(value);
		}

		if (m_bMouseIsActive)
		{
			for (const auto& button : iter->second.mouse.buttons)
				if (const auto value = m_pInput->get_mouse_button_down(button))
					return static_cast<float>(value);

			//TODO: Consider whether or not to support mouse delta in controls.
			// the values of mouse delta cannot be normalized. although it does agree that 0 is no input.
			for (const auto& axis : iter->second.mouse.axes)
			{
				if (m_pInput->get_mouse_cursor_mode() == gdk::mouse::CursorMode::Locked)
				{
					const auto delta(m_pInput->get_mouse_delta());

					switch (axis.first)
					{
					case mouse::Axis::X:
					{
						const auto value(delta.x);

						if (axis.second > 0 && delta.x > 0) return value * axis.second;
						if (axis.second < 0 && delta.x < 0) return value * axis.second;
					} break;

					case mouse::Axis::Y:
					{
						const auto value(delta.y);

						if (axis.second > 0 && delta.y > 0) return value * axis.second;
						if (axis.second < 0 && delta.y < 0) return value * axis.second;
					} break;

					default: throw std::invalid_argument("unhandled axis type");
					}
				}
			}
		}
        
		if (m_ActiveGamepad)
		{
			auto m_pGamepad = m_pInput->get_gamepad(*m_ActiveGamepad);
			{
				if (const auto current_gamepad_iter = iter->second.gamepads.find(std::string(m_pGamepad->get_name())); 
					current_gamepad_iter != iter->second.gamepads.end())
				{
					for (const auto& button : current_gamepad_iter->second.buttons)
						if (const auto value = static_cast<float>(m_pGamepad->get_button_down(button))) 
							return value;
					
					for (const auto& hat : current_gamepad_iter->second.hats)
					{
						//TODO: ignore center case. center should be permissive state
						if (auto a = hat.second, b = m_pGamepad->get_hat(std::stoi(hat.first)); a.x == b.x && a.y == b.y) return 1;
					}

					for (const auto& axis : current_gamepad_iter->second.axes)
					{
						if (const auto value = static_cast<float>(m_pGamepad->get_axis(std::stoi(axis.first))))
						{
							const float minimum(axis.second);

							if (minimum >= 0 && value > minimum) 
								return value;
							else if (minimum < 0 && value < minimum) 
								return static_cast<double>(value) * -1;
						}
					}
				}
			}
		}

        return 0;
    }

    void controls_impl::bind(const std::string &aName, const keyboard::Key aKey)
    {
        m_Inputs[aName].keys.insert(aKey);
    }

	void controls_impl::unbind(const std::string& aName)
	{
		m_Inputs[aName].keys.clear();

		m_Inputs[aName].mouse.axes.clear();
		m_Inputs[aName].mouse.buttons.clear();

		for (auto& g : m_Inputs[aName].gamepads)
		{
			g.second.axes.clear();
			g.second.buttons.clear();
			g.second.hats.clear();
		}
	}

	void controls_impl::unbind(const std::string& aName, const keyboard::Key aKey)
	{
		m_Inputs[aName].keys.erase(aKey);
	}

    void controls_impl::bind(const std::string &aName, const mouse::Button aButton)
    {
        m_Inputs[aName].mouse.buttons.insert(aButton);
    }

	void controls_impl::unbind(const std::string& aName, const mouse::Button aButton)
	{ 
		m_Inputs[aName].mouse.buttons.erase(aButton);
	}

    void controls_impl::bind(const std::string &aName, const mouse::Axis aAxis, const double aScaleAndDirection)
    {
        m_Inputs[aName].mouse.axes.insert({aAxis, aScaleAndDirection});
    }

    void controls_impl::bind(const std::string &aInputName, const std::string &aGamepadName, const int aAxisIndex, const float aMinimum)
    {
        m_Inputs[aInputName].gamepads[aGamepadName].axes.insert({std::to_string(aAxisIndex), aMinimum});
    }

    void controls_impl::bind(const std::string &aInputName, const std::string &aGamepadName, const int aHatIndex, const gamepad::hat_state_type aHatState)
    {
        m_Inputs[aInputName].gamepads[aGamepadName].hats.insert({std::to_string(aHatIndex), aHatState});
    }

    void controls_impl::bind(const std::string &aInputName, const std::string &aGamepadName, const int aButtonIndex)
    {
        m_Inputs[aInputName].gamepads[aGamepadName].buttons.insert(aButtonIndex);
    }

	void controls_impl::unbind(const std::string& aMappingName, const std::string& aGamepadName, const int aButtonIndex)	
	{ 
		throw std::runtime_error("unimplemented"); 
	}

    std::string controls_impl::serialize_to_json()
    {
        std::string s;
        jsoncons::encode_json(m_Inputs, s, jsoncons::indenting::indent);

        return s;
    }

	void controls_impl::activate_keyboard()
	{
		m_bKeyboardIsActive = true;
	}

	void controls_impl::deactivate_keyboard()
	{
		m_bKeyboardIsActive = false;
	}

	bool controls_impl::is_keyboard_active()
	{
		return m_bKeyboardIsActive;
	}

	void controls_impl::activate_mouse()
	{
		m_bMouseIsActive = true;
	}

	void controls_impl::deactivate_mouse()
	{
		m_bMouseIsActive = false;
	}

	bool controls_impl::is_mouse_active()
	{
		return m_bMouseIsActive;
	}

	void controls_impl::activate_gamepad(size_t i)
	{
		m_ActiveGamepad = {i};
	}

	void controls_impl::deactivate_gamepad()
	{
		m_ActiveGamepad = {};
	}

	std::optional<size_t> controls_impl::is_gamepad_active()
	{
		return m_ActiveGamepad;
	}

	controls::bindings controls_impl::get_bindings(const std::string& aBindingName)
	{
		controls::bindings value;

		value.keys = m_Inputs[aBindingName].keys;
		
		value.mouse_axes = m_Inputs[aBindingName].mouse.axes;
		value.mouse_buttons = m_Inputs[aBindingName].mouse.buttons;

		for (const auto& g : m_Inputs[aBindingName].gamepads)
		{
			decltype(value.gamepads)::value_type gamepad_binding;
			
			for (const auto& axis : g.second.axes)
			{
				gamepad_binding.axes[std::stoi(axis.first)] = axis.second;
			}

			gamepad_binding.buttons = g.second.buttons;

			for (const auto& hat : g.second.hats)
			{
				gamepad_binding.hats[std::stoi(hat.first)] = hat.second;
			}

			value.gamepads.push_back(gamepad_binding);
		}

		return value;
	}

	std::map<std::string, controls::bindings> controls_impl::get_bindings()
	{
		std::map<std::string, controls::bindings> map;

		for (const auto& bindings : m_Inputs)
		{
			map[bindings.first] = get_bindings(bindings.first);
		}

		return map;
	}
}

// keyboard
JSONCONS_UTIL_ENUM_TRAITS_DECL(gdk::keyboard::Key);

// mouse
JSONCONS_UTIL_ENUM_TRAITS_DECL(gdk::mouse::Button);

JSONCONS_UTIL_ENUM_TRAITS_DECL(gdk::mouse::Axis);
JSONCONS_MEMBER_TRAITS_DECL(decltype(gdk::controls_impl::bindings::mouse), buttons, axes);

// gamepad
JSONCONS_UTIL_ENUM_TRAITS_DECL(gdk::gamepad::hat_state_type::vertical_direction);
JSONCONS_UTIL_ENUM_TRAITS_DECL(gdk::gamepad::hat_state_type::horizontal_direction);
JSONCONS_MEMBER_TRAITS_DECL   (gdk::gamepad::hat_state_type, x, y);

JSONCONS_MEMBER_TRAITS_DECL(decltype(gdk::controls_impl::bindings::gamepads)::value_type::second_type, buttons, hats, axes);

// all
JSONCONS_MEMBER_TRAITS_DECL(gdk::controls_impl::bindings, keys, gamepads, mouse);

// test
JSONCONS_MEMBER_TRAITS_DECL(gdk::controls_impl, hat_test, axes_test);
