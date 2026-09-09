// © Joseph Cameron - All Rights Reserved

#ifndef GDK_INPUT_BINDING_H
#define GDK_INPUT_BINDING_H

#include <gdk/input/gamepad.h>
#include <gdk/input/types.h>

#include <iosfwd>
#include <string>

namespace gdk::input {
    /// \brief one physical control, named in the terms the hardware uses
    struct binding final {
        using value_type = gdk::input::value_type;

        enum class kind {
            key,
            mouse_button,
            pointer_axis,   //!< cursor motion or the wheel, \see mouse::axis
            gamepad_button, //!< standard layout
            gamepad_axis,   //!< standard layout
            device_button,  //!< a raw index on the device named by `guid`
            device_axis     //!< a raw index
        };

        kind which{kind::key};

        keyboard::key key{};
        mouse::button mouseButton{};
        mouse::axis pointerAxis{};
        gamepad::button gamepadButton{};
        gamepad::axis gamepadAxis{};

        std::string guid; 
        gamepad::index_type index{};

        /// \brief multiplies an axis; unused by the button kinds
        value_type scale{1};

        [[nodiscard]] static binding of(const keyboard::key aKey);
        [[nodiscard]] static binding of(const mouse::button aButton);
        [[nodiscard]] static binding of(const gamepad::button aButton);
        [[nodiscard]] static binding of(const gamepad::axis aAxis, const value_type aScale = 1);

        /// \brief bind cursor motion or the wheel
        [[nodiscard]] static binding of(const mouse::axis aAxis, const value_type aScale = 1);

        [[nodiscard]] static binding of_device_button(const std::string &aGuid,
            const gamepad::index_type aIndex);

        [[nodiscard]] static binding of_device_axis(const std::string &aGuid,
            const gamepad::index_type aIndex, const value_type aScale = 1);

        //! whether this names a control on a gamepad, in either of the two forms
        [[nodiscard]] bool is_gamepad() const;

        //! whether this names an axis, in either form. The other kinds are all buttons.
        [[nodiscard]] bool is_axis() const;

        [[nodiscard]] bool operator==(const binding &) const;
    };
}

/// \brief a short human-readable form, for a settings screen to show and a person to read in a log
/// note this is not a serialization format.
/// \todo possibly remove this as it could be a source of confusion
std::ostream &operator<<(std::ostream &, const gdk::input::binding &);

#endif
