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

/// \brief user interface to the example manager
class inputmanager
{
public:
    //! get a ptr to the keyboard
    virtual std::shared_ptr<gdk::keyboard> getKeyboard() = 0;
    
    //! get a ptr to the mouse
    virtual std::shared_ptr<gdk::mouse> getMouse() = 0;
    
    //! get a ptr to one of the gamepads
    virtual std::shared_ptr<gdk::gamepad> getGamepad(size_t index) = 0;
};

/// \brief basic example input manager implementation
class inputmanager_glfw : public inputmanager
{
    std::shared_ptr<gdk::keyboard> m_Keyboard;
    std::shared_ptr<gdk::mouse> m_Mouse;
    std::vector<std::shared_ptr<gdk::gamepad_glfw>> m_Gamepad;

public:
    virtual std::shared_ptr<gdk::keyboard> getKeyboard() override
    {
        return m_Keyboard;
    }

    virtual std::shared_ptr<gdk::mouse> getMouse() override
    {
        return m_Mouse;
    }

    virtual std::shared_ptr<gdk::gamepad> getGamepad(size_t index) override
    {
        return std::static_pointer_cast<gdk::gamepad>(m_Gamepad[index]);
    }

    void update()
    {
        glfwPollEvents();

        for (decltype(m_Gamepad)::size_type i = 0, s = m_Gamepad.size(); i < s; ++i) m_Gamepad[i]->update();
    }
//std::make_shared<gdk::mouse_glfw>(gdk::mouse_glfw(pWindow))
    inputmanager_glfw(std::shared_ptr<GLFWwindow> pWindow)
    : m_Keyboard(std::make_shared<gdk::keyboard_glfw>(gdk::keyboard_glfw(pWindow)))
    //, m_Mouse(std::make_shared<gdk::mouse_glfw>(gdk::mouse_glfw(pWindow)))
    //, m_Gamepad
    {}
};

/// \brief example engine interface
class engine
{
public:
    virtual std::shared_ptr<inputmanager> getInputManager() = 0;

    //getRenderer

    //get blah blah blah

public:
    virtual ~engine() = default;
};

/// \brief example engine implementation
class engine_impl : public engine
{
    std::shared_ptr<GLFWwindow> m_pWindow;

    std::shared_ptr<inputmanager_glfw> m_InputManager;

    std::function<void(engine *const)> m_UserUpdate;

public:
    virtual std::shared_ptr<inputmanager> getInputManager() override
    {
        return std::static_pointer_cast<inputmanager>(m_InputManager);
    }

    std::shared_ptr<GLFWwindow> getGLFWwindow()
    {
        return m_pWindow;
    }

    void update()
    {
        m_InputManager->update();

        m_UserUpdate(this);
    }

    engine_impl(decltype(m_UserUpdate) aUserInitFunctor, const decltype(m_UserUpdate) &aUserUpdateFunctor)
    : m_pWindow([](){return initGLFW();}())
    , m_InputManager ([&](){return std::make_shared<inputmanager_glfw>(inputmanager_glfw(m_pWindow));}())
    , m_UserUpdate(aUserUpdateFunctor)
    {
        aUserInitFunctor(this);
    }
    
    virtual ~engine_impl() = default;
};

void user_init(engine *const pEngine);
void user_update(engine *const pEngine);

int main(int argc, char **argv)
{
    engine_impl MyCoolEngine(&user_init, &user_update);

    while(!glfwWindowShouldClose(MyCoolEngine.getGLFWwindow().get())) MyCoolEngine.update();

    return EXIT_SUCCESS;
}

// Finally its user code
std::shared_ptr<gdk::keyboard> pKeyboard;// = std::make_shared<gdk::keyboard_glfw>(gdk::keyboard_glfw(pWindow));

void user_init(engine *const pEngine)
{
    pKeyboard = pEngine->getInputManager()->getKeyboard();
}

void user_update(engine *const pEngine)
{
    if (pKeyboard->getKeyDown(gdk::keyboard::Key::Q)) std::cout << "Q\n";
}

