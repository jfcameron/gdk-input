// © 2019 Joseph Cameron - All Rights Reserved

#include <type_traits>
#include <iostream>

#include <gdk/controls.h>

namespace gdk
{
    controls::controls(std::shared_ptr<keyboard> aKeyboard, 
        std::shared_ptr<mouse> aMouse, 
        std::shared_ptr<gamepad> aGamepad)    
    : m_Keyboard(aKeyboard)
    , m_Mouse(aMouse)
    , m_Gamepad(aGamepad)
    {}

    float controls::get(const std::string &aName) const
    {
        auto iter = m_Inputs.find(aName); 

        if (iter == m_Inputs.end()) throw std::invalid_argument(std::string("TAG").append("not a valid button: ").append(aName));

        if (m_Keyboard)
        {
            for (const auto &key : iter->second.keys)
            {
                if (const auto raw = static_cast<float>(m_Keyboard->getKeyDown(key))) return raw;
            }
        }

        if (m_Mouse) 
        {
            for (const auto &button : iter->second.mouse.buttons)
            {
                if (const auto raw = static_cast<float>(m_Mouse->getButtonDown(button))) return raw;
            }

            //TODO: Mouse axes
        }
       
        if (m_Gamepad)
        {
            if (const auto current_gamepad_iter = iter->second.gamepads.find(std::string(m_Gamepad->getName())); current_gamepad_iter != iter->second.gamepads.end())
            {
                for (const auto &button : current_gamepad_iter->second.buttons)
                {   
                    if (const auto raw = static_cast<float>(m_Gamepad->getButtonDown(button))) return raw;
                }

                //TODO: Hats, axes
            }
            else std::cerr << m_Gamepad->getName() << " not configured for these controls\n";
        }

        return 0;
    }

    void controls::setKeyboard(std::shared_ptr<keyboard> aKeyboard)
    {
        m_Keyboard = aKeyboard;
    }

    void controls::setMouse(std::shared_ptr<mouse> aMouse)
    {
        m_Mouse = aMouse;
    }

    void controls::setGamepad(std::shared_ptr<gamepad> aGamepad)
    {
        m_Gamepad = aGamepad;
    }
    
    void controls::addKeyMapping(const std::string &aName, const keyboard::Key aKey)
    {
        auto &keys = m_Inputs[aName].keys;

        keys.insert(aKey);
    }
    
    void controls::addMouseButtonMapping(const std::string &aName, const mouse::Button aButton)
    {
        auto &buttons = m_Inputs[aName].mouse.buttons;

        buttons.insert(aButton);
    }

    void controls::addGamepadButtonMapping(const std::string &aInputName, const std::string &aGamepadName, const int aButtonIndex)
    {
        auto &current_gamepad = m_Inputs[aInputName].gamepads[aGamepadName];

        current_gamepad.buttons.insert(aButtonIndex);
    }
}

/*
"Configuration":
{
    "Input": [
        {
            "Name": "Up",
            "Keys": [ 
                Q
            ],
            "Gamepads": [
                {
                    "Name": "XboxController",
                    "Button_Indicies": [
                        0
                    ],
                    "Hats": [
                        { "Index": 0, "Hat_State": {0, 1} }
                    ],
                    "Axes_Indicies": [
                        0 
                    ]
                },
                {
                    "Name": "Logitech",
                    "Button_Indicies": [
                        1
                    ],
                    "Hats": [
                        { "Index": 2, "Hat_State": {0, -1} }
                    ],
                    "Axes_Indicies": [
                        3 
                    ]
                }
            ]
        }
    ]
    }
 */

        /*controls(
            std::vector< //inputs
                std::tuple<
                    std::string, //name of this input (e.g: Up)
                    std::vector<Key>, //mapped keys
                    std::vector< //List of supported gamepads
                        std::tuple<
                            std::string, //gamepad name (e.g: Xbox)
                            std::vector<int>, //button indicies
                            std::vector< //hats
                                std::pair<
                                    int, //hat index
                                    hat_state_type //hat state
                                >
                            >,
                            std::vector<int>, //Axes indicies
                        >
                    >
                >
            >
        )*/
