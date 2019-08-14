#include <cstdlib>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>
#include <vector>
#include <sstream>

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

    std::shared_ptr<gdk::gamepad_glfw> pGamepad = nullptr; //std::make_shared<gdk::gamepad_glfw>(0);
    const auto gamepadName = pGamepad ? std::string(pGamepad->getName()) : "nah";

    std::shared_ptr<gdk::mouse_glfw> pMouse = std::make_shared<gdk::mouse_glfw>(gdk::mouse_glfw(pWindow));
    std::shared_ptr<gdk::keyboard> pKeyboard = std::make_shared<gdk::keyboard_glfw>(gdk::keyboard_glfw(pWindow));

    pMouse->setCursorMode(gdk::mouse::CursorMode::Locked);

    gdk::controls player_controls(pKeyboard, pMouse, pGamepad);

    player_controls.addKeyMapping("Jump", gdk::keyboard::Key::Space);
    player_controls.addGamepadButtonMapping("Jump", std::string(gamepadName), 0);

    player_controls.addKeyMapping("Run", gdk::keyboard::Key::LeftShift);
    player_controls.addGamepadButtonMapping("Run", std::string(gamepadName), 1);

    player_controls.addGamepadHatMapping("Forward", std::string(gamepadName), 0, {0, +1});
    player_controls.addGamepadAxisMapping("Forward", std::string(gamepadName), 1, -0.05);
    player_controls.addKeyMapping("Forward", gdk::keyboard::Key::W);
   
    player_controls.addGamepadHatMapping("Backward", std::string(gamepadName), 0, {0, -1});
    player_controls.addGamepadAxisMapping("Backward", std::string(gamepadName), 1, +0.05);
    player_controls.addKeyMapping("Backward", gdk::keyboard::Key::S);
    
    player_controls.addGamepadHatMapping("StrafeLeft", std::string(gamepadName), 0, {-1, 0});
    player_controls.addGamepadAxisMapping("StrafeLeft", std::string(gamepadName), 0, -0.05);
    player_controls.addKeyMapping("StrafeLeft", gdk::keyboard::Key::A);

    player_controls.addGamepadHatMapping("StrafeRight", std::string(gamepadName), 0, {+1, 0});
    player_controls.addGamepadAxisMapping("StrafeRight", std::string(gamepadName), 0, +0.05);
    player_controls.addKeyMapping("StrafeRight", gdk::keyboard::Key::D);

    /* //BRAINSTORMING on "stick acceleration" etc. functions that modify raw stick to some function of it.
    player_controls.addGamepadAxisMapping(
        "LookLeft", //Name of the control
        std::string(gamepadName), //Name of the supported hardware
        1, 
        {
            -0.05, //deadzone and direction of axis
            [](const float raw) // functor that converts raw axis value to output value. //TODO: this function should probably be interpreted, so this function can be loaded as data. ulisp or lua. or some kind of curve description?
            {
                return raw; //in this case the relationship is linear.
            }
        }
    );
    */

    player_controls.addMouseAxisMapping("LookLeft" , gdk::mouse::Axis::X, -1);
    player_controls.addKeyMapping("LookLeft", gdk::keyboard::Key::LeftArrow);

    player_controls.addMouseAxisMapping("LookRight" , gdk::mouse::Axis::X, +1);
    player_controls.addKeyMapping("LookRight", gdk::keyboard::Key::RightArrow);

    player_controls.addMouseAxisMapping("LookUp" , gdk::mouse::Axis::Y, -1);
    player_controls.addKeyMapping("LookUp", gdk::keyboard::Key::UpArrow);

    player_controls.addMouseAxisMapping("LookDown" , gdk::mouse::Axis::Y, +1);
    player_controls.addKeyMapping("LookDown", gdk::keyboard::Key::DownArrow);

    while(!glfwWindowShouldClose(pWindow.get()))
    { 
        glfwPollEvents();

        if (pGamepad) pGamepad->update();
        
        if (pMouse) pMouse->update();

        if (player_controls.get("Forward")) std::cout << "Moving Forward: " << player_controls.get("Forward") << "\n";
        if (player_controls.get("Backward")) std::cout << "Moving Backward: " << player_controls.get("Backward") << "\n";
        if (player_controls.get("StrafeLeft")) std::cout << "Moving left!\n";
        if (player_controls.get("StrafeRight")) std::cout << "Moving right!\n";
        
        if (player_controls.get("Jump")) std::cout << "Jumping!\n";
        if (player_controls.get("Run")) std::cout << "Running!\n";

        float up = 0, down = 0, left = 0, right = 0;

        if (const auto value = player_controls.get("LookLeft")) left = value;
        if (const auto value = player_controls.get("LookRight")) right = value;

        if (const auto value = player_controls.get("LookUp")) up = value;
        if (const auto value = player_controls.get("LookDown")) down = value;


        bool shouldPrint = false;

        std::stringstream output;

        output << "{ ";

        if (up || down || left || right)
        {
            shouldPrint = true;

            output << "\"Look\": { \"L\": " << left << ", \"R\": " << right << ", \"U\": " << up << ", \"D\": " << down << "}";
        }

        output << " }\n";

        if (shouldPrint) std::cout << output.str();
    }

    return EXIT_SUCCESS;
}

