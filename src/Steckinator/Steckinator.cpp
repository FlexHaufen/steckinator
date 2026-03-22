



#include "Steckinator/Steckinator.h"

#include <pico/stdlib.h>
#include "Config.h"
#include "Log/Log.h"

#include "Driver/StepperMotor.h"

namespace Steckinator {



    Steckinator::Steckinator() {
        stdio_init_all();
    }

    void Steckinator::Run() {

        //! Just some debug code to see if the motors are working
        
        StepperMotor motor0(pio0, 0, GPIO_M0_STEP, GPIO_M0_DIR, 10.f);
        StepperMotor motor1(pio0, 1, GPIO_M1_STEP, GPIO_M1_DIR, 10.f);

        motor0.move(100);
        motor1.move(-100);


        while (true) {


            sleep_ms(1);
        }


    }
}