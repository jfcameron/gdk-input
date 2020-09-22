#include<gdk/glfw_context.h>

using namespace gdk;
using namespace gdk::input;

glfw_context::glfw_context(GLFWwindow *const apGLFWWindow)
	: m_pGLFWWindow(apGLFWWindow)
	, m_Keyboard(apGLFWWindow)
{}

bool glfw_context::get_key_down(const keyboard::Key& aKeyCode) const
{
	return m_Keyboard.getKeyDown(aKeyCode);
}

bool glfw_context::get_key_just_pressed(const keyboard::Key& aKeyCode) const
{
	return m_Keyboard.getKeyJustDown(aKeyCode);
}

void glfw_context::update()
{
	m_Keyboard.update();
}
