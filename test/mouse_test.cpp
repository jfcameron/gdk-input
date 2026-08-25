// © Joseph Cameron - All Rights Reserved

#include <jfc/catch.hpp>

#include "test_include.h"

#include <magic_enum/magic_enum.hpp>

#include <cmath>
#include <sstream>
#include <string>

using namespace gdk::input;

TEST_CASE("every mouse button is mapped", "[mouse]") {
    input_fixture f;

    for (const auto button : magic_enum::enum_values<mouse::button>()) {
        INFO("button: " << std::string(magic_enum::enum_name(button)));

        REQUIRE_NOTHROW(f.pContext->mouse_button_down(button));
        REQUIRE_NOTHROW(f.pContext->mouse_button_just_pressed(button));
        REQUIRE_NOTHROW(f.pContext->mouse_button_just_released(button));
    }

    REQUIRE(magic_enum::enum_values<mouse::button>().size() == 8);
}

TEST_CASE("no mouse button is down without input", "[mouse]") {
    input_fixture f;

    for (const auto button : magic_enum::enum_values<mouse::button>()) {
        INFO("button: " << std::string(magic_enum::enum_name(button)));

        REQUIRE_FALSE(f.pContext->mouse_button_down(button));
        REQUIRE_FALSE(f.pContext->mouse_button_just_pressed(button));
        REQUIRE_FALSE(f.pContext->mouse_button_just_released(button));
    }

    REQUIRE_FALSE(f.pContext->any_mouse_button_down().has_value());
}

TEST_CASE("cursor mode round trips", "[mouse]") {
    input_fixture f;

    SECTION("it starts normal") {
        REQUIRE(f.pContext->mouse_cursor_mode() == mouse::cursor_mode::normal);
    }

    SECTION("every mode reads back as itself") {
        for (const auto mode : magic_enum::enum_values<mouse::cursor_mode>()) {
            INFO("mode: " << std::string(magic_enum::enum_name(mode)));

            REQUIRE_NOTHROW(f.pContext->set_mouse_cursor_mode(mode));
            REQUIRE(f.pContext->mouse_cursor_mode() == mode);
        }
    }

    SECTION("returning to normal is possible from locked") {
        f.pContext->set_mouse_cursor_mode(mouse::cursor_mode::locked);
        REQUIRE(f.pContext->mouse_cursor_mode() == mouse::cursor_mode::locked);

        f.pContext->set_mouse_cursor_mode(mouse::cursor_mode::normal);
        REQUIRE(f.pContext->mouse_cursor_mode() == mouse::cursor_mode::normal);
    }
}

TEST_CASE("cursor position is normalized", "[mouse]") {
    input_fixture f;

    const auto position = f.pContext->mouse_cursor_position();

    REQUIRE(std::isfinite(position.x));
    REQUIRE(std::isfinite(position.y));
}

TEST_CASE("deltas are zero without movement", "[mouse]") {
    input_fixture f;

    SECTION("cursor delta") {
        for (int i = 0; i < 3; ++i) {
            f.poll();

            REQUIRE(f.pContext->mouse_delta().x == Approx(0.0));
            REQUIRE(f.pContext->mouse_delta().y == Approx(0.0));
        }
    }

    SECTION("scroll delta") {
        for (int i = 0; i < 3; ++i) {
            f.poll();

            REQUIRE(f.pContext->mouse_scroll_delta().x == Approx(0.0));
            REQUIRE(f.pContext->mouse_scroll_delta().y == Approx(0.0));
        }
    }

    SECTION("no axis is reported as moving") {
        REQUIRE_FALSE(f.pContext->any_mouse_axis_down(0.0001f).has_value());
    }
}

TEST_CASE("a context can be replaced on the same window", "[mouse]") {
    input_fixture f;

    {
        auto pSecond = glfw_context::make(f.pWindow);

        REQUIRE(pSecond != nullptr);
        REQUIRE(pSecond->mouse_scroll_delta().y == Approx(0.0));
    }

    f.poll();

    REQUIRE(f.pContext->mouse_scroll_delta().y == Approx(0.0));
    REQUIRE_FALSE(f.pContext->any_mouse_button_down().has_value());
}

TEST_CASE("mouse enums stream as their names", "[mouse]") {
    SECTION("button") {
        std::ostringstream out;
        out << mouse::button::right;

        REQUIRE(out.str() == "right");
    }

    SECTION("cursor_mode") {
        std::ostringstream out;
        out << mouse::cursor_mode::locked;

        REQUIRE(out.str() == "locked");
    }

    SECTION("axis") {
        std::ostringstream out;
        out << mouse::axis::y;

        REQUIRE(out.str() == "y");
    }
}

TEST_CASE("any_mouse_axis_down honours its threshold", "[mouse]") {
    input_fixture f;

    SECTION("a still mouse reports nothing, whatever the threshold") {
        REQUIRE_FALSE(f.pContext->any_mouse_axis_down(0.f).has_value());
        REQUIRE_FALSE(f.pContext->any_mouse_axis_down(0.5f).has_value());
    }

    SECTION("and the platform really does refuse to move the cursor") {
        const auto before = f.pContext->mouse_cursor_position();

        glfwSetCursorPos(f.pWindow.get(), 300.0, 50.0);
        f.poll();

        REQUIRE(f.pContext->mouse_cursor_position().x == Approx(before.x));
        REQUIRE(f.pContext->mouse_delta().x == Approx(0.0));
    }
}
