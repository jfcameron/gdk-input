// © Joseph Cameron - All Rights Reserved

#ifndef GDK_INPUT_TEXT_H
#define GDK_INPUT_TEXT_H

#include <gdk/input/types.h>

#include <array>
#include <cstddef>
#include <string>
#include <variant>

namespace gdk::input::text {
    //! the modifier keys held when an editing key was pressed
    struct modifiers final {
        bool shift = false;
        bool control = false;
        bool alt = false;
        bool super = false;

        [[nodiscard]] bool operator==(const modifiers &) const = default;
    };

    /// \brief the keys reported as edits: moving the caret, deleting, and ending the text
    inline constexpr std::array<keyboard::key, 14> EDITING_KEYS {
        keyboard::key::backspace, keyboard::key::deletekey,
        keyboard::key::leftarrow, keyboard::key::rightarrow,
        keyboard::key::uparrow, keyboard::key::downarrow,
        keyboard::key::home, keyboard::key::end,
        keyboard::key::pageup, keyboard::key::pagedown,
        keyboard::key::enter, keyboard::key::numenter,
        keyboard::key::escape, keyboard::key::tab
    };

    [[nodiscard]] constexpr bool is_editing_key(const keyboard::key aKey) {
        for (const auto key : EDITING_KEYS) if (key == aKey) return true;

        return false;
    }

    struct edit final {
        keyboard::key key = keyboard::key::backspace; //!< one of \ref EDITING_KEYS

        modifiers held;

        /// \brief the keyboard repeating a key held down, rather than a press
        bool repeat = false;

        [[nodiscard]] bool operator==(const edit &) const = default;
    };

    using event = std::variant<std::string, edit>;

    /// \brief the text an input method is composing, before it commits it
    struct composition final {
        std::string text;

        std::size_t caret = 0;

        std::size_t focusBegin = 0;
        std::size_t focusEnd = 0;

        [[nodiscard]] bool empty() const { return text.empty(); }

        [[nodiscard]] bool operator==(const composition &) const = default;
    };

    /// \brief where the caret of the field with focus is in the window
    ///
    /// Normalised across the window as the cursor is (\ref context::mouse_cursor_position): 0,0 the
    /// bottom left, 1,1 the top right. x and y are the caret's bottom left corner.
    struct caret final {
        double x = 0;
        double y = 0;
        double width = 0;
        double height = 0;

        [[nodiscard]] bool operator==(const caret &) const = default;
    };
}

#endif
