// © 2020 Joseph Cameron - All Rights Reserved

#include <gdk/keyboard.h>
#include <gdk/mouse.h>
#include <gdk/gamepad.h>

#include <magic_enum/magic_enum.hpp>

using namespace gdk;

//Keyboard
std::ostream& operator<<(std::ostream& s, const keyboard::Key a)
{
	s << magic_enum::enum_name(a);

	return s;
}

//Mouse
std::ostream& operator<<(std::ostream& s, const mouse::Button a)
{
	s << magic_enum::enum_name(a);

	return s;
}
std::ostream& operator<<(std::ostream& s, const mouse::ButtonState a)
{
	s << magic_enum::enum_name(a);

	return s;
}
std::ostream& operator<<(std::ostream& s, const mouse::CursorMode a)
{
	s << magic_enum::enum_name(a);

	return s;
}
std::ostream& operator<<(std::ostream& s, const mouse::Axis a)
{
	s << magic_enum::enum_name(a);

	return s;
}

//Gamepad
/*std::ostream& operator<<(std::ostream& s, const gamepad::hat_state_type::vertical_direction a)
{
	s << magic_enum::enum_name(a);

	return s;
}
std::ostream& operator<<(std::ostream& s, const gamepad::hat_state_type::horizontal_direction a)
{
	s << magic_enum::enum_name(a);

	return s;
}*/
