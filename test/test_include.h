#ifndef JFC_TEST_INCLUDE_H
#define JFC_TEST_INCLUDE_H

#include <gdkinput/buildinfo.h>

#include <stdexcept>
#include <iostream>

#include <GLFW/glfw3.h>

/// \brief init glfw, create a window
inline std::shared_ptr<GLFWwindow> initGLFW()
{
    static constexpr auto TAG = __func__;

    glfwSetErrorCallback([](int, const char *msg)
    {
        throw std::runtime_error(std::string(TAG).append("/").append(msg));
    });

    if (!glfwInit()) throw std::runtime_error(std::string(TAG).append("/glfwInit failed"));

    if (std::shared_ptr<GLFWwindow> pWindow(glfwCreateWindow(300, 300, "demo", nullptr, nullptr)
        ,[](GLFWwindow *const ptr)
        {
            glfwDestroyWindow(ptr);
        }); !pWindow) 
    {
        throw std::runtime_error("unable to initialize glfw window!");
    }
    else
    {
        glfwSetWindowCloseCallback(pWindow.get(), [](GLFWwindow *const pCurrentWindow)
        {
            glfwSetWindowShouldClose(pCurrentWindow, GLFW_TRUE);
        });

        return pWindow;
    }

    return {};
}

#endif

