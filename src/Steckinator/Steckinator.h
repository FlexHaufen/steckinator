/**
 * @file Steckinator.h
 * @brief Main implementation of the project
 * @version 0.1
 * @date 2026-03-27
 * 
 * @copyright Copyright (c) 2026
 * 
 */
#pragma once

// *** INCLUDES ***
#include "Driver/Led/Led.h"

// *** NAMESPACE **
namespace Steckinator {


    class Steckinator {
    public:
    
        Steckinator();
        ~Steckinator() {}

        /**
         * @brief Application main loop
         * @note Will not return
         * 
         */
        void Run();

    private:

        static void Core1Run();
        

    private:


        // ** Members **
        Led m_led_power;

    };

}