/**
 * @file VacuumPump.h
 * @brief Driver for a VacuumPump
 * @version 0.1
 * @date 2026-03-23
 * 
 * @copyright Copyright (c) 2026
 * 
 */
#pragma once

// *** INCLUDES ***
#include <pico/stdlib.h>


// *** NAMESPACE ***
namespace Steckinator {

    /**
     * @brief Simple wrapper class for a VacuumPump
     * 
     */
    class VacuumPump {
    public:

        /**
         * @brief Initialize a new VacuumPump
         * 
         * @param pin   The pin of the VacuumPump
         */
        void Init(uint pinIn1, uint pinIn2) {
            m_pinIn1 = pinIn1;
            m_pinIn2 = pinIn2;
            gpio_init(m_pinIn1); gpio_set_dir(m_pinIn1, GPIO_OUT); gpio_set_function(m_pinIn1, GPIO_FUNC_SIO); gpio_put(m_pinIn1, false);
            gpio_init(m_pinIn2); gpio_set_dir(m_pinIn2, GPIO_OUT); gpio_set_function(m_pinIn2, GPIO_FUNC_SIO); gpio_put(m_pinIn2, false);
        }

        void On() { gpio_put(m_pinIn1, true); }
        void Off() {gpio_put(m_pinIn1, false); }


    private:
        // ** Members **
        uint m_pinIn1 = 0;
        uint m_pinIn2 = 0;

    };

}
