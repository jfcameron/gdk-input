// © Joseph Cameron - MIT License

#include <jfc/catch.hpp>

#include "conformance.h"

#include <gdk/input/context.h>
#include <gdk/input/controls.h>

using namespace gdk::input;

TEST_CASE("conformance: an action's edges are the action's, not a source's", "[conformance][controls]")
{
    INFO("implementation: " << conformance::implementation());

    const auto pDriver = conformance::make_driver();
    const auto pControls = controls::make(pDriver->context());

    pControls->bind("forward", keyboard::key::w);
    pControls->bind("forward", keyboard::key::uparrow);

    pDriver->frame();

    pDriver->press(keyboard::key::w);
    pDriver->frame();

    REQUIRE(pControls->just_pressed("forward"));

    pDriver->press(keyboard::key::uparrow);
    pDriver->frame();

    REQUIRE(pControls->down("forward"));
    REQUIRE_FALSE(pControls->just_pressed("forward"));

    pDriver->release(keyboard::key::w);
    pDriver->frame();

    REQUIRE(pControls->down("forward"));
    REQUIRE_FALSE(pControls->just_released("forward"));

    pDriver->release(keyboard::key::uparrow);
    pDriver->frame();

    REQUIRE_FALSE(pControls->down("forward"));
    REQUIRE(pControls->just_released("forward"));
}

TEST_CASE("conformance: the mouse drives an action", "[conformance][controls]")
{
    INFO("implementation: " << conformance::implementation());

    const auto pDriver = conformance::make_driver();
    const auto pControls = controls::make(pDriver->context());

    pControls->bind("fire", mouse::button::left);
    pControls->bind("look_x", mouse::axis::x, 0.1f);
    pControls->bind("zoom", mouse::axis::scroll_y, 0.5f);

    pDriver->frame();

    pDriver->press(mouse::button::left);
    pDriver->move_cursor(5, 0);
    pDriver->scroll(0, 1);
    pDriver->frame();

    REQUIRE(pControls->just_pressed("fire"));
    REQUIRE(pControls->get("look_x") == Approx(0.5f));
    REQUIRE(pControls->get("zoom") == Approx(0.5f));

    pDriver->frame();

    REQUIRE(pControls->get("look_x") == 0);
    REQUIRE(pControls->get("zoom") == 0);
}

TEST_CASE("conformance: a gamepad drives an action", "[conformance][controls][gamepad]")
{
    INFO("implementation: " << conformance::implementation());

    const auto pDriver = conformance::make_driver();
    const auto pControls = controls::make(pDriver->context());

    pControls->bind("jump", gamepad::button::a);
    pControls->bind("move_left", gamepad::axis::left_x, -1);

    pDriver->frame();

    if (!pDriver->attach_gamepad()) return;

    pDriver->frame();

    pDriver->press(gamepad::button::a);
    pDriver->set_axis(gamepad::axis::left_x, -0.9f);
    pDriver->frame();

    REQUIRE(pControls->just_pressed("jump"));
    REQUIRE(pControls->just_pressed("move_left"));
    REQUIRE(pControls->get("move_left") == Approx(0.9f));

    pDriver->detach_gamepad();
    pDriver->frame();

    REQUIRE_FALSE(pControls->down("jump"));
    REQUIRE(pControls->get("move_left") == 0);
}
