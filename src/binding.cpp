// © Joseph Cameron - All Rights Reserved

#include <gdk/input/binding.h>

#include <ostream>

using namespace gdk::input;

binding binding::of(const keyboard::key aKey) {
    binding b;
    b.which = kind::key;
    b.key = aKey;

    return b;
}

binding binding::of(const mouse::button aButton) {
    binding b;
    b.which = kind::mouse_button;
    b.mouseButton = aButton;

    return b;
}

binding binding::of(const gamepad::button aButton) {
    binding b;
    b.which = kind::gamepad_button;
    b.gamepadButton = aButton;

    return b;
}

binding binding::of(const gamepad::axis aAxis, const value_type aScale) {
    binding b;
    b.which = kind::gamepad_axis;
    b.gamepadAxis = aAxis;
    b.scale = aScale;

    return b;
}

binding binding::of_device_button(const std::string &aGuid, const gamepad::index_type aIndex) {
    binding b;
    b.which = kind::device_button;
    b.guid = aGuid;
    b.index = aIndex;

    return b;
}

binding binding::of_device_axis(const std::string &aGuid, const gamepad::index_type aIndex,
    const value_type aScale) {
    binding b;
    b.which = kind::device_axis;
    b.guid = aGuid;
    b.index = aIndex;
    b.scale = aScale;

    return b;
}

bool binding::is_gamepad() const {
    return which != kind::key && which != kind::mouse_button;
}

bool binding::is_axis() const {
    return which == kind::gamepad_axis || which == kind::device_axis;
}

bool binding::operator==(const binding &aOther) const {
    if (which != aOther.which) return false;

    switch (which) {
        case kind::key: return key == aOther.key;
        case kind::mouse_button: return mouseButton == aOther.mouseButton;
        case kind::gamepad_button: return gamepadButton == aOther.gamepadButton;
        case kind::gamepad_axis: return gamepadAxis == aOther.gamepadAxis && scale == aOther.scale;
        case kind::device_button: return guid == aOther.guid && index == aOther.index;
        case kind::device_axis:
            return guid == aOther.guid && index == aOther.index && scale == aOther.scale;
    }

    return false;
}


std::ostream &operator<<(std::ostream &aStream, const gdk::input::binding &aBinding) {
    using kind = gdk::input::binding::kind;

    const auto direction = [&](const gdk::input::binding::value_type aScale) {
        return aScale < 0 ? "-" : "+";
    };

    switch (aBinding.which) {
        case kind::key: aStream << "key " << aBinding.key; break;
        case kind::mouse_button: aStream << "mouse " << aBinding.mouseButton; break;
        case kind::gamepad_button: aStream << "gamepad " << aBinding.gamepadButton; break;

        case kind::gamepad_axis:
            aStream << "gamepad " << direction(aBinding.scale) << aBinding.gamepadAxis;
        break;

        case kind::device_button:
            aStream << "button " << aBinding.index << " on " << aBinding.guid;
        break;

        case kind::device_axis:
            aStream << "axis " << direction(aBinding.scale) << aBinding.index
                << " on " << aBinding.guid;
        break;
    }

    return aStream;
}
