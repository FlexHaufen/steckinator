/**
 * @file Switch.h
 * @author flexhaufen
 * @brief Driver for a Switch
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
     * @brief Simple wrapper class for a Switch
     * 
     */
    class Switch {
    public:

        /**
         * @brief Initialize a new Switch
         * 
         * @param pin   The pin of the Switch
         */
        void Init(uint pin) {
            m_pin = pin;
            gpio_init(m_pin);
            gpio_set_dir(m_pin, GPIO_IN);
        }

        // inverted
        bool Get() { return !gpio_get(m_pin); }

    private:
        // ** Members **
        uint m_pin = 0;

    };

}
