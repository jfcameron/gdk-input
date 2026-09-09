// © Joseph Cameron - All Rights Reserved

#ifndef GDK_INPUT_TYPES_H
#define GDK_INPUT_TYPES_H

#include <cstddef>
#include <iosfwd>
#include <memory>

namespace gdk::input {
    class context;
    class controls;
    class capture;
    class gamepad;

    using context_ptr_type = std::shared_ptr<context>;
    using controls_ptr_type = std::shared_ptr<controls>;
    using capture_ptr_type = std::shared_ptr<capture>;

    using gamepad_ptr_type = std::shared_ptr<gamepad>;
    using value_type = float;
}

namespace gdk::input::mouse {
    enum class button { 
        left, 
        right,
        middle,
        four, 
        five,
        six,
        seven,
        eight
    };

    enum class button_state {
        up,
        just_pressed, 
        just_released,
        held_down 
    };

    enum class cursor_mode {
        normal,
        hidden,
        locked
    };

    enum class axis {
        x, y,
        scroll_x, scroll_y
    };
    
    struct cursor_2d_type {
        double x, y;
    };

    struct scroll_2d_type {
        double x, y;
    };
}

namespace gdk::input::keyboard {
    enum class key_state {
        up,
        just_pressed, 
        just_released,
        held_down 
    };

    enum class key {
        escape, 
        f1, f2, f3, f4, f5, f6, f7, f8, f9, f10, f11, f12, 
        printscreen, scrolllock, pausebreak, 
        q, w, e, r, t, y, u, i, o, p, a, s, d, f, g, h, j, k, l, z, x, c, v, b, n, m, 
        one, two, three, four, five, six, seven, eight, nine, zero, 
        tilda, minus, equals, backspace, home, end, 
        tab, openbracket, closebracket, backslash, insert, pageup, 
        capslock, semicolon, quote, enter, deletekey, pagedown, 
        leftshift, comma, period, forwardslash, rightshift, 
        leftcontrol, leftalt, space, rightalt, rightcontrol,
        leftarrow, rightarrow, uparrow, downarrow, 
        numlock, numslash, numasterisk, numminus, 
        num7, num8, num9, numplus, 
        num4, num5, num6, 
        num1, num2, num3, numenter, 
        num0, numperiod
    };
}
std::ostream& operator<<(std::ostream&, const gdk::input::keyboard::key);
std::ostream& operator<<(std::ostream&, const gdk::input::keyboard::key_state);
std::ostream& operator<<(std::ostream&, const gdk::input::mouse::axis);
std::ostream& operator<<(std::ostream&, const gdk::input::mouse::button);
std::ostream& operator<<(std::ostream&, const gdk::input::mouse::button_state);
std::ostream& operator<<(std::ostream&, const gdk::input::mouse::cursor_mode);

#endif
