// © Joseph Cameron - MIT License

#include <jfc/catch.hpp>

#include <gdk/input/controls.h>
#include <gdk/input/impl_text_input.h>
#include <gdk/input/null_context.h>
#include <gdk/input/text.h>

#include <string>
#include <variant>
#include <vector>

using namespace gdk::input;

namespace {
    [[nodiscard]] text::edit edit_of(const keyboard::key aKey, const bool aRepeat = false) {
        text::edit out;

        out.key = aKey;
        out.repeat = aRepeat;

        return out;
    }

    [[nodiscard]] std::string text_at(const std::vector<text::event> &aEvents, const std::size_t aIndex) {
        return std::get<std::string>(aEvents.at(aIndex));
    }

    [[nodiscard]] text::edit edit_at(const std::vector<text::event> &aEvents, const std::size_t aIndex) {
        return std::get<text::edit>(aEvents.at(aIndex));
    }
}

TEST_CASE("code points encode as UTF-8", "[text]") {
    const auto encoded = [](const char32_t aCodePoint) {
        std::string out;

        append_utf8(out, aCodePoint);

        return out;
    };

    SECTION("at the edges of each length") {
        REQUIRE(encoded(0x41) == "A");
        REQUIRE(encoded(0x7F) == "\x7F");
        REQUIRE(encoded(0x80) == "\xC2\x80");
        REQUIRE(encoded(0x7FF) == "\xDF\xBF");
        REQUIRE(encoded(0x800) == "\xE0\xA0\x80");
        REQUIRE(encoded(0x304B) == "\xE3\x81\x8B");            // か
        REQUIRE(encoded(0xFFFF) == "\xEF\xBF\xBF");
        REQUIRE(encoded(0x10000) == "\xF0\x90\x80\x80");
        REQUIRE(encoded(0x1F600) == "\xF0\x9F\x98\x80");       // an emoji
        REQUIRE(encoded(0x10FFFF) == "\xF4\x8F\xBF\xBF");
    }

    SECTION("and what is not a character becomes the replacement character") {
        const std::string replacement = "\xEF\xBF\xBD";

        REQUIRE(encoded(0xD800) == replacement);
        REQUIRE(encoded(0xDFFF) == replacement);
        REQUIRE(encoded(0x110000) == replacement);
        REQUIRE(encoded(0xFFFFFFFF) == replacement);

        REQUIRE(encoded(0xD7FF) == "\xED\x9F\xBF");
        REQUIRE(encoded(0xE000) == "\xEE\x80\x80");
    }
}

TEST_CASE("text committed one piece after another is one string, and an edit parts it", "[text]") {
    std::vector<text::event> events;

    append_text(events, "a");
    append_text(events, "b");
    events.emplace_back(edit_of(keyboard::key::backspace));
    append_text(events, "c");
    append_text(events, "");

    REQUIRE(events.size() == 3);
    REQUIRE(text_at(events, 0) == "ab");
    REQUIRE(edit_at(events, 1).key == keyboard::key::backspace);
    REQUIRE(text_at(events, 2) == "c");
}

TEST_CASE("a withheld key", "[text][keyboard]") {
    withheld_key key;

    using state = keyboard::key_state;

    SECTION("without a text field it reads as it is") {
        REQUIRE(key.advance(false, false) == state::up);
        REQUIRE(key.advance(true, false) == state::just_pressed);
        REQUIRE(key.advance(true, false) == state::held_down);
        REQUIRE(key.advance(false, false) == state::just_released);
        REQUIRE(key.advance(false, false) == state::up);
    }

    SECTION("held when a field takes the keyboard, it reads as released once, then up") {
        REQUIRE(key.advance(true, false) == state::just_pressed);
        REQUIRE(key.advance(true, true) == state::just_released);
        REQUIRE(key.advance(true, true) == state::up);
        REQUIRE(key.advance(false, true) == state::up);
    }

    SECTION("pressed while a field has the keyboard, it never reads as down") {
        REQUIRE(key.advance(true, true) == state::up);
        REQUIRE(key.advance(true, true) == state::up);
        REQUIRE(key.advance(false, true) == state::up);
    }

    SECTION("still down when the field hands the keyboard back, it reads as up until let go") {
        REQUIRE(key.advance(true, true) == state::up);
        REQUIRE(key.advance(true, false) == state::up);
        REQUIRE(key.advance(true, false) == state::up);
        REQUIRE(key.advance(false, false) == state::up);

        SECTION("and pressed again, it is the game's") {
            REQUIRE(key.advance(true, false) == state::just_pressed);
            REQUIRE(key.advance(true, false) == state::held_down);
        }
    }
}

TEST_CASE("the null context types as a keyboard would", "[text][null]") {
    auto pContext = null_context::make();

    pContext->type_text("a");
    pContext->type_text("b");
    pContext->press_edit(edit_of(keyboard::key::backspace));
    pContext->type_text("c");

    REQUIRE(pContext->text_events().size() == 3);
    REQUIRE(text_at(pContext->text_events(), 0) == "ab");
    REQUIRE(text_at(pContext->text_events(), 2) == "c");

    SECTION("until the next update") {
        pContext->update();

        REQUIRE(pContext->text_events().empty());
    }

    SECTION("and composes, until the field hands the keyboard back") {
        text::composition composition;

        composition.text = "abc";
        composition.caret = 3;

        pContext->set_composition(composition);
        pContext->update();

        REQUIRE(pContext->text_composition() == composition);

        pContext->set_text_input_focus(true);
        pContext->set_text_input_focus(false);

        REQUIRE(pContext->text_composition().empty());
    }

    SECTION("and remembers where the caret was said to be") {
        const text::caret caret{0.25, 0.5, 0.01, 0.05};

        pContext->set_text_input_caret(caret);

        REQUIRE(pContext->text_input_caret() == caret);
    }
}

TEST_CASE("a text field with the keyboard withholds it from the game", "[text][null][keyboard]") {
    auto pContext = null_context::make();

    auto pControls = controls::make(pContext);

    pControls->bind("walk", keyboard::key::w);

    pContext->press_key(keyboard::key::w);
    pContext->update();

    REQUIRE(pContext->key_down(keyboard::key::w));
    REQUIRE(pControls->down("walk"));

    pContext->set_text_input_focus(true);

    SECTION("a key held reads as released, once, then up") {
        REQUIRE_FALSE(pContext->key_down(keyboard::key::w));
        REQUIRE(pContext->key_just_released(keyboard::key::w));
        REQUIRE_FALSE(pControls->down("walk"));

        pContext->update();

        REQUIRE_FALSE(pContext->key_just_released(keyboard::key::w));
        REQUIRE_FALSE(pContext->any_key_down());
    }

    SECTION("a key pressed while it has the keyboard never reads as down") {
        pContext->update();
        pContext->press_key(keyboard::key::s);

        REQUIRE_FALSE(pContext->key_down(keyboard::key::s));
        REQUIRE_FALSE(pContext->key_just_pressed(keyboard::key::s));

        pContext->release_key(keyboard::key::s);

        REQUIRE_FALSE(pContext->key_just_released(keyboard::key::s));
    }

    SECTION("the mouse is not the field's") {
        pContext->press_mouse_button(mouse::button::left);

        REQUIRE(pContext->mouse_button_down(mouse::button::left));
    }

    SECTION("a key still down when it hands the keyboard back reads as up until let go") {
        pContext->update();
        pContext->press_key(keyboard::key::enter);
        pContext->set_text_input_focus(false);

        REQUIRE_FALSE(pContext->key_down(keyboard::key::enter));
        REQUIRE_FALSE(pContext->key_down(keyboard::key::w));

        pContext->update();
        pContext->release_key(keyboard::key::enter);

        REQUIRE_FALSE(pContext->key_just_released(keyboard::key::enter));

        SECTION("then it is the game's again") {
            pContext->update();
            pContext->press_key(keyboard::key::enter);

            REQUIRE(pContext->key_just_pressed(keyboard::key::enter));
            REQUIRE(pContext->key_down(keyboard::key::enter));
        }
    }
}
