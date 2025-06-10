#include<gdk/glfw_context.h>

#include <array>
#include <iostream>

using namespace gdk;
using namespace gdk::input;

context_ptr_type glfw_context::make(std::shared_ptr<GLFWwindow> apGLFWWindow) {
	return std::make_shared<glfw_context>(glfw_context(apGLFWWindow));
}

glfw_context::glfw_context(glfw_window_ptr apGLFWWindow)
: m_Keyboard(apGLFWWindow)
, m_Mouse(apGLFWWindow) {
	for (decltype(GLFW_JOYSTICK_1) i(0); i < m_Gamepads.size(); ++i)
		m_Gamepads[i] = glfw_gamepad_ptr(new gamepad_glfw(i));
}

std::optional<keyboard::key> glfw_context::any_key_down() const {
	return m_Keyboard.any_key_down();
}

bool glfw_context::key_down(const keyboard::key &aKeyCode) const {
	return m_Keyboard.key_down(aKeyCode);
}

bool glfw_context::key_just_pressed(const keyboard::key &aKeyCode) const {
	return m_Keyboard.key_just_down(aKeyCode);
}

bool glfw_context::key_just_released(const keyboard::key &aKeyCode) const {
	return m_Keyboard.key_just_released(aKeyCode);
}

bool glfw_context::mouse_button_down(const mouse::button& aButton) const {
	return m_Mouse.button_down(aButton);
}

std::optional<mouse::button> glfw_context::any_mouse_button_down() const {
    static constexpr std::array<mouse::button, 8> MOUSE_BUTTONS{
        mouse::button::left,
        mouse::button::right,
        mouse::button::middle,
        mouse::button::four,
        mouse::button::five,
        mouse::button::six,
        mouse::button::seven,
        mouse::button::eight,
    };
	for (const auto& a : MOUSE_BUTTONS) if (mouse_button_down(a)) return { a };
	return {};
}

std::optional<mouse::axis> glfw_context::any_mouse_axis_down(float threshold) const {
	auto delta = mouse_delta();
	if (mouse_delta().x) return mouse::axis::x;
	if (mouse_delta().y) return mouse::axis::y;
	return {};
}

bool glfw_context::mouse_button_just_pressed(const mouse::button &aButton) const {
	return m_Mouse.button_just_down(aButton);
}

bool glfw_context::mouse_button_just_released(const mouse::button &aButton) const {
	return m_Mouse.button_just_released(aButton);
}

mouse::cursor_2d_type glfw_context::mouse_cursor_position() const {
	return m_Mouse.cursor_position();
}

mouse::cursor_mode glfw_context::mouse_cursor_mode() const {
	return m_Mouse.get_cursor_mode();
}

void glfw_context::set_mouse_cursor_mode(mouse::cursor_mode mode) {
	m_Mouse.set_cursor_mode(mode);
}

mouse::cursor_2d_type glfw_context::mouse_delta() const {
	return m_Mouse.delta();
}

context::gamepad_ptr glfw_context::get_gamepad(const size_t index) {
	if (index > m_Gamepads.size()) throw std::invalid_argument("index exceeds maximum number of gamepads: " +
		std::to_string(index) + "/" + std::to_string(m_Gamepads.size()));

	return std::static_pointer_cast<gamepad>(m_Gamepads[index]);
}

context::gamepad_collection_type glfw_context::gamepads() {
    context::gamepad_collection_type output;
    for (auto gamepad : m_Gamepads)
        output.push_back(gamepad);

	return output;
}

void glfw_context::update() {
	m_Keyboard.update();
	m_Mouse.update();

	for (auto &a : m_Gamepads) a->update();
}

