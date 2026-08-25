// © Joseph Cameron - All Rights Reserved

#ifndef GDK_INPUT_CONTROLS_H
#define GDK_INPUT_CONTROLS_H

#include <gdk/input/binding.h>
#include <gdk/input/gamepad.h>
#include <gdk/input/types.h>

#include <map>
#include <memory>
#include <string>
#include <vector>

namespace gdk::input {
    /// \brief what the game calls an input, and the physical inputs that produce it
    class controls final {
    public:
        using value_type = gdk::input::value_type;

        /// \param aContext where the physical input comes from
        /// \param aPlayer which player's gamepad to read. Keyboard and mouse are shared.
        [[nodiscard]] static std::shared_ptr<controls> make(context_ptr_type aContext,
            const std::size_t aPlayer = 0);

        /// \brief which player's gamepad this reads
        [[nodiscard]] std::size_t player() const;

        void bind(const std::string &aAction, const keyboard::key aKey);
        void bind(const std::string &aAction, const mouse::button aButton);

        //! applies to any gamepad with a standard mapping, whatever the hardware
        void bind(const std::string &aAction, const gamepad::button aButton);

        /// \param aScale multiplies the axis. Negative binds the opposite direction, which is how an
        /// action that means "left" is built out of an axis that reads negative when pushed left.
        void bind(const std::string &aAction, const gamepad::axis aAxis, const value_type aScale = 1);

        /// \brief bind a raw control on one specific kind of device
        void bind_device_button(const std::string &aAction, const std::string &aGuid,
            const gamepad::index_type aIndex);

        void bind_device_axis(const std::string &aAction, const std::string &aGuid,
            const gamepad::index_type aIndex, const value_type aScale = 1);

        /// \brief bind a source described as a value
        void bind(const std::string &aAction, const binding &aBinding);

        //! forget every source bound to an action
        void unbind(const std::string &aAction);

        //! forget one source, if the action has it. \returns whether it did
        bool unbind(const std::string &aAction, const binding &aBinding);

        //! every action that has at least one source bound, in a stable order
        [[nodiscard]] std::vector<std::string> actions() const;

        //! how many sources an action has
        [[nodiscard]] std::size_t source_count(const std::string &aAction) const;

        /// \brief every source bound to an action, in the order they were bound
        [[nodiscard]] std::vector<binding> sources(const std::string &aAction) const;

        /// \brief the strongest contribution from any source bound to this action
        ///
        /// A button contributes 1 while it is down. An axis contributes its value times its scale.
        /// The result is the contribution with the largest magnitude, so a keyboard key and a stick
        /// can drive one action and the one being used wins.
        [[nodiscard]] value_type get(const std::string &aAction) const;

        /// \brief whether the action is being driven, which is \ref get past \ref threshold
        [[nodiscard]] bool down(const std::string &aAction) const;

        //! whether any bound source began this frame
        [[nodiscard]] bool just_pressed(const std::string &aAction) const;

        //! whether any bound source ended this frame
        [[nodiscard]] bool just_released(const std::string &aAction) const;

        //! how far an axis must move before it counts as down
        [[nodiscard]] value_type threshold() const;
        void set_threshold(const value_type aThreshold);

    private:
        controls(context_ptr_type aContext, const std::size_t aPlayer);

        [[nodiscard]] const std::vector<binding> *sources_for(const std::string &aAction) const;

        [[nodiscard]] gamepad_ptr_type pad() const;

        context_ptr_type mpContext;
        std::size_t mPlayer;
        value_type mThreshold{0.1f};

        std::map<std::string, std::vector<binding>> mBindings;
    };

}

#endif
