#include<gdk/glfw_context.h>

#include <iostream>
#include <set>

using namespace gdk;
using namespace gdk::input;

static const std::set<mouse::Button> MOUSE_BUTTONS{
	mouse::Button::Left,
	mouse::Button::Right,
	mouse::Button::Middle,
	mouse::Button::Four,
	mouse::Button::Five,
	mouse::Button::Six,
	mouse::Button::Seven,
	mouse::Button::Eight,
};

glfw_context::glfw_context(GLFWwindow *const apGLFWWindow)
	: m_pGLFWWindow(apGLFWWindow)
	, m_Keyboard(apGLFWWindow)
	, m_Mouse(apGLFWWindow)
{
	for (decltype(GLFW_JOYSTICK_1) i(0); i < m_Gamepads.size(); ++i)
		m_Gamepads[i] = glfw_gamepad_ptr(new gamepad_glfw(i));
}

std::optional<keyboard::Key> glfw_context::get_any_key_down() const
{
	return m_Keyboard.getAnyKeyDown();
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

std::optional<mouse::Button> glfw_context::get_any_mouse_button_down() const
{
	for (const auto& a : MOUSE_BUTTONS) if (get_mouse_button_down(a)) return { a };

	return {};
}

std::optional<mouse::Axis> glfw_context::get_any_mouse_axis_down(float threshold) const
{
	auto delta = get_mouse_delta();

	if (get_mouse_delta().x) return mouse::Axis::X;

	if (get_mouse_delta().y) return mouse::Axis::Y;

	return {};
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

context::gamepad_collection_type glfw_context::get_gamepads()
{
	return {m_Gamepads.front(), m_Gamepads.back()};
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
