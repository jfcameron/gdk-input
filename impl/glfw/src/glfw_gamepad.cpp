// © Joseph Cameron - All Rights Reserved

#include <gdk/input/impl_gamepad_mappings.h>

#include <gdk/input/impl_glfw_gamepad.h>

#include <cassert>
#include <cmath>
#include <cstdio>
#include <cstring>
#include <iostream>
#include <map>
#include <vector>

using namespace gdk::input;

gamepad_glfw::gamepad_glfw() = default;

void gamepad_glfw::attach(const int aJoystickIndex, const std::string &aGuid) {
    m_JoystickIndex = aJoystickIndex;
    m_LastJoystickIndex = aJoystickIndex;
    m_Guid = aGuid;
    m_JustConnected = true;
    m_JustDisconnected = false;
}

void gamepad_glfw::detach() {
    const bool wasConnected = m_JoystickIndex.has_value();

    m_JoystickIndex.reset();
    m_JustConnected = false;
    m_JustDisconnected = wasConnected;

    m_Buttons.clear();
    m_Axes.clear();
    m_LastAxes.clear();
    m_Hats.clear();
    m_HasStandardMapping = false;
}

std::optional<int> gamepad_glfw::joystick_index() const { return m_JoystickIndex; }

std::optional<int> gamepad_glfw::last_joystick_index() const { return m_LastJoystickIndex; }

std::string_view gamepad_glfw::guid() const { return m_Guid; }

bool gamepad_glfw::just_connected() const { return m_JustConnected; }

bool gamepad_glfw::just_disconnected() const { return m_JustDisconnected; }

float gamepad_glfw::axis_value(gamepad::index_type index, axis_value_type threshold) const {
    if (index >= m_Axes.size()) return 0;

    auto value = m_Axes[index];

    if (std::abs(value) < threshold) return 0;

    return value;
}

gamepad_glfw::axis_value_type gamepad_glfw::previous_axis_value(const index_type index,
    const axis_value_type threshold) const {
    if (index >= m_LastAxes.size()) return 0;

    const auto value = m_LastAxes[index];

    if (std::abs(value) < threshold) return 0;

    return value;
}

bool gamepad_glfw::axis_just_exceeded_threshold(const index_type index, const axis_value_type threshold) const {
    if (index >= m_Axes.size()) return false;

    const auto lastValue = index < m_LastAxes.size() ? m_LastAxes[index] : 0;

    return std::abs(m_Axes[index]) >= threshold && std::abs(lastValue) < threshold;
}

bool gamepad_glfw::axis_just_dropped_below_threshold(const index_type index, const axis_value_type threshold) const {
    if (index >= m_Axes.size()) return false;

    const auto lastValue = index < m_LastAxes.size() ? m_LastAxes[index] : 0;

    return std::abs(m_Axes[index]) < threshold && std::abs(lastValue) >= threshold;
}

bool gamepad_glfw::button_down(const gamepad::index_type index) const {
    if (index >= m_Buttons.size()) return false;

    return m_Buttons[index] == button_state::HELD_DOWN
        || m_Buttons[index] == button_state::JUST_PRESSED;
}

bool gamepad_glfw::button_just_pressed(const index_type index) const {
    if (index >= m_Buttons.size()) return false;

    return m_Buttons[index] == button_state::JUST_PRESSED;
}

bool gamepad_glfw::button_just_released(const index_type index) const {
    if (index >= m_Buttons.size()) return false;

    return m_Buttons[index] == button_state::JUST_RELEASED;
}

std::optional<gamepad::button_collection_type::size_type> gamepad_glfw::any_button_down() const {
    for (decltype(m_Buttons)::size_type i(0); i < m_Buttons.size(); ++i)
        if (m_Buttons[i] != button_state::UP) return i;

    return {};
}

std::optional<std::pair<gamepad::index_type, gamepad_glfw::axis_value_type>> gamepad_glfw::any_axis_down(axis_value_type threshold) const {
    for (decltype(m_Axes)::size_type i(0); i < m_Axes.size(); ++i) {
        if (m_Axes[i]) {
            if (auto val = axis_value(i, threshold)) return {{
                i,
                val
            }};
        }
    }

    return {};
}

std::string_view gamepad_glfw::name() const {
    return m_Name;
}

bool gamepad_glfw::connected() const {
    return m_JoystickIndex && glfwJoystickPresent(*m_JoystickIndex);
}

bool gamepad_glfw::has_standard_mapping() const { return m_HasStandardMapping; }

std::size_t gamepad_glfw::button_count() const { return m_Buttons.size(); }

std::size_t gamepad_glfw::axis_count() const { return m_Axes.size(); }

std::size_t gamepad_glfw::hat_count() const { return m_Hats.size(); }

gamepad::hat_state_type gamepad_glfw::hat(const index_type index) const {
    if (index >= m_Hats.size()) return {};

    return m_Hats[index];
}

namespace {
    [[nodiscard]] gamepad::hat_state_type to_hat_state(const unsigned char aGLFWHat) {
        gamepad::hat_state_type out;

        if (aGLFWHat & GLFW_HAT_LEFT)  out.x = gamepad::hat_state_type::horizontal::left;
        if (aGLFWHat & GLFW_HAT_RIGHT) out.x = gamepad::hat_state_type::horizontal::right;
        if (aGLFWHat & GLFW_HAT_UP)    out.y = gamepad::hat_state_type::vertical::up;
        if (aGLFWHat & GLFW_HAT_DOWN)  out.y = gamepad::hat_state_type::vertical::down;

        return out;
    }
}

void gamepad_glfw::update() {
    const bool wasJustConnected = m_JustConnected;
    const bool wasJustDisconnected = m_JustDisconnected;

    if (wasJustDisconnected) m_JustDisconnected = false;

    if (!m_JoystickIndex || !glfwJoystickPresent(*m_JoystickIndex)) {
        m_Buttons.clear();
        m_Axes.clear();
        m_LastAxes.clear();
        m_Hats.clear();
        m_HasStandardMapping = false;
        return;
    }

    if (wasJustConnected) m_JustConnected = false;

    const int joystick = *m_JoystickIndex;

    m_LastAxes = m_Axes;

    m_HasStandardMapping = glfwJoystickIsGamepad(joystick);

    std::vector<decltype(GLFW_PRESS)> buttons;

    if (m_HasStandardMapping) {
        GLFWgamepadstate state{};

        if (glfwGetGamepadState(joystick, &state)) {
            if (const auto *pName = glfwGetGamepadName(joystick)) m_Name = pName;

            m_Axes.assign(std::begin(state.axes), std::end(state.axes));

            for (std::size_t i = 0; i < m_Axes.size(); ++i)
                if (is_standard_trigger(i)) m_Axes[i] = standard_trigger_value(m_Axes[i]);

            buttons.assign(std::begin(state.buttons), std::end(state.buttons));

            m_Hats.clear();
        }
    }
    else {
        if (const auto *pName = glfwGetJoystickName(joystick)) m_Name = pName;

        int axisCount = 0;
        const float *pAxes = glfwGetJoystickAxes(joystick, &axisCount);
        m_Axes = pAxes
            ? decltype(m_Axes)(pAxes, pAxes + axisCount)
            : decltype(m_Axes)();

        int hatCount = 0;
        const unsigned char *pHats = glfwGetJoystickHats(joystick, &hatCount);
        m_Hats.clear();
        if (pHats) for (int i = 0; i < hatCount; ++i) m_Hats.push_back(to_hat_state(pHats[i]));

        int buttonCount = 0;
        const unsigned char *pButtons = glfwGetJoystickButtons(joystick, &buttonCount);
        if (!pButtons) {
            m_Buttons.clear();
            return;
        }

        buttons.assign(pButtons, pButtons + buttonCount);
    }

    if (m_Buttons.size() == buttons.size()) {
        for (size_t i(0); i < buttons.size(); ++i) {
            switch (m_Buttons[i])
            {
            case button_state::HELD_DOWN: 
                m_Buttons[i] = buttons[i]
                    ? button_state::HELD_DOWN
                    : button_state::JUST_RELEASED;
            break;

            case button_state::UP: 
                m_Buttons[i] = buttons[i]
                    ? button_state::JUST_PRESSED
                    : button_state::UP;
            break;
            
            case button_state::JUST_PRESSED: 
                m_Buttons[i] = buttons[i]
                    ? button_state::HELD_DOWN
                    : button_state::JUST_RELEASED;
            break;
            
            case button_state::JUST_RELEASED: 
                m_Buttons[i] = buttons[i]
                    ? button_state::JUST_PRESSED
                    : button_state::UP;
            break;
            }
        }
    }
    else {
        m_Buttons = std::vector<button_state>(buttons.size());
    }
}

