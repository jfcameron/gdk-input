// © Joseph Cameron - All Rights Reserved

#include <gdk/input/capture.h>
#include <gdk/input/context.h>
#include <gdk/input/controls.h>
#include <gdk/input/gamepad.h>
#include <gdk/input/glfw_context.h>

#if GDK_INPUT_DEMO_HAS_LUA
#include <gdk/input/ext/lua_bindings.h>
#endif

#include <GLFW/glfw3.h>

#include <array>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <memory>
#include <iterator>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

using namespace gdk;

namespace {
    const std::array<std::string, 6> ACTIONS{
        "jump", "fire", "crouch", "move_left", "move_right", "look_x"};

    [[nodiscard]] std::shared_ptr<GLFWwindow> make_window() {
        glfwSetErrorCallback([](int, const char *const aMessage) {
            std::cerr << "glfw: " << aMessage << "\n";
        });

        if (!glfwInit()) throw std::runtime_error("glfwInit failed");

        std::shared_ptr<GLFWwindow> pWindow(
            glfwCreateWindow(400, 300, "gdk-input rebinding", nullptr, nullptr),
            [](GLFWwindow *const p) { if (p) glfwDestroyWindow(p); });

        if (!pWindow) throw std::runtime_error("could not create a window");

        glfwMakeContextCurrent(pWindow.get());

        return pWindow;
    }

    void apply_defaults(input::controls &aControls) {
        aControls.bind("jump", input::keyboard::key::space);
        aControls.bind("jump", input::gamepad::button::a);

        aControls.bind("fire", input::keyboard::key::f);
        aControls.bind("fire", input::gamepad::button::right_bumper);

        aControls.bind("crouch", input::keyboard::key::c);
        aControls.bind("crouch", input::gamepad::button::b);

        aControls.bind("move_left", input::keyboard::key::a);
        aControls.bind("move_left", input::gamepad::axis::left_x, -1.f);

        aControls.bind("move_right", input::keyboard::key::d);
        aControls.bind("move_right", input::gamepad::axis::left_x, 1.f);

        aControls.bind("look_x", input::gamepad::axis::right_x, 1.f);
    }

    void print_bindings(const input::controls &aControls) {
        std::cout << "\n-- bindings --------------------------------------------------\n";

        for (std::size_t i = 0; i < ACTIONS.size(); ++i) {
            std::cout << "  " << (i + 1) << ". " << ACTIONS[i] << ":";

            const auto sources = aControls.sources(ACTIONS[i]);

            if (sources.empty()) std::cout << " (nothing)";

            for (const auto &source : sources) std::cout << " [" << source << "]";

            std::cout << "\n";
        }

        std::cout << "  press 1-" << ACTIONS.size() << " to rebind, escape to quit\n";
    }

    void print_device(const input::gamepad &aGamepad) {
        std::cout << "  gamepad: " << aGamepad.name()
            << "\n    guid: " << aGamepad.guid()
            << "\n    standard mapping: " << (aGamepad.has_standard_mapping() ? "yes" : "NO")
            << "\n    buttons: " << aGamepad.button_count()
            << ", axes: " << aGamepad.axis_count()
            << ", hats: " << aGamepad.hat_count() << "\n";
    }

    void print_glfw_joysticks() {
        std::cout << "  glfw joystick slots:\n";

        bool any = false;

        for (int id = GLFW_JOYSTICK_1; id <= GLFW_JOYSTICK_LAST; ++id) {
            if (!glfwJoystickPresent(id)) continue;

            any = true;

            const auto *const pName = glfwGetJoystickName(id);
            const auto *const pGuid = glfwGetJoystickGUID(id);
            const auto isGamepad = glfwJoystickIsGamepad(id);

            std::cout << "    [" << id << "] " << (pName ? pName : "(no name)")
                << "\n        guid: " << (pGuid ? pGuid : "(none)")
                << "\n        glfw has a mapping for it: " << (isGamepad ? "yes" : "NO");

            if (isGamepad)
                if (const auto *const pMapped = glfwGetGamepadName(id))
                    std::cout << " (" << pMapped << ")";

            std::cout << "\n";
        }

        if (!any) std::cout << "    (none -- the operating system is not offering glfw a joystick)\n";
    }

    void print_raw_state(const input::gamepad &aGamepad) {
        std::cout << "    axes now:";

        for (std::size_t i = 0; i < aGamepad.axis_count(); ++i)
            std::cout << " [" << i << "]="
                << aGamepad.axis_value(static_cast<input::gamepad::index_type>(i), 0.f);

        std::cout << "\n    buttons now:";

        for (std::size_t i = 0; i < aGamepad.button_count(); ++i)
            std::cout << " [" << i << "]="
                << (aGamepad.button_down(static_cast<input::gamepad::index_type>(i)) ? 1 : 0);

        std::cout << "\n";
    }

    void print_report(input::context &aContext, const std::size_t aBound) {
        std::cout << "\n-- devices ---------------------------------------------------\n";

        print_glfw_joysticks();

        bool any = false;

        for (std::size_t player = 0; player < 8; ++player) {
            const auto pGamepad = aContext.get_gamepad(player);

            if (!pGamepad->connected()) continue;

            any = true;

            std::cout << "  player " << player << (player == aBound ? "  <- bound to controls" : "")
                << "\n";

            print_device(*pGamepad);
            print_raw_state(*pGamepad);
        }

        if (!any) std::cout << "  no player holds a device\n";

        std::cout << "  controls are reading player " << aBound << " (p cycles)\n";
    }

    using axis_memory_type = std::array<std::vector<float>, 8>;

    void echo_raw(input::context &aContext, axis_memory_type &aMemory) {
        for (std::size_t player = 0; player < 8; ++player) {
            const auto pGamepad = aContext.get_gamepad(player);

            if (!pGamepad->connected()) { aMemory[player].clear(); continue; }

            for (std::size_t i = 0; i < pGamepad->button_count(); ++i) {
                const auto index = static_cast<input::gamepad::index_type>(i);

                if (pGamepad->button_just_pressed(index))
                    std::cout << "  raw: player " << player << " button " << i << "\n";
            }

            auto &last = aMemory[player];

            if (last.size() != pGamepad->axis_count()) {
                last.assign(pGamepad->axis_count(), 0.f);

                for (std::size_t i = 0; i < pGamepad->axis_count(); ++i)
                    last[i] = pGamepad->axis_value(static_cast<input::gamepad::index_type>(i), 0.f);

                continue;
            }

            for (std::size_t i = 0; i < pGamepad->axis_count(); ++i) {
                const auto index = static_cast<input::gamepad::index_type>(i);
                const auto value = pGamepad->axis_value(index, 0.f);

                if (std::abs(value - last[i]) < 0.15f) continue;

                std::cout << "  raw: player " << player << " axis " << i << " " << last[i]
                    << " -> " << value << "\n";

                last[i] = value;
            }
        }
    }

    [[nodiscard]] std::string state_line(const input::controls &aControls) {
        std::ostringstream out;

        for (const auto &action : ACTIONS) {
            const auto value = aControls.get(action);

            if (value == 0) continue;

            out << action << "=" << value << " ";
        }

        return out.str();
    }
}

int main() {
    auto pWindow = make_window();
    auto pContext = input::glfw_context::make(pWindow);
    auto pGlfw = std::static_pointer_cast<input::glfw_context>(pContext);

    if (std::ifstream file("gamecontrollerdb.txt"); file) {
        const std::string text{std::istreambuf_iterator<char>(file),
            std::istreambuf_iterator<char>()};

        std::cout << "loaded gamecontrollerdb.txt: " << pContext->add_gamepad_mappings(text)
            << " entries\n";
    }
    else std::cout << "no gamecontrollerdb.txt beside the executable; using glfw's built-in set\n"
        << "  (an unrecognised pad still works -- rebind onto it and capture will bind it by guid)\n";

    auto pControls = input::controls::make(pContext, 0);

    apply_defaults(*pControls);

    std::cout << "gdk-input rebinding demo. player one.\n";

    glfwPollEvents();
    pGlfw->update();

    std::size_t boundPlayer = 0;
    axis_memory_type axisMemory;
    bool joining = false;

    print_report(*pContext, boundPlayer);

    print_bindings(*pControls);

    std::cout << "  (i re-reports devices, p binds the next player, j joins by pressing a button)\n";

    std::shared_ptr<input::capture> pCapture;
    std::string capturingFor;

    std::string lastState;
    std::string lastGuid;

    while (!glfwWindowShouldClose(pWindow.get())) {
        glfwPollEvents();
        pGlfw->update();

        if (pContext->key_just_released(input::keyboard::key::escape)) break;

        const auto pGamepad = pContext->get_gamepad(boundPlayer);

        if (pContext->key_just_pressed(input::keyboard::key::i) && !pCapture)
            print_report(*pContext, boundPlayer);

        if (joining) {
            for (std::size_t player = 0; player < 8; ++player) {
                const auto pCandidate = pContext->get_gamepad(player);

                if (!pCandidate->connected() || !pCandidate->any_button_down()) continue;

                if (player != boundPlayer) {
                    pContext->swap_players(boundPlayer, player);

                    std::cout << "  " << pContext->get_gamepad(boundPlayer)->name()
                        << " is now player " << boundPlayer << "\n";
                }
                else std::cout << "  that one is already player " << boundPlayer << "\n";

                joining = false;

                print_report(*pContext, boundPlayer);

                break;
            }

            continue;
        }

        if (pContext->key_just_pressed(input::keyboard::key::j) && !pCapture) {
            joining = true;

            std::cout << "\npress a button on the controller you want as player "
                << boundPlayer << "...\n";

            continue;
        }

        if (pContext->key_just_pressed(input::keyboard::key::p) && !pCapture) {
            boundPlayer = (boundPlayer + 1) % 8;

            pControls = input::controls::make(pContext, boundPlayer);

            apply_defaults(*pControls);

            std::cout << "\ncontrols now read player " << boundPlayer << "\n";

            print_bindings(*pControls);
        }

        echo_raw(*pContext, axisMemory);

        if (pGamepad->just_connected()) {
            std::cout << "\nconnected:\n";

            print_device(*pGamepad);

            if (std::string(pGamepad->guid()) != lastGuid && !lastGuid.empty())
                std::cout << "  (a different device than before -- standard bindings still apply,"
                    " device bindings do not)\n";

            lastGuid = pGamepad->guid();
        }

        if (pGamepad->just_disconnected())
            std::cout << "\ndisconnected -- gamepad bindings go quiet, keyboard ones do not\n";

        if (pCapture) {
            if (const auto pressed = pCapture->poll()) {
                std::cout << "  bound " << capturingFor << " to [" << *pressed << "]\n";

                pControls->unbind(capturingFor);
                pControls->bind(capturingFor, *pressed);

                pCapture.reset();

                print_bindings(*pControls);
            }

            continue;
        }

        for (std::size_t i = 0; i < ACTIONS.size(); ++i) {
            const auto digit = static_cast<input::keyboard::key>(
                static_cast<int>(input::keyboard::key::one) + static_cast<int>(i));

            if (!pContext->key_just_pressed(digit)) continue;

            capturingFor = ACTIONS[i];
            pCapture = input::capture::make(pContext, boundPlayer);

            std::cout << "\npress anything for \"" << capturingFor << "\"...\n";

            break;
        }

        if (const auto state = state_line(*pControls); state != lastState) {
            if (!state.empty()) std::cout << "  " << state << "\n";

            lastState = state;
        }
    }

#if GDK_INPUT_DEMO_HAS_LUA
    std::cout << "\n-- what these bindings would be saved as ---------------------\n"
        << input::ext::to_string(*pControls) << "\n";
#endif

    return EXIT_SUCCESS;
}
