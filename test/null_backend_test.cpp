// © Joseph Cameron - All Rights Reserved

#include <jfc/catch.hpp>

#include <algorithm>

#include <gdk/input/context.h>
#include <gdk/input/controls.h>
#include <gdk/input/null_context.h>

#include <string>

using namespace gdk::input;

TEST_CASE("the contract holds with nothing attached", "[null][input]")
{
    const context_ptr_type pContext = null_context::make();

    REQUIRE(pContext);

    SECTION("get_gamepad never returns null, at any index")
    {
        for (const std::size_t index : {std::size_t(0), std::size_t(1), std::size_t(15)})
        {
            INFO("index: " << index);

            REQUIRE(pContext->get_gamepad(index) != nullptr);
        }
    }

    SECTION("including indices past the last slot")
    {
        REQUIRE(pContext->get_gamepad(9999) != nullptr);
    }

    SECTION("an absent gamepad polls quietly rather than lying")
    {
        auto pGamepad = pContext->get_gamepad(0);

        REQUIRE_FALSE(pGamepad->connected());
        REQUIRE_FALSE(pGamepad->button_down(0));
        REQUIRE_FALSE(pGamepad->any_button_down().has_value());
        REQUIRE(pGamepad->axis_value(0) == 0);
    }
}

TEST_CASE("a driven gamepad behaves like hardware", "[null][input]")
{
    auto pContext = null_context::make();

    const auto slot = pContext->attach_gamepad("test pad", "030000005e040000e002000000000000");

    REQUIRE(slot.has_value());
    REQUIRE(*slot == 0);

    auto pGamepad = pContext->get_gamepad(*slot);

    REQUIRE(pGamepad->connected());
    REQUIRE(pGamepad->name() == "test pad");

    SECTION("a press is just_pressed for exactly one frame, then held")
    {
        pContext->gamepad_at(*slot)->press(3);

        pContext->update();

        REQUIRE(pGamepad->button_just_pressed(3));
        REQUIRE(pGamepad->button_down(3));

        pContext->update();

        REQUIRE_FALSE(pGamepad->button_just_pressed(3));
        REQUIRE(pGamepad->button_down(3));
    }

    SECTION("and a release is just_released for exactly one frame, then up")
    {
        pContext->gamepad_at(*slot)->press(3);
        pContext->update();
        pContext->gamepad_at(*slot)->release(3);
        pContext->update();

        REQUIRE(pGamepad->button_just_released(3));
        REQUIRE_FALSE(pGamepad->button_down(3));

        pContext->update();

        REQUIRE_FALSE(pGamepad->button_just_released(3));
        REQUIRE_FALSE(pGamepad->button_down(3));
    }

    SECTION("axes respect the threshold they are asked about")
    {
        pContext->gamepad_at(*slot)->set_axis(0, 0.05f);
        pContext->update();

        REQUIRE(pGamepad->axis_value(0, 0.1f) == 0);
        REQUIRE(pGamepad->axis_value(0, 0.01f) == Approx(0.05f));
    }

    SECTION("any_button_down finds one, and stops finding it when released")
    {
        pContext->gamepad_at(*slot)->press(7);
        pContext->update();

        REQUIRE(pGamepad->any_button_down() == 7);

        pContext->gamepad_at(*slot)->release(7);
        pContext->update();
        pContext->update();

        REQUIRE_FALSE(pGamepad->any_button_down().has_value());
    }
}

TEST_CASE("a controller comes back to the player it had", "[null][input][disconnect]")
{
    auto pContext = null_context::make();

    const auto a = pContext->attach_gamepad("pad A", "guid-a");
    const auto b = pContext->attach_gamepad("pad B", "guid-b");

    REQUIRE(*a == 0);
    REQUIRE(*b == 1);

    SECTION("unplugging one leaves the other where it was")
    {
        pContext->detach_gamepad(*a);

        REQUIRE_FALSE(pContext->get_gamepad(0)->connected());
        REQUIRE(pContext->get_gamepad(1)->connected());
        REQUIRE(pContext->get_gamepad(1)->name() == "pad B");
    }

    SECTION("and it returns to its own player, not the lowest free one")
    {
        pContext->detach_gamepad(*a);

        const auto again = pContext->attach_gamepad("pad A", "guid-a");

        REQUIRE(*again == 0);
        REQUIRE(pContext->get_gamepad(0)->guid() == "guid-a");
    }

    SECTION("player two's controller returns to player two, not to the free player one")
    {
        pContext->detach_gamepad(*a);
        pContext->detach_gamepad(*b);

        const auto backB = pContext->attach_gamepad("pad B", "guid-b");

        REQUIRE(*backB == 1);
        REQUIRE(pContext->get_gamepad(1)->guid() == "guid-b");
        REQUIRE_FALSE(pContext->get_gamepad(0)->connected());
    }

    SECTION("a device nobody recognises takes the lowest free player")
    {
        pContext->detach_gamepad(*a);

        const auto c = pContext->attach_gamepad("pad C", "guid-c");

        REQUIRE(*c == 0);
        REQUIRE(pContext->get_gamepad(0)->guid() == "guid-c");
    }

    SECTION("and the game can tell, which is the part that was missing entirely")
    {
        pContext->detach_gamepad(*a);

        REQUIRE(pContext->get_gamepad(0)->just_disconnected());

        const auto c = pContext->attach_gamepad("pad C", "guid-c");

        REQUIRE(pContext->get_gamepad(*c)->just_connected());
        REQUIRE(pContext->get_gamepad(*c)->guid() != "guid-a");
    }

    SECTION("the connect and disconnect edges last exactly one frame")
    {
        pContext->detach_gamepad(*a);

        REQUIRE(pContext->get_gamepad(0)->just_disconnected());

        pContext->update();

        REQUIRE_FALSE(pContext->get_gamepad(0)->just_disconnected());

        const auto again = pContext->attach_gamepad("pad A", "guid-a");

        REQUIRE(pContext->get_gamepad(*again)->just_connected());

        pContext->update();

        REQUIRE_FALSE(pContext->get_gamepad(*again)->just_connected());
    }

    SECTION("a detached device reports nothing rather than its last state")
    {
        pContext->gamepad_at(*a)->press(1);
        pContext->update();

        REQUIRE(pContext->get_gamepad(*a)->button_down(gamepad::index_type(1)));

        pContext->detach_gamepad(*a);

        REQUIRE_FALSE(pContext->get_gamepad(*a)->connected());
        REQUIRE_FALSE(pContext->get_gamepad(*a)->button_down(gamepad::index_type(1)));
    }
}

TEST_CASE("the port breaks the tie between identical controllers", "[null][input][disconnect]")
{
    auto pContext = null_context::make();

    const auto one = pContext->attach_gamepad_at_port(0, "Xbox Controller", "same-guid");
    const auto two = pContext->attach_gamepad_at_port(1, "Xbox Controller", "same-guid");

    REQUIRE(*one == 0);
    REQUIRE(*two == 1);

    SECTION("both are unplugged, and each returns to its own port's player")
    {
        pContext->detach_gamepad(*one);
        pContext->detach_gamepad(*two);

        const auto backTwo = pContext->attach_gamepad_at_port(1, "Xbox Controller", "same-guid");

        REQUIRE(*backTwo == 1);
    }

    SECTION("but plugged into a different port, the guid is all there is to go on")
    {
        pContext->detach_gamepad(*two);

        const auto moved = pContext->attach_gamepad_at_port(7, "Xbox Controller", "same-guid");

        REQUIRE(*moved == 1);
    }
}

TEST_CASE("every player taken means the next device waits", "[null][input][disconnect]")
{
    auto pContext = null_context::make();

    for (std::size_t i = 0; i < null_context::GAMEPAD_SLOT_COUNT; ++i)
    {
        INFO("filling player " << i);

        REQUIRE(pContext->attach_gamepad("pad", "guid-" + std::to_string(i)).has_value());
    }

    REQUIRE_FALSE(pContext->attach_gamepad("one too many", "guid-extra").has_value());
}

TEST_CASE("keyboard and mouse are drivable too", "[null][input]")
{
    auto pContext = null_context::make();

    SECTION("a key is just_pressed for one frame")
    {
        pContext->press_key(keyboard::key::space);

        REQUIRE(pContext->key_just_pressed(keyboard::key::space));
        REQUIRE(pContext->key_down(keyboard::key::space));

        pContext->update();

        REQUIRE_FALSE(pContext->key_just_pressed(keyboard::key::space));
        REQUIRE(pContext->key_down(keyboard::key::space));
    }

    SECTION("any_key_down finds what is held")
    {
        REQUIRE_FALSE(pContext->any_key_down().has_value());

        pContext->press_key(keyboard::key::a);

        REQUIRE(pContext->any_key_down() == keyboard::key::a);
    }

    SECTION("the cursor mode round trips")
    {
        REQUIRE(pContext->mouse_cursor_mode() == mouse::cursor_mode::normal);

        pContext->set_mouse_cursor_mode(mouse::cursor_mode::locked);

        REQUIRE(pContext->mouse_cursor_mode() == mouse::cursor_mode::locked);
    }

    SECTION("mouse delta answers any_mouse_axis_down")
    {
        pContext->set_mouse_delta({0.5, 0.0});

        REQUIRE(pContext->any_mouse_axis_down(0.1f) == mouse::axis::x);
        REQUIRE_FALSE(pContext->any_mouse_axis_down(0.9f).has_value());

        pContext->set_mouse_delta({0.0, 0.0});

        REQUIRE_FALSE(pContext->any_mouse_axis_down(0.f).has_value());

        pContext->set_mouse_delta({0.0, -0.4});

        REQUIRE(pContext->any_mouse_axis_down(0.f) == mouse::axis::y);
        REQUIRE(pContext->any_mouse_axis_down(0.3f) == mouse::axis::y);
        REQUIRE_FALSE(pContext->any_mouse_axis_down(0.5f).has_value());
    }
}

TEST_CASE("a mapped device presents the standard layout", "[null][input][mapping]")
{
    auto pContext = null_context::make();

    SECTION("a recognised pad says so")
    {
        const auto slot = pContext->attach_gamepad("Xbox Controller", "guid-xbox");

        REQUIRE(pContext->get_gamepad(*slot)->has_standard_mapping());
        REQUIRE(pContext->get_gamepad(*slot)->hat_count() == 0);
    }

    SECTION("an unrecognised one says that, and reports hats instead")
    {
        pContext->gamepad_at(0)->attach_unmapped("Flight Stick", "guid-stick", 12, 4, 1);

        auto pGamepad = pContext->get_gamepad(0);

        REQUIRE(pGamepad->connected());
        REQUIRE_FALSE(pGamepad->has_standard_mapping());
        REQUIRE(pGamepad->hat_count() == 1);
        REQUIRE(pGamepad->hat(0).centred());
    }

    SECTION("a hat points where it is pushed")
    {
        pContext->gamepad_at(0)->attach_unmapped("Flight Stick", "guid-stick", 12, 4, 1);

        pContext->gamepad_at(0)->set_hat(0,
            {gamepad::hat_state_type::horizontal::left, gamepad::hat_state_type::vertical::up});

        const auto state = pContext->get_gamepad(0)->hat(0);

        REQUIRE(state.x == gamepad::hat_state_type::horizontal::left);
        REQUIRE(state.y == gamepad::hat_state_type::vertical::up);
        REQUIRE_FALSE(state.centred());
    }

    SECTION("a hat index the device does not have is centred rather than an error")
    {
        pContext->gamepad_at(0)->attach_unmapped("Flight Stick", "guid-stick", 12, 4, 1);

        REQUIRE(pContext->get_gamepad(0)->hat(7).centred());
    }

    SECTION("nothing attached has no mapping and no hats")
    {
        REQUIRE_FALSE(pContext->get_gamepad(0)->has_standard_mapping());
        REQUIRE(pContext->get_gamepad(0)->hat_count() == 0);
    }

    SECTION("and unplugging a mapped device takes its mapping with it")
    {
        const auto slot = pContext->attach_gamepad("Xbox Controller", "guid-xbox");

        REQUIRE(pContext->get_gamepad(*slot)->has_standard_mapping());

        pContext->detach_gamepad(*slot);

        REQUIRE_FALSE(pContext->get_gamepad(*slot)->has_standard_mapping());
    }
}

TEST_CASE("the named controls address the same things as the indices", "[null][input][named]")
{
    auto pContext = null_context::make();

    const auto slot = pContext->attach_gamepad("Xbox Controller", "guid-xbox");

    auto pDriver = pContext->gamepad_at(*slot);
    auto pGamepad = pContext->get_gamepad(*slot);

    SECTION("a named button is the index the standard layout gives it")
    {
        pDriver->press(static_cast<gamepad::index_type>(gamepad::button::x));
        pContext->update();

        REQUIRE(pGamepad->button_down(gamepad::button::x));
        REQUIRE(pGamepad->button_just_pressed(gamepad::button::x));
        REQUIRE_FALSE(pGamepad->button_down(gamepad::button::a));
    }

    SECTION("the dpad is buttons on a mapped device")
    {
        pDriver->press(static_cast<gamepad::index_type>(gamepad::button::dpad_left));
        pContext->update();

        REQUIRE(pGamepad->button_down(gamepad::button::dpad_left));
        REQUIRE_FALSE(pGamepad->button_down(gamepad::button::dpad_right));
        REQUIRE(pGamepad->hat_count() == 0);
    }

    SECTION("a named axis reads the same value the index does")
    {
        pDriver->set_axis(static_cast<gamepad::index_type>(gamepad::axis::right_y), 0.8f);
        pContext->update();

        REQUIRE(pGamepad->axis_value(gamepad::axis::right_y) == Approx(0.8f));
        REQUIRE(pGamepad->axis_value(gamepad::axis::right_y)
            == pGamepad->axis_value(static_cast<gamepad::index_type>(gamepad::axis::right_y)));
        REQUIRE(pGamepad->axis_value(gamepad::axis::left_y) == 0);
    }

    SECTION("named axis edges work the same way")
    {
        pDriver->set_axis(static_cast<gamepad::index_type>(gamepad::axis::left_trigger), 0.9f);
        pContext->update();

        REQUIRE(pGamepad->axis_just_exceeded_threshold(gamepad::axis::left_trigger));
    }
}

TEST_CASE("an unmapped device answers nothing to a named control", "[null][input][named]")
{
    auto pContext = null_context::make();

    pContext->gamepad_at(0)->attach_unmapped("Flight Stick", "guid-stick", 12, 4, 1);

    auto pDriver = pContext->gamepad_at(0);
    auto pGamepad = pContext->get_gamepad(0);

    pDriver->press(0);
    pDriver->set_axis(0, 1.0f);
    pContext->update();

    SECTION("the index form still reads it, because the device is working fine")
    {
        REQUIRE(pGamepad->button_down(gamepad::index_type(0)));
        REQUIRE(pGamepad->axis_value(gamepad::index_type(0)) == Approx(1.0f));
    }

    SECTION("but the named form does not pretend index 0 is the A button")
    {
        REQUIRE_FALSE(pGamepad->button_down(gamepad::button::a));
        REQUIRE(pGamepad->axis_value(gamepad::axis::left_x) == 0);
    }
}

TEST_CASE("an axis edge is about leaving the dead zone, not about being positive", "[null][gamepad]")
{
    auto pContext = null_context::make();

    const auto player = pContext->attach_gamepad("pad", "guid-pad").value();

    auto pGamepad = pContext->gamepad_at(player);

    const auto left_x = static_cast<gamepad::index_type>(gamepad::axis::left_x);

    const auto push = [&](const float aValue) {
        pGamepad->set_axis(left_x, aValue);
        pContext->update();
    };

    SECTION("pushed one way")
    {
        push(0.8f);

        REQUIRE(pGamepad->axis_just_exceeded_threshold(left_x, 0.1f));
        REQUIRE_FALSE(pGamepad->axis_just_dropped_below_threshold(left_x, 0.1f));
    }

    SECTION("pushed the other way, which is the case that used to be invisible")
    {
        push(-0.8f);

        REQUIRE(pGamepad->axis_just_exceeded_threshold(left_x, 0.1f));
        REQUIRE_FALSE(pGamepad->axis_just_dropped_below_threshold(left_x, 0.1f));
    }

    SECTION("and released from either side")
    {
        push(-0.8f);
        push(0.0f);

        REQUIRE(pGamepad->axis_just_dropped_below_threshold(left_x, 0.1f));
        REQUIRE_FALSE(pGamepad->axis_just_exceeded_threshold(left_x, 0.1f));
    }

    SECTION("moving further within the zone is not another edge")
    {
        push(0.8f);
        push(0.9f);

        REQUIRE_FALSE(pGamepad->axis_just_exceeded_threshold(left_x, 0.1f));
    }
}

TEST_CASE("a gamepad reports what it read last frame", "[null][gamepad]")
{
    auto pContext = null_context::make();

    const auto player = pContext->attach_gamepad("pad", "guid-pad").value();

    auto pGamepad = pContext->gamepad_at(player);

    const auto left_y = static_cast<gamepad::index_type>(gamepad::axis::left_y);

    pGamepad->set_axis(left_y, -0.9f);
    pContext->update();

    REQUIRE(pGamepad->axis_value(left_y, 0.1f) == Approx(-0.9f));
    REQUIRE(pGamepad->previous_axis_value(left_y, 0.1f) == 0);

    pGamepad->set_axis(left_y, 0.0f);
    pContext->update();

    REQUIRE(pGamepad->axis_value(left_y, 0.1f) == 0);
    REQUIRE(pGamepad->previous_axis_value(left_y, 0.1f) == Approx(-0.9f));

    REQUIRE(pGamepad->previous_axis_value(left_y, 0.95f) == 0);
}

TEST_CASE("a gamepad says how many controls it has", "[null][gamepad]")
{
    auto pContext = null_context::make();

    auto pGamepad = pContext->gamepad_at(0);

    SECTION("nothing attached")
    {
        REQUIRE(pGamepad->button_count() == 0);
        REQUIRE(pGamepad->axis_count() == 0);
    }

    SECTION("a standard pad has at least the whole named layout")
    {
        REQUIRE(pContext->attach_gamepad("pad", "guid-pad"));

        REQUIRE(pGamepad->button_count()
            > static_cast<std::size_t>(gamepad::button::dpad_left));
        REQUIRE(pGamepad->axis_count()
            > static_cast<std::size_t>(gamepad::axis::right_trigger));
    }

    SECTION("and unusual hardware reports its own shape")
    {
        pGamepad->attach_unmapped("Flight Stick", "guid-stick", 12, 4, 1);

        REQUIRE(pGamepad->button_count() == 12);
        REQUIRE(pGamepad->axis_count() == 4);
        REQUIRE(pGamepad->hat_count() == 1);
    }
}

TEST_CASE("a trigger rests at zero and reaches one", "[null][gamepad][trigger]")
{
    auto pContext = null_context::make();

    const auto player = pContext->attach_gamepad("pad", "guid-pad").value();

    auto pDriver = pContext->gamepad_at(player);
    auto pGamepad = pContext->get_gamepad(player);

    const auto trigger = static_cast<gamepad::index_type>(gamepad::axis::left_trigger);

    const auto press = [&](const float aRaw) {
        pDriver->set_axis(trigger, aRaw);
        pContext->update();
    };

    SECTION("at rest it reads nothing, and is not already past the dead zone")
    {
        press(-1.f);

        REQUIRE(pGamepad->axis_value(gamepad::axis::left_trigger) == 0);
        REQUIRE_FALSE(pGamepad->axis_just_exceeded_threshold(gamepad::axis::left_trigger));
    }

    SECTION("pressed it reads one, and that is an edge")
    {
        press(-1.f);
        press(1.f);

        REQUIRE(pGamepad->axis_value(gamepad::axis::left_trigger) == Approx(1.f));
        REQUIRE(pGamepad->axis_just_exceeded_threshold(gamepad::axis::left_trigger));
    }

    SECTION("released it is an edge the other way")
    {
        press(-1.f);
        press(1.f);
        press(-1.f);

        REQUIRE(pGamepad->axis_value(gamepad::axis::left_trigger) == 0);
        REQUIRE(pGamepad->axis_just_dropped_below_threshold(gamepad::axis::left_trigger));
    }

    SECTION("half pressed is half")
    {
        press(0.f);

        REQUIRE(pGamepad->axis_value(gamepad::axis::left_trigger) == Approx(0.5f));
    }

    SECTION("the index form says the same thing as the named one")
    {
        press(0.5f);

        REQUIRE(pGamepad->axis_value(trigger) == pGamepad->axis_value(gamepad::axis::left_trigger));
    }

    SECTION("a stick is untouched by any of this")
    {
        pDriver->set_axis(static_cast<gamepad::index_type>(gamepad::axis::left_x), -0.8f);
        pContext->update();

        REQUIRE(pGamepad->axis_value(gamepad::axis::left_x) == Approx(-0.8f));
    }

    SECTION("and an action bound to a trigger behaves")
    {
        auto pControls = controls::make(pContext, player);

        pControls->bind("accelerate", gamepad::axis::left_trigger, 1.f);

        press(-1.f);

        REQUIRE(pControls->get("accelerate") == 0);
        REQUIRE_FALSE(pControls->down("accelerate"));

        press(1.f);

        REQUIRE(pControls->get("accelerate") == Approx(1.f));
        REQUIRE(pControls->down("accelerate"));
        REQUIRE(pControls->just_pressed("accelerate"));
    }
}

TEST_CASE("hardware the backend does not know can be taught", "[null][gamepad][mapping]")
{
    auto pContext = null_context::make();

    const std::string database =
        "# a comment line\n"
        "\n"
        "050000005e040000e002000003090000,8Bitdo SN30 Pro,a:b0,b:b1,platform:Linux,\n"
        "not a mapping at all\n"
        "03000000de280000ff11000001000000,Steam Virtual Gamepad,a:b0,platform:Linux,\n";

    SECTION("the count is of entries that look like mappings")
    {
        REQUIRE(pContext->add_gamepad_mappings(database) == 2);
        REQUIRE(pContext->add_gamepad_mappings("# nothing here\n") == 0);
        REQUIRE(pContext->add_gamepad_mappings("") == 0);
        REQUIRE(pContext->add_gamepad_mappings("tooshort,name,a:b0,\n") == 0);
        REQUIRE(pContext->add_gamepad_mappings(
            "zzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzz,name,a:b0,\n") == 0);
    }

    SECTION("a device already attached picks up a mapping added afterwards")
    {
        pContext->gamepad_at(0)->attach_unmapped("8Bitdo SN30 Pro",
            "050000005e040000e002000003090000", 16, 6);
        pContext->update();

        auto pGamepad = pContext->get_gamepad(0);

        REQUIRE_FALSE(pGamepad->has_standard_mapping());
        REQUIRE_FALSE(pGamepad->button_down(gamepad::button::a));

        REQUIRE(pContext->add_gamepad_mappings(database) == 2);

        REQUIRE(pGamepad->has_standard_mapping());

        SECTION("and its named controls start meaning something")
        {
            pContext->gamepad_at(0)->press(static_cast<gamepad::index_type>(gamepad::button::a));
            pContext->update();

            REQUIRE(pGamepad->button_down(gamepad::button::a));
        }

        SECTION("including its triggers, which now rest where they should")
        {
            pContext->gamepad_at(0)->set_axis(
                static_cast<gamepad::index_type>(gamepad::axis::left_trigger), -1.f);
            pContext->update();

            REQUIRE(pGamepad->axis_value(gamepad::axis::left_trigger) == 0);
        }
    }

    SECTION("a device the database says nothing about is left alone")
    {
        pContext->gamepad_at(0)->attach_unmapped("Flight Stick", "guid-stick", 12, 4);
        pContext->update();

        REQUIRE(pContext->add_gamepad_mappings(database) == 2);

        REQUIRE_FALSE(pContext->get_gamepad(0)->has_standard_mapping());
    }
}

TEST_CASE("a game can decide which device is which player", "[null][gamepad][players]")
{
    const std::string VIRTUAL_GUID = "03000000de280000ff11000001000000";
    const std::string LOCK_GUID = "030000008a2400007388000011010000";
    const std::string PAD_GUID = "050000005e040000e002000003090000";

    auto pContext = null_context::make();

    REQUIRE(pContext->attach_gamepad("Virtual Gamepad", VIRTUAL_GUID) == 0);
    REQUIRE(pContext->attach_gamepad("USB LOCK KEY", LOCK_GUID) == 1);
    REQUIRE(pContext->attach_gamepad("8Bitdo SN30 Pro", PAD_GUID) == 2);

    pContext->gamepad_at(1)->set_standard_mapping(false);
    pContext->gamepad_at(2)->set_standard_mapping(false);

    pContext->update();

    REQUIRE(pContext->get_gamepad(0)->name() == "Virtual Gamepad");
    REQUIRE(pContext->get_gamepad(2)->name() == "8Bitdo SN30 Pro");

    SECTION("the one somebody pressed a button on becomes player one")
    {
        pContext->gamepad_at(2)->press(0);
        pContext->update();

        std::optional<std::size_t> pressed;

        for (std::size_t player = 0; player < 4; ++player)
            if (pContext->get_gamepad(player)->any_button_down()) pressed = player;

        REQUIRE(pressed == 2);

        pContext->swap_players(0, *pressed);

        REQUIRE(pContext->get_gamepad(0)->name() == "8Bitdo SN30 Pro");
        REQUIRE(pContext->get_gamepad(2)->name() == "Virtual Gamepad");
    }

    SECTION("controls follow the player, so bindings move with the swap")
    {
        REQUIRE(pContext->add_gamepad_mappings(
            PAD_GUID + ",8Bitdo SN30 Pro,a:b0,b:b1,platform:Linux,\n") == 1);

        REQUIRE(pContext->get_gamepad(2)->has_standard_mapping());

        auto pControls = controls::make(pContext, 0);

        pControls->bind("jump", gamepad::button::a);

        pContext->gamepad_at(2)->press(static_cast<gamepad::index_type>(gamepad::button::a));
        pContext->update();

        REQUIRE_FALSE(pControls->down("jump"));

        pContext->swap_players(0, 2);

        REQUIRE(pControls->down("jump"));
    }

    SECTION("the exchange survives the device going away and coming back")
    {
        pContext->swap_players(0, 2);

        REQUIRE(pContext->get_gamepad(0)->guid() == PAD_GUID);

        const auto port = pContext->port_of(0);

        REQUIRE(port);

        pContext->detach_gamepad(0);

        REQUIRE_FALSE(pContext->get_gamepad(0)->connected());

        REQUIRE(pContext->attach_gamepad_at_port(*port, "8Bitdo SN30 Pro", PAD_GUID) == 0);

        REQUIRE(pContext->get_gamepad(0)->name() == "8Bitdo SN30 Pro");
    }

    SECTION("swapping a player with itself, or past the end, does nothing")
    {
        pContext->swap_players(0, 0);
        pContext->swap_players(0, 9999);
        pContext->swap_players(9999, 0);

        REQUIRE(pContext->get_gamepad(0)->name() == "Virtual Gamepad");
        REQUIRE(pContext->get_gamepad(2)->name() == "8Bitdo SN30 Pro");
    }

    SECTION("an empty slot can be swapped into, which is how a player is set aside")
    {
        pContext->swap_players(2, 5);

        REQUIRE_FALSE(pContext->get_gamepad(2)->connected());
        REQUIRE(pContext->get_gamepad(5)->name() == "8Bitdo SN30 Pro");
    }
}

TEST_CASE("every pressed key is reported, not just the first", "[null][keyboard]")
{
    auto pContext = null_context::make();

    context_ptr_type pShared{pContext};

    pContext->press_key(keyboard::key::leftshift);
    pContext->press_key(keyboard::key::a);

    const auto keys = pShared->keys_down();

    REQUIRE(keys.size() == 2);
    REQUIRE(std::find(keys.begin(), keys.end(), keyboard::key::a) != keys.end());
    REQUIRE(std::find(keys.begin(), keys.end(), keyboard::key::leftshift) != keys.end());

    SECTION("in enum order, so two runs agree")
    {
        REQUIRE(std::is_sorted(keys.begin(), keys.end()));
    }

    SECTION("releasing one leaves the other")
    {
        pContext->release_key(keyboard::key::a);
        pContext->update();

        REQUIRE(pShared->keys_down() == std::vector<keyboard::key>{keyboard::key::leftshift});
    }

    SECTION("mouse buttons the same way")
    {
        pContext->press_mouse_button(mouse::button::left);
        pContext->press_mouse_button(mouse::button::right);

        REQUIRE(pShared->mouse_buttons_down().size() == 2);
        REQUIRE(pShared->any_mouse_button_down() == pShared->mouse_buttons_down().front());
    }
}
