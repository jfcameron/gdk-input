// © Joseph Cameron - All Rights Reserved

#include <gdk/glfw_keyboard.h>

#include <GLFW/glfw3.h>
#include <magic_enum/magic_enum.hpp>

#include <functional>
#include <iostream>
#include <set>
#include <stdexcept>
#include <string>
#include <type_traits>

using namespace gdk;

static constexpr char TAG[] = "keyboard_glfw";

static const auto GDK_INPUT_KEY_SET = magic_enum::enum_values<keyboard::key>();

static const std::set<decltype(GLFW_KEY_A)> KEY_SET{
	GLFW_KEY_ESCAPE,
	GLFW_KEY_F1,
	GLFW_KEY_F2,
	GLFW_KEY_F3,
	GLFW_KEY_F4,
	GLFW_KEY_F5,
	GLFW_KEY_F6,
	GLFW_KEY_F7,
	GLFW_KEY_F8,
	GLFW_KEY_F9,
	GLFW_KEY_F10,
	GLFW_KEY_F11,
	GLFW_KEY_F12,
	GLFW_KEY_PRINT_SCREEN,
	GLFW_KEY_SCROLL_LOCK,
	GLFW_KEY_PAUSE,
	GLFW_KEY_Q,
	GLFW_KEY_W,
	GLFW_KEY_E,
	GLFW_KEY_R,
	GLFW_KEY_T,
	GLFW_KEY_Y,
	GLFW_KEY_U,
	GLFW_KEY_I,
	GLFW_KEY_O,
	GLFW_KEY_P,
	GLFW_KEY_A,
	GLFW_KEY_S,
	GLFW_KEY_D,
	GLFW_KEY_F,
	GLFW_KEY_G,
	GLFW_KEY_H,
	GLFW_KEY_J,
	GLFW_KEY_K,
	GLFW_KEY_L,
	GLFW_KEY_Z,
	GLFW_KEY_X,
	GLFW_KEY_C,
	GLFW_KEY_V,
	GLFW_KEY_B,
	GLFW_KEY_N,
	GLFW_KEY_M,
	GLFW_KEY_1,
	GLFW_KEY_2,
	GLFW_KEY_3,
	GLFW_KEY_4,
	GLFW_KEY_5,
	GLFW_KEY_6,
	GLFW_KEY_7,
	GLFW_KEY_8,
	GLFW_KEY_9,
	GLFW_KEY_0,
	GLFW_KEY_GRAVE_ACCENT,
	GLFW_KEY_MINUS,
	GLFW_KEY_EQUAL,
	GLFW_KEY_BACKSPACE,
	GLFW_KEY_HOME,
	GLFW_KEY_END,
	GLFW_KEY_TAB,
	GLFW_KEY_LEFT_BRACKET,
	GLFW_KEY_RIGHT_BRACKET,
	GLFW_KEY_BACKSLASH,
	GLFW_KEY_INSERT,
	GLFW_KEY_PAGE_UP,
	GLFW_KEY_CAPS_LOCK,
	GLFW_KEY_SEMICOLON,
	GLFW_KEY_APOSTROPHE,
	GLFW_KEY_ENTER,
	GLFW_KEY_DELETE,
	GLFW_KEY_PAGE_DOWN,
	GLFW_KEY_LEFT_SHIFT,
	GLFW_KEY_COMMA,
	GLFW_KEY_PERIOD,
	GLFW_KEY_SLASH,
	GLFW_KEY_RIGHT_SHIFT,
	GLFW_KEY_LEFT_CONTROL,
	GLFW_KEY_LEFT_ALT,
	GLFW_KEY_SPACE,
	GLFW_KEY_RIGHT_ALT,
	GLFW_KEY_RIGHT_CONTROL,
	GLFW_KEY_LEFT,
	GLFW_KEY_RIGHT,
	GLFW_KEY_UP,
	GLFW_KEY_DOWN,
	GLFW_KEY_NUM_LOCK,
	GLFW_KEY_SLASH,
	GLFW_KEY_KP_MULTIPLY,
	GLFW_KEY_KP_SUBTRACT,
	GLFW_KEY_KP_7,
	GLFW_KEY_KP_8,
	GLFW_KEY_KP_9,
	GLFW_KEY_KP_ADD,
	GLFW_KEY_KP_4,
	GLFW_KEY_KP_5,
	GLFW_KEY_KP_6,
	GLFW_KEY_KP_1,
	GLFW_KEY_KP_2,
	GLFW_KEY_KP_3,
	GLFW_KEY_KP_ENTER,
	GLFW_KEY_KP_0,
	GLFW_KEY_KP_DECIMAL
};

static inline decltype(GLFW_KEY_ESCAPE) glfwKeyCodeFromKey(const keyboard::key a) {
    switch(a) {
        //Top Row
        case keyboard::key::escape:       return GLFW_KEY_ESCAPE;
        case keyboard::key::f1:           return GLFW_KEY_F1;
        case keyboard::key::f2:           return GLFW_KEY_F2;
        case keyboard::key::f3:           return GLFW_KEY_F3;
        case keyboard::key::f4:           return GLFW_KEY_F4;
        case keyboard::key::f5:           return GLFW_KEY_F5;
        case keyboard::key::f6:           return GLFW_KEY_F6;
        case keyboard::key::f7:           return GLFW_KEY_F7;
        case keyboard::key::f8:           return GLFW_KEY_F8;
        case keyboard::key::f9:           return GLFW_KEY_F9;
        case keyboard::key::f10:          return GLFW_KEY_F10;
        case keyboard::key::f11:          return GLFW_KEY_F11;
        case keyboard::key::f12:          return GLFW_KEY_F12;
        case keyboard::key::printscreen:  return GLFW_KEY_PRINT_SCREEN;
        case keyboard::key::scrolllock:   return GLFW_KEY_SCROLL_LOCK;
        case keyboard::key::pausebreak:   return GLFW_KEY_PAUSE;
        
        //Alphabetical characters
        case keyboard::key::q:            return GLFW_KEY_Q;
        case keyboard::key::w:            return GLFW_KEY_W;
        case keyboard::key::e:            return GLFW_KEY_E;
        case keyboard::key::r:            return GLFW_KEY_R;
        case keyboard::key::t:            return GLFW_KEY_T;
        case keyboard::key::y:            return GLFW_KEY_Y;
        case keyboard::key::u:            return GLFW_KEY_U;
        case keyboard::key::i:            return GLFW_KEY_I;
        case keyboard::key::o:            return GLFW_KEY_O;
        case keyboard::key::p:            return GLFW_KEY_P;
        case keyboard::key::a:            return GLFW_KEY_A;
        case keyboard::key::s:            return GLFW_KEY_S;
        case keyboard::key::d:            return GLFW_KEY_D;
        case keyboard::key::f:            return GLFW_KEY_F;
        case keyboard::key::g:            return GLFW_KEY_G;
        case keyboard::key::h:            return GLFW_KEY_H;
        case keyboard::key::j:            return GLFW_KEY_J;
        case keyboard::key::k:            return GLFW_KEY_K;
        case keyboard::key::l:            return GLFW_KEY_L;
        case keyboard::key::z:            return GLFW_KEY_Z;
        case keyboard::key::x:            return GLFW_KEY_X;
        case keyboard::key::c:            return GLFW_KEY_C;
        case keyboard::key::v:            return GLFW_KEY_V;
        case keyboard::key::b:            return GLFW_KEY_B;
        case keyboard::key::n:            return GLFW_KEY_N;
        case keyboard::key::m:            return GLFW_KEY_M;
        
        //Number row
        case keyboard::key::one:          return GLFW_KEY_1;
        case keyboard::key::two:          return GLFW_KEY_2;
        case keyboard::key::three:        return GLFW_KEY_3;
        case keyboard::key::four:         return GLFW_KEY_4;
        case keyboard::key::five:         return GLFW_KEY_5;
        case keyboard::key::six:          return GLFW_KEY_6;
        case keyboard::key::seven:        return GLFW_KEY_7;
        case keyboard::key::eight:        return GLFW_KEY_8;
        case keyboard::key::nine:         return GLFW_KEY_9;
        case keyboard::key::zero:         return GLFW_KEY_0;
        case keyboard::key::tilda:        return GLFW_KEY_GRAVE_ACCENT;
        case keyboard::key::minus:        return GLFW_KEY_MINUS;
        case keyboard::key::equals:       return GLFW_KEY_EQUAL;
        case keyboard::key::backspace:    return GLFW_KEY_BACKSPACE;
        case keyboard::key::home:         return GLFW_KEY_HOME;
        case keyboard::key::end:          return GLFW_KEY_END;
        
        //Q row
        case keyboard::key::tab:          return GLFW_KEY_TAB;
        case keyboard::key::openbracket:  return GLFW_KEY_LEFT_BRACKET;
        case keyboard::key::closebracket: return GLFW_KEY_RIGHT_BRACKET;
        case keyboard::key::backslash:    return GLFW_KEY_BACKSLASH;
        case keyboard::key::insert:       return GLFW_KEY_INSERT;
        case keyboard::key::pageup:       return GLFW_KEY_PAGE_UP;
        
        //A row
        case keyboard::key::capslock:     return GLFW_KEY_CAPS_LOCK;
        case keyboard::key::semicolon:    return GLFW_KEY_SEMICOLON;
        case keyboard::key::quote:        return GLFW_KEY_APOSTROPHE;
        case keyboard::key::enter:        return GLFW_KEY_ENTER;
        case keyboard::key::deletekey:    return GLFW_KEY_DELETE;
        case keyboard::key::pagedown:     return GLFW_KEY_PAGE_DOWN;
            
        //Z row
        case keyboard::key::leftshift:    return GLFW_KEY_LEFT_SHIFT;
        case keyboard::key::comma:        return GLFW_KEY_COMMA;
        case keyboard::key::period:       return GLFW_KEY_PERIOD;
        case keyboard::key::forwardslash: return GLFW_KEY_SLASH;
        case keyboard::key::rightshift:   return GLFW_KEY_RIGHT_SHIFT;
        
        //Bottom row
        case keyboard::key::leftcontrol:  return GLFW_KEY_LEFT_CONTROL;
        case keyboard::key::leftalt:      return GLFW_KEY_LEFT_ALT;
        case keyboard::key::space:        return GLFW_KEY_SPACE;
        case keyboard::key::rightalt:     return GLFW_KEY_RIGHT_ALT;
        case keyboard::key::rightcontrol: return GLFW_KEY_RIGHT_CONTROL;
        
        //Arrow keys
        case keyboard::key::leftarrow:    return GLFW_KEY_LEFT;
        case keyboard::key::rightarrow:   return GLFW_KEY_RIGHT;
        case keyboard::key::uparrow:      return GLFW_KEY_UP;
        case keyboard::key::downarrow:    return GLFW_KEY_DOWN;
        
        //Numpad
        case keyboard::key::numlock:      return GLFW_KEY_NUM_LOCK;
        case keyboard::key::numslash:     return GLFW_KEY_SLASH;
        case keyboard::key::numasterisk:  return GLFW_KEY_KP_MULTIPLY;
        case keyboard::key::numminus:     return GLFW_KEY_KP_SUBTRACT;
        case keyboard::key::num7:         return GLFW_KEY_KP_7;
        case keyboard::key::num8:         return GLFW_KEY_KP_8;
        case keyboard::key::num9:         return GLFW_KEY_KP_9;
        case keyboard::key::numplus:      return GLFW_KEY_KP_ADD;
        case keyboard::key::num4:         return GLFW_KEY_KP_4;
        case keyboard::key::num5:         return GLFW_KEY_KP_5;
        case keyboard::key::num6:         return GLFW_KEY_KP_6;
        case keyboard::key::num1:         return GLFW_KEY_KP_1;
        case keyboard::key::num2:         return GLFW_KEY_KP_2;
        case keyboard::key::num3:         return GLFW_KEY_KP_3;
        case keyboard::key::numenter:     return GLFW_KEY_KP_ENTER;
        case keyboard::key::num0:         return GLFW_KEY_KP_0;
        case keyboard::key::numperiod:    return GLFW_KEY_KP_DECIMAL;
    }

    throw std::invalid_argument(std::string("Unable to convert keyboard key \"")
		.append(std::to_string(static_cast< std::underlying_type< decltype(a)>::type>(a)))
		.append("\" to GLFW_KEY")); 
}

keyboard_glfw::keyboard_glfw(decltype(m_pWindow) pWindow)
: m_pWindow(pWindow)
{}

std::optional<keyboard::key> keyboard_glfw::any_key_down() const {
	for (const auto key : GDK_INPUT_KEY_SET)
		if (key_down(key)) return key;

	return {};
}

bool keyboard_glfw::key_down(const keyboard::key &aKeyCode) const {
	bool value(false);

	if (auto search = m_CurrentState.find(glfwKeyCodeFromKey(aKeyCode)); search != m_CurrentState.end())
		value = search->second == gdk::keyboard::key_state::held_down || 
				search->second == gdk::keyboard::key_state::just_pressed;

	return value;
}

bool keyboard_glfw::key_just_down(const keyboard::key &aKeyCode) const {
	bool value(false);

	if (auto search = m_CurrentState.find(glfwKeyCodeFromKey(aKeyCode)); search != m_CurrentState.end())
		value = search->second == gdk::keyboard::key_state::just_pressed;

	return value;
}

bool keyboard_glfw::key_just_released(const keyboard::key &aKeyCode) const {
	bool value(false);

	if (auto search = m_CurrentState.find(glfwKeyCodeFromKey(aKeyCode)); search != m_CurrentState.end())
		value = search->second == gdk::keyboard::key_state::just_released;

	return value;
}

void keyboard_glfw::update() {
	for (const auto &glfwKey : KEY_SET) {
		auto lastKeyState = m_KeyboardLastState[glfwKey];

		auto currentKeyState = glfwGetKey(m_pWindow.get(), glfwKey);

		auto newState = keyboard::key_state::up;

		if (currentKeyState == GLFW_RELEASE && lastKeyState != GLFW_RELEASE)
			newState = keyboard::key_state::just_released;
		
		else if (currentKeyState != GLFW_RELEASE)
			newState = lastKeyState == GLFW_RELEASE
				? newState = keyboard::key_state::just_pressed
				: newState = keyboard::key_state::held_down;
		
		m_CurrentState[glfwKey] = newState;

		m_KeyboardLastState[glfwKey] = currentKeyState;
	}
}

