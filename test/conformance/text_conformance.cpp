// © Joseph Cameron - MIT License

#include <jfc/catch.hpp>

#include "conformance.h"

#include <gdk/input/context.h>

#include <string>
#include <variant>

using namespace gdk::input;

TEST_CASE("conformance: what is typed is one frame's text", "[conformance][text]")
{
    INFO("implementation: " << conformance::implementation());

    const auto pDriver = conformance::make_driver();
    const auto pContext = pDriver->context();

    pContext->set_text_input_focus(true);
    pDriver->frame();

    pDriver->type("h\xC3\xA9");
    pDriver->frame();

    const auto &events = pContext->text_events();

    REQUIRE(events.size() == 1);
    REQUIRE(std::get<std::string>(events.front()) == "h\xC3\xA9");

    pDriver->frame();

    REQUIRE(pContext->text_events().empty());
}

TEST_CASE("conformance: a text field with the keyboard withholds it from the game", "[conformance][text]")
{
    INFO("implementation: " << conformance::implementation());

    const auto pDriver = conformance::make_driver();
    const auto pContext = pDriver->context();

    pContext->set_text_input_focus(true);
    pDriver->frame();

    REQUIRE(pContext->text_input_focus());

    pDriver->press(keyboard::key::w);
    pDriver->frame();

    REQUIRE_FALSE(pContext->key_down(keyboard::key::w));
    REQUIRE_FALSE(pContext->key_just_pressed(keyboard::key::w));

    pDriver->release(keyboard::key::w);
    pContext->set_text_input_focus(false);
    pDriver->frame();

    REQUIRE_FALSE(pContext->key_just_released(keyboard::key::w));

    pDriver->press(keyboard::key::w);
    pDriver->frame();

    REQUIRE(pContext->key_just_pressed(keyboard::key::w));
}
