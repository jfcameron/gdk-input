// © Joseph Cameron - All Rights Reserved

#include <gdk/input/impl_joysticks.h>

#include <GLFW/glfw3.h>

#ifdef __EMSCRIPTEN__
#include <emscripten/html5.h>
#endif

#include <array>
#include <cstring>

using namespace gdk::input;

#ifdef __EMSCRIPTEN__

namespace {
    bool sampled = false;

    [[nodiscard]] bool status_of(const int aJoystick, EmscriptenGamepadEvent &aInto) {
        if (aJoystick < 0 || !sampled) return false;

        return emscripten_get_gamepad_status(aJoystick, &aInto) == EMSCRIPTEN_RESULT_SUCCESS && aInto.connected;
    }

    constexpr std::array<int, 15> STANDARD_BUTTONS{0, 1, 2, 3, 4, 5, 8, 9, 16, 10, 11, 12, 15, 13, 14};

    constexpr int STANDARD_LEFT_TRIGGER = 6, STANDARD_RIGHT_TRIGGER = 7;
}

std::size_t gdk::input::joystick_count() {
    if (!sampled) return 0;

    const auto count = emscripten_get_num_gamepads();

    return count > 0 ? static_cast<std::size_t>(count) : 0;
}

void gdk::input::sample_joysticks() {
    sampled = emscripten_sample_gamepad_data() == EMSCRIPTEN_RESULT_SUCCESS;
}

bool gdk::input::joystick_present(const int aJoystick) {
    EmscriptenGamepadEvent status;

    return status_of(aJoystick, status);
}

std::string gdk::input::joystick_guid(const int aJoystick) {
    EmscriptenGamepadEvent status;

    if (!status_of(aJoystick, status)) return {};

    return status.id;
}

joystick_reading gdk::input::read_joystick(const int aJoystick) {
    joystick_reading out;

    EmscriptenGamepadEvent status;

    if (!status_of(aJoystick, status)) return out;

    out.present = true;
    out.name = status.id;
    out.standard = std::strcmp(status.mapping, "standard") == 0;

    const auto axis = [&status](const int aIndex) {
        return aIndex < status.numAxes ? static_cast<float>(status.axis[aIndex]) : 0.f;
    };

    const auto digital = [&status](const int aIndex) {
        return aIndex < status.numButtons && status.digitalButton[aIndex] ? GLFW_PRESS : GLFW_RELEASE;
    };

    const auto analog = [&status](const int aIndex) {
        return aIndex < status.numButtons ? static_cast<float>(status.analogButton[aIndex]) : 0.f;
    };

    if (out.standard) {
        out.axes = {axis(0), axis(1), axis(2), axis(3),
            analog(STANDARD_LEFT_TRIGGER) * 2 - 1, analog(STANDARD_RIGHT_TRIGGER) * 2 - 1};

        for (const auto button : STANDARD_BUTTONS) out.buttons.push_back(static_cast<unsigned char>(digital(button)));

        return out;
    }

    for (int i = 0; i < status.numAxes; ++i) out.axes.push_back(axis(i));

    for (int i = 0; i < status.numButtons; ++i) out.buttons.push_back(static_cast<unsigned char>(digital(i)));

    return out;
}

bool gdk::input::update_gamepad_mappings(const std::string &) {
    return false;
}

#else

std::size_t gdk::input::joystick_count() {
    return GLFW_JOYSTICK_LAST + 1;
}

void gdk::input::sample_joysticks() {}

bool gdk::input::joystick_present(const int aJoystick) {
    return aJoystick >= 0 && aJoystick <= GLFW_JOYSTICK_LAST && glfwJoystickPresent(aJoystick);
}

std::string gdk::input::joystick_guid(const int aJoystick) {
    const auto *const pGuid = joystick_present(aJoystick) ? glfwGetJoystickGUID(aJoystick) : nullptr;

    return pGuid ? pGuid : "";
}

joystick_reading gdk::input::read_joystick(const int aJoystick) {
    joystick_reading out;

    if (!joystick_present(aJoystick)) return out;

    out.present = true;
    out.standard = glfwJoystickIsGamepad(aJoystick);

    if (out.standard) {
        GLFWgamepadstate state{};

        if (!glfwGetGamepadState(aJoystick, &state)) {
            out.present = false;

            return out;
        }

        if (const auto *pName = glfwGetGamepadName(aJoystick)) out.name = pName;

        out.axes.assign(std::begin(state.axes), std::end(state.axes));
        out.buttons.assign(std::begin(state.buttons), std::end(state.buttons));

        return out;
    }

    if (const auto *pName = glfwGetJoystickName(aJoystick)) out.name = pName;

    int axisCount = 0;
    const float *pAxes = glfwGetJoystickAxes(aJoystick, &axisCount);
    if (pAxes) out.axes.assign(pAxes, pAxes + axisCount);

    int hatCount = 0;
    const unsigned char *pHats = glfwGetJoystickHats(aJoystick, &hatCount);
    if (pHats) out.hats.assign(pHats, pHats + hatCount);

    int buttonCount = 0;
    const unsigned char *pButtons = glfwGetJoystickButtons(aJoystick, &buttonCount);
    if (pButtons) out.buttons.assign(pButtons, pButtons + buttonCount);

    return out;
}

bool gdk::input::update_gamepad_mappings(const std::string &aMappings) {
    return glfwUpdateGamepadMappings(aMappings.c_str()) == GLFW_TRUE;
}

#endif
