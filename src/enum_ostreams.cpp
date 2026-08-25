// © Joseph Cameron - All Rights Reserved

#include <gdk/input/context.h>
#include <gdk/input/gamepad.h>

#include <iostream>
#include <magic_enum/magic_enum.hpp>

using namespace gdk::input;

std::ostream& operator<<(std::ostream& s, const gdk::input::keyboard::key a) { s << magic_enum::enum_name(a); return s; }
std::ostream& operator<<(std::ostream& s, const gdk::input::keyboard::key_state a) { s << magic_enum::enum_name(a); return s; }
std::ostream& operator<<(std::ostream& s, const gdk::input::mouse::button a) { s << magic_enum::enum_name(a); return s; }
std::ostream& operator<<(std::ostream& s, const mouse::axis a) { s << magic_enum::enum_name(a); return s; }
std::ostream& operator<<(std::ostream& s, const mouse::button_state a) { s << magic_enum::enum_name(a); return s; }
std::ostream& operator<<(std::ostream& s, const mouse::cursor_mode a) { s << magic_enum::enum_name(a); return s; }
std::ostream& operator<<(std::ostream& s, const gamepad::button a) { s << magic_enum::enum_name(a); return s; }
std::ostream& operator<<(std::ostream& s, const gamepad::axis a) { s << magic_enum::enum_name(a); return s; }
