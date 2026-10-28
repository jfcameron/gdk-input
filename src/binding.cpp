// © Joseph Cameron - MIT License

#include <gdk/input/binding.h>

#include <ostream>
#include <string_view>

using namespace gdk::input;

bool gdk::input::is_modifier(const keyboard::key aKey) {
    using keyboard::key;

    return aKey == key::leftshift || aKey == key::rightshift || aKey == key::leftcontrol || aKey == key::rightcontrol
        || aKey == key::leftalt || aKey == key::rightalt;
}

binding binding::of(const keyboard::key aKey, const text::modifiers aWith) {
    binding b;
    b.which = kind::key;
    b.key = aKey;
    b.with = aWith;

    return b;
}

binding binding::of(const mouse::button aButton, const text::modifiers aWith) {
    binding b;
    b.which = kind::mouse_button;
    b.mouseButton = aButton;
    b.with = aWith;

    return b;
}

binding binding::of(const mouse::axis aAxis, const value_type aScale) {
    binding b;
    b.which = kind::pointer_axis;
    b.pointerAxis = aAxis;
    b.scale = aScale;

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

binding binding::of_device_button(std::string_view aGuid, const gamepad::index_type aIndex) {
    binding b;
    b.which = kind::device_button;
    b.guid = aGuid;
    b.index = aIndex;

    return b;
}

binding binding::of_device_axis(std::string_view aGuid, const gamepad::index_type aIndex,
    const value_type aScale) {
    binding b;
    b.which = kind::device_axis;
    b.guid = aGuid;
    b.index = aIndex;
    b.scale = aScale;

    return b;
}

bool binding::is_gamepad() const {
    return which != kind::key && which != kind::mouse_button && which != kind::pointer_axis;
}

bool binding::is_axis() const {
    return which == kind::gamepad_axis || which == kind::device_axis
        || which == kind::pointer_axis;
}

bool binding::is_chord() const {
    return (which == kind::key || which == kind::mouse_button) && with != text::modifiers{};
}

bool binding::operator==(const binding &aOther) const {
    if (which != aOther.which) return false;

    switch (which) {
        case kind::key: return key == aOther.key && with == aOther.with;
        case kind::mouse_button: return mouseButton == aOther.mouseButton && with == aOther.with;
        case kind::pointer_axis: return pointerAxis == aOther.pointerAxis && scale == aOther.scale;
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

    const auto held = [&] {
        if (aBinding.with.shift) aStream << "shift+";
        if (aBinding.with.control) aStream << "control+";
        if (aBinding.with.alt) aStream << "alt+";
        if (aBinding.with.super) aStream << "super+";
    };

    switch (aBinding.which) {
        case kind::key: aStream << "key "; held(); aStream << aBinding.key; break;
        case kind::mouse_button: aStream << "mouse "; held(); aStream << aBinding.mouseButton; break;

        case kind::pointer_axis:
            aStream << "pointer " << direction(aBinding.scale) << aBinding.pointerAxis;
        break;

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
