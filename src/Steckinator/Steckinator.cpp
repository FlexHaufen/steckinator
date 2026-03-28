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

#include "Steckinator/GCodeParser/GCodeParser.h"

#include "Steckinator/Driver/Stepper/StepperMotor.h"
#include "Steckinator/Driver/VacuumPump/VacuumPump.h"

// *** NAMESPACE ***
namespace Steckinator {



    Steckinator::Steckinator() {
        stdio_init_all();

        m_led_power.Init(GPIO_DEBUG_LED);
        m_led_status.Init(GPIO_LED_0);
        m_led_error.Init(GPIO_LED_1);

        m_led_power.On();
        //m_led_status.On();
        //m_led_error.On();

    }

    void Steckinator::Run() {

        //! Just some debug code to see if the motors are working
        
        
        // TODO (flex): Implement enable pin
        gpio_init(GPIO_M_EN);   gpio_set_dir(GPIO_M_EN,  GPIO_OUT);  gpio_put(GPIO_M_EN,  false); // disabled

        StepperMotor motor0;
        motor0.Init(pio0, 0, GPIO_M0_STEP, GPIO_M0_DIR);
        motor0.Move(-3000);


        //VacuumPump vacuumPump;
        //vacuumPump.Init(GPIO_M0_DC_OUT1, GPIO_M0_DC_OUT2);
        //vacuumPump.On();

        while (true) {


            sleep_ms(1);
        }


    }
}