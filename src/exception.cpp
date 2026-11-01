// © Joseph Cameron - MIT License

#include <gdk/input/exception.h>

using namespace gdk::input;

exception::exception(const std::string &aWhat)
: mpWhat(std::make_shared<const std::string>(aWhat))
{}

const char *exception::what() const noexcept { return mpWhat->c_str(); }
