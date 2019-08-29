// © 2019 Joseph Cameron - All Rights Reserved

#include <type_traits>
#include <iostream>

#include <nlohmann/json.hpp>

#include <gdk/controls.h>

using namespace nlohmann;

namespace gdk
{
    controls::controls(std::shared_ptr<keyboard> aKeyboard, std::shared_ptr<mouse> aMouse, std::shared_ptr<gamepad> aGamepad)    
    : m_pKeyboard(aKeyboard)
    , m_pMouse(aMouse)
    , m_pGamepad(aGamepad)
    {}

    double controls::get(const std::string &aName) const
    {
        auto iter = m_Inputs.find(aName); 

        if (iter == m_Inputs.end()) throw std::invalid_argument(std::string("TAG").append("not a valid button: ").append(aName));

        if (m_pKeyboard)
        {
            for (const auto &key : iter->second.keys)
            {
                if (const auto value = static_cast<float>(m_pKeyboard->getKeyDown(key))) return value;
            }
        }

        if (m_pMouse) 
        {
            for (const auto &button : iter->second.mouse.buttons)
            {
                if (const auto value = static_cast<float>(m_pMouse->getButtonDown(button))) return value;
            }

            //TODO: Mouse axes
            for (const auto &axis : iter->second.mouse.axes)
            {
                if (m_pMouse->getCursorMode() == gdk::mouse::CursorMode::Locked)
                {
                    const auto delta = m_pMouse->getDelta();

                    switch(axis.first)
                    {
                        case mouse::Axis::X:
                        {
                            const auto value = delta.x;

                            if (axis.second > 0 && delta.x > 0) return value * axis.second; 

                            if (axis.second < 0 && delta.x < 0) return value * axis.second; 
                        } break;

                        case mouse::Axis::Y:
                        {
                            const auto value = delta.y;

                            if (axis.second > 0 && delta.y > 0) return value * axis.second; 

                            if (axis.second < 0 && delta.y < 0) return value * axis.second; 
                        } break;

                        default: throw std::invalid_argument("unhandled axis type");
                    }
                }
            }
        }
       
        if (m_pGamepad)
        {
            if (const auto current_gamepad_iter = iter->second.gamepads.find(std::string(m_pGamepad->getName())); current_gamepad_iter != iter->second.gamepads.end())
            {
                for (const auto &button : current_gamepad_iter->second.buttons)
                {   
                    if (const auto value = static_cast<float>(m_pGamepad->getButtonDown(button))) return value;
                }

                for (const auto &hat : current_gamepad_iter->second.hats)
                {   
                    if (auto a = hat.second, b = m_pGamepad->getHat(hat.first); a.x == b.x && a.y == b.y) return 1;
                }

                for (const auto &axis : current_gamepad_iter->second.axes)
                {
                    if (const auto value = static_cast<float>(m_pGamepad->getAxis(axis.first))) 
                    {
                        const float minimum = axis.second;

                        if (minimum >= 0 && value > minimum) return value; 
                        else if (minimum < 0 && value < minimum) return value * -1;
                    }
                }
            }
            else std::cerr << m_pGamepad->getName() << " not configured for these controls\n";
        }

        return 0;
    }

    void controls::setKeyboard(std::shared_ptr<keyboard> aKeyboard)
    {
        m_pKeyboard = aKeyboard;
    }

    void controls::setMouse(std::shared_ptr<mouse> aMouse)
    {
        m_pMouse = aMouse;
    }

    void controls::setGamepad(std::shared_ptr<gamepad> aGamepad)
    {
        m_pGamepad = aGamepad;
    }
    
    void controls::addKeyToMapping(const std::string &aName, const keyboard::Key aKey)
    {
        m_Inputs[aName].keys.insert(aKey);
    }
    
    void controls::addMouseButtonToMapping(const std::string &aName, const mouse::Button aButton)
    {
        m_Inputs[aName].mouse.buttons.insert(aButton);
    }

    void controls::addMouseAxisToMapping(const std::string &aName, const mouse::Axis aAxis, const double aScaleAndDirection)
    {
        m_Inputs[aName].mouse.axes.insert({aAxis, aScaleAndDirection});
    }

    void controls::addGamepadAxisToMapping(const std::string &aInputName, const std::string &aGamepadName, const int aAxisIndex, const float aMinimum)
    {
        m_Inputs[aInputName].gamepads[aGamepadName].axes.insert({aAxisIndex, aMinimum});
    }

    void controls::addGamepadHatToMapping(const std::string &aInputName, const std::string &aGamepadName, const int aHatIndex, const gamepad::hat_state_type aHatState)
    {
        m_Inputs[aInputName].gamepads[aGamepadName].hats.insert({aHatIndex, aHatState});
    }

    void controls::addGamepadButtonToMapping(const std::string &aInputName, const std::string &aGamepadName, const int aButtonIndex)
    {
        m_Inputs[aInputName].gamepads[aGamepadName].buttons.insert(aButtonIndex);
    }
      
namespace jfc::nlohmann::json_util
{
    std::optional<json> get_property(const json &object, const std::string &propertyName, const json::value_t &type)
    {
        return object.contains(propertyName) && object[propertyName].type() == type
            ? std::optional<json>(object[propertyName])
            : std::nullopt;
    }
}

using namespace jfc::nlohmann;

void controls::addMappingsFromJSON(const std::string &aJSONData)
{
    constexpr auto deserializeMapping = [] (const json &root)
    {
        //TOOD: possibly catch nlohmanns exceptions and throw ones more appropriate? nolhman is a hidden dependency. 
        //dont expose?

        auto mappingName(root.at("Name").get<std::string>());
        //auto mappingName = json_util::get_optional_property<std::string, json_util::required_type>(root, "Name");

        std::cout << "mapname: " << mappingName << "\n";

        if (const auto keys = json_util::get_property(root, "Keys", json::value_t::array))
        {
            for (const auto &key : *keys)
            {
                if (key.type() == json::value_t::string)
                {
                //TODO: 
                //addKeyToMapping(mappingName, stringToKeyboardKeyEnum(key.get<std::string>());
                std::cout << "JSON keys: " <<  key.get<std::string>() << "\n";
                }
                else
                {
                    
                }
            }
        }

        if (const auto mouse = json_util::get_property(root, "Mouse", json::value_t::object))
        {
            if (mouse->contains("Buttons")) for (const auto &mouseButton : (*mouse)["Buttons"])
            {
                //TODO: button to string fucntion needed
                //std::cout << "JSON mouse buttons: " << mouseButton.get<std::string>() << "\n";
            }


            if (mouse->contains("Axes")) for (const auto &mouseAxis : (*mouse)["Axes"])
            {
                //TODO: 
                //std::cout << "JSON mouse axes: " << mouseAxis.get<std::string>() << "\n";
            }
        }

        if (root.contains("Gamepads") && root["Gamepads"].type() == json::value_t::array)
        {
            for (const auto gamepad : root["Gamepads"])
            {
                if (gamepad.contains("Name") && gamepad["Name"].type() == json::value_t::string)
                {
                    const auto name = gamepad["Name"].get<std::string>();

                    if (gamepad.contains("Buttons") && gamepad["Buttons"].type() == json::value_t::array)
                    {
                        for (const auto button : gamepad["Buttons"])
                        {
                            //TODO:
                            std::cout << "JSON gamepad buttons: " << button.get<int>() << "\n";
                        }
                    }
                    
                    if (gamepad.contains("Axes") && gamepad["Axes"].type() == json::value_t::array)
                    {
                        for (const auto axis : gamepad["Axes"])
                        {
                            //TODO:
                            std::cout << "JSON gamepad axes: " << axis.get<int>() << "\n";
                        }
                    }
                    
                    if (gamepad.contains("Hats") && gamepad["Hats"].type() == json::value_t::array)
                    {
                        for (const auto hat : gamepad["Hats"])
                        {
                            //TODO:
                            std::cout << "JSON gamepad hats: " << hat.get<int>() << "\n";
                        }
                    }
                }
            }
        }
    };
    
    const auto root = json::parse(aJSONData);

    switch (root.type())
    {
        case json::value_t::array: for (const auto &item : root) deserializeMapping(item); break;

        case json::value_t::object: deserializeMapping(root); break;

        default: throw std::invalid_argument("mapping should be an array (multiple mappings) or an object (single mapping)");
    }
}
}

