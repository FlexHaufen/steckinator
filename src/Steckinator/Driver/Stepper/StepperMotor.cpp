/**
 * @file StepperMotor.cpp
 * @brief Implementation of StepperMotor
 * @version 0.1
 * @date 2026-03-22
 * 
 * @copyright Copyright (c) 2026
 * 
 */

// *** INCLUDES ***
#include "Steckinator/Driver/Stepper/StepperMotor.h"

#include <hardware/clocks.h>
#include <cmath>
#include <cassert>

// *** NAMESPACE ***
namespace Steckinator {


    void StepperMotor::Init(PIO pio, uint sm, uint stepPin, uint dirPin) {
        m_pio = pio;
        m_stateMachineIndex = sm;
        m_pinStep = stepPin;
        m_pinDir = dirPin; 
        
        uint offset = pio_add_program(pio, &stepper_program);

        pio_sm_config c = stepper_program_get_default_config(offset);

        sm_config_set_set_pins(&c, m_pinStep, 1);
        sm_config_set_clkdiv(&c, 1250.0f);

        // Configure m_pinStep as output owned by PIO
        pio_gpio_init(pio, m_pinStep);
        pio_sm_set_consecutive_pindirs(pio, sm, m_pinStep, 1, true);

        pio_sm_init(pio, sm, offset, &c);
        pio_sm_set_enabled(pio, sm, true);

        // DIR managed by CPU directly (they don't need tight timing)
        gpio_init(m_pinDir); gpio_set_dir(m_pinDir, GPIO_OUT);
    }


    void StepperMotor::Move(int32_t steps) {
        if (steps == 0) {
            return;
        }
        gpio_put(m_pinDir, (steps > 0));
        uint32_t count = static_cast<uint32_t>(abs(steps)) - 1;
        pio_sm_put_blocking(m_pio, m_stateMachineIndex, count);
    }

}