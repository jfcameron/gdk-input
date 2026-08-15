// © Joseph Cameron - All Rights Reserved

#include <gdk/glfw_gamepad.h>

#include <cassert>
#include <cmath>
#include <cstdio>
#include <cstring>
#include <iostream>
#include <map>
#include <vector>

using namespace gdk;

gamepad_glfw::gamepad_glfw(const int aJoystickIndex)
: m_JoystickIndex(aJoystickIndex) 
{}

float gamepad_glfw::axis(gamepad::index_type index, axis_value_type threshold) const {
    if (index >= m_Axes.size()) return 0;

    auto value = m_Axes[index];

    if (std::abs(value) < threshold) return 0;

    return value;
}

bool gamepad_glfw::axis_just_exceeded_threshold(const index_type index, const axis_value_type threshold) const {
    if (index >= m_Axes.size()) return false;

    const auto currentValue = m_Axes[index];

    const auto lastValue = index < m_LastAxes.size()
        ? m_LastAxes[index] : 0;

    if (currentValue >= threshold && lastValue < threshold) return true;
    
    return false;
}

bool gamepad_glfw::axis_just_dropped_below_threshold(const index_type index, const axis_value_type threshold) const {
    if (index >= m_Axes.size()) return false;

    const auto currentValue = m_Axes[index];

    const auto lastValue = index < m_LastAxes.size()
        ? m_LastAxes[index] : 0;

    if (currentValue <= threshold && lastValue > threshold) return true;

    return false;
}

bool gamepad_glfw::button_down(const gamepad::index_type index) const {
    if (index >= m_Buttons.size()) return false;

    return m_Buttons[index] != button_state::UP;
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
            if (auto val = axis(i, threshold)) return {{
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
    return glfwJoystickPresent(m_JoystickIndex);
}

void gamepad_glfw::update() {
    if (!glfwJoystickPresent(m_JoystickIndex)) {
        m_Buttons.clear();
        m_Axes.clear();
        m_LastAxes.clear();
        return;
    }

    if (const auto *pName = glfwGetJoystickName(m_JoystickIndex)) m_Name = pName;

    m_LastAxes = m_Axes;

    int axisCount = 0;
    const float *pAxes = glfwGetJoystickAxes(m_JoystickIndex, &axisCount);
    m_Axes = pAxes
        ? decltype(m_Axes)(pAxes, pAxes + axisCount)
        : decltype(m_Axes)();

    int buttonCount = 0;
    const unsigned char *pButtons = glfwGetJoystickButtons(m_JoystickIndex, &buttonCount);
    if (!pButtons) {
        m_Buttons.clear();
        return;
    }

    std::vector<decltype(GLFW_PRESS)> buttons(pButtons, pButtons + buttonCount);
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

