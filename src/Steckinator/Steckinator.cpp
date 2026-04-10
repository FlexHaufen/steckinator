/**
 * @file Steckinator.cpp
 * @brief Implementation of Steckinator
 * @version 0.1
 * @date 2026-03-27
 * 
 * @copyright Copyright (c) 2026
 * 
 */

// *** INCLUDES ***
#include "Steckinator/Steckinator.h"

#include <pico/stdlib.h>
#include "Config.h"
#include "Log/Log.h"

#include "GCodeParser/GCodeParser.h"
#include "Motion/MotionController.h"

// *** NAMESPACE ***
namespace Steckinator {



    Steckinator::Steckinator() {
        stdio_init_all();
        //LOG_WAIT_FOR_USB;
        LOG_INFO("Setup");

        m_led_power.Init(GPIO_DEBUG_LED);
        m_led_power.On();
    }

    void Steckinator::Run() {
        //! Just some debug code to see if the motors are working

        // TODO (flex): Move this shit to the StepperMotor driver
        // currently this is used for enabling the drivers
        // as the enable pin is common
        gpio_init(GPIO_M_EN);
        gpio_set_dir(GPIO_M_EN, GPIO_OUT);
        gpio_put(GPIO_M_EN, false);

   
        MotionController mc;
        mc.Init();

        mc.Push({ MotionType::G28 });           // Homing
        mc.Push({ MotionType::G1, .x=100, .f=15 });
        mc.Push({ MotionType::G1, .y=100, .f=15 });
        //mc.Push({ MotionType::G1, .x=10,  .f=15 });
        //mc.Push({ MotionType::G1, .y=10,  .f=15 });
        
        // ** SUPER LOOP **
        while (true) {
            mc.Update();
            sleep_ms(10);
        }


    }
}