// © Joseph Cameron - All Rights Reserved

#include <jfc/catch.hpp>

#include "test_include.h"

using namespace gdk::input;

TEST_CASE("context creation", "[context]") {
    auto pWindow = initGLFW();

    SECTION("a context can be made from a window") {
        REQUIRE(glfw_context::make(pWindow) != nullptr);
    }

    SECTION("a context outlives the caller's handle to the window") {
        context_ptr_type pContext;

        {
            auto pLocalWindow = pWindow;
            pContext = glfw_context::make(pLocalWindow);
        }

        REQUIRE(pContext != nullptr);
        REQUIRE_NOTHROW(pContext->key_down(keyboard::key::space));
    }
}

TEST_CASE("a context is usable before anything is polled", "[context]") {
    auto pWindow = initGLFW();
    auto pContext = glfw_context::make(pWindow);

    REQUIRE_FALSE(pContext->key_down(keyboard::key::space));
    REQUIRE_FALSE(pContext->mouse_button_down(mouse::button::left));
    REQUIRE_FALSE(pContext->any_key_down().has_value());
    REQUIRE_FALSE(pContext->any_mouse_button_down().has_value());
    REQUIRE(pContext->get_gamepad(0) != nullptr);
}

TEST_CASE("update is repeatable", "[context]") {
    input_fixture f;

    SECTION("many polls in a row are harmless") {
        for (int i = 0; i < 20; ++i) REQUIRE_NOTHROW(f.poll());
    }

    SECTION("cursor mode survives polling") {
        f.pContext->set_mouse_cursor_mode(mouse::cursor_mode::hidden);

        for (int i = 0; i < 5; ++i) f.poll();

        REQUIRE(f.pContext->mouse_cursor_mode() == mouse::cursor_mode::hidden);
    }
}

TEST_CASE("two windows carry independent contexts", "[context]") {
    input_fixture first;
    input_fixture second;

    REQUIRE(first.pWindow != second.pWindow);
    REQUIRE(first.pContext != second.pContext);

    first.pContext->set_mouse_cursor_mode(mouse::cursor_mode::locked);

    REQUIRE(first.pContext->mouse_cursor_mode() == mouse::cursor_mode::locked);
    REQUIRE(second.pContext->mouse_cursor_mode() == mouse::cursor_mode::normal);
}

TEST_CASE("the glfw backend accepts a mapping database", "[context][mapping]") {
    input_fixture f;

    REQUIRE(f.pContext->add_gamepad_mappings(
        "050000005e040000e002000003090000,8Bitdo SN30 Pro,a:b0,b:b1,platform:Linux,\n") == 1);

    REQUIRE(f.pContext->add_gamepad_mappings("# just a comment\n") == 0);
    REQUIRE(f.pContext->add_gamepad_mappings("") == 0);
}

TEST_CASE("the whole set of pressed keys and buttons is reachable", "[context]") {
    input_fixture f;

    SECTION("nothing pressed is an empty set, not a surprise") {
        REQUIRE(f.pContext->keys_down().empty());
        REQUIRE(f.pContext->mouse_buttons_down().empty());
        REQUIRE_FALSE(f.pContext->any_key_down().has_value());
        REQUIRE_FALSE(f.pContext->any_mouse_button_down().has_value());
    }

    SECTION("and the singular queries agree with the plural ones") {
        const auto keys = f.pContext->keys_down();
        const auto any = f.pContext->any_key_down();

        REQUIRE(keys.empty() == !any.has_value());

        if (any) REQUIRE(keys.front() == *any);
    }
}
