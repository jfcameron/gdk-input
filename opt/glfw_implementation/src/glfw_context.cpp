#include<gdk/glfw_context.h>

#include <iostream>

using namespace gdk;
using namespace gdk::input;

glfw_context::glfw_context(GLFWwindow *const apGLFWWindow)
	: m_pGLFWWindow(apGLFWWindow)
	, m_Keyboard(apGLFWWindow)
	, m_Mouse(apGLFWWindow)
{
	for (decltype(GLFW_JOYSTICK_1) i(0); i < m_Gamepads.size(); ++i)
		m_Gamepads[i] = glfw_gamepad_ptr(new gamepad_glfw(i));
}

bool glfw_context::get_key_down(const keyboard::Key& aKeyCode) const
{
	return m_Keyboard.getKeyDown(aKeyCode);
}

bool glfw_context::get_key_just_pressed(const keyboard::Key& aKeyCode) const
{
	return m_Keyboard.getKeyJustDown(aKeyCode);
}

bool glfw_context::get_key_just_released(const keyboard::Key& aKeyCode) const
{
	return m_Keyboard.getKeyJustReleased(aKeyCode);
}

bool glfw_context::get_mouse_button_down(const mouse::Button& aButton) const
{
	return m_Mouse.getButtonDown(aButton);
}

bool glfw_context::get_mouse_button_just_pressed(const mouse::Button& aButton) const
{
	return m_Mouse.getButtonJustDown(aButton);
}

bool glfw_context::get_mouse_button_just_released(const mouse::Button& aButton) const
{
	return m_Mouse.getButtonJustReleased(aButton);
}

mouse::cursor_2d_type glfw_context::get_mouse_cursor_position() const
{
	return m_Mouse.getCursorPosition();
}

mouse::CursorMode glfw_context::get_mouse_cursor_mode() const
{
	return m_Mouse.getCursorMode();
}

void glfw_context::set_mouse_cursor_mode(mouse::CursorMode mode)
{
	m_Mouse.setCursorMode(mode);
}

mouse::cursor_2d_type glfw_context::get_mouse_delta() const
{
	return m_Mouse.getDelta();
}

void glfw_context::set_mouse_delta_sensitivity(const double sens)
{
	m_Mouse.setDeltaSensitivity(sens);
}

context::gamepad_ptr glfw_context::get_gamepad(const size_t index)
{
	if (index > m_Gamepads.size()) throw std::invalid_argument("index exceeds maximum number of gamepads: " +
		std::to_string(index) + "/" + std::to_string(m_Gamepads.size()));

	return std::static_pointer_cast<gamepad>(m_Gamepads[index]);
}

/*size_t glfw_context::get_gamepad_maximum_count() const
{
	return m_Gamepads.size();
}*/

void glfw_context::update()
{
	m_Keyboard.update();

	m_Mouse.update();

	for (auto& a : m_Gamepads) a->update();
}
