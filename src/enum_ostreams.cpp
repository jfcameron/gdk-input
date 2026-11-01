// © Joseph Cameron - MIT License

#include <gdk/input/context.h>
#include <gdk/input/gamepad.h>

#include <iostream>
#include <magic_enum/magic_enum.hpp>

using namespace gdk::input;

std::ostream& operator<<(std::ostream& aStream, const gdk::input::keyboard::key aValue) { aStream << magic_enum::enum_name(aValue); return aStream; }
std::ostream& operator<<(std::ostream& aStream, const gdk::input::keyboard::key_state aValue) { aStream << magic_enum::enum_name(aValue); return aStream; }
std::ostream& operator<<(std::ostream& aStream, const gdk::input::mouse::button aValue) { aStream << magic_enum::enum_name(aValue); return aStream; }
std::ostream& operator<<(std::ostream& aStream, const mouse::axis aValue) { aStream << magic_enum::enum_name(aValue); return aStream; }
std::ostream& operator<<(std::ostream& aStream, const mouse::button_state aValue) { aStream << magic_enum::enum_name(aValue); return aStream; }
std::ostream& operator<<(std::ostream& aStream, const mouse::cursor_mode aValue) { aStream << magic_enum::enum_name(aValue); return aStream; }
std::ostream& operator<<(std::ostream& aStream, const gamepad::button aValue) { aStream << magic_enum::enum_name(aValue); return aStream; }
std::ostream& operator<<(std::ostream& aStream, const gamepad::axis aValue) { aStream << magic_enum::enum_name(aValue); return aStream; }
