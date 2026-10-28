// © Joseph Cameron - MIT License

#include <jfc/catch.hpp>

#include "conformance.h"

#include <gdk/input/context.h>

using namespace gdk::input;

TEST_CASE("conformance: every player has a gamepad, connected or not", "[conformance][gamepad]")
{
    INFO("implementation: " << conformance::implementation());

    const auto pDriver = conformance::make_driver();
    const auto pContext = pDriver->context();

    pDriver->frame();

    for (std::size_t player = 0; player < 16; ++player) {
        INFO("player " << player);

        const auto pGamepad = pContext->get_gamepad(player);

        REQUIRE(pGamepad);
        REQUIRE_FALSE(pGamepad->connected());
        REQUIRE_FALSE(pGamepad->any_button_down());
        REQUIRE_FALSE(pGamepad->any_axis_down());
        REQUIRE(pGamepad->axis_value(gamepad::axis::left_x) == 0);
    }

    for (const auto &pGamepad : pContext->gamepads()) REQUIRE(pGamepad);
}

TEST_CASE("conformance: a gamepad comes and goes", "[conformance][gamepad]")
{
    INFO("implementation: " << conformance::implementation());

    const auto pDriver = conformance::make_driver();
    const auto pContext = pDriver->context();

    pDriver->frame();

    if (!pDriver->attach_gamepad()) return;

    pDriver->frame();

    const auto pGamepad = pContext->get_gamepad(0);

    REQUIRE(pGamepad->connected());
    REQUIRE(pGamepad->just_connected());
    REQUIRE(pGamepad->has_standard_mapping());
    REQUIRE_FALSE(pGamepad->guid().empty());

    pDriver->frame();

    REQUIRE(pGamepad->connected());
    REQUIRE_FALSE(pGamepad->just_connected());

    pDriver->detach_gamepad();
    pDriver->frame();

    REQUIRE_FALSE(pContext->get_gamepad(0)->connected());
    REQUIRE(pContext->get_gamepad(0)->just_disconnected());

    pDriver->frame();

    REQUIRE_FALSE(pContext->get_gamepad(0)->just_disconnected());
}

TEST_CASE("conformance: a gamepad's buttons and axes", "[conformance][gamepad]")
{
    INFO("implementation: " << conformance::implementation());

    const auto pDriver = conformance::make_driver();
    const auto pContext = pDriver->context();

    pDriver->frame();

    if (!pDriver->attach_gamepad()) return;

    pDriver->frame();

    const auto pGamepad = pContext->get_gamepad(0);

    SECTION("a press is one frame of just_pressed") {
        pDriver->press(gamepad::button::a);
        pDriver->frame();

        REQUIRE(pGamepad->button_down(gamepad::button::a));
        REQUIRE(pGamepad->button_just_pressed(gamepad::button::a));
        REQUIRE(pGamepad->any_button_down() == static_cast<std::size_t>(gamepad::button::a));

        pDriver->frame();

        REQUIRE(pGamepad->button_down(gamepad::button::a));
        REQUIRE_FALSE(pGamepad->button_just_pressed(gamepad::button::a));

        pDriver->release(gamepad::button::a);
        pDriver->frame();

        REQUIRE_FALSE(pGamepad->button_down(gamepad::button::a));
        REQUIRE(pGamepad->button_just_released(gamepad::button::a));
        REQUIRE_FALSE(pGamepad->any_button_down());
    }

    SECTION("the triggers rest at zero") {
        REQUIRE(pGamepad->axis_value(gamepad::axis::left_trigger) == Approx(0));
        REQUIRE(pGamepad->axis_value(gamepad::axis::right_trigger) == Approx(0));

        pDriver->set_axis(gamepad::axis::right_trigger, 1);
        pDriver->frame();

        REQUIRE(pGamepad->axis_value(gamepad::axis::right_trigger) == Approx(1));
    }

    SECTION("a stick reads as it is pushed, and its edges come once") {
        pDriver->set_axis(gamepad::axis::left_x, -0.75f);
        pDriver->frame();

        REQUIRE(pGamepad->axis_value(gamepad::axis::left_x) == Approx(-0.75f));
        REQUIRE(pGamepad->axis_just_exceeded_threshold(gamepad::axis::left_x));

        pDriver->frame();

        REQUIRE_FALSE(pGamepad->axis_just_exceeded_threshold(gamepad::axis::left_x));

        pDriver->set_axis(gamepad::axis::left_x, 0);
        pDriver->frame();

        REQUIRE(pGamepad->axis_just_dropped_below_threshold(gamepad::axis::left_x));
        REQUIRE(pGamepad->previous_axis_value(gamepad::axis::left_x) == Approx(-0.75f));
    }
}
