// © Joseph Cameron - All Rights Reserved

#ifndef GDK_INPUT_IMPL_TEXT_INPUT_H
#define GDK_INPUT_IMPL_TEXT_INPUT_H

#include <gdk/input/text.h>

#include <string>
#include <vector>

namespace gdk::input {
    /// \brief append a code point to UTF-8
    void append_utf8(std::string &aText, char32_t aCodePoint);

    /// \brief text committed as UTF-8, joined onto the text before it if that was text too
    void append_text(std::vector<text::event> &aEvents, const std::string &aText);

    /// \brief keeps the keyboard from everything but text input while a text field has it
    ///
    /// One per key. \see context::set_text_input_focus
    class withheld_key final {
    public:
        /// \brief what the key reads as this update, told whether it is down and a field has the keyboard
        [[nodiscard]] keyboard::key_state advance(const bool aDown, const bool aTextFocus);

    private:
        bool m_WasDown = false;
        bool m_Withheld = false;
    };
}

#endif
