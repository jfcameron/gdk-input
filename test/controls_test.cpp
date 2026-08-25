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
