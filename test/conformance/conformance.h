// © Joseph Cameron - MIT License

#ifndef GDK_INPUT_CONFORMANCE_H
#define GDK_INPUT_CONFORMANCE_H

#include <gdk/input/gamepad.h>
#include <gdk/input/types.h>

#include <memory>
#include <string_view>

namespace gdk::input::conformance {
    /// \brief plays the platform for one implementation: what it does between two frames, the context sees after the second
    class driver {
    public:
        [[nodiscard]] virtual context_ptr_type context() = 0;

        virtual void frame() = 0;

        virtual void press(const keyboard::key aKey) = 0;
        virtual void release(const keyboard::key aKey) = 0;

        virtual void press(const mouse::button aButton) = 0;
        virtual void release(const mouse::button aButton) = 0;

        //! in pixels, y up
        virtual void move_cursor(const double aX, const double aY) = 0;

        //! in notches
        virtual void scroll(const double aX, const double aY) = 0;

        virtual void type(std::string_view aText) = 0;

        //! a pad with a standard mapping, for player 0. False where the implementation cannot have one attached
        [[nodiscard]] virtual bool attach_gamepad() = 0;
        virtual void detach_gamepad() = 0;

        virtual void press(const gamepad::button aButton) = 0;
        virtual void release(const gamepad::button aButton) = 0;

        //! as the game reads it: sticks -1 to 1, triggers 0 at rest to 1
        virtual void set_axis(const gamepad::axis aAxis, const float aValue) = 0;

        virtual ~driver() = default;
    };

    [[nodiscard]] std::unique_ptr<driver> make_driver();

    [[nodiscard]] std::string_view implementation();
}

#endif
