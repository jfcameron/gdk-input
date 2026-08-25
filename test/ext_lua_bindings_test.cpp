// © Joseph Cameron - All Rights Reserved

#include <jfc/catch.hpp>

#include <gdk/input/exception.h>
#include <gdk/input/ext/lua_bindings.h>
#include <gdk/input/null_context.h>

#include <string>

using namespace gdk::input;

namespace {
    struct fixture final {
        std::shared_ptr<null_context> pNull{null_context::make()};
        context_ptr_type pContext{pNull};

        [[nodiscard]] controls_ptr_type make_controls(const std::size_t aPlayer = 0) {
            return controls::make(pContext, aPlayer);
        }

        [[nodiscard]] controls_ptr_type populated() {
            auto pControls = make_controls();

            pControls->bind("jump", keyboard::key::space);
            pControls->bind("jump", gamepad::button::a);
            pControls->bind("fire", mouse::button::left);
            pControls->bind("move_left", gamepad::axis::left_x, -1.f);
            pControls->bind("move_right", gamepad::axis::left_x, 1.f);
            pControls->bind_device_button("brake", "guid-stick", 7);
            pControls->bind_device_axis("throttle", "guid-stick", 2, -1.f);

            return pControls;
        }
    };
}

TEST_CASE("a controls written out and read back is the same controls", "[ext][lua]")
{
    fixture f;

    auto pOriginal = f.populated();

    pOriginal->set_threshold(0.25f);

    auto pLoaded = f.make_controls();

    REQUIRE(ext::from_string(*pLoaded, ext::to_string(*pOriginal)) == 0);

    REQUIRE(pLoaded->actions() == pOriginal->actions());
    REQUIRE(pLoaded->threshold() == Approx(pOriginal->threshold()));

    for (const auto &action : pOriginal->actions())
    {
        INFO("action " << action);

        REQUIRE(pLoaded->sources(action) == pOriginal->sources(action));
    }
}

TEST_CASE("the same bindings write the same bytes", "[ext][lua]")
{
    fixture f;

    auto pOne = f.make_controls();
    auto pTwo = f.make_controls();

    pOne->bind("jump", keyboard::key::space);
    pOne->bind("crouch", keyboard::key::c);
    pOne->bind("attack", gamepad::button::x);

    pTwo->bind("attack", gamepad::button::x);
    pTwo->bind("jump", keyboard::key::space);
    pTwo->bind("crouch", keyboard::key::c);

    REQUIRE(ext::to_string(*pOne) == ext::to_string(*pTwo));
}

TEST_CASE("controls are stored by name, not by number", "[ext][lua]")
{
    fixture f;

    auto pControls = f.make_controls();

    pControls->bind("jump", keyboard::key::space);
    pControls->bind("aim", gamepad::axis::right_x, -1.f);

    const auto text = ext::to_string(*pControls);

    INFO(text);

    REQUIRE(text.find("\"space\"") != std::string::npos);
    REQUIRE(text.find("\"right_x\"") != std::string::npos);

    auto pDevice = f.make_controls();

    pDevice->bind_device_button("brake", "guid-stick", 7);

    REQUIRE(ext::to_string(*pDevice).find("guid-stick") != std::string::npos);
}

TEST_CASE("loading replaces rather than merges", "[ext][lua]")
{
    fixture f;

    auto pControls = f.make_controls();

    pControls->bind("jump", keyboard::key::space);

    const auto saved = ext::to_string(*pControls);

    pControls->bind("jump", keyboard::key::w);
    pControls->bind("crouch", keyboard::key::c);

    REQUIRE(ext::from_string(*pControls, saved) == 0);

    REQUIRE(pControls->actions() == std::vector<std::string>{"jump"});
    REQUIRE(pControls->sources("jump") == std::vector<binding>{binding::of(keyboard::key::space)});
}

TEST_CASE("an entry naming something unknown costs one binding, not all of them", "[ext][lua]")
{
    fixture f;

    auto pControls = f.make_controls();

    const std::string text =
        "{"
        "[\"actions\"]={"
            "[\"jump\"]={[1]={[\"kind\"]=\"key\",[\"key\"]=\"space\"},"
                       "[2]={[\"kind\"]=\"key\",[\"key\"]=\"hyperspace\"}},"
            "[\"warp\"]={[1]={[\"kind\"]=\"telepathy\"}}"
        "},"
        "[\"version\"]=1"
        "}";

    REQUIRE(ext::from_string(*pControls, text) == 2);

    REQUIRE(pControls->sources("jump") == std::vector<binding>{binding::of(keyboard::key::space)});
    REQUIRE(pControls->source_count("warp") == 0);
}

TEST_CASE("a file that is not this format at all says so", "[ext][lua]")
{
    fixture f;

    auto pControls = f.make_controls();

    SECTION("no version")
    {
        REQUIRE_THROWS_AS(ext::from_string(*pControls, "{[\"actions\"]={}}"), exception);
    }

    SECTION("a version from the future")
    {
        REQUIRE_THROWS_AS(ext::from_string(*pControls, "{[\"version\"]=99,[\"actions\"]={}}"),
            exception);
    }

    SECTION("no actions")
    {
        REQUIRE_THROWS_AS(ext::from_string(*pControls, "{[\"version\"]=1}"), exception);
    }

    SECTION("not a table at all")
    {
        REQUIRE_THROWS(ext::from_string(*pControls, "this is not lua"));
    }

    SECTION("and a failed load leaves what was already there alone")
    {
        pControls->bind("jump", keyboard::key::space);

        REQUIRE_THROWS(ext::from_string(*pControls, "{[\"version\"]=1}"));

        REQUIRE(pControls->down("jump") == false);
        REQUIRE(pControls->source_count("jump") == 1);
    }
}

TEST_CASE("the text is read back without being run", "[ext][lua]")
{
    fixture f;

    auto pControls = f.make_controls();

    REQUIRE_THROWS(ext::from_string(*pControls, "os.exit()"));
    REQUIRE_THROWS(ext::from_string(*pControls, "{[\"version\"]=1,[\"actions\"]=(function() end)()}"));
}

TEST_CASE("an empty controls round trips", "[ext][lua]")
{
    fixture f;

    auto pEmpty = f.make_controls();
    auto pLoaded = f.populated();

    REQUIRE(ext::from_string(*pLoaded, ext::to_string(*pEmpty)) == 0);

    REQUIRE(pLoaded->actions().empty());
}

TEST_CASE("the player index is recorded but not applied", "[ext][lua]")
{
    fixture f;

    auto pSecond = f.make_controls(1);

    pSecond->bind("jump", keyboard::key::space);

    auto pFirst = f.make_controls(0);

    REQUIRE(ext::from_string(*pFirst, ext::to_string(*pSecond)) == 0);

    REQUIRE(pFirst->player() == 0);
    REQUIRE(ext::to_data_table(*pSecond).get_number("player") == 1);
}
