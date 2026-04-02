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

#include "Steckinator/Log/Log.h"

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
        
        m_program_offset = pio_add_program(pio, &stepper_program);

        pio_sm_config c = stepper_program_get_default_config(m_program_offset);

        sm_config_set_set_pins(&c, m_pinStep, 1);
        sm_config_set_clkdiv(&c, 1000.0f);
        // Configure m_pinStep as output owned by PIO
        pio_gpio_init(pio, m_pinStep);
        pio_sm_set_consecutive_pindirs(pio, sm, m_pinStep, 1, true);
        pio_sm_init(pio, sm, m_program_offset, &c);
        pio_sm_set_enabled(pio, sm, true);
        pio_sm_exec(pio, sm, pio_encode_push(false, false)); // Prime RX FIFO so IsBusy() returns false initially

        // DIR managed by CPU directly (they don't need tight timing)
        gpio_init(m_pinDir); gpio_set_dir(m_pinDir, GPIO_OUT);

       
    }


    void StepperMotor::MoveRelative(Steps deltaSteps) {
        if (deltaSteps == 0) {
            return;
        }

        // Drain any stale completion tokens
        while (!pio_sm_is_rx_fifo_empty(m_pio, m_stateMachineIndex)) {
            pio_sm_get(m_pio, m_stateMachineIndex);
        }

        gpio_put(m_pinDir, (deltaSteps > 0));
        Steps count = static_cast<Steps>(std::abs(deltaSteps)) - 1;

        pio_sm_put_blocking(m_pio, m_stateMachineIndex, count);
        
        m_currentSteps += deltaSteps;
    }


    bool StepperMotor::IsBusy() {
        return (pio_sm_is_rx_fifo_empty(m_pio, m_stateMachineIndex));
    }

    void StepperMotor::SetSpeed(float stepsPerSecond) {
        if (stepsPerSecond < 10.0f) stepsPerSecond = 10.0f;      // minimum reasonable speed
        if (stepsPerSecond > 45000.0f) stepsPerSecond = 45000.0f; // safety limit

        // Calculate clock divider for desired step frequency
        // Each step takes ~22 PIO cycles (set high 10 + set low 10 + jmp)
        float pioCyclesPerStep = 22.0f;
        float requiredPioFreq = stepsPerSecond * pioCyclesPerStep;

        float clkdiv = 125000000.0f / requiredPioFreq;   // RP2040 runs at 125 MHz

        // Clamp divider (valid range is roughly 1.0 to 65535)
        if (clkdiv < 1.0f) clkdiv = 1.0f;
        if (clkdiv > 65535.0f) clkdiv = 65535.0f;

        pio_sm_set_clkdiv(m_pio, m_stateMachineIndex, clkdiv);
    }

}