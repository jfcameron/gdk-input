// © Joseph Cameron - MIT License

#include <jfc/catch.hpp>

#include "conformance.h"

#include <gdk/input/context.h>

#include <algorithm>

using namespace gdk::input;

TEST_CASE("conformance: a key press is one frame of just_pressed", "[conformance][keyboard]")
{
    INFO("implementation: " << conformance::implementation());

    const auto pDriver = conformance::make_driver();
    const auto pContext = pDriver->context();

    pDriver->frame();

    REQUIRE(pContext->keys_down().empty());
    REQUIRE_FALSE(pContext->any_key_down());

    pDriver->press(keyboard::key::space);
    pDriver->frame();

    REQUIRE(pContext->key_down(keyboard::key::space));
    REQUIRE(pContext->key_just_pressed(keyboard::key::space));
    REQUIRE_FALSE(pContext->key_just_released(keyboard::key::space));

    pDriver->frame();

    REQUIRE(pContext->key_down(keyboard::key::space));
    REQUIRE_FALSE(pContext->key_just_pressed(keyboard::key::space));

    pDriver->release(keyboard::key::space);
    pDriver->frame();

    REQUIRE_FALSE(pContext->key_down(keyboard::key::space));
    REQUIRE(pContext->key_just_released(keyboard::key::space));

    pDriver->frame();

    REQUIRE_FALSE(pContext->key_just_released(keyboard::key::space));
}

TEST_CASE("conformance: a key tapped between two frames is seen", "[conformance][keyboard]")
{
    INFO("implementation: " << conformance::implementation());

    const auto pDriver = conformance::make_driver();
    const auto pContext = pDriver->context();

    pDriver->frame();

    pDriver->press(keyboard::key::e);
    pDriver->release(keyboard::key::e);
    pDriver->frame();

    REQUIRE(pContext->key_just_pressed(keyboard::key::e));
}

TEST_CASE("conformance: every key held is listed", "[conformance][keyboard]")
{
    INFO("implementation: " << conformance::implementation());

    const auto pDriver = conformance::make_driver();
    const auto pContext = pDriver->context();

    pDriver->frame();

    pDriver->press(keyboard::key::a);
    pDriver->press(keyboard::key::numslash);
    pDriver->frame();

    const auto held = pContext->keys_down();

    REQUIRE(held.size() == 2);
    REQUIRE(std::count(held.begin(), held.end(), keyboard::key::a) == 1);
    REQUIRE(std::count(held.begin(), held.end(), keyboard::key::numslash) == 1);
    REQUIRE_FALSE(pContext->key_down(keyboard::key::forwardslash));
}

TEST_CASE("conformance: the modifiers held are reported", "[conformance][keyboard]")
{
    INFO("implementation: " << conformance::implementation());

    const auto pDriver = conformance::make_driver();
    const auto pContext = pDriver->context();

    pDriver->frame();

    pDriver->press(keyboard::key::leftshift);
    pDriver->press(keyboard::key::rightcontrol);
    pDriver->frame();

    const auto held = pContext->modifiers_held();

    REQUIRE(held.shift);
    REQUIRE(held.control);
    REQUIRE_FALSE(held.alt);
}
