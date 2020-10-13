// © 2018 Joseph Cameron - All Rights Reserved

#ifndef GDK_INPUT_MOUSE_H
#define GDK_INPUT_MOUSE_H

#include <memory>

//TODO: add scroll support
namespace gdk
{
    /// \brief Get mouse keys and position on a window    
    class mouse
    {      
    public:
        /// \brief Represents all detectable mouse buttons
        enum class Button
        { 
            Left, //!< left mouse button
            Right, //!< right mouse button 
            Middle, //!< middle mouse button
            Four, //!< fourth button, if the mouse has one
            Five, //!< fifth button, if the mouse has one
            Six, //!< sixth button, if the mouse has one
            Seven, //!< seventh button, if the mouse has one
            Eight //!< eighth button, if the mouse has one
        };

		/// \brief button state
		enum class ButtonState
		{
			UP = 0, //!< unpressed
			JUST_PRESSED, //!< single frame down input
			JUST_RELEASED, //!< single frame release input
			HELD_DOWN //!< multiple frames
		};

        /// \brief mode affects cursor behaviour
        enum class CursorMode
        {
            Normal,
            Hidden,
            Locked
        };

        /// \brief represents the two axes of a mouse
        enum class Axis
        {
            X,
            Y
        };
        
        /// \brief Used to represent cursor delta or position within a window
        struct cursor_2d_type
        {
            double x, y;
        };
        
		/// \brief Check if the key is being held down
		virtual bool getButtonDown(const Button aKeyCode) const = 0;

		/// \brief check if key was just pressed down
		virtual bool getButtonJustDown(const Button aKeyCode) const = 0;

		/// \brief check if the key was just released
		virtual bool getButtonJustReleased(const Button aKeyCode) const = 0;
		
		/// \brief Viewcoordinate of cursor
		virtual cursor_2d_type getCursorPosition() const = 0;

		/// \brief Get amount of cursor movement since the last frame
		virtual cursor_2d_type getDelta() const = 0;

		/// \brief affects values returned by getDelta
		virtual void setDeltaSensitivity(double sens) = 0;

        /// \brief change cursor mode
        virtual void setCursorMode(const CursorMode aCursorMode) = 0;

        /// \brief check cursor mode
        virtual mouse::CursorMode getCursorMode() const = 0;
			   
        virtual ~mouse() = default;
    };
}

std::ostream& operator<<(std::ostream&, const gdk::mouse::Button);
std::ostream& operator<<(std::ostream&, const gdk::mouse::ButtonState);
std::ostream& operator<<(std::ostream&, const gdk::mouse::CursorMode);
std::ostream& operator<<(std::ostream&, const gdk::mouse::Axis);

#endif
