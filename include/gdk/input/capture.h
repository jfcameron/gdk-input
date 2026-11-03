// © Joseph Cameron - MIT License

#ifndef GDK_INPUT_CAPTURE_H
#define GDK_INPUT_CAPTURE_H

#include <gdk/input/binding.h>
#include <gdk/input/types.h>

#include <memory>
#include <optional>
#include <string>
#include <utility>
#include <vector>

namespace gdk::input {
    /// \brief waits for the player to press something and says what it was
    class capture final {
    public:
        using value_type = binding::value_type;

        /// \brief how far an axis must move to count as an answer
        static constexpr value_type DEFAULT_THRESHOLD{0.6f};

        /// \param aContext where the physical input comes from
        /// \param aPlayer whose gamepad to watch. Keyboard and mouse are shared, as everywhere else.
        [[nodiscard]] static std::shared_ptr<capture> make(context_ptr_type aContext,
            const std::size_t aPlayer = 0);

        /// \brief look at the current frame; call once per frame after the context has updated
        ///
        /// A key or mouse button pressed while a modifier is held answers as a chord. A modifier answers alone when it is
        /// let go with nothing pressed while it was held, and no other modifier held with it.
        ///
        /// \return nothing until the player presses something, then that something on every call
        /// until \ref reset. Latching rather than one-shot so a game can poll, draw the answer and
        /// wait for a confirmation without having to store it.
        std::optional<binding> poll();

        //! the last result
        [[nodiscard]] std::optional<binding> result() const;

        [[nodiscard]] bool captured() const;

        /// \brief clear the result and try to grab a new one
        void reset();

        [[nodiscard]] value_type threshold() const;
        void set_threshold(const value_type aThreshold);

        /// \brief whether a mouse button may be the answer. Off by default
        [[nodiscard]] bool accepts_mouse() const;
        void set_accepts_mouse(const bool aAccepts);

        /// \brief whether a turn of the wheel may be the answer, as a pointer axis whose scale is the way it turned. Off
        /// by default
        [[nodiscard]] bool accepts_wheel() const;
        void set_accepts_wheel(const bool aAccepts);

    private:
        capture(context_ptr_type aContext, const std::size_t aPlayer);

        /// \brief everything that is active right now
        struct observation final {
            std::vector<keyboard::key> keys;
            std::vector<mouse::button> mouseButtons;
            std::vector<gamepad::index_type> gamepadButtons;

            std::vector<std::pair<gamepad::index_type, value_type>> gamepadAxes;

            mouse::scroll_2d_type scroll{0, 0};

            std::string guid; 
        };

        [[nodiscard]] observation observe() const;

        [[nodiscard]] std::optional<binding> answer_from(const observation &aNow) const;

        //! a modifier let go alone, keeping track of the one held meanwhile
        [[nodiscard]] std::optional<binding> alone(const observation &aNow);

        [[nodiscard]] gamepad_ptr_type pad() const;

        context_ptr_type mpContext;
        std::size_t mPlayer;
        value_type mThreshold{DEFAULT_THRESHOLD};
        bool mAcceptsMouse{false};
        bool mAcceptsWheel{false};

        //! the modifier pressed with nothing else since, which answers if it is let go so
        std::optional<keyboard::key> mLoneModifier;
        //! something was pressed beside a modifier still held, so none answers alone until all are let go
        bool mModifierSpoiled{false};

        observation mPrevious;
        std::optional<binding> mResult;
    };

}

#endif
