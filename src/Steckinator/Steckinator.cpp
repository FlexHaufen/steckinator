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

        m_led_power.Init(GPIO_DEBUG_LED);
        //LOG_WAIT_FOR_USB;

        //m_led_status.On();
        //m_led_error.On();
        m_led_power.On();
    }

    void Steckinator::Run() {

        //! Just some debug code to see if the motors are working
        LOG_INFO("Setup");

        gpio_init(GPIO_M_EN);
        gpio_set_dir(GPIO_M_EN, GPIO_OUT);
        gpio_put(GPIO_M_EN, false);

   
        MotionController mc;
        mc.Init();

        // TODO do the events move relative every time?
        mc.Push({ MotionType::G28,});
        mc.Push({ MotionType::G0, .x=400, .y=0});
        mc.Push({ MotionType::G0, .x=400, .y=400});
        mc.Push({ MotionType::G0, .x=10, .y=400});
        mc.Push({ MotionType::G0, .x=10, .y=10});


        while (true) {
            mc.Update();
            sleep_ms(10);
        }


    }
}