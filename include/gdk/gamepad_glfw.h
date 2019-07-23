// © 2019 Joseph Cameron - All Rights Reserved

#ifndef GDK_GAMEPAD_GLFW_H
#define GDK_GAMEPAD_GLFW_H

#include <gdk/gamepads.h>

namespace gdk
{
    class gamepad_glfw : public gamepad
    {
        //! glfw index for this joystick
        int m_JoystickIndex;

        //! Buffer containing name of device
        std::string m_Name;
        
        //! Buffer containing most recent button states
        std::vector<const unsigned char> m_Buttons;
       
        //! Buffer containing most recent axis states
        std::vector<const float> m_Axes;

    public:
        virtual float getAxis(int index) const override;
        
        virtual bool getButtonDown(int index) const override;

        virtual gamepad::hat_type getHat(int index) const override;

        virtual std::string getName() const override;

        virtual void update() override;

        gamepad_glfw(const int joystickID);
    };
}

#endif

