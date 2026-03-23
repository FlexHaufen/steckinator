
#pragma once

// *** INCLUDES ***
#include "Driver/Led/Led.h"


namespace Steckinator {


    class Steckinator {
    public:
    
        Steckinator();
        ~Steckinator() {}

        void Run();


    private:

        Led m_led_power;
        Led m_led_status;
        Led m_led_error;



    };


}