// © Joseph Cameron - All Rights Reserved

#include <gdk/input/context.h>

#include <magic_enum/magic_enum.hpp>

using namespace gdk::input;

std::vector<keyboard::key> context::keys_down() const {
    std::vector<keyboard::key> out;

    for (const auto key : magic_enum::enum_values<keyboard::key>())
        if (key_down(key)) out.push_back(key);

    return out;
}

std::optional<keyboard::key> context::any_key_down() const {
    for (const auto key : magic_enum::enum_values<keyboard::key>())
        if (key_down(key)) return key;

    return {};
}

std::vector<mouse::button> context::mouse_buttons_down() const {
    std::vector<mouse::button> out;

    for (const auto button : magic_enum::enum_values<mouse::button>())
        if (mouse_button_down(button)) out.push_back(button);

    return out;
}

std::optional<mouse::button> context::any_mouse_button_down() const {
    for (const auto button : magic_enum::enum_values<mouse::button>())
        if (mouse_button_down(button)) return button;

    return {};
}
