// © 2019 Joseph Cameron - All Rights Reserved

#include <gdk/controls.h>

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

namespace gdk
{
    controls::controls(std::shared_ptr<keyboard> aKeyboard)
    : m_Keyboard(aKeyboard)
    {
        
    }

    float controls::get(const std::string &aName)
    {
        auto iter = m_Inputs.find(aName); 

        if (iter == m_Inputs.end()) throw std::invalid_argument(std::string("TAG").append("not a valid button: ").append(aName));

        const auto &blar = iter->second;

        for (key_collection_type::size_type i = 0, s = blar.size(); i < s; ++i)
        {
            return static_cast<float>(m_Keyboard->getKeyDown(blar[i]));
        }

        return 0;
    }
}

