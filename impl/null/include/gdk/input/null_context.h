// © Joseph Cameron - All Rights Reserved

#ifndef GDK_INPUT_NULL_CONTEXT_H
#define GDK_INPUT_NULL_CONTEXT_H

#include <gdk/input/context.h>
#include <gdk/input/null_gamepad.h>

#include <array>
#include <memory>
#include <set>
#include <vector>

namespace gdk::input {
    /// \brief an input context with no window and no hardware behind it
    class null_context final : public context {
    public:
        static constexpr std::size_t GAMEPAD_SLOT_COUNT = 16;

        [[nodiscard]] static std::shared_ptr<null_context> make();

        [[nodiscard]] virtual bool key_down(const keyboard::key &aKey) const override;
        [[nodiscard]] virtual bool key_just_pressed(const keyboard::key &aKey) const override;
        [[nodiscard]] virtual bool key_just_released(const keyboard::key &aKey) const override;

        [[nodiscard]] virtual bool mouse_button_down(const mouse::button &aButton) const override;
        [[nodiscard]] virtual bool mouse_button_just_pressed(const mouse::button &aButton) const override;
        [[nodiscard]] virtual bool mouse_button_just_released(const mouse::button &aButton) const override;

        [[nodiscard]] virtual mouse::cursor_2d_type mouse_cursor_position() const override;
        [[nodiscard]] virtual mouse::cursor_2d_type mouse_delta() const override;
        [[nodiscard]] virtual mouse::scroll_2d_type mouse_scroll_delta() const override;
        [[nodiscard]] virtual std::optional<mouse::axis> any_mouse_axis_down(float threshold) const override;
        [[nodiscard]] virtual mouse::cursor_mode mouse_cursor_mode() const override;
        virtual void set_mouse_cursor_mode(mouse::cursor_mode aMode) override;

        [[nodiscard]] virtual gamepad_ptr get_gamepad(const size_t index) override;
        [[nodiscard]] virtual gamepad_collection_type gamepads() override;
        virtual void swap_players(const std::size_t aLeft, const std::size_t aRight) override;
        virtual std::size_t add_gamepad_mappings(const std::string &aMappings) override;

        [[nodiscard]] std::shared_ptr<null_gamepad> gamepad_at(const std::size_t index);

        [[nodiscard]] std::optional<std::size_t> attach_gamepad(const std::string &aName,
            const std::string &aGuid);

        [[nodiscard]] std::optional<std::size_t> attach_gamepad_at_port(const std::size_t aPort,
            const std::string &aName, const std::string &aGuid);

        void detach_gamepad(const std::size_t aPlayer);

        [[nodiscard]] std::optional<std::size_t> port_of(const std::size_t aPlayer) const;

        void press_key(const keyboard::key aKey);
        void release_key(const keyboard::key aKey);
        void press_mouse_button(const mouse::button aButton);
        void release_mouse_button(const mouse::button aButton);
        void set_cursor_position(const mouse::cursor_2d_type aPosition);
        void set_mouse_delta(const mouse::cursor_2d_type aDelta);
        void set_scroll_delta(const mouse::scroll_2d_type aDelta);

        void update();

        virtual ~null_context() override = default;

    private:
        null_context();

        template <typename key_type> struct edge_set final {
            std::set<key_type> held;
            std::set<key_type> pressedThisFrame;
            std::set<key_type> releasedThisFrame;

            void advance() {
                pressedThisFrame.clear();
                releasedThisFrame.clear();
            }
        };

        edge_set<keyboard::key> mKeys;
        edge_set<mouse::button> mMouseButtons;

        mouse::cursor_2d_type mCursorPosition{0, 0};
        mouse::cursor_2d_type mMouseDelta{0, 0};
        mouse::scroll_2d_type mScrollDelta{0, 0};
        mouse::cursor_mode mCursorMode{mouse::cursor_mode::normal};

        std::array<std::shared_ptr<null_gamepad>, GAMEPAD_SLOT_COUNT> mGamepads;

        void promote_mapped_devices();

        std::set<std::size_t> mPortsInUse;

        std::set<std::string> mMappedGuids;
    };
}

#endif
