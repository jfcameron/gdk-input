// © Joseph Cameron - All Rights Reserved

#include <gdk/input/impl_text_input.h>

#include <utility>

using namespace gdk::input;

void gdk::input::append_utf8(std::string &aText, char32_t aCodePoint) {
    constexpr char32_t REPLACEMENT = 0xFFFD;

    if (aCodePoint > 0x10FFFF || (aCodePoint >= 0xD800 && aCodePoint <= 0xDFFF))
        aCodePoint = REPLACEMENT;

    const auto byte = [&aText](const char32_t aValue) { aText.push_back(static_cast<char>(aValue)); };

    if (aCodePoint < 0x80) {
        byte(aCodePoint);
    }
    else if (aCodePoint < 0x800) {
        byte(0xC0 | (aCodePoint >> 6));
        byte(0x80 | (aCodePoint & 0x3F));
    }
    else if (aCodePoint < 0x10000) {
        byte(0xE0 | (aCodePoint >> 12));
        byte(0x80 | ((aCodePoint >> 6) & 0x3F));
        byte(0x80 | (aCodePoint & 0x3F));
    }
    else {
        byte(0xF0 | (aCodePoint >> 18));
        byte(0x80 | ((aCodePoint >> 12) & 0x3F));
        byte(0x80 | ((aCodePoint >> 6) & 0x3F));
        byte(0x80 | (aCodePoint & 0x3F));
    }
}

void gdk::input::append_text(std::vector<text::event> &aEvents, const std::string &aText) {
    if (aText.empty()) return;

    if (!aEvents.empty())
        if (auto *const pText = std::get_if<std::string>(&aEvents.back())) {
            pText->append(aText);

            return;
        }

    aEvents.emplace_back(aText);
}

keyboard::key_state withheld_key::advance(const bool aDown, const bool aTextFocus) {
    if (aTextFocus) m_Withheld = true;
    else if (!aDown) m_Withheld = false;

    const bool down = aDown && !m_Withheld;
    const bool wasDown = std::exchange(m_WasDown, down);

    if (down) return wasDown ? keyboard::key_state::held_down : keyboard::key_state::just_pressed;

    return wasDown ? keyboard::key_state::just_released : keyboard::key_state::up;
}
