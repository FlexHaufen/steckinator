/**
 * @file Led.h
 * @author flexhaufen
 * @brief Driver for a LED
 * @version 0.1
 * @date 2026-03-23
 * 
 * @copyright Copyright (c) 2026
 * 
 */
#pragma once

// *** INCLUDES ***

#include "pico/stdlib.h"


// *** NAMESPACE ***
namespace Steckinator {

    /**
     * @brief Simple wrapper class for a LED
     * 
     */
    class Led {
    public:

        /**
         * @brief Initialize a new LED
         * 
         * @param pin   The pin of the LED
         */
        void Init(uint pin) {
            m_pin = pin;
            gpio_init(m_pin);
            gpio_set_dir(m_pin, GPIO_OUT);
            gpio_put(m_pin, false);
        }

        void On() { gpio_put(m_pin, true); }
        void Off() { gpio_put(m_pin, false); }
        void Toggle() { gpio_put(m_pin, !gpio_get(m_pin)); }


    private:
        // ** Members **
        uint m_pin = 0;

    };

}
