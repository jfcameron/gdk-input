// © 2019 Joseph Cameron - All Rights Reserved

#include <gdk/mouse_glfw.h>

#include <GLFW/glfw3.h>

#include <iostream>
#include <stdexcept>
#include <string>
#include <set>

static constexpr char TAG[] = "mouse";

using namespace gdk;

static const std::set<decltype(GLFW_MOUSE_BUTTON_LEFT)> BUTTON_SET{
    GLFW_MOUSE_BUTTON_LEFT,
    GLFW_MOUSE_BUTTON_RIGHT,
    GLFW_MOUSE_BUTTON_MIDDLE,
    GLFW_MOUSE_BUTTON_4,
    GLFW_MOUSE_BUTTON_5,
    GLFW_MOUSE_BUTTON_6,
    GLFW_MOUSE_BUTTON_7,
    GLFW_MOUSE_BUTTON_8,
};

static inline decltype(GLFW_MOUSE_BUTTON_1) glfwmouseButtonFromButton(const gdk::mouse::Button a)
{
    switch(a)
    {
        case gdk::mouse::Button::Left: return GLFW_MOUSE_BUTTON_LEFT;
        case gdk::mouse::Button::Right: return GLFW_MOUSE_BUTTON_RIGHT;
        case gdk::mouse::Button::Middle: return GLFW_MOUSE_BUTTON_MIDDLE;
        case gdk::mouse::Button::Four: return GLFW_MOUSE_BUTTON_4;
        case gdk::mouse::Button::Five: return GLFW_MOUSE_BUTTON_5;
        case gdk::mouse::Button::Six: return GLFW_MOUSE_BUTTON_6;
        case gdk::mouse::Button::Seven: return GLFW_MOUSE_BUTTON_7;
        case gdk::mouse::Button::Eight: return GLFW_MOUSE_BUTTON_8;

		default: throw std::invalid_argument(std::string("Unable to convert mouse button \"")
			.append(std::to_string(static_cast<std::underlying_type< decltype(a)>::type>(a)))
			.append("\" to GLFW_MOUSE_BUTTON"));
    }
}

mouse_glfw::mouse_glfw(decltype(m_pWindow) pWindow)
    : m_pWindow(pWindow)
    , m_LastDeltaCallCursorPosition(getCursorPosition())
{}

void mouse_glfw::setCursorMode(const CursorMode aCursorMode)
{
    decltype(GLFW_CURSOR_NORMAL) cursorModeBuffer;

    switch (aCursorMode)
    {
        case CursorMode::Normal: cursorModeBuffer = GLFW_CURSOR_NORMAL; break;
        case CursorMode::Hidden: cursorModeBuffer = GLFW_CURSOR_HIDDEN; break;
        case CursorMode::Locked: cursorModeBuffer = GLFW_CURSOR_DISABLED; break;

        default: throw std::invalid_argument(std::string("Unhandled CursorMode: ")
            .append(std::to_string(static_cast<std::underlying_type<decltype(aCursorMode)>::type>(aCursorMode))));
    }

    glfwSetInputMode(m_pWindow, GLFW_CURSOR, cursorModeBuffer);

    m_CursorMode = aCursorMode;
}

mouse::CursorMode mouse_glfw::getCursorMode() const
{
    return m_CursorMode;    
}

mouse::cursor_2d_type mouse_glfw::getCursorPosition() const
{
	int width, height;
	glfwGetWindowSize(m_pWindow, &width, &height);

	double x, y;
    glfwGetCursorPos(m_pWindow, &x, &y);

	x = x / width;
	y = y / height * -1 + 1;

	//x = x > 1 ? 1 : x < 0 ? 0 : x;
	//y = y > 1 ? 1 : y < 0 ? 0 : y;

	return { x, y };
}

mouse::cursor_2d_type mouse_glfw::getDelta() const
{
    return m_Delta;
}

void mouse_glfw::setDeltaSensitivity(double sens)
{
	m_DeltaSensitivity = sens;
}

bool mouse_glfw::getButtonDown(const mouse::Button aButton) const
{
    bool value(false);

    if (auto search = m_CurrentState.find(glfwmouseButtonFromButton(aButton)); search != m_CurrentState.end())
        value = search->second == gdk::mouse::ButtonState::HELD_DOWN ||
        search->second == gdk::mouse::ButtonState::JUST_PRESSED;
	
    return value;
}

bool mouse_glfw::getButtonJustDown(const Button aKeyCode) const
{
    bool value(false);

    if (auto search = m_CurrentState.find(glfwmouseButtonFromButton(aKeyCode)); search != m_CurrentState.end())
        value = search->second == gdk::mouse::ButtonState::JUST_PRESSED;

    return value;
}

bool mouse_glfw::getButtonJustReleased(const Button aKeyCode) const
{
    bool value(false);

    if (auto search = m_CurrentState.find(glfwmouseButtonFromButton(aKeyCode)); search != m_CurrentState.end())
        value = search->second == gdk::mouse::ButtonState::JUST_RELEASED;

    return value;
}

void mouse_glfw::update()
{
    // update button maps
    for (const auto glfwButton : BUTTON_SET)
    {
        auto lastButtonState = m_MouseButtonLastState[glfwButton];

        auto currentButtonState = glfwGetMouseButton(m_pWindow, glfwButton);

        auto newState = mouse::ButtonState::UP;

        if (currentButtonState == GLFW_RELEASE && lastButtonState != GLFW_RELEASE)
            newState = mouse::ButtonState::JUST_RELEASED;

        else if (currentButtonState != GLFW_RELEASE)
            newState = lastButtonState == GLFW_RELEASE
            ? newState = mouse::ButtonState::JUST_PRESSED
            : newState = mouse::ButtonState::HELD_DOWN;

        m_CurrentState[glfwButton] = newState;

        m_MouseButtonLastState[glfwButton] = currentButtonState;
    }

    // update cursor position
    auto currentCursorPosition = getCursorPosition();

    m_Delta.x = currentCursorPosition.x - m_LastDeltaCallCursorPosition.x * m_DeltaSensitivity;
    m_Delta.y = currentCursorPosition.y - m_LastDeltaCallCursorPosition.y * m_DeltaSensitivity;

    m_LastDeltaCallCursorPosition = currentCursorPosition;
}
