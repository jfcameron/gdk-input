// © Joseph Cameron - All Rights Reserved

#ifndef JFC_TEST_INCLUDE_H
#define JFC_TEST_INCLUDE_H

#include <gdk/input/glfw_context.h>
#include <gdk/input/context.h>

#include <GLFW/glfw3.h>

#include <memory>
#include <stdexcept>
#include <string>

[[nodiscard]] inline std::shared_ptr<GLFWwindow> initGLFW() {
    static std::string lastError;

    glfwSetErrorCallback([](int, const char *const aMessage) { lastError = aMessage; });

    glfwInitHint(GLFW_PLATFORM, GLFW_PLATFORM_NULL);

    if (!glfwInit()) throw std::runtime_error("glfwInit failed: " + lastError);

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);

    std::shared_ptr<GLFWwindow> pWindow(glfwCreateWindow(320, 240, "gdkinput test", nullptr, nullptr),
        [](GLFWwindow *const p) { if (p) glfwDestroyWindow(p); });

    if (!pWindow) throw std::runtime_error("glfwCreateWindow failed: " + lastError);

    return pWindow;
}

/// \brief a window, a context, and one poll, which is the state every test starts from
struct input_fixture final {
    std::shared_ptr<GLFWwindow> pWindow = initGLFW();
    gdk::input::context_ptr_type pContext = gdk::input::glfw_context::make(pWindow);

    /// \brief advance one frame the way a caller's loop would
    void poll() {
        glfwPollEvents();
        std::static_pointer_cast<gdk::input::glfw_context>(pContext)->update();
    }

    input_fixture() { poll(); }
};

#endif
