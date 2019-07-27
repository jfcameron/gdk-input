// © 2018 Joseph Cameron - All Rights Reserved

#ifndef GDK_INPUT_MOUSE_H
#define GDK_INPUT_MOUSE_H

#include <memory>

//TODO: add scroll support
namespace gdk
{
    //struct Vector2;
    
    /// \brief Get mouse keys and position on a window    
    class mouse
    {      
    public:
        /// \brief Represents all detectable mouse buttons
        enum class Button
        { 
            Left, 
            Right, 
            Middle, 
            Four, 
            Five, 
            Six, 
            Seven, 
            Eight 
        };

        /// \brief mode
        enum class CursorMode
        {
            Normal,
            Hidden,
            Locked
        };
        
        /// \brief Used to represent cursor delta or position within a window
        struct cursor_2d_type
        {
            double x, y;
        };
           
        /// \brief Check if the button is being held down
        virtual bool getButtonDown(const Button aKeyCode) = 0;

        /// \brief change cursor mode
        virtual void setCursorMode(const CursorMode aCursorMode) = 0; //normal, hidden, locked

        /// \brief Check if the button was just pressed
        //virtual bool getButton(const Button &aKeyCode) = 0;

        /// \brief Viewcoordinate of cursor
        //virtual Vector2 getCursorPosition() = 0;

        /// \brief Get amount of cursor movement since the last frame
        //virtual Vector2 getDelta() = 0;

        //glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

        virtual ~mouse() = default;
    };
}

#endif
