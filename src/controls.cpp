// © 2019 Joseph Cameron - All Rights Reserved

#include <type_traits>
#include <iostream>

#include <gdk/controls.h>

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
    
    void controls::addKeyMapping(const std::string &aName, const keyboard::Key aKey)
    {
        m_Inputs[aName].keys.insert(aKey);
    }
    
    void controls::addMouseButtonMapping(const std::string &aName, const mouse::Button aButton)
    {
        m_Inputs[aName].mouse.buttons.insert(aButton);
    }

    void controls::addMouseAxisMapping(const std::string &aName, const mouse::Axis aAxis, const double aScaleAndDirection)
    {
        m_Inputs[aName].mouse.axes.insert({aAxis, aScaleAndDirection});
    }

    void controls::addGamepadAxisMapping(const std::string &aInputName, const std::string &aGamepadName, const int aAxisIndex, const float aMinimum)
    {
        m_Inputs[aInputName].gamepads[aGamepadName].axes.insert({aAxisIndex, aMinimum});
    }

    void controls::addGamepadHatMapping(const std::string &aInputName, const std::string &aGamepadName, const int aHatIndex, const gamepad::hat_state_type aHatState)
    {
        m_Inputs[aInputName].gamepads[aGamepadName].hats.insert({aHatIndex, aHatState});
    }

    void controls::addGamepadButtonMapping(const std::string &aInputName, const std::string &aGamepadName, const int aButtonIndex)
    {
        m_Inputs[aInputName].gamepads[aGamepadName].buttons.insert(aButtonIndex);
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
