// © Joseph Cameron - All Rights Reserved

#include <gdk/input/exception.h>

using namespace gdk::input;

exception::exception(const std::string &aWhat)
: std::runtime_error(aWhat)
{}
