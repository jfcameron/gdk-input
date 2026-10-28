// © Joseph Cameron - MIT License

#include <jfc/catch.hpp>

#include "conformance.h"

#include <gdk/input/context.h>

#include <cmath>

using namespace gdk::input;

TEST_CASE("conformance: a mouse button press is one frame of just_pressed", "[conformance][mouse]")
{
    INFO("implementation: " << conformance::implementation());

    const auto pDriver = conformance::make_driver();
    const auto pContext = pDriver->context();

    pDriver->frame();

    REQUIRE_FALSE(pContext->any_mouse_button_down());

    pDriver->press(mouse::button::left);
    pDriver->frame();

    REQUIRE(pContext->mouse_button_down(mouse::button::left));
    REQUIRE(pContext->mouse_button_just_pressed(mouse::button::left));
    REQUIRE(pContext->any_mouse_button_down() == mouse::button::left);

    pDriver->frame();

    REQUIRE(pContext->mouse_button_down(mouse::button::left));
    REQUIRE_FALSE(pContext->mouse_button_just_pressed(mouse::button::left));

    pDriver->release(mouse::button::left);
    pDriver->frame();

    REQUIRE_FALSE(pContext->mouse_button_down(mouse::button::left));
    REQUIRE(pContext->mouse_button_just_released(mouse::button::left));
}

TEST_CASE("conformance: cursor motion is one frame's, in pixels, y up", "[conformance][mouse]")
{
    INFO("implementation: " << conformance::implementation());

    const auto pDriver = conformance::make_driver();
    const auto pContext = pDriver->context();

    pDriver->frame();

    REQUIRE(pContext->mouse_delta().x == Approx(0));
    REQUIRE(pContext->mouse_delta().y == Approx(0));

    pDriver->move_cursor(10, 4);
    pDriver->frame();

    REQUIRE(pContext->mouse_delta().x == Approx(10));
    REQUIRE(pContext->mouse_delta().y == Approx(4));
    REQUIRE(pContext->any_mouse_axis_down(1) == mouse::axis::x);

    SECTION("and moving further moves the cursor that way") {
        const auto before = pContext->mouse_cursor_position();

        pDriver->move_cursor(20, 20);
        pDriver->frame();

        const auto after = pContext->mouse_cursor_position();

        REQUIRE(std::isfinite(after.x));
        REQUIRE(std::isfinite(after.y));
        REQUIRE(after.x > before.x);
        REQUIRE(after.y > before.y);
    }

    SECTION("it lasts one frame") {
        pDriver->frame();

        REQUIRE(pContext->mouse_delta().x == Approx(0));
        REQUIRE(pContext->mouse_delta().y == Approx(0));
        REQUIRE_FALSE(pContext->any_mouse_axis_down(0.5f));
    }

    SECTION("moves within one frame add up") {
        pDriver->move_cursor(3, 0);
        pDriver->move_cursor(4, -2);
        pDriver->frame();

        REQUIRE(pContext->mouse_delta().x == Approx(7));
        REQUIRE(pContext->mouse_delta().y == Approx(-2));
    }
}

TEST_CASE("conformance: the wheel is one frame's, in notches", "[conformance][mouse]")
{
    INFO("implementation: " << conformance::implementation());

    const auto pDriver = conformance::make_driver();
    const auto pContext = pDriver->context();

    pDriver->frame();

    pDriver->scroll(0, 1);
    pDriver->scroll(0, 2);
    pDriver->frame();

    REQUIRE(pContext->mouse_scroll_delta().y == Approx(3));
    REQUIRE(pContext->any_mouse_axis_down(0.5f) == mouse::axis::scroll_y);

    pDriver->frame();

    REQUIRE(pContext->mouse_scroll_delta().y == Approx(0));
    REQUIRE(pContext->mouse_scroll_delta().x == Approx(0));
}

TEST_CASE("conformance: the cursor mode reads back", "[conformance][mouse]")
{
    INFO("implementation: " << conformance::implementation());

    const auto pDriver = conformance::make_driver();
    const auto pContext = pDriver->context();

    for (const auto mode : {mouse::cursor_mode::hidden, mouse::cursor_mode::locked, mouse::cursor_mode::normal}) {
        pContext->set_mouse_cursor_mode(mode);
        pDriver->frame();

        REQUIRE(pContext->mouse_cursor_mode() == mode);
    }
}
