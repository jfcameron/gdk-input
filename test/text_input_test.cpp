// © Joseph Cameron - All Rights Reserved

#include <jfc/catch.hpp>

#include "test_include.h"

#include <gdk/input/controls.h>
#include <gdk/input/impl_glfw_text_input.h>
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

    struct installed_callbacks final {
        GLFWcharfun character;
        GLFWkeyfun key;
#if GDK_INPUT_GLFW_IME
        GLFWpreeditfun preedit;
#endif

        explicit installed_callbacks(GLFWwindow *const pWindow) {
            character = glfwSetCharCallback(pWindow, nullptr);
            glfwSetCharCallback(pWindow, character);

            key = glfwSetKeyCallback(pWindow, nullptr);
            glfwSetKeyCallback(pWindow, key);
#if GDK_INPUT_GLFW_IME
            preedit = glfwSetPreeditCallback(pWindow, nullptr);
            glfwSetPreeditCallback(pWindow, preedit);
#endif
        }
    };

    struct typing_fixture final {
        input_fixture input;
        installed_callbacks callbacks{input.pWindow.get()};

        void type(const char32_t aCodePoint) {
            callbacks.character(input.pWindow.get(), static_cast<unsigned int>(aCodePoint));
        }

        void key(const int aKey, const int aAction, const int aMods = 0) {
            callbacks.key(input.pWindow.get(), aKey, 0, aAction, aMods);
        }

        [[nodiscard]] const std::vector<text::event> &events() const {
            return input.pContext->text_events();
        }
    };
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

TEST_CASE("the glfw context collects what is typed", "[text][glfw]") {
    typing_fixture f;

    SECTION("nothing, to begin with") {
        REQUIRE(f.events().empty());
        REQUIRE(f.input.pContext->text_composition().empty());
        REQUIRE_FALSE(f.input.pContext->text_input_focus());
    }

    SECTION("characters, as UTF-8, joined into one string") {
        f.type('a');
        f.type(0x304B);     // か
        f.type(0x1F600);    // an emoji

        SECTION("once the next update has been") {
            REQUIRE(f.events().empty());

            f.input.poll();

            REQUIRE(f.events().size() == 1);
            REQUIRE(text_at(f.events(), 0) == "a\xE3\x81\x8B\xF0\x9F\x98\x80");

            SECTION("and only until the one after") {
                f.input.poll();

                REQUIRE(f.events().empty());
            }
        }
    }

    SECTION("editing keys as they are pressed and repeated, not released") {
        f.key(GLFW_KEY_BACKSPACE, GLFW_PRESS);
        f.key(GLFW_KEY_BACKSPACE, GLFW_REPEAT);
        f.key(GLFW_KEY_BACKSPACE, GLFW_RELEASE);

        f.input.poll();

        REQUIRE(f.events().size() == 2);
        REQUIRE(edit_at(f.events(), 0) == edit_of(keyboard::key::backspace));
        REQUIRE(edit_at(f.events(), 1) == edit_of(keyboard::key::backspace, true));
    }

    SECTION("every editing key, and nothing else") {
        const std::vector<std::pair<int, keyboard::key>> keys {
            {GLFW_KEY_BACKSPACE, keyboard::key::backspace}, {GLFW_KEY_DELETE, keyboard::key::deletekey},
            {GLFW_KEY_LEFT, keyboard::key::leftarrow}, {GLFW_KEY_RIGHT, keyboard::key::rightarrow},
            {GLFW_KEY_UP, keyboard::key::uparrow}, {GLFW_KEY_DOWN, keyboard::key::downarrow},
            {GLFW_KEY_HOME, keyboard::key::home}, {GLFW_KEY_END, keyboard::key::end},
            {GLFW_KEY_PAGE_UP, keyboard::key::pageup}, {GLFW_KEY_PAGE_DOWN, keyboard::key::pagedown},
            {GLFW_KEY_ENTER, keyboard::key::enter}, {GLFW_KEY_KP_ENTER, keyboard::key::numenter},
            {GLFW_KEY_ESCAPE, keyboard::key::escape}, {GLFW_KEY_TAB, keyboard::key::tab}};

        REQUIRE(keys.size() == text::EDITING_KEYS.size());

        for (const auto &[glfwKey, key] : keys) f.key(glfwKey, GLFW_PRESS);

        f.key(GLFW_KEY_W, GLFW_PRESS);
        f.key(GLFW_KEY_SPACE, GLFW_PRESS);
        f.key(GLFW_KEY_LEFT_SHIFT, GLFW_PRESS);

        f.input.poll();

        REQUIRE(f.events().size() == keys.size());

        for (std::size_t i = 0; i < keys.size(); ++i) {
            INFO("edit " << i);

            REQUIRE(edit_at(f.events(), i).key == keys[i].second);
            REQUIRE(text::is_editing_key(keys[i].second));
        }

        REQUIRE_FALSE(text::is_editing_key(keyboard::key::w));
    }

    SECTION("the modifiers held, each as itself") {
        f.key(GLFW_KEY_LEFT, GLFW_PRESS, 0);
        f.key(GLFW_KEY_LEFT, GLFW_PRESS, GLFW_MOD_SHIFT);
        f.key(GLFW_KEY_LEFT, GLFW_PRESS, GLFW_MOD_CONTROL);
        f.key(GLFW_KEY_LEFT, GLFW_PRESS, GLFW_MOD_ALT);
        f.key(GLFW_KEY_LEFT, GLFW_PRESS, GLFW_MOD_SUPER);
        f.key(GLFW_KEY_LEFT, GLFW_PRESS, GLFW_MOD_SHIFT | GLFW_MOD_CONTROL | GLFW_MOD_ALT | GLFW_MOD_SUPER);

        f.input.poll();

        REQUIRE(f.events().size() == 6);

        const auto held = [&f](const std::size_t aIndex) { return edit_at(f.events(), aIndex).held; };

        REQUIRE(held(0) == text::modifiers{});
        REQUIRE(held(1) == text::modifiers{true, false, false, false});
        REQUIRE(held(2) == text::modifiers{false, true, false, false});
        REQUIRE(held(3) == text::modifiers{false, false, true, false});
        REQUIRE(held(4) == text::modifiers{false, false, false, true});
        REQUIRE(held(5) == text::modifiers{true, true, true, true});
    }

    SECTION("text and edits in the order they happened") {
        f.type('a');
        f.type('b');
        f.key(GLFW_KEY_BACKSPACE, GLFW_PRESS);
        f.type('c');

        f.input.poll();

        REQUIRE(f.events().size() == 3);
        REQUIRE(text_at(f.events(), 0) == "ab");
        REQUIRE(edit_at(f.events(), 1).key == keyboard::key::backspace);
        REQUIRE(text_at(f.events(), 2) == "c");
    }
}

#if GDK_INPUT_GLFW_IME
TEST_CASE("the glfw context reads an input method's composition", "[text][glfw][ime]") {
    typing_fixture f;

    auto *const pWindow = f.input.pWindow.get();

    unsigned int composing[] = {0x306B, 0x307B, 0x3093, 0x3054};
    int blocks[] = {2, 2};

    f.callbacks.preedit(pWindow, 4, composing, 2, blocks, 1, 4);

    SECTION("in bytes of UTF-8, once the next update has been") {
        REQUIRE(f.input.pContext->text_composition().empty());

        f.input.poll();

        const auto composition = f.input.pContext->text_composition();

        REQUIRE(composition.text == "\xE3\x81\xAB\xE3\x81\xBB\xE3\x82\x93\xE3\x81\x94");
        REQUIRE(composition.caret == 12);
        REQUIRE(composition.focusBegin == 6);
        REQUIRE(composition.focusEnd == 12);

        SECTION("and it lasts until it changes") {
            f.input.poll();

            REQUIRE(f.input.pContext->text_composition() == composition);

            SECTION("to nothing, when the input method commits or abandons it") {
                f.callbacks.preedit(pWindow, 0, composing, 0, blocks, -1, 0);
                f.input.poll();

                REQUIRE(f.input.pContext->text_composition().empty());
            }
        }

        SECTION("and it goes when the field hands the keyboard back") {
            f.input.pContext->set_text_input_focus(true);
            f.input.pContext->set_text_input_focus(false);

            REQUIRE(f.input.pContext->text_composition().empty());

            f.input.poll();

            REQUIRE(f.input.pContext->text_composition().empty());
        }
    }

    SECTION("a caret or segment out of range is kept in range") {
        f.callbacks.preedit(pWindow, 4, composing, 2, blocks, 0, 9);
        f.input.poll();

        const auto composition = f.input.pContext->text_composition();

        REQUIRE(composition.caret == 12);
        REQUIRE(composition.focusBegin == 0);
        REQUIRE(composition.focusEnd == 6);

        f.callbacks.preedit(pWindow, 4, composing, 2, blocks, 5, -3);
        f.input.poll();

        REQUIRE(f.input.pContext->text_composition().caret == 0);
        REQUIRE(f.input.pContext->text_composition().focusEnd == 0);

        f.callbacks.preedit(pWindow, 4, nullptr, 2, nullptr, 0, 2);
        f.input.poll();

        REQUIRE(f.input.pContext->text_composition().empty());
    }
}
#endif

TEST_CASE("the glfw context shares the window's callbacks", "[text][glfw]") {
    auto pWindow = initGLFW();

    static std::vector<unsigned int> theirs;

    theirs.clear();

    const GLFWcharfun their_callback = [](GLFWwindow *, const unsigned int aCodePoint) {
        theirs.push_back(aCodePoint);
    };

    glfwSetCharCallback(pWindow.get(), their_callback);

    {
        const auto pContext = glfw_context::make(pWindow);

        installed_callbacks callbacks(pWindow.get());

        REQUIRE(callbacks.character != their_callback);

        callbacks.character(pWindow.get(), 'x');

        SECTION("calling the one installed before it") {
            REQUIRE(theirs == std::vector<unsigned int>{'x'});

            glfwPollEvents();
            std::static_pointer_cast<glfw_context>(pContext)->update();

            REQUIRE(text_at(pContext->text_events(), 0) == "x");
        }
    }

    SECTION("and giving it back when the context goes") {
        installed_callbacks callbacks(pWindow.get());

        REQUIRE(callbacks.character == their_callback);
    }
}

TEST_CASE("the glfw context's text input focus", "[text][glfw]") {
    input_fixture f;

    REQUIRE_FALSE(f.pContext->text_input_focus());

    f.pContext->set_text_input_focus(true);

    REQUIRE(f.pContext->text_input_focus());

    f.pContext->set_text_input_focus(false);

    REQUIRE_FALSE(f.pContext->text_input_focus());
}

#if GDK_INPUT_GLFW_IME
TEST_CASE("the glfw context gives the input method the caret in the window's pixels, from the top",
    "[text][glfw][ime]") {
    input_fixture f;

    int width, height;

    glfwGetWindowSize(f.pWindow.get(), &width, &height);

    REQUIRE(width == 320);
    REQUIRE(height == 240);

    f.pContext->set_text_input_caret({0.25, 0.5, 0.01, 0.05});

    int x, y, w, h;

    glfwGetPreeditCursorRectangle(f.pWindow.get(), &x, &y, &w, &h);

    REQUIRE(x == 80);
    REQUIRE(y == 108);
    REQUIRE(w == 3);
    REQUIRE(h == 12);
}
#endif

TEST_CASE("the glfw context withholds the keyboard while a field has it", "[text][glfw][keyboard]") {
    input_fixture f;

    auto *const pWindow = f.pWindow.get();

    auto pControls = controls::make(f.pContext);

    pControls->bind("walk", keyboard::key::w);

    input_glfw_key(pWindow, GLFW_KEY_W, GLFW_PRESS);
    f.poll();

    REQUIRE(f.pContext->key_just_pressed(keyboard::key::w));
    REQUIRE(pControls->down("walk"));

    f.pContext->set_text_input_focus(true);
    f.poll();

    SECTION("a key held reads as released, once, then up") {
        REQUIRE(f.pContext->key_just_released(keyboard::key::w));
        REQUIRE_FALSE(f.pContext->key_down(keyboard::key::w));
        REQUIRE_FALSE(pControls->down("walk"));

        f.poll();

        REQUIRE_FALSE(f.pContext->key_just_released(keyboard::key::w));
        REQUIRE_FALSE(f.pContext->any_key_down());
    }

    SECTION("a key pressed while it has the keyboard never reads as down, but types") {
        input_glfw_key(pWindow, GLFW_KEY_BACKSPACE, GLFW_PRESS);
        f.poll();

        REQUIRE_FALSE(f.pContext->key_down(keyboard::key::backspace));
        REQUIRE_FALSE(f.pContext->key_just_pressed(keyboard::key::backspace));
        REQUIRE(edit_at(f.pContext->text_events(), 0).key == keyboard::key::backspace);
    }

    SECTION("a key still down when it hands the keyboard back reads as up until let go") {
        input_glfw_key(pWindow, GLFW_KEY_ENTER, GLFW_PRESS);
        f.poll();

        f.pContext->set_text_input_focus(false);
        f.poll();

        REQUIRE_FALSE(f.pContext->key_down(keyboard::key::enter));
        REQUIRE_FALSE(f.pContext->key_just_pressed(keyboard::key::enter));
        REQUIRE_FALSE(f.pContext->key_down(keyboard::key::w));

        input_glfw_key(pWindow, GLFW_KEY_ENTER, GLFW_RELEASE);
        f.poll();

        REQUIRE_FALSE(f.pContext->key_just_released(keyboard::key::enter));

        SECTION("then it is the game's again") {
            input_glfw_key(pWindow, GLFW_KEY_ENTER, GLFW_PRESS);
            f.poll();

            REQUIRE(f.pContext->key_just_pressed(keyboard::key::enter));
        }
    }
}

TEST_CASE("the glfw context sees a key tapped between two polls", "[text][glfw][keyboard]") {
    input_fixture f;

    auto *const pWindow = f.pWindow.get();

    auto pControls = controls::make(f.pContext);

    pControls->bind("open_chat", keyboard::key::enter);

    f.poll();

    input_glfw_key(pWindow, GLFW_KEY_ENTER, GLFW_PRESS);
    input_glfw_key(pWindow, GLFW_KEY_ENTER, GLFW_RELEASE);
    f.poll();

    SECTION("pressed, and down, for the one poll; released the next") {
        REQUIRE(f.pContext->key_just_pressed(keyboard::key::enter));
        REQUIRE(f.pContext->key_down(keyboard::key::enter));
        REQUIRE(pControls->just_pressed("open_chat"));

        f.poll();

        REQUIRE(f.pContext->key_just_released(keyboard::key::enter));
        REQUIRE_FALSE(f.pContext->key_down(keyboard::key::enter));
        REQUIRE_FALSE(pControls->just_pressed("open_chat"));

        f.poll();

        REQUIRE_FALSE(f.pContext->key_just_released(keyboard::key::enter));
    }

    SECTION("while a field has the keyboard, a tap is withheld as a held key is, and types") {
        f.pContext->set_text_input_focus(true);
        f.poll();

        input_glfw_key(pWindow, GLFW_KEY_ENTER, GLFW_PRESS);
        input_glfw_key(pWindow, GLFW_KEY_ENTER, GLFW_RELEASE);
        f.poll();

        REQUIRE_FALSE(f.pContext->key_just_pressed(keyboard::key::enter));
        REQUIRE_FALSE(pControls->just_pressed("open_chat"));
        REQUIRE(edit_at(f.pContext->text_events(), 0).key == keyboard::key::enter);
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
