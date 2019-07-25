#include <cstdlib>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>
#include <vector>

#include <GLFW/glfw3.h>

#include <gdk/controls.h>
#include <gdk/gamepad_glfw.h>
#include <gdk/keyboard_glfw.h>
#include <gdk/mouse_glfw.h>

static auto constexpr TAG = "demo";

/// \brief init glfw, create a window
std::shared_ptr<GLFWwindow> initGLFW()
{
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

int main(int argc, char **argv)
{
    std::vector<std::string>(argv, argv + argc);

    auto pWindow = initGLFW();

    std::shared_ptr<gdk::mouse> pMouse = std::make_shared<gdk::mouse_glfw>(gdk::mouse_glfw(pWindow));
    std::shared_ptr<gdk::keyboard> pKeyboard = std::make_shared<gdk::keyboard_glfw>(gdk::keyboard_glfw(pWindow));
    std::shared_ptr<gdk::gamepad> pGamepad = std::make_shared<gdk::gamepad_glfw>(0);

    gdk::controls player_controls(pKeyboard, pMouse, pGamepad);

    player_controls.addKeyMapping("Jump", gdk::keyboard::Key::C);
    player_controls.addGamepadButtonMapping("Jump", std::string(pGamepad->getName()), 0);

    player_controls.addKeyMapping("Run", gdk::keyboard::Key::X);
    player_controls.addGamepadButtonMapping("Run", std::string(pGamepad->getName()), 1);

    while(!glfwWindowShouldClose(pWindow.get()))
    { 
        glfwPollEvents();

        pGamepad->update();

        /*if (pMouse->getButtonDown(gdk::mouse::Button::Left))  std::cout << "Left\n";
        if (pMouse->getButtonDown(gdk::mouse::Button::Right)) std::cout << "Right\n";
        
        if (pKeyboard->getKeyDown(gdk::keyboard::Key::Q)) std::cout << "Q\n";
        if (pKeyboard->getKeyDown(gdk::keyboard::Key::W)) std::cout << "W\n";
        if (pKeyboard->getKeyDown(gdk::keyboard::Key::E)) std::cout << "E\n";

        for (size_t i = 0, s = gamepad.getHatCount(); i < s; ++i)
        {
            auto state = gamepad.getHat(i);

            if (state.x || state.y) std::cout << i << ": {" << state.x << ", " << state.y  << "}, " << "\n";
        }*/

        if (player_controls.get("Jump")) std::cout << "Jumping!\n";
        if (player_controls.get("Run")) std::cout << "Running!\n";
    }

    return EXIT_SUCCESS;
}

