// © Joseph Cameron - MIT License

#include "conformance.h"

#include <gdk/input/null_context.h>

#include <functional>
#include <string>
#include <utility>
#include <vector>

using namespace gdk::input;

namespace {
    class null_driver final : public conformance::driver {
    public:
        context_ptr_type context() override { return m_pContext; }

        void frame() override {
            for (const auto &step : std::exchange(m_BeforeUpdate, {})) step();

            m_pContext->update();

            for (const auto &step : std::exchange(m_AfterUpdate, {})) step();

            if (m_Delta.x != 0 || m_Delta.y != 0) m_pContext->set_mouse_delta(std::exchange(m_Delta, {0, 0}));
            if (m_Scroll.x != 0 || m_Scroll.y != 0) m_pContext->set_scroll_delta(std::exchange(m_Scroll, {0, 0}));
        }

        void press(const keyboard::key aKey) override { later([=, this] { m_pContext->press_key(aKey); }); }
        void release(const keyboard::key aKey) override { later([=, this] { m_pContext->release_key(aKey); }); }

        void press(const mouse::button aButton) override {
            later([=, this] { m_pContext->press_mouse_button(aButton); });
        }

        void release(const mouse::button aButton) override {
            later([=, this] { m_pContext->release_mouse_button(aButton); });
        }

        void move_cursor(const double aX, const double aY) override {
            m_Delta.x += aX;
            m_Delta.y += aY;

            m_Cursor.x += aX / 320;
            m_Cursor.y += aY / 240;

            later([=, this] { m_pContext->set_cursor_position(m_Cursor); });
        }

        void scroll(const double aX, const double aY) override {
            m_Scroll.x += aX;
            m_Scroll.y += aY;
        }

        void type(std::string_view aText) override {
            later([=, this, text = std::string(aText)] { m_pContext->type_text(text); });
        }

        bool attach_gamepad() override {
            later([this] { static_cast<void>(m_pContext->attach_gamepad("null pad", "0300000000000000000000000000ab01")); });

            return true;
        }

        void detach_gamepad() override { later([this] { m_pContext->detach_gamepad(0); }); }

        void press(const gamepad::button aButton) override {
            first([=, this] { m_pContext->gamepad_at(0)->press(static_cast<gamepad::index_type>(aButton)); });
        }

        void release(const gamepad::button aButton) override {
            first([=, this] { m_pContext->gamepad_at(0)->release(static_cast<gamepad::index_type>(aButton)); });
        }

        void set_axis(const gamepad::axis aAxis, const float aValue) override {
            const bool trigger = aAxis == gamepad::axis::left_trigger || aAxis == gamepad::axis::right_trigger;

            first([=, this] {
                m_pContext->gamepad_at(0)->set_axis(static_cast<gamepad::index_type>(aAxis),
                    trigger ? aValue * 2 - 1 : aValue);
            });
        }

    private:
        void first(std::function<void()> aStep) { m_BeforeUpdate.push_back(std::move(aStep)); }
        void later(std::function<void()> aStep) { m_AfterUpdate.push_back(std::move(aStep)); }

        std::shared_ptr<null_context> m_pContext = null_context::make();

        std::vector<std::function<void()>> m_BeforeUpdate;
        std::vector<std::function<void()>> m_AfterUpdate;

        mouse::cursor_2d_type m_Cursor{0.5, 0.5};
        mouse::cursor_2d_type m_Delta{0, 0};
        mouse::scroll_2d_type m_Scroll{0, 0};
    };
}

namespace gdk::input::conformance {
    std::unique_ptr<driver> make_driver() { return std::make_unique<null_driver>(); }

    std::string_view implementation() { return "null"; }
}
