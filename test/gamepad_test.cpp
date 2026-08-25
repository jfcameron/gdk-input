// © Joseph Cameron - All Rights Reserved

#include <jfc/catch.hpp>

#include "test_include.h"

#include <gdk/input/gamepad.h>

using namespace gdk::input;

TEST_CASE("get_gamepad never returns null", "[gamepad]") {
    input_fixture f;

    SECTION("the first slot") {
        REQUIRE(f.pContext->get_gamepad(0) != nullptr);
    }

    SECTION("slots that cannot be occupied") {
        for (const std::size_t index : {std::size_t(1), std::size_t(4), std::size_t(15)}) {
            INFO("index: " << index);

            REQUIRE(f.pContext->get_gamepad(index) != nullptr);
        }
    }
}

TEST_CASE("an absent gamepad polls quietly", "[gamepad]") {
    input_fixture f;

    auto pGamepad = f.pContext->get_gamepad(0);

    REQUIRE(pGamepad != nullptr);

    SECTION("it reports itself disconnected") {
        REQUIRE_FALSE(pGamepad->connected());
    }

    SECTION("no button is down") {
        for (gamepad::index_type i = 0; i < 16; ++i) {
            INFO("button: " << i);

            REQUIRE_FALSE(pGamepad->button_down(i));
            REQUIRE_FALSE(pGamepad->button_just_pressed(i));
            REQUIRE_FALSE(pGamepad->button_just_released(i));
        }

        REQUIRE_FALSE(pGamepad->any_button_down().has_value());
    }

    SECTION("every axis reads zero") {
        for (gamepad::index_type i = 0; i < 8; ++i) {
            INFO("axis: " << i);

            REQUIRE(pGamepad->axis_value(i) == Approx(0.0f));
            REQUIRE_FALSE(pGamepad->axis_just_exceeded_threshold(i));
            REQUIRE_FALSE(pGamepad->axis_just_dropped_below_threshold(i));
        }

        REQUIRE_FALSE(pGamepad->any_axis_down().has_value());
    }

    SECTION("indices far past any real pad are still safe") {
        REQUIRE_NOTHROW(pGamepad->button_down(255));
        REQUIRE_NOTHROW(pGamepad->axis_value(255));
    }

    SECTION("it has a name") {
        REQUIRE_NOTHROW(pGamepad->name());
    }
}

TEST_CASE("the gamepad collection is consistent with get_gamepad", "[gamepad]") {
    input_fixture f;

    const auto pads = f.pContext->gamepads();

    SECTION("no entry is null") {
        for (const auto &pPad : pads) REQUIRE(pPad != nullptr);
    }

    SECTION("indexing agrees with the collection") {
        for (std::size_t i = 0; i < pads.size(); ++i) {
            INFO("index: " << i);

            REQUIRE(f.pContext->get_gamepad(i) != nullptr);
        }
    }
}

TEST_CASE("polling does not connect a gamepad that is not there", "[gamepad]") {
    input_fixture f;

    for (int i = 0; i < 5; ++i) {
        f.poll();

        REQUIRE_FALSE(f.pContext->get_gamepad(0)->connected());
    }
}
