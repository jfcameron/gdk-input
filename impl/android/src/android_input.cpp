// © Joseph Cameron - All Rights Reserved

#include <gdk/input/android_input.h>

#include <gdk/input/gamepad.h>
#include <gdk/input/null_gamepad.h>
#include <gdk/input/text.h>

#include <android/input.h>
#include <android/keycodes.h>

#include <algorithm>
#include <optional>
#include <string>

using namespace gdk::input;

namespace {
    using keyboard::key;
    using pad = gamepad::button;

    /// \brief a keyboard's or a remote's key as gdk's, if it has one
    [[nodiscard]] std::optional<key> key_of(const std::int32_t aCode) {
        if (aCode >= AKEYCODE_A && aCode <= AKEYCODE_Z) {
            static constexpr key LETTERS[] = {key::a, key::b, key::c, key::d, key::e, key::f, key::g, key::h, key::i,
                key::j, key::k, key::l, key::m, key::n, key::o, key::p, key::q, key::r, key::s, key::t, key::u, key::v,
                key::w, key::x, key::y, key::z};

            return LETTERS[aCode - AKEYCODE_A];
        }

        if (aCode >= AKEYCODE_0 && aCode <= AKEYCODE_9) {
            static constexpr key DIGITS[] = {key::zero, key::one, key::two, key::three, key::four, key::five,
                key::six, key::seven, key::eight, key::nine};

            return DIGITS[aCode - AKEYCODE_0];
        }

        switch (aCode) {
            case AKEYCODE_DPAD_UP: return key::uparrow;
            case AKEYCODE_DPAD_DOWN: return key::downarrow;
            case AKEYCODE_DPAD_LEFT: return key::leftarrow;
            case AKEYCODE_DPAD_RIGHT: return key::rightarrow;
            case AKEYCODE_DPAD_CENTER: return key::enter;   // a remote's select
            case AKEYCODE_ENTER: return key::enter;
            case AKEYCODE_NUMPAD_ENTER: return key::numenter;
            case AKEYCODE_BACK: return key::escape;         // a remote's, or the phone's, back
            case AKEYCODE_ESCAPE: return key::escape;
            case AKEYCODE_DEL: return key::backspace;
            case AKEYCODE_FORWARD_DEL: return key::deletekey;
            case AKEYCODE_SPACE: return key::space;
            case AKEYCODE_TAB: return key::tab;
            case AKEYCODE_PAGE_UP: return key::pageup;
            case AKEYCODE_PAGE_DOWN: return key::pagedown;
            case AKEYCODE_MOVE_HOME: return key::home;
            case AKEYCODE_MOVE_END: return key::end;
            case AKEYCODE_INSERT: return key::insert;
            case AKEYCODE_MINUS: return key::minus;
            case AKEYCODE_EQUALS: return key::equals;
            case AKEYCODE_LEFT_BRACKET: return key::openbracket;
            case AKEYCODE_RIGHT_BRACKET: return key::closebracket;
            case AKEYCODE_BACKSLASH: return key::backslash;
            case AKEYCODE_SEMICOLON: return key::semicolon;
            case AKEYCODE_APOSTROPHE: return key::quote;
            case AKEYCODE_GRAVE: return key::tilda;
            case AKEYCODE_COMMA: return key::comma;
            case AKEYCODE_PERIOD: return key::period;
            case AKEYCODE_SLASH: return key::forwardslash;
            case AKEYCODE_SHIFT_LEFT: return key::leftshift;
            case AKEYCODE_SHIFT_RIGHT: return key::rightshift;
            case AKEYCODE_CTRL_LEFT: return key::leftcontrol;
            case AKEYCODE_CTRL_RIGHT: return key::rightcontrol;
            case AKEYCODE_ALT_LEFT: return key::leftalt;
            case AKEYCODE_ALT_RIGHT: return key::rightalt;
            case AKEYCODE_CAPS_LOCK: return key::capslock;
            case AKEYCODE_F1: return key::f1;
            case AKEYCODE_F2: return key::f2;
            case AKEYCODE_F3: return key::f3;
            case AKEYCODE_F4: return key::f4;
            case AKEYCODE_F5: return key::f5;
            case AKEYCODE_F6: return key::f6;
            case AKEYCODE_F7: return key::f7;
            case AKEYCODE_F8: return key::f8;
            case AKEYCODE_F9: return key::f9;
            case AKEYCODE_F10: return key::f10;
            case AKEYCODE_F11: return key::f11;
            case AKEYCODE_F12: return key::f12;

            // a remote's media and television keys; a headset's one button plays and pauses. The
            // volume keys aren't here: they're the system's, the television's volume
            case AKEYCODE_MEDIA_PLAY_PAUSE: return key::playpause;
            case AKEYCODE_HEADSETHOOK: return key::playpause;
            case AKEYCODE_MEDIA_PLAY: return key::play;
            case AKEYCODE_MEDIA_PAUSE: return key::pause;
            case AKEYCODE_MEDIA_STOP: return key::stop;
            case AKEYCODE_MEDIA_REWIND: return key::rewind;
            case AKEYCODE_MEDIA_SKIP_BACKWARD: return key::rewind;
            case AKEYCODE_MEDIA_FAST_FORWARD: return key::fastforward;
            case AKEYCODE_MEDIA_SKIP_FORWARD: return key::fastforward;
            case AKEYCODE_MEDIA_PREVIOUS: return key::previoustrack;
            case AKEYCODE_MEDIA_NEXT: return key::nexttrack;
            case AKEYCODE_CHANNEL_UP: return key::channelup;
            case AKEYCODE_CHANNEL_DOWN: return key::channeldown;
            case AKEYCODE_INFO: return key::info;
            case AKEYCODE_CAPTIONS: return key::captions;
            case AKEYCODE_MENU: return key::menu;
            case AKEYCODE_SEARCH: return key::search;
            default: return {};
        }
    }

    /// \brief a gamepad's button, on the standard layout
    [[nodiscard]] std::optional<pad> pad_button_of(const std::int32_t aCode) {
        switch (aCode) {
            case AKEYCODE_BUTTON_A: return pad::a;
            case AKEYCODE_BUTTON_B: return pad::b;
            case AKEYCODE_BUTTON_X: return pad::x;
            case AKEYCODE_BUTTON_Y: return pad::y;
            case AKEYCODE_BUTTON_L1: return pad::left_bumper;
            case AKEYCODE_BUTTON_R1: return pad::right_bumper;
            case AKEYCODE_BUTTON_SELECT: return pad::back;
            case AKEYCODE_BUTTON_START: return pad::start;
            case AKEYCODE_BUTTON_MODE: return pad::guide;
            case AKEYCODE_BUTTON_THUMBL: return pad::left_thumb;
            case AKEYCODE_BUTTON_THUMBR: return pad::right_thumb;
            case AKEYCODE_DPAD_UP: return pad::dpad_up;
            case AKEYCODE_DPAD_RIGHT: return pad::dpad_right;
            case AKEYCODE_DPAD_DOWN: return pad::dpad_down;
            case AKEYCODE_DPAD_LEFT: return pad::dpad_left;
            default: return {};
        }
    }

    /// \brief the character a printable key makes, as UTF-8: what an activity's key events say
    /// without Java's KeyCharacterMap, which is the US layout's ASCII
    [[nodiscard]] std::optional<std::string> text_of(const std::int32_t aCode, const bool aShift) {
        if (aCode >= AKEYCODE_A && aCode <= AKEYCODE_Z) return std::string(1, static_cast<char>((aShift ? 'A' : 'a') + (aCode - AKEYCODE_A)));

        if (aCode >= AKEYCODE_0 && aCode <= AKEYCODE_9) {
            static constexpr char SHIFTED[] = ")!@#$%^&*(";

            return std::string(1, aShift ? SHIFTED[aCode - AKEYCODE_0] : static_cast<char>('0' + (aCode - AKEYCODE_0)));
        }

        switch (aCode) {
            case AKEYCODE_SPACE: return " ";
            case AKEYCODE_MINUS: return aShift ? "_" : "-";
            case AKEYCODE_EQUALS: return aShift ? "+" : "=";
            case AKEYCODE_LEFT_BRACKET: return aShift ? "{" : "[";
            case AKEYCODE_RIGHT_BRACKET: return aShift ? "}" : "]";
            case AKEYCODE_BACKSLASH: return aShift ? "|" : "\\";
            case AKEYCODE_SEMICOLON: return aShift ? ":" : ";";
            case AKEYCODE_APOSTROPHE: return aShift ? "\"" : "'";
            case AKEYCODE_GRAVE: return aShift ? "~" : "`";
            case AKEYCODE_COMMA: return aShift ? "<" : ",";
            case AKEYCODE_PERIOD: return aShift ? ">" : ".";
            case AKEYCODE_SLASH: return aShift ? "?" : "/";
            case AKEYCODE_AT: return "@";
            case AKEYCODE_PLUS: return "+";
            case AKEYCODE_POUND: return "#";
            case AKEYCODE_STAR: return "*";
            default: return {};
        }
    }

    [[nodiscard]] bool from_gamepad(const AInputEvent *const apEvent) {
        const auto source = AInputEvent_getSource(apEvent);

        return (source & AINPUT_SOURCE_GAMEPAD) == AINPUT_SOURCE_GAMEPAD
            || (source & AINPUT_SOURCE_JOYSTICK) == AINPUT_SOURCE_JOYSTICK;
    }

    [[nodiscard]] gamepad::index_type index_of(const pad aButton) { return static_cast<gamepad::index_type>(aButton); }

    [[nodiscard]] gamepad::index_type index_of(const gamepad::axis aAxis) { return static_cast<gamepad::index_type>(aAxis); }
}

android_input::android_input(std::shared_ptr<null_context> apContext)
: m_pContext(std::move(apContext))
{}

std::optional<std::size_t> android_input::player_of(const std::int32_t aDevice) {
    if (const auto found = m_Players.find(aDevice); found != m_Players.end()) return found->second;

    const auto player = m_pContext->attach_gamepad("Android gamepad " + std::to_string(aDevice),
        "android-" + std::to_string(aDevice));

    if (player) m_Players[aDevice] = *player;

    return player;
}

bool android_input::feed(const AInputEvent *const apEvent, const std::pair<int, int> aWindowSize) {
    switch (AInputEvent_getType(apEvent)) {
        case AINPUT_EVENT_TYPE_KEY: return feed_key(apEvent);
        case AINPUT_EVENT_TYPE_MOTION: return feed_motion(apEvent, aWindowSize);
        default: return false;
    }
}

bool android_input::feed_key(const AInputEvent *const apEvent) {
    const auto code = AKeyEvent_getKeyCode(apEvent);
    const auto action = AKeyEvent_getAction(apEvent);
    const bool down = action == AKEY_EVENT_ACTION_DOWN;

    if (action != AKEY_EVENT_ACTION_DOWN && action != AKEY_EVENT_ACTION_UP) return false;

    // a gamepad's buttons, as its own: its d-pad too, where it sends keys for it
    if (from_gamepad(apEvent)) {
        if (const auto button = pad_button_of(code)) {
            if (const auto player = player_of(AInputEvent_getDeviceId(apEvent))) {
                const auto pGamepad = m_pContext->gamepad_at(*player);

                if (down) pGamepad->press(index_of(*button));
                else pGamepad->release(index_of(*button));

                return true;
            }
        }
    }

    const auto gdkKey = key_of(code);

    // the volume keys, the power button and the rest are the system's
    if (!gdkKey) return false;

    const bool repeat = AKeyEvent_getRepeatCount(apEvent) > 0;

    if (down && !repeat) m_pContext->press_key(*gdkKey);
    else if (!down) m_pContext->release_key(*gdkKey);

    // text and editing, as a field with focus takes them
    if (down && m_pContext->text_input_focus()) {
        const auto meta = AKeyEvent_getMetaState(apEvent);

        text::modifiers held;

        held.shift = meta & AMETA_SHIFT_ON;
        held.control = meta & AMETA_CTRL_ON;
        held.alt = meta & AMETA_ALT_ON;

        if (text::is_editing_key(*gdkKey) || (held.control && text::is_shortcut_key(*gdkKey)))
            m_pContext->press_edit({.key = *gdkKey, .held = held, .repeat = repeat});
        else if (!held.control && !held.alt)
            if (const auto typed = text_of(code, held.shift)) m_pContext->type_text(*typed);
    }

    return true;
}

bool android_input::feed_motion(const AInputEvent *const apEvent, const std::pair<int, int> aWindowSize) {
    const auto action = AMotionEvent_getAction(apEvent) & AMOTION_EVENT_ACTION_MASK;

    // a gamepad's sticks, triggers and hat, on GLFW's scales: sticks -1 to 1, y down; triggers -1
    // at rest to 1; the hat as the d-pad
    if (from_gamepad(apEvent) && action == AMOTION_EVENT_ACTION_MOVE) {
        const auto player = player_of(AInputEvent_getDeviceId(apEvent));

        if (!player) return false;

        const auto pGamepad = m_pContext->gamepad_at(*player);
        const auto axis = [apEvent](const std::int32_t aAxis) { return AMotionEvent_getAxisValue(apEvent, aAxis, 0); };
        const auto trigger = [](const float aValue) { return aValue * 2 - 1; };

        pGamepad->set_axis(index_of(gamepad::axis::left_x), axis(AMOTION_EVENT_AXIS_X));
        pGamepad->set_axis(index_of(gamepad::axis::left_y), axis(AMOTION_EVENT_AXIS_Y));
        pGamepad->set_axis(index_of(gamepad::axis::right_x), axis(AMOTION_EVENT_AXIS_Z));
        pGamepad->set_axis(index_of(gamepad::axis::right_y), axis(AMOTION_EVENT_AXIS_RZ));
        pGamepad->set_axis(index_of(gamepad::axis::left_trigger),
            trigger(std::max(axis(AMOTION_EVENT_AXIS_LTRIGGER), axis(AMOTION_EVENT_AXIS_BRAKE))));
        pGamepad->set_axis(index_of(gamepad::axis::right_trigger),
            trigger(std::max(axis(AMOTION_EVENT_AXIS_RTRIGGER), axis(AMOTION_EVENT_AXIS_GAS))));

        const auto hatX = axis(AMOTION_EVENT_AXIS_HAT_X), hatY = axis(AMOTION_EVENT_AXIS_HAT_Y);
        const auto hat = [&pGamepad](const pad aButton, const bool aDown) {
            if (aDown) pGamepad->press(index_of(aButton));
            else pGamepad->release(index_of(aButton));
        };

        hat(pad::dpad_left, hatX < -0.5f);
        hat(pad::dpad_right, hatX > 0.5f);
        hat(pad::dpad_up, hatY < -0.5f);
        hat(pad::dpad_down, hatY > 0.5f);

        return true;
    }

    const auto width = static_cast<double>(std::max(1, aWindowSize.first));
    const auto height = static_cast<double>(std::max(1, aWindowSize.second));

    // where, normalised as gdk's cursor is: 0,0 bottom left, 1,1 top right
    const auto place = [&]() {
        const mouse::cursor_2d_type position{AMotionEvent_getX(apEvent, 0) / width, 1 - AMotionEvent_getY(apEvent, 0) / height};

        m_pContext->set_mouse_delta({(position.x - m_Cursor.x) * width, (position.y - m_Cursor.y) * height});
        m_pContext->set_cursor_position(position);
        m_Cursor = position;
    };

    switch (action) {
        case AMOTION_EVENT_ACTION_DOWN:
            place();
            m_pContext->press_mouse_button(mouse::button::left);
            return true;

        case AMOTION_EVENT_ACTION_MOVE:
        case AMOTION_EVENT_ACTION_HOVER_MOVE:
            place();
            return true;

        case AMOTION_EVENT_ACTION_UP:
        case AMOTION_EVENT_ACTION_CANCEL:
            place();
            m_pContext->release_mouse_button(mouse::button::left);
            return true;

        case AMOTION_EVENT_ACTION_SCROLL:
            m_pContext->set_scroll_delta({AMotionEvent_getAxisValue(apEvent, AMOTION_EVENT_AXIS_HSCROLL, 0),
                AMotionEvent_getAxisValue(apEvent, AMOTION_EVENT_AXIS_VSCROLL, 0)});
            return true;

        default:
            return false;
    }
}
