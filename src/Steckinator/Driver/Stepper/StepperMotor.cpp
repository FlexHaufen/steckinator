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
#include "Steckinator/Config.h"

#include <hardware/clocks.h>
#include <cmath>
#include <cassert>

// *** NAMESPACE ***
namespace Steckinator {

    void StepperMotor::Init(PIO pio, uint stateMachineIndex, uint programOffset, uint pinStep, uint pinDir, uint stepsPerMm) {
        m_pio = pio;
        m_stateMachineIndex = stateMachineIndex;
        m_programOffset = programOffset;
        m_pinStep = pinStep;
        m_pinDir = pinDir; 
        m_stepsPerMm = stepsPerMm;
        

        // DIR managed by CPU directly (they don't need tight timing)
        gpio_init(m_pinDir); gpio_set_dir(m_pinDir, GPIO_OUT);

        // PIO configuration
        pio_sm_config c = stepper_program_get_default_config(m_programOffset);

        sm_config_set_set_pins(&c, m_pinStep, 1);
        constexpr float clkdiv = SYS_CLK_HZ / 1000000.0f;   // TODO (flex): Make define
        sm_config_set_clkdiv(&c, clkdiv);

        pio_gpio_init(m_pio, m_pinStep);
        pio_sm_set_consecutive_pindirs(m_pio, m_stateMachineIndex, m_pinStep, 1, true);
        pio_sm_init(m_pio, m_stateMachineIndex, m_programOffset, &c);

        // DMA configuration
        m_dmaChannel = dma_claim_unused_channel(true);

        LOG_INFO("Stepper initialized");

        return;
    }

    void StepperMotor::Stop() {

        dma_channel_abort(m_dmaChannel);

        // Gracefully stop the state machine
        pio_sm_set_enabled(m_pio, m_stateMachineIndex, false);
        pio_sm_clear_fifos(m_pio, m_stateMachineIndex);

        // Restart the state machine so it's ready for the next move
        pio_sm_restart(m_pio, m_stateMachineIndex);

        m_stepTable.clear();
    }

    void StepperMotor::MoveRelative(Steps steps, float feedrate, AccelerationMethod accelerationMethod) {
        if (steps == 0) {
            return;
        }

        // Set the direction
        gpio_put(m_pinDir, (steps > 0));


        // build the step table
        const auto absSteps = static_cast<Steps>(std::abs(steps));
        switch (accelerationMethod) {
            case AccelerationMethod::RAMP:
                BuildStepTable_Ramp(absSteps, feedrate);
                break;

            case AccelerationMethod::NONE:  // intended fallthrough
            default:
                BuildStepTable_Constant(absSteps, feedrate);
                break;
        }


        dma_channel_config dmaConfig = dma_channel_get_default_config(m_dmaChannel);
        channel_config_set_transfer_data_size(&dmaConfig, DMA_SIZE_32);
        channel_config_set_read_increment(&dmaConfig, true);
        channel_config_set_write_increment(&dmaConfig, false);
        channel_config_set_dreq(&dmaConfig, pio_get_dreq(m_pio, m_stateMachineIndex, true));

        dma_channel_configure(
            m_dmaChannel,
            &dmaConfig,
            &m_pio->txf[m_stateMachineIndex],
            m_stepTable.data(),
            m_stepTable.size(),
            false
        );

        pio_sm_set_enabled(m_pio, m_stateMachineIndex, true);
        dma_channel_start(m_dmaChannel);

    }

    bool StepperMotor::IsBusy() {
        if (dma_channel_is_busy(m_dmaChannel)) {
            return true;
        }
        if (!pio_sm_is_tx_fifo_empty(m_pio, m_stateMachineIndex)) {
            return true;
        }

        return false;
    }

    
    // *** PRIVATE FUNCTIONS ***

    void StepperMotor::BuildStepTable_Ramp(uint32_t total_steps, float feedrate) {
        if (total_steps == 0) {
            m_stepTable.clear();
            return;
        }

        // 1.   Convert everything to step units
        const float v_0 = 1'000'000.0f / static_cast<float>(m_initialPeriodUs);     // initial speed [steps/s]
        const float v_t = feedrate * m_stepsPerMm;                                  // target speed [steps/s]
        const float a = STEPPER_MOTOR_ACCELERATION * m_stepsPerMm;                  // acceleration [steps/s^2]

        // 2.    Early-out for constant-speed move (no acceleration possible/needed)
        if (v_t <= v_0 || a <= 0.0f) {
            const uint32_t period = static_cast<uint32_t>(1'000'000.0f / v_t + 0.5f);
            m_stepTable.assign(total_steps, period);
            return;
        }

        // 3.   Calculate required ramp steps for full acceleration to desired feedrate
        //      s = (v_t^2 - v_0^2) / (2a) 
        const float s_f = (std::pow(v_t, 2) - std::pow(v_0, 2)) / (2.0f * a);
        const uint32_t s = static_cast<uint32_t>(std::ceil(s_f));

        // 4.   Decide trapezoidal vs. triangular profile
        uint32_t ramp;
        uint32_t cruise;
        float actual_cruise_freq;

        if (s * 2u <= total_steps) {
            // Full trapezoidal: we can reach the desired feedrate
            ramp = s;
            cruise = total_steps - ramp * 2u;
            actual_cruise_freq = v_t;
        } 
        else {
            // Triangular profile: not enough distance -> reduce peak speed automatically
            ramp = total_steps / 2u;
            cruise = total_steps % 2u;
            // New peak velocity that exactly fits the available steps
            const float peak_v_sq = v_0 * v_0 + 2.0f * a * static_cast<float>(ramp);
            actual_cruise_freq = std::sqrt(peak_v_sq);
        }

        const uint32_t min_period = static_cast<uint32_t>(1'000'000.0f / actual_cruise_freq + 0.5f);

        // 5.   Resize once (efficient, no reallocations)
        m_stepTable.resize(total_steps);
        uint32_t* const tbl = m_stepTable.data();

        // 6.   Acceleration phase
        for (uint32_t i = 0; i < ramp; ++i) {
            const float v = std::sqrt(v_0 * v_0 + 2.0f * a * static_cast<float>(i));
            const float period_f = 1'000'000.0f / v;
            tbl[i] = static_cast<uint32_t>(period_f + 0.5f);
        }

        // 7. Cruise phase at constant (minimum) period
        for (uint32_t i = 0; i < cruise; ++i) {
            tbl[ramp + i] = min_period;
        }

        // 8. Deceleration phase – (mirror the acceleration ramp)
        for (uint32_t i = 0; i < ramp; ++i) {
            tbl[ramp + cruise + i] = tbl[ramp - 1 - i];
        }
    }

    void StepperMotor::BuildStepTable_Constant(uint32_t total_steps, float feedrate) {
        if (total_steps == 0) {
            m_stepTable.clear();
            return;
        }

        // Convert feedrate to step period in microseconds
        const float steps_per_second = feedrate * m_stepsPerMm;
        const uint32_t period_us = (steps_per_second > 0.0f) ? static_cast<uint32_t>(1'000'000.0f / steps_per_second + 0.5f) : m_initialPeriodUs;

        // Fill the entire table with the same period
        m_stepTable.assign(total_steps, period_us);
        return;
    }
}