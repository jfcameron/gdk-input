// © Joseph Cameron - MIT License

#include <jfc/catch.hpp>

#include <gdk/input/gamepad.h>
#include <gdk/input/types.h>

#include <magic_enum/magic_enum.hpp>

#include <sstream>
#include <string>

using namespace gdk::input;

namespace {
    template <typename T>
    [[nodiscard]] std::string streamed(const T aValue) {
        std::ostringstream out;
        out << aValue;

        return out.str();
    }
}

TEST_CASE("the key enum is worth sweeping", "[keyboard]") {
    REQUIRE(magic_enum::enum_values<keyboard::key>().size() > 90);

    REQUIRE_FALSE(magic_enum::enum_name(keyboard::key::search).empty());

    REQUIRE(magic_enum::enum_values<keyboard::key>().back() == keyboard::key::search);
}

TEST_CASE("enums stream as their names", "[enum]") {
    REQUIRE(streamed(keyboard::key::space) == "space");
    REQUIRE(streamed(keyboard::key_state::just_pressed) == "just_pressed");
    REQUIRE(streamed(mouse::button::right) == "right");
    REQUIRE(streamed(mouse::cursor_mode::locked) == "locked");
    REQUIRE(streamed(mouse::axis::y) == "y");
    REQUIRE(streamed(mouse::button_state::held_down) == "held_down");
    REQUIRE(streamed(gamepad::button::left_bumper) == "left_bumper");
    REQUIRE(streamed(gamepad::axis::right_trigger) == "right_trigger");
}
