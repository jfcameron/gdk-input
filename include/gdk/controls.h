// © 2019 Joseph Cameron - All Rights Reserved

#ifndef GDK_CONTROLS_H
#define GDK_CONTROLS_H

#include <map>
#include <string_view>
#include <string>

#include <gdk/gamepads.h>
#include <gdk/keyboard.h>
#include <gdk/mouse.h>

namespace gdk
{
    /// \brief user input mapping abstraction
    /// abstraction on top of gamepads, mouse, keyboards.
    class controls
    {
        using key_collection_type = std::vector<keyboard::Key>;

        std::map<std::string, key_collection_type> m_Inputs;

        std::shared_ptr<keyboard> m_Keyboard;

    public:
        float get(const std::string &aName);

        controls() = delete;

        controls(std::shared_ptr<keyboard> aKeyboard);
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
    };
}

#endif

