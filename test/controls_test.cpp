// © Joseph Cameron - All Rights Reserved

#include <jfc/catch.hpp>

#include <gdk/input/controls.h>
#include <gdk/input/null_context.h>

#include <string>
#include <vector>

using namespace gdk::input;

namespace {
    struct fixture final {
        std::shared_ptr<null_context> pNull{null_context::make()};
        context_ptr_type pContext{pNull};
        controls_ptr_type pControls{controls::make(pContext, 0)};

        std::size_t attach(const std::string &aName = "pad", const std::string &aGuid = "guid-pad") {
            const auto slot = pNull->attach_gamepad(aName, aGuid);

            return slot.value_or(0);
        }
    };
}

TEST_CASE("an action is the strongest of whatever is bound to it", "[controls]")
{
    fixture f;

    const auto player = f.attach();

    f.pControls->bind("jump", keyboard::key::space);
    f.pControls->bind("jump", gamepad::button::a);

    SECTION("nothing bound down means nothing")
    {
        REQUIRE(f.pControls->get("jump") == 0);
        REQUIRE_FALSE(f.pControls->down("jump"));
    }

    SECTION("either source drives it")
    {
        f.pNull->press_key(keyboard::key::space);

        REQUIRE(f.pControls->get("jump") == 1);
        REQUIRE(f.pControls->down("jump"));

        f.pNull->release_key(keyboard::key::space);
        f.pNull->update();

        REQUIRE_FALSE(f.pControls->down("jump"));

        f.pNull->gamepad_at(player)->press(static_cast<gamepad::index_type>(gamepad::button::a));
        f.pNull->update();

        REQUIRE(f.pControls->down("jump"));
    }

    SECTION("an action nobody bound is zero rather than an error")
    {
        REQUIRE(f.pControls->get("never_bound") == 0);
        REQUIRE_FALSE(f.pControls->down("never_bound"));
        REQUIRE_FALSE(f.pControls->just_pressed("never_bound"));
    }

    SECTION("actions come back in a stable order")
    {
        f.pControls->bind("crouch", keyboard::key::c);
        f.pControls->bind("attack", keyboard::key::x);

        REQUIRE(f.pControls->actions() == std::vector<std::string>{"attack", "crouch", "jump"});
    }
}

TEST_CASE("edges come through from whichever source produced them", "[controls]")
{
    fixture f;

    const auto player = f.attach();

    f.pControls->bind("fire", keyboard::key::space);
    f.pControls->bind("fire", gamepad::button::right_bumper);

    SECTION("a key press is one frame of just_pressed")
    {
        f.pNull->press_key(keyboard::key::space);

        REQUIRE(f.pControls->just_pressed("fire"));

        f.pNull->update();

        REQUIRE_FALSE(f.pControls->just_pressed("fire"));
        REQUIRE(f.pControls->down("fire"));
    }

    SECTION("and so is a gamepad press")
    {
        f.pNull->gamepad_at(player)->press(
            static_cast<gamepad::index_type>(gamepad::button::right_bumper));
        f.pNull->update();

        REQUIRE(f.pControls->just_pressed("fire"));

        f.pNull->update();

        REQUIRE_FALSE(f.pControls->just_pressed("fire"));
    }

    SECTION("releasing reports just_released once")
    {
        f.pNull->press_key(keyboard::key::space);
        f.pNull->update();
        f.pNull->release_key(keyboard::key::space);

        REQUIRE(f.pControls->just_released("fire"));

        f.pNull->update();

        REQUIRE_FALSE(f.pControls->just_released("fire"));
    }
}

TEST_CASE("an axis binding carries its scale", "[controls]")
{
    fixture f;

    const auto player = f.attach();

    f.pControls->bind("move_right", gamepad::axis::left_x, 1.0f);
    f.pControls->bind("move_left", gamepad::axis::left_x, -1.0f);

    f.pNull->gamepad_at(player)->set_axis(
        static_cast<gamepad::index_type>(gamepad::axis::left_x), -0.75f);
    f.pNull->update();

    SECTION("the direction the stick is pushed reads positive on its own action")
    {
        REQUIRE(f.pControls->get("move_left") == Approx(0.75f));
        REQUIRE(f.pControls->down("move_left"));
    }

    SECTION("and the opposite action reads the other sign")
    {
        REQUIRE(f.pControls->get("move_right") == Approx(-0.75f));
    }

    SECTION("a key bound alongside an axis wins only when it is stronger") {
        f.pControls->bind("move_left", keyboard::key::a);

        REQUIRE(f.pControls->get("move_left") == Approx(0.75f));

        f.pNull->press_key(keyboard::key::a);

        REQUIRE(f.pControls->get("move_left") == Approx(1.0f));
    }

    SECTION("and the strongest wins wherever it sits in the binding order") {
        f.pControls->bind("aim", keyboard::key::a);
        f.pControls->bind("aim", gamepad::axis::right_x, 1.0f);

        f.pNull->press_key(keyboard::key::a);
        f.pNull->gamepad_at(player)->set_axis(
            static_cast<gamepad::index_type>(gamepad::axis::right_x), 0.3f);
        f.pNull->update();

        REQUIRE(f.pControls->get("aim") == Approx(1.0f));
    }
}

TEST_CASE("a standard binding works on any recognised pad", "[controls][mapping]") {
    fixture f;

    f.pControls->bind("confirm", gamepad::button::a);

    SECTION("one pad")
    {
        const auto player = f.attach("Xbox Controller", "guid-xbox");

        f.pNull->gamepad_at(player)->press(static_cast<gamepad::index_type>(gamepad::button::a));
        f.pNull->update();

        REQUIRE(f.pControls->down("confirm"));
    }

    SECTION("a different pad, same binding, no rebinding")
    {
        const auto player = f.attach("PlayStation Controller", "guid-ps");

        f.pNull->gamepad_at(player)->press(static_cast<gamepad::index_type>(gamepad::button::a));
        f.pNull->update();

        REQUIRE(f.pControls->down("confirm"));
    }

    SECTION("but not on hardware with no standard mapping") {
        f.pNull->gamepad_at(0)->attach_unmapped("Flight Stick", "guid-stick", 12, 4);

        f.pNull->gamepad_at(0)->press(0);
        f.pNull->update();

        REQUIRE_FALSE(f.pControls->down("confirm"));
    }
}

TEST_CASE("unmapped hardware is bound by device", "[controls][mapping]")
{
    fixture f;

    f.pNull->gamepad_at(0)->attach_unmapped("Flight Stick", "guid-stick", 12, 4);

    f.pControls->bind_device_button("fire", "guid-stick", 0);
    f.pControls->bind_device_axis("throttle", "guid-stick", 2, 1.0f);

    SECTION("its own binding fires")
    {
        f.pNull->gamepad_at(0)->press(0);
        f.pNull->update();

        REQUIRE(f.pControls->down("fire"));
        REQUIRE(f.pControls->just_pressed("fire"));
    }

    SECTION("and its axes read")
    {
        f.pNull->gamepad_at(0)->set_axis(2, 0.6f);
        f.pNull->update();

        REQUIRE(f.pControls->get("throttle") == Approx(0.6f));
    }

    SECTION("a binding for one device does not fire for another in the same slot") {
        f.pNull->detach_gamepad(0);
        f.pNull->gamepad_at(0)->attach_unmapped("Dance Mat", "guid-mat", 8, 0);

        f.pNull->gamepad_at(0)->press(0);
        f.pNull->update();

        REQUIRE_FALSE(f.pControls->down("fire"));
    }
}

TEST_CASE("controls follow a player, not a device", "[controls][disconnect]") {
    fixture f;

    const auto player = f.attach("pad A", "guid-a");

    REQUIRE(player == 0);

    f.pControls->bind("jump", gamepad::button::a);

    f.pNull->gamepad_at(player)->press(static_cast<gamepad::index_type>(gamepad::button::a));
    f.pNull->update();

    REQUIRE(f.pControls->down("jump"));

    SECTION("unplugged, the action goes quiet")
    {
        f.pNull->detach_gamepad(player);

        REQUIRE_FALSE(f.pControls->down("jump"));
    }

    SECTION("plugged back in, it works again with no rebinding")
    {
        f.pNull->detach_gamepad(player);

        const auto again = f.pNull->attach_gamepad("pad A", "guid-a");

        REQUIRE(*again == player);

        f.pNull->gamepad_at(player)->press(static_cast<gamepad::index_type>(gamepad::button::a));
        f.pNull->update();

        REQUIRE(f.pControls->down("jump"));
    }
}

TEST_CASE("player two has its own controls", "[controls]")
{
    fixture f;

    auto pSecond = controls::make(f.pContext, 1);

    const auto one = f.attach("pad A", "guid-a");
    const auto two = f.pNull->attach_gamepad("pad B", "guid-b").value();

    f.pControls->bind("jump", gamepad::button::a);
    pSecond->bind("jump", gamepad::button::a);

    f.pNull->gamepad_at(two)->press(static_cast<gamepad::index_type>(gamepad::button::a));
    f.pNull->update();

    REQUIRE_FALSE(f.pControls->down("jump"));
    REQUIRE(pSecond->down("jump"));
    REQUIRE(one == 0);
}

TEST_CASE("unbinding removes every source", "[controls]")
{
    fixture f;

    f.pControls->bind("jump", keyboard::key::space);
    f.pControls->bind("jump", keyboard::key::w);

    REQUIRE(f.pControls->source_count("jump") == 2);

    f.pNull->press_key(keyboard::key::space);

    REQUIRE(f.pControls->down("jump"));

    f.pControls->unbind("jump");

    REQUIRE(f.pControls->source_count("jump") == 0);
    REQUIRE_FALSE(f.pControls->down("jump"));
    REQUIRE(f.pControls->actions().empty());
}

TEST_CASE("an axis edge is magnitude, and a binding reads its direction off the value", "[controls]") {
    fixture f;

    const auto player = f.attach();

    f.pControls->bind("move_right", gamepad::axis::left_x, 1.0f);
    f.pControls->bind("move_left", gamepad::axis::left_x, -1.0f);

    const auto push = [&](const float aValue) {
        f.pNull->gamepad_at(player)->set_axis(
            static_cast<gamepad::index_type>(gamepad::axis::left_x), aValue);
        f.pNull->update();
    };

    SECTION("pushed left, only the left action begins")
    {
        push(-0.9f);

        REQUIRE(f.pControls->just_pressed("move_left"));
        REQUIRE_FALSE(f.pControls->just_pressed("move_right"));

        REQUIRE(f.pControls->down("move_left"));
        REQUIRE_FALSE(f.pControls->down("move_right"));
    }

    SECTION("pushed right, only the right one")
    {
        push(0.9f);

        REQUIRE(f.pControls->just_pressed("move_right"));
        REQUIRE_FALSE(f.pControls->just_pressed("move_left"));
    }

    SECTION("and released, only the one that was down ends")
    {
        push(-0.9f);
        push(0.0f);

        REQUIRE(f.pControls->just_released("move_left"));
        REQUIRE_FALSE(f.pControls->just_released("move_right"));
    }

    SECTION("released from the other side, the other one")
    {
        push(0.9f);
        push(0.0f);

        REQUIRE(f.pControls->just_released("move_right"));
        REQUIRE_FALSE(f.pControls->just_released("move_left"));
    }
}

TEST_CASE("down is signed, because an action is a scalar", "[controls]") {
    fixture f;

    const auto player = f.attach();

    f.pControls->bind("move_left", gamepad::axis::left_x, -1.0f);

    f.pNull->gamepad_at(player)->set_axis(
        static_cast<gamepad::index_type>(gamepad::axis::left_x), 0.9f);
    f.pNull->update();

    REQUIRE(f.pControls->get("move_left") == Approx(-0.9f));
    REQUIRE_FALSE(f.pControls->down("move_left"));
}

TEST_CASE("a binding can be removed on its own", "[controls]")
{
    fixture f;

    f.pControls->bind("jump", keyboard::key::space);
    f.pControls->bind("jump", keyboard::key::w);

    REQUIRE_FALSE(f.pControls->unbind("jump", binding::of(keyboard::key::c)));
    REQUIRE(f.pControls->source_count("jump") == 2);

    REQUIRE(f.pControls->unbind("jump", binding::of(keyboard::key::w)));
    REQUIRE(f.pControls->sources("jump") == std::vector<binding>{binding::of(keyboard::key::space)});

    REQUIRE(f.pControls->unbind("jump", binding::of(keyboard::key::space)));
    REQUIRE(f.pControls->actions().empty());
}

TEST_CASE("a pointer axis is a bindable source", "[controls][pointer_axis]")
{
    fixture f;

    SECTION("cursor motion drives an action, scaled")
    {
        f.pControls->bind("look_x", mouse::axis::x, 0.1f);

        f.pNull->set_mouse_delta({5, 0});
        REQUIRE(f.pControls->get("look_x") == Approx(0.5f));

        f.pNull->set_mouse_delta({0, 9});
        REQUIRE(f.pControls->get("look_x") == 0);
    }

    SECTION("a still pointer contributes nothing")
    {
        f.pControls->bind("look_y", mouse::axis::y, 0.1f);

        f.pNull->set_mouse_delta({0, 0});
        REQUIRE(f.pControls->get("look_y") == 0);
        REQUIRE_FALSE(f.pControls->down("look_y"));
    }

    SECTION("a negative scale binds the opposite direction")
    {
        f.pControls->bind("look_left", mouse::axis::x, -0.1f);

        f.pNull->set_mouse_delta({5, 0});
        REQUIRE(f.pControls->get("look_left") == Approx(-0.5f));
    }

    SECTION("**the scaled delta is clamped into the range a stick reports**")
    {
        f.pControls->bind("look_x", mouse::axis::x, 0.1f);

        f.pNull->set_mouse_delta({400, 0});
        REQUIRE(f.pControls->get("look_x") == Approx(1.0f));

        f.pNull->set_mouse_delta({-400, 0});
        REQUIRE(f.pControls->get("look_x") == Approx(-1.0f));
    }

    SECTION("the wheel is a pointer axis like any other")
    {
        f.pControls->bind("zoom", mouse::axis::scroll_y, 0.5f);

        f.pNull->set_scroll_delta({0, 1});
        REQUIRE(f.pControls->get("zoom") == Approx(0.5f));

        f.pNull->set_scroll_delta({1, 0});
        REQUIRE(f.pControls->get("zoom") == 0);
    }

    SECTION("the other wheel axis is separately bindable")
    {
        f.pControls->bind("pan", mouse::axis::scroll_x, 1.0f);

        f.pNull->set_scroll_delta({3, 0});
        REQUIRE(f.pControls->get("pan") == Approx(3.0f));

        f.pNull->set_scroll_delta({0, 3});
        REQUIRE(f.pControls->get("pan") == 0);
    }

    SECTION("**the wheel is not clamped, so a fast spin still counts every notch**")
    {
        f.pControls->bind("zoom", mouse::axis::scroll_y, 0.75f);

        f.pNull->set_scroll_delta({0, 1});
        REQUIRE(f.pControls->get("zoom") == Approx(0.75f));

        f.pNull->set_scroll_delta({0, 2});
        REQUIRE(f.pControls->get("zoom") == Approx(1.5f));

        f.pNull->set_scroll_delta({0, -4});
        REQUIRE(f.pControls->get("zoom") == Approx(-3.0f));
    }

    SECTION("cursor motion is still clamped, and the wheel does not change that")
    {
        f.pControls->bind("look_x", mouse::axis::x, 4.0f);
        f.pControls->bind("zoom", mouse::axis::scroll_y, 4.0f);

        f.pNull->set_mouse_delta({0.5, 0});
        f.pNull->set_scroll_delta({0, 0.5});

        REQUIRE(f.pControls->get("look_x") == Approx(1.0f));
        REQUIRE(f.pControls->get("zoom") == Approx(2.0f));
    }

    SECTION("a mouse and a stick can drive one action, and the one being used wins")
    {
        const auto player = f.attach();

        f.pControls->bind("look_x", mouse::axis::x, 0.1f);
        f.pControls->bind("look_x", gamepad::axis::left_x, 1.0f);

        f.pNull->set_mouse_delta({2, 0});
        REQUIRE(f.pControls->get("look_x") == Approx(0.2f));

        f.pNull->gamepad_at(player)->set_axis(
            static_cast<gamepad::index_type>(gamepad::axis::left_x), 0.8f);
        f.pNull->update();
        REQUIRE(f.pControls->get("look_x") == Approx(0.8f));

        f.pNull->set_mouse_delta({9000, 0});
        REQUIRE(f.pControls->get("look_x") == Approx(1.0f));
    }

    SECTION("a pointer axis reports no edges")
    {
        f.pControls->bind("look_x", mouse::axis::x, 0.1f);

        f.pNull->set_mouse_delta({5, 0});
        REQUIRE(f.pControls->down("look_x"));
        REQUIRE_FALSE(f.pControls->just_pressed("look_x"));
        REQUIRE_FALSE(f.pControls->just_released("look_x"));
    }

    SECTION("it round trips as a binding value")
    {
        f.pControls->bind("look_x", mouse::axis::x, 0.1f);

        const auto sources = f.pControls->sources("look_x");

        REQUIRE(sources.size() == 1);
        REQUIRE(sources.at(0).which == binding::kind::pointer_axis);
        REQUIRE(sources.at(0).pointerAxis == mouse::axis::x);
        REQUIRE(sources.at(0).is_axis());
        REQUIRE_FALSE(sources.at(0).is_gamepad());
        REQUIRE(sources.at(0) == binding::of(mouse::axis::x, 0.1f));
        REQUIRE_FALSE(sources.at(0) == binding::of(mouse::axis::y, 0.1f));
        REQUIRE_FALSE(sources.at(0) == binding::of(mouse::axis::x, 0.2f));

        REQUIRE(f.pControls->unbind("look_x", binding::of(mouse::axis::x, 0.1f)));
        REQUIRE(f.pControls->source_count("look_x") == 0);
    }
}
