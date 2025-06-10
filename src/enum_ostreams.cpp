// © Joseph Cameron - All Rights Reserved

#include <gdk/input_context.h>
#include <gdk/gamepad.h>

#include <iostream>
#include <magic_enum/magic_enum.hpp>

using namespace gdk;

std::ostream& operator<<(std::ostream& s, const gdk::keyboard::key a) { s << magic_enum::enum_name(a); return s; }
std::ostream& operator<<(std::ostream& s, const gdk::keyboard::key_state a) { s << magic_enum::enum_name(a); return s; }
std::ostream& operator<<(std::ostream& s, const gdk::mouse::button a) { s << magic_enum::enum_name(a); return s; }
std::ostream& operator<<(std::ostream& s, const mouse::axis a) { s << magic_enum::enum_name(a); return s; }
std::ostream& operator<<(std::ostream& s, const mouse::button_state a) { s << magic_enum::enum_name(a); return s; }
std::ostream& operator<<(std::ostream& s, const mouse::cursor_mode a) { s << magic_enum::enum_name(a); return s; }

