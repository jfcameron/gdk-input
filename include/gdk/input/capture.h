// © Joseph Cameron - All Rights Reserved

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
        /// TODO: this should be moved to a policy object. 
        static constexpr value_type DEFAULT_THRESHOLD{0.6f};

        /// \param aContext where the physical input comes from
        /// \param aPlayer whose gamepad to watch. Keyboard and mouse are shared, as everywhere else.
        [[nodiscard]] static std::shared_ptr<capture> make(context_ptr_type aContext,
            const std::size_t aPlayer = 0);

        /// \brief look at the current frame; call once per frame after the context has updated
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

    private:
        capture(context_ptr_type aContext, const std::size_t aPlayer);

        /// \brief everything that is active right now
        struct observation final {
            std::vector<keyboard::key> keys;
            std::vector<mouse::button> mouseButtons;
            std::vector<gamepad::index_type> gamepadButtons;

            std::vector<std::pair<gamepad::index_type, value_type>> gamepadAxes;

            std::string guid; 
        };

        [[nodiscard]] observation observe() const;

        [[nodiscard]] std::optional<binding> answer_from(const observation &aNow) const;

        [[nodiscard]] gamepad_ptr_type pad() const;

        context_ptr_type mpContext;
        std::size_t mPlayer;
        value_type mThreshold{DEFAULT_THRESHOLD};
        bool mAcceptsMouse{false};

        observation mPrevious;
        std::optional<binding> mResult;
    };

}

#endif
