// © Joseph Cameron - All Rights Reserved

#include <gdk/glfw_mouse.h>

#include <GLFW/glfw3.h>

#include <array>
#include <iostream>
#include <stdexcept>
#include <string>
#include <set>

static constexpr char TAG[] = "mouse";

using namespace gdk;

static inline decltype(GLFW_MOUSE_BUTTON_1) glfwmouseButtonFromButton(const gdk::mouse::button a) {
    switch(a) {
        case gdk::mouse::button::left: return GLFW_MOUSE_BUTTON_LEFT;
        case gdk::mouse::button::right: return GLFW_MOUSE_BUTTON_RIGHT;
        case gdk::mouse::button::middle: return GLFW_MOUSE_BUTTON_MIDDLE;
        case gdk::mouse::button::four: return GLFW_MOUSE_BUTTON_4;
        case gdk::mouse::button::five: return GLFW_MOUSE_BUTTON_5;
        case gdk::mouse::button::six: return GLFW_MOUSE_BUTTON_6;
        case gdk::mouse::button::seven: return GLFW_MOUSE_BUTTON_7;
        case gdk::mouse::button::eight: return GLFW_MOUSE_BUTTON_8;

		default: throw std::invalid_argument(std::string("Unable to convert mouse button \"")
			.append(std::to_string(static_cast<std::underlying_type< decltype(a)>::type>(a)))
			.append("\" to GLFW_MOUSE_BUTTON"));
    }
}

mouse_glfw::mouse_glfw(decltype(m_pWindow) pWindow)
: m_pWindow(pWindow)
, m_LastDeltaCallCursorPosition(cursor_position())
{}

void mouse_glfw::set_cursor_mode(const mouse::cursor_mode acursor_mode) {
    decltype(GLFW_CURSOR_NORMAL) cursorModeBuffer;

    switch (acursor_mode) {
        case mouse::cursor_mode::normal: cursorModeBuffer = GLFW_CURSOR_NORMAL; break;
        case mouse::cursor_mode::hidden: cursorModeBuffer = GLFW_CURSOR_HIDDEN; break;
        case mouse::cursor_mode::locked: cursorModeBuffer = GLFW_CURSOR_DISABLED; break;

        default: throw std::invalid_argument(std::string("Unhandled cursor_mode: ")
            .append(std::to_string(static_cast<std::underlying_type<decltype(acursor_mode)>::type>(acursor_mode))));
    }

    glfwSetInputMode(m_pWindow.get(), GLFW_CURSOR, cursorModeBuffer);

    m_cursor_mode = acursor_mode;
}

mouse::cursor_mode mouse_glfw::get_cursor_mode() const {
    return m_cursor_mode;    
}

mouse::cursor_2d_type mouse_glfw::cursor_position() const {
	int width, height;
	glfwGetWindowSize(m_pWindow.get(), &width, &height);

	double x, y;
    glfwGetCursorPos(m_pWindow.get(), &x, &y);

	x = x / width;
	y = y / height * -1 + 1;

	return { x, y };
}

mouse::cursor_2d_type mouse_glfw::delta() const {
    return m_Delta;
}

bool mouse_glfw::button_down(const mouse::button aButton) const {
    bool value(false);

    if (auto search = m_CurrentState.find(glfwmouseButtonFromButton(aButton)); search != m_CurrentState.end())
        value = search->second == gdk::mouse::button_state::held_down ||
        search->second == gdk::mouse::button_state::just_pressed;
	
    return value;
}

bool mouse_glfw::button_just_down(const mouse::button aKeyCode) const {
    bool value(false);

    if (auto search = m_CurrentState.find(glfwmouseButtonFromButton(aKeyCode)); search != m_CurrentState.end())
        value = search->second == gdk::mouse::button_state::just_pressed;

    return value;
}

bool mouse_glfw::button_just_released(const mouse::button aKeyCode) const {
    bool value(false);

    if (auto search = m_CurrentState.find(glfwmouseButtonFromButton(aKeyCode)); search != m_CurrentState.end())
        value = search->second == gdk::mouse::button_state::just_released;

    return value;
}

void mouse_glfw::update() {
    static constexpr std::array<decltype(GLFW_MOUSE_BUTTON_LEFT), 8> BUTTON_SET{
        GLFW_MOUSE_BUTTON_LEFT,
        GLFW_MOUSE_BUTTON_RIGHT,
        GLFW_MOUSE_BUTTON_MIDDLE,
        GLFW_MOUSE_BUTTON_4,
        GLFW_MOUSE_BUTTON_5,
        GLFW_MOUSE_BUTTON_6,
        GLFW_MOUSE_BUTTON_7,
        GLFW_MOUSE_BUTTON_8,
    };

    for (const auto glfwButton : BUTTON_SET) {
        auto lastButtonState = m_MouseButtonLastState[glfwButton];
        auto currentButtonState = glfwGetMouseButton(m_pWindow.get(), glfwButton);
        auto newState = mouse::button_state::up;

        if (currentButtonState == GLFW_RELEASE && lastButtonState != GLFW_RELEASE)
            newState = mouse::button_state::just_released;
        else if (currentButtonState != GLFW_RELEASE)
            newState = lastButtonState == GLFW_RELEASE
            ? newState = mouse::button_state::just_pressed
            : newState = mouse::button_state::held_down;

        m_CurrentState[glfwButton] = newState;
        m_MouseButtonLastState[glfwButton] = currentButtonState;
    }

    auto currentCursorPosition = cursor_position();
    m_Delta.x = currentCursorPosition.x - m_LastDeltaCallCursorPosition.x;
    m_Delta.y = currentCursorPosition.y - m_LastDeltaCallCursorPosition.y;
    m_LastDeltaCallCursorPosition = currentCursorPosition;
}

