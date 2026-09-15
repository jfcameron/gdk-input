#include <cmath>
#include <gdk/input/glfw_context.h>

#include <gdk/input/impl_glfw_gamepad.h>
#include <gdk/input/impl_glfw_keyboard.h>
#include <gdk/input/impl_glfw_mouse.h>
#include <gdk/input/impl_glfw_text_input.h>

#include <gdk/input/impl_gamepad_mappings.h>
#include <gdk/input/impl_slot_assignment.h>

#include <array>
#include <set>
#include <iostream>

using namespace gdk::input;

context_ptr_type glfw_context::make(std::shared_ptr<GLFWwindow> apGLFWWindow) {
	return context_ptr_type(new glfw_context(std::move(apGLFWWindow)));
}

/// \brief everything the backend actually holds
struct glfw_context::impl final {
	using glfw_gamepad_ptr = std::shared_ptr<gamepad_glfw>;

	keyboard_glfw keyboard;
	mouse_glfw mouse;
	text_input_glfw text;

	std::array<glfw_gamepad_ptr, GLFW_JOYSTICK_LAST + 1> gamepads;

	explicit impl(std::shared_ptr<GLFWwindow> apWindow)
	: keyboard(apWindow)
	, mouse(apWindow)
	, text(apWindow) {
		for (auto &pGamepad : gamepads) pGamepad = glfw_gamepad_ptr(new gamepad_glfw());
	}

	void reconcile_devices();
};

glfw_context::glfw_context(std::shared_ptr<GLFWwindow> apGLFWWindow)
: m_pImpl(std::make_unique<impl>(std::move(apGLFWWindow))) {
	m_pImpl->reconcile_devices();
}

glfw_context::~glfw_context() = default;

bool glfw_context::key_down(const keyboard::key &aKeyCode) const {
	return m_pImpl->keyboard.key_down(aKeyCode);
}

bool glfw_context::key_just_pressed(const keyboard::key &aKeyCode) const {
	return m_pImpl->keyboard.key_just_down(aKeyCode);
}

bool glfw_context::key_just_released(const keyboard::key &aKeyCode) const {
	return m_pImpl->keyboard.key_just_released(aKeyCode);
}

bool glfw_context::mouse_button_down(const mouse::button& aButton) const {
	return m_pImpl->mouse.button_down(aButton);
}

std::optional<mouse::axis> glfw_context::any_mouse_axis_down(float threshold) const {
	const auto delta = mouse_delta();

	if (std::abs(delta.x) > threshold) return mouse::axis::x;
	if (std::abs(delta.y) > threshold) return mouse::axis::y;

	const auto scroll = mouse_scroll_delta();

	if (std::abs(scroll.x) > threshold) return mouse::axis::scroll_x;
	if (std::abs(scroll.y) > threshold) return mouse::axis::scroll_y;

	return {};
}

bool glfw_context::mouse_button_just_pressed(const mouse::button &aButton) const {
	return m_pImpl->mouse.button_just_down(aButton);
}

bool glfw_context::mouse_button_just_released(const mouse::button &aButton) const {
	return m_pImpl->mouse.button_just_released(aButton);
}

mouse::cursor_2d_type glfw_context::mouse_cursor_position() const {
	return m_pImpl->mouse.cursor_position();
}

mouse::cursor_mode glfw_context::mouse_cursor_mode() const {
	return m_pImpl->mouse.get_cursor_mode();
}

void glfw_context::set_mouse_cursor_mode(mouse::cursor_mode mode) {
	m_pImpl->mouse.set_cursor_mode(mode);
}

mouse::cursor_2d_type glfw_context::mouse_delta() const {
	return m_pImpl->mouse.delta();
}

mouse::scroll_2d_type glfw_context::mouse_scroll_delta() const {
	return m_pImpl->mouse.scroll_delta();
}

const std::vector<text::event> &glfw_context::text_events() const {
	return m_pImpl->text.events();
}

text::composition glfw_context::text_composition() const {
	return m_pImpl->text.composition();
}

bool glfw_context::text_input_focus() const {
	return m_pImpl->text.focus();
}

void glfw_context::set_text_input_focus(const bool aFocus) {
	m_pImpl->text.set_focus(aFocus);
}

void glfw_context::set_text_input_caret(const text::caret &aCaret) {
	m_pImpl->text.set_caret(aCaret);
}

context::gamepad_ptr glfw_context::get_gamepad(const size_t index) {
	if (index >= m_pImpl->gamepads.size()) {
		static const auto pAbsent = std::make_shared<gamepad_glfw>();

		return pAbsent;
	}

	return std::static_pointer_cast<gamepad>(m_pImpl->gamepads[index]);
}

void glfw_context::swap_players(const std::size_t aLeft, const std::size_t aRight) {
	if (aLeft == aRight || aLeft >= m_pImpl->gamepads.size() || aRight >= m_pImpl->gamepads.size()) return;

	std::swap(m_pImpl->gamepads[aLeft], m_pImpl->gamepads[aRight]);
}

std::size_t glfw_context::add_gamepad_mappings(const std::string &aMappings) {
	glfwUpdateGamepadMappings(aMappings.c_str());

	return mapping_guids(aMappings).size();
}

void glfw_context::impl::reconcile_devices() {
	std::set<int> claimed;

	for (std::size_t player = 0; player < gamepads.size(); ++player) {
		const auto joystick = gamepads[player]->joystick_index();

		if (!joystick) continue;

		if (glfwJoystickPresent(*joystick)) {
			claimed.insert(*joystick);

			continue;
		}

		gamepads[player]->detach();
	}

	for (int joystick = GLFW_JOYSTICK_1; joystick <= GLFW_JOYSTICK_LAST; ++joystick) {
		if (!glfwJoystickPresent(joystick) || claimed.count(joystick)) continue;

		const auto *pGuid = glfwGetJoystickGUID(joystick);

		const std::string guid = pGuid ? pGuid : "";

		std::vector<slot_memory> slots;

		slots.reserve(gamepads.size());

		for (const auto &pGamepad : gamepads) {
			slot_memory memory;

			memory.occupied = pGamepad->joystick_index().has_value();
			memory.guid = std::string(pGamepad->guid());

			if (const auto last = pGamepad->last_joystick_index())
				memory.backendIndex = static_cast<std::size_t>(*last);

			slots.push_back(std::move(memory));
		}

		const auto player = assign_slot(slots, guid, static_cast<std::size_t>(joystick));

		if (!player) continue;   

		gamepads[*player]->attach(joystick, guid);
	}
}

context::gamepad_collection_type glfw_context::gamepads() {
    context::gamepad_collection_type output;
    for (auto gamepad : m_pImpl->gamepads) output.push_back(gamepad);

    return output;
}

void glfw_context::update() {
	m_pImpl->text.update();
	m_pImpl->keyboard.update(m_pImpl->text.focus(), m_pImpl->text.presses());
	m_pImpl->mouse.update();

	m_pImpl->reconcile_devices();

	for (auto &a : m_pImpl->gamepads) a->update();
}

