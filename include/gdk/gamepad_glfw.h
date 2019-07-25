// © 2019 Joseph Cameron - All Rights Reserved

#ifndef GDK_GAMEPAD_GLFW_H
#define GDK_GAMEPAD_GLFW_H

#include <gdk/gamepads.h>

namespace gdk
{
    class gamepad_glfw : public gamepad
    {
    private:
        //! glfw index for this joystick
        int m_JoystickIndex;

        //! Buffer containing name of device
        std::string m_Name;
        
        //! Buffer containing most recent button states
        std::vector<unsigned char> m_Buttons;
       
        //! Buffer containing most recent axis states
        std::vector<float> m_Axes;

        //! Buffer containing the most recent hat states
        std::vector<unsigned char> m_Hats;

    public:
        virtual float getAxis(int index) const override;

        virtual gamepad::size_type getAxisCount() const override;
        
        virtual bool getButtonDown(int index) const override;

        virtual gamepad::size_type getButtonCount() const override;
        
        virtual gamepad::hat_state_type getHat(int index) const override;
        
        virtual gamepad::size_type getHatCount() const override;

        virtual std::string_view getName() const override;

        virtual void update() override;

        gamepad_glfw(const int joystickID);
    };
}

#endif

