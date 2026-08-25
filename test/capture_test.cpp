// © Joseph Cameron - All Rights Reserved

#include <jfc/catch.hpp>

#include <gdk/input/capture.h>
#include <gdk/input/controls.h>
#include <gdk/input/null_context.h>

#include <sstream>
#include <string>

using namespace gdk::input;

namespace {
    struct fixture final {
        std::shared_ptr<null_context> pNull{null_context::make()};
        context_ptr_type pContext{pNull};

        [[nodiscard]] std::shared_ptr<capture> begin(const std::size_t aPlayer = 0) {
            return capture::make(pContext, aPlayer);
        }

        std::size_t attach(const std::string &aName = "pad", const std::string &aGuid = "guid-pad") {
            return pNull->attach_gamepad(aName, aGuid).value_or(0);
        }

        std::size_t attach_settled(capture &aCapture, const std::string &aName = "pad",
            const std::string &aGuid = "guid-pad") {
            const auto player = attach(aName, aGuid);

            idle(aCapture);

            return player;
        }

        void idle(capture &aCapture) {
            pNull->update();
            aCapture.poll();
        }
    };

    [[nodiscard]] std::string render(const binding &a) {
        std::ostringstream out;
        out << a;

        return out.str();
    }
}

TEST_CASE("a capture answers with what was pressed", "[capture]") {
    fixture f;

    auto pCapture = f.begin();

    REQUIRE_FALSE(pCapture->captured());
    REQUIRE_FALSE(pCapture->poll());

    SECTION("a key") {
        f.pNull->press_key(keyboard::key::j);

        const auto pressed = pCapture->poll();

        REQUIRE(pressed);
        REQUIRE(*pressed == binding::of(keyboard::key::j));
        REQUIRE(pCapture->captured());
    }

    SECTION("a gamepad button, named, because the device has a standard mapping") {
        const auto player = f.attach_settled(*pCapture);

        f.pNull->gamepad_at(player)->press(static_cast<gamepad::index_type>(gamepad::button::y));
        f.pNull->update();

        const auto pressed = pCapture->poll();

        REQUIRE(pressed);
        REQUIRE(*pressed == binding::of(gamepad::button::y));
    }

    SECTION("a stick, with the direction it was pushed") {
        const auto player = f.attach_settled(*pCapture);

        f.pNull->gamepad_at(player)->set_axis(
            static_cast<gamepad::index_type>(gamepad::axis::left_x), -0.9f);
        f.pNull->update();

        const auto pressed = pCapture->poll();

        REQUIRE(pressed);
        REQUIRE(*pressed == binding::of(gamepad::axis::left_x, -1.f));

        REQUIRE(*pressed != binding::of(gamepad::axis::left_x, 1.f));
    }

    SECTION("and the answer keeps being the answer until it is thrown away") {
        f.pNull->press_key(keyboard::key::j);
        pCapture->poll();

        f.pNull->release_key(keyboard::key::j);
        f.idle(*pCapture);
        f.idle(*pCapture);

        REQUIRE(pCapture->result() == binding::of(keyboard::key::j));

        pCapture->reset();

        REQUIRE_FALSE(pCapture->captured());
    }
}

TEST_CASE("what the player was already holding is not the answer", "[capture]") {
    fixture f;

    SECTION("the key that opened the prompt") {
        f.pNull->press_key(keyboard::key::enter);
        f.pNull->update();

        auto pCapture = f.begin();

        f.idle(*pCapture);
        f.idle(*pCapture);

        REQUIRE_FALSE(pCapture->captured());

        SECTION("and it stays excluded for as long as it is held, not just for a frame") {
            for (int i = 0; i < 10; ++i) f.idle(*pCapture);

            REQUIRE_FALSE(pCapture->captured());
        }

        SECTION("but pressing it again, after letting go, is a real answer") {
            f.pNull->release_key(keyboard::key::enter);
            f.idle(*pCapture);

            f.pNull->press_key(keyboard::key::enter);

            REQUIRE(pCapture->poll() == binding::of(keyboard::key::enter));
        }
    }

    SECTION("a gamepad button held from before") {
        const auto player = f.attach();

        f.pNull->gamepad_at(player)->press(static_cast<gamepad::index_type>(gamepad::button::a));
        f.pNull->update();

        auto pCapture = f.begin();

        f.idle(*pCapture);

        REQUIRE_FALSE(pCapture->captured());

        f.pNull->gamepad_at(player)->press(static_cast<gamepad::index_type>(gamepad::button::b));
        f.pNull->update();

        REQUIRE(pCapture->poll() == binding::of(gamepad::button::b));
    }

    SECTION("a stick resting off centre") {
        const auto player = f.attach();

        f.pNull->gamepad_at(player)->set_axis(
            static_cast<gamepad::index_type>(gamepad::axis::right_y), 0.3f);
        f.pNull->update();

        auto pCapture = f.begin();

        f.idle(*pCapture);
        f.idle(*pCapture);

        REQUIRE_FALSE(pCapture->captured());

        SECTION("until it is actually pushed") {
            f.pNull->gamepad_at(player)->set_axis(
                static_cast<gamepad::index_type>(gamepad::axis::right_y), 0.95f);
            f.pNull->update();

            REQUIRE(pCapture->poll() == binding::of(gamepad::axis::right_y, 1.f));
        }
    }

    SECTION("and a stick already pushed hard when the capture began") {
        const auto player = f.attach();

        f.pNull->gamepad_at(player)->set_axis(
            static_cast<gamepad::index_type>(gamepad::axis::left_y), 0.95f);
        f.pNull->update();

        auto pCapture = f.begin();

        f.idle(*pCapture);

        REQUIRE_FALSE(pCapture->captured());
    }
}

TEST_CASE("the mouse is not an answer unless asked for", "[capture]") {
    fixture f;

    auto pCapture = f.begin();

    f.pNull->press_mouse_button(mouse::button::left);

    REQUIRE_FALSE(pCapture->poll());

    SECTION("switched on, it is") {
        pCapture->set_accepts_mouse(true);

        f.pNull->release_mouse_button(mouse::button::left);
        f.idle(*pCapture);

        f.pNull->press_mouse_button(mouse::button::right);

        REQUIRE(pCapture->poll() == binding::of(mouse::button::right));
    }
}

TEST_CASE("hardware with no standard mapping captures as itself", "[capture][mapping]") {
    fixture f;

    f.pNull->gamepad_at(0)->attach_unmapped("Flight Stick", "guid-stick", 12, 4);

    auto pCapture = f.begin();

    SECTION("a button") {
        f.pNull->gamepad_at(0)->press(7);
        f.pNull->update();

        const auto pressed = pCapture->poll();

        REQUIRE(pressed);
        REQUIRE(*pressed == binding::of_device_button("guid-stick", 7));
        REQUIRE_FALSE(*pressed == binding::of(gamepad::button::start));
    }

    SECTION("an axis, with its direction") {
        f.pNull->gamepad_at(0)->set_axis(3, -0.8f);
        f.pNull->update();

        REQUIRE(pCapture->poll() == binding::of_device_axis("guid-stick", 3, -1.f));
    }

    SECTION("and a control past the standard layout on a mapped device does too") {
        f.pNull->gamepad_at(0)->attach("Fancy Pad", "guid-fancy", 21, 6);

        auto pLater = f.begin();

        f.idle(*pLater);

        f.pNull->gamepad_at(0)->press(20);
        f.pNull->update();

        REQUIRE(pLater->poll() == binding::of_device_button("guid-fancy", 20));
    }
}

TEST_CASE("a capture watches its own player", "[capture]") {
    fixture f;

    const auto one = f.attach("pad A", "guid-a");
    const auto two = f.pNull->attach_gamepad("pad B", "guid-b").value();

    auto pSecond = f.begin(1);

    f.pNull->gamepad_at(one)->press(static_cast<gamepad::index_type>(gamepad::button::a));
    f.pNull->update();

    REQUIRE_FALSE(pSecond->poll());

    f.pNull->gamepad_at(two)->press(static_cast<gamepad::index_type>(gamepad::button::x));
    f.pNull->update();

    REQUIRE(pSecond->poll() == binding::of(gamepad::button::x));
}

TEST_CASE("a device swapped mid-capture does not answer for the old one", "[capture][disconnect]") {
    fixture f;

    f.pNull->gamepad_at(0)->attach_unmapped("Flight Stick", "guid-stick", 12, 4);

    auto pCapture = f.begin();

    f.pNull->detach_gamepad(0);
    f.pNull->gamepad_at(0)->attach_unmapped("Dance Mat", "guid-mat", 8, 0);
    f.pNull->gamepad_at(0)->press(2);
    f.pNull->update();

    REQUIRE_FALSE(pCapture->captured());

    f.pNull->gamepad_at(0)->release(2);
    f.idle(*pCapture);
    f.pNull->gamepad_at(0)->press(2);
    f.pNull->update();

    REQUIRE(pCapture->poll() == binding::of_device_button("guid-mat", 2));
}

TEST_CASE("what a capture produces is what controls binds", "[capture][controls]") {
    fixture f;

    const auto player = f.attach();

    auto pControls = controls::make(f.pContext, 0);
    auto pCapture = f.begin();

    pControls->bind("jump", keyboard::key::space);

    f.pNull->gamepad_at(player)->press(static_cast<gamepad::index_type>(gamepad::button::b));
    f.pNull->update();

    const auto pressed = pCapture->poll();

    REQUIRE(pressed);

    pControls->unbind("jump");
    pControls->bind("jump", *pressed);

    REQUIRE(pControls->down("jump"));
    REQUIRE(pControls->sources("jump") == std::vector<binding>{binding::of(gamepad::button::b)});

    SECTION("and a captured axis drives the action it was captured for") {
        auto pAxisCapture = f.begin();

        f.pNull->gamepad_at(player)->set_axis(
            static_cast<gamepad::index_type>(gamepad::axis::left_x), -0.85f);
        f.pNull->update();

        const auto axis = pAxisCapture->poll();

        REQUIRE(axis);

        pControls->bind("move_left", *axis);

        REQUIRE(pControls->get("move_left") == Approx(0.85f));
        REQUIRE(pControls->down("move_left"));
    }
}

TEST_CASE("a binding says what it is in words", "[capture][binding]") {
    REQUIRE(render(binding::of(keyboard::key::space)) == "key space");
    REQUIRE(render(binding::of(gamepad::button::a)) == "gamepad a");
    REQUIRE(render(binding::of(gamepad::axis::left_x, -1.f)) == "gamepad -left_x");
    REQUIRE(render(binding::of_device_button("guid-stick", 7)) == "button 7 on guid-stick");
}

