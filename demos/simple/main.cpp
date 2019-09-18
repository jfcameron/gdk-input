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

const char *JSONControlsMappingString = R"(
{
    "Jump": {
        "gamepads": {
                "8bitdo": {
                    "buttons": [0]
                }
        },
        "keys": ["Q", "Space", "LeftControl"],
        "mouse": {
            "buttons": []
        }
    }
}
)";

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

    const std::shared_ptr<gdk::gamepad_glfw> pGamepad = []()
    {
        try
        {   
            return std::make_shared<decltype(pGamepad)::element_type>(0);
        }
        catch(const std::invalid_argument &ex)
        {
            return decltype(pGamepad)(nullptr);
        }
    }();

    const auto gamepadName = pGamepad ? std::string(pGamepad->getName()) : "null";

    std::shared_ptr<gdk::mouse_glfw> pMouse = std::make_shared<gdk::mouse_glfw>(gdk::mouse_glfw(pWindow));
    std::shared_ptr<gdk::keyboard> pKeyboard = std::make_shared<gdk::keyboard_glfw>(gdk::keyboard_glfw(pWindow));

    pMouse->setCursorMode(gdk::mouse::CursorMode::Locked);

    auto player_controls = gdk::controls::make_from_json(JSONControlsMappingString, pKeyboard, pMouse, pGamepad);

    /*player_controls->addKeyToMapping("Jump", gdk::keyboard::Key::Space);
    player_controls->addKeyToMapping("Jump", gdk::keyboard::Key::Q);

    player_controls->addGamepadButtonToMapping("Jump", std::string(gamepadName), 0);*/

    /*player_controls->addKeyToMapping("Run", gdk::keyboard::Key::LeftShift);
    player_controls->addGamepadButtonToMapping("Run", std::string(gamepadName), 1);

    player_controls->addGamepadHatToMapping("Forward", std::string(gamepadName), 0, {gdk::gamepad::hat_state_type::horizontal_direction::Center, gdk::gamepad::hat_state_type::vertical_direction::Up});
    player_controls->addGamepadAxisToMapping("Forward", std::string(gamepadName), 1, -0.05);
    player_controls->addKeyToMapping("Forward", gdk::keyboard::Key::W);
   
    //player_controls->addGamepadHatToMapping("Backward", std::string(gamepadName), 0, {0, -1});
    player_controls->addGamepadAxisToMapping("Backward", std::string(gamepadName), 1, +0.05);
    player_controls->addKeyToMapping("Backward", gdk::keyboard::Key::S);
    
    //player_controls->addGamepadHatToMapping("StrafeLeft", std::string(gamepadName), 0, {-1, 0});
    player_controls->addGamepadAxisToMapping("StrafeLeft", std::string(gamepadName), 0, -0.05);
    player_controls->addKeyToMapping("StrafeLeft", gdk::keyboard::Key::A);

    //player_controls->addGamepadHatToMapping("StrafeRight", std::string(gamepadName), 0, {+1, 0});
    player_controls->addGamepadAxisToMapping("StrafeRight", std::string(gamepadName), 0, +0.05);
    player_controls->addKeyToMapping("StrafeRight", gdk::keyboard::Key::D);

    player_controls->addMouseAxisToMapping("LookLeft", gdk::mouse::Axis::X, -1);
    player_controls->addKeyToMapping("LookLeft", gdk::keyboard::Key::LeftArrow);

    player_controls->addMouseAxisToMapping("LookRight" , gdk::mouse::Axis::X, +1);
    player_controls->addKeyToMapping("LookRight", gdk::keyboard::Key::RightArrow);

    player_controls->addMouseAxisToMapping("LookUp" , gdk::mouse::Axis::Y, -1);
    player_controls->addKeyToMapping("LookUp", gdk::keyboard::Key::UpArrow);

    player_controls->addMouseAxisToMapping("LookDown" , gdk::mouse::Axis::Y, +1);
    player_controls->addKeyToMapping("LookDown", gdk::keyboard::Key::DownArrow);*/

    std::cout << "serializing...\n";
    auto data = player_controls->serializeToJSON();
    std::cout << data << "\n";

    while(!glfwWindowShouldClose(pWindow.get()))
    { 
        glfwPollEvents();

        if (pGamepad) pGamepad->update();
        
        if (pMouse) pMouse->update();

        //if (player_controls->get("Forward")) std::cout << "Moving Forward: " << player_controls->get("Forward") << "\n";
        //if (player_controls->get("Backward")) std::cout << "Moving Backward: " << player_controls->get("Backward") << "\n";
        //if (player_controls->get("StrafeLeft")) std::cout << "Moving left!\n";
        //if (player_controls->get("StrafeRight")) std::cout << "Moving right!\n";
        
        if (player_controls->get("Jump")) std::cout << "Jumping!\n";
        //if (player_controls->get("Run")) std::cout << "Running!\n";

        float up = 0, down = 0, left = 0, right = 0;

        /*if (const auto value = player_controls->get("LookLeft")) left = value;
        if (const auto value = player_controls->get("LookRight")) right = value;

        if (const auto value = player_controls->get("LookUp")) up = value;
        if (const auto value = player_controls->get("LookDown")) down = value;*/


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

