// © Joseph Cameron - All Rights Reserved

#ifndef GDK_INPUT_IMPL_JOYSTICKS_H
#define GDK_INPUT_IMPL_JOYSTICKS_H

#include <cstddef>
#include <string>
#include <vector>

namespace gdk::input {
    /// \brief one poll of a joystick
    struct joystick_reading final {
        bool present{false};

        //! laid out as GLFW's gamepad: 15 buttons and 6 axes in its order, the triggers -1 to 1
        bool standard{false};

        std::string name;

        std::vector<float> axes;

        std::vector<unsigned char> buttons;

        std::vector<unsigned char> hats;
    };

    [[nodiscard]] std::size_t joystick_count();

    /// \brief should be called once per frame 
    void sample_joysticks();

    [[nodiscard]] bool joystick_present(const int aJoystick);

    [[nodiscard]] std::string joystick_guid(const int aJoystick);

    [[nodiscard]] joystick_reading read_joystick(const int aJoystick);

    [[nodiscard]] bool update_gamepad_mappings(const std::string &aMappings);
}

#endif
