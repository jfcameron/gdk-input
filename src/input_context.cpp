// © 2020 Joseph Cameron - All Rights Reserved

#include <gdk/input_context.h>
#include <gdk/glfw_context.h>

#include <stdexcept>

using namespace gdk;
using namespace gdk::input;

context::context_ptr_type context::make(GLFWwindow *const apGLFWWindow)
{
	return std::make_unique<glfw_context>(glfw_context(apGLFWWindow));
}




