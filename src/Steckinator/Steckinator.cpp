



#include "Steckinator/Steckinator.h"

#include <pico/stdlib.h>
#include "Config.h"
#include "Log/Log.h"

#include "Steckinator/GCodeParser/GCodeParser.h"

#include "Steckinator/Driver/Stepper/StepperMotor.h"
#include "Steckinator/Driver/VacuumPump/VacuumPump.h"

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
        
        //StepperMotor motor0(pio0, 0, GPIO_M0_STEP, GPIO_M0_DIR, 10.f);
        //StepperMotor motor1(pio0, 1, GPIO_M1_STEP, GPIO_M1_DIR, 10.f);

        //motor0.move(100);
        //motor1.move(-100);


        //VacuumPump vacuumPump;
        //vacuumPump.Init(GPIO_M0_DC_OUT1, GPIO_M0_DC_OUT2);
        //vacuumPump.On();

        while (true) {


            sleep_ms(1);
        }


    }
}