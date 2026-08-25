// © Joseph Cameron - All Rights Reserved

#ifndef GDK_INPUT_NULL_GAMEPAD_H
#define GDK_INPUT_NULL_GAMEPAD_H

#include <gdk/input/gamepad.h>

#include <string>
#include <vector>

namespace gdk::input {
    class null_gamepad final : public gamepad {
    public:
        [[nodiscard]] virtual bool connected() const override;
        [[nodiscard]] virtual std::string_view name() const override;
        [[nodiscard]] virtual std::string_view guid() const override;
        [[nodiscard]] virtual bool just_connected() const override;
        [[nodiscard]] virtual bool just_disconnected() const override;
        [[nodiscard]] virtual bool has_standard_mapping() const override;
        [[nodiscard]] virtual std::size_t button_count() const override;
        [[nodiscard]] virtual std::size_t axis_count() const override;
        [[nodiscard]] virtual std::size_t hat_count() const override;
        [[nodiscard]] virtual hat_state_type hat(const index_type index) const override;

        [[nodiscard]] virtual axis_value_type axis_value(const index_type index,
            const axis_value_type threshold) const override;
        [[nodiscard]] virtual bool axis_just_exceeded_threshold(const index_type index,
            const axis_value_type threshold) const override;
        [[nodiscard]] virtual bool axis_just_dropped_below_threshold(const index_type index,
            const axis_value_type threshold) const override;
        [[nodiscard]] virtual axis_value_type previous_axis_value(const index_type index,
            const axis_value_type threshold) const override;
        [[nodiscard]] virtual std::optional<std::pair<index_type, axis_value_type>> any_axis_down(
            axis_value_type threshold) const override;

        [[nodiscard]] virtual bool button_down(const index_type index) const override;
        [[nodiscard]] virtual bool button_just_pressed(const index_type index) const override;
        [[nodiscard]] virtual bool button_just_released(const index_type index) const override;
        [[nodiscard]] virtual std::optional<button_collection_type::size_type> any_button_down()
            const override;

        void attach(const std::string &aName, const std::string &aGuid,
            const std::size_t aButtonCount = 15, const std::size_t aAxisCount = 6);

        void set_port(const std::size_t aPort);

        [[nodiscard]] std::optional<std::size_t> last_port() const;

        void attach_unmapped(const std::string &aName, const std::string &aGuid,
            const std::size_t aButtonCount, const std::size_t aAxisCount,
            const std::size_t aHatCount = 1);

        void set_hat(const index_type index, const hat_state_type aState);

        void set_standard_mapping(const bool aMapped);

        void detach();

        void press(const index_type index);

        void release(const index_type index);

        void set_axis(const index_type index, const axis_value_type value);

        void update();

    private:
        void rest_triggers();

        enum class button_state { up, just_pressed, just_released, held_down };

        bool mConnected{false};
        std::string mName;
        std::string mGuid;

        std::vector<button_state> mButtons;
        std::vector<bool> mHeld;                
        std::vector<axis_value_type> mPending;  
        std::vector<axis_value_type> mAxes;
        std::vector<axis_value_type> mLastAxes;
        std::vector<hat_state_type> mHats;
        bool mHasStandardMapping{true};
        std::optional<std::size_t> mLastPort;
        bool mJustConnected{false};
        bool mJustDisconnected{false};
    };
}

#endif
