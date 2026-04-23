/**
 * @file Servo.h
 * @brief Servo
 * @version 0.1
 * @date 2026-03-27
 * 
 * @copyright Copyright (c) 2026
 * 
 */
#pragma once

// *** INCLUDES ***
#include <pico/stdlib.h>
#include <hardware/pwm.h>


// *** NAMESPACE **
namespace Steckinator {

    class Servo {
    public:
    
        void Init(uint gpio) {
            m_gpio = gpio;
            m_min_pulse = 1000.0f;  // very bad magic numbers
            m_max_pulse = 2000.0f;  // change to defines

            gpio_init(m_gpio);
            gpio_set_function(m_gpio, GPIO_FUNC_PWM);
            gpio_set_dir(m_gpio, GPIO_OUT);

            m_slice = pwm_gpio_to_slice_num(m_gpio);
            m_channel = pwm_gpio_to_channel(m_gpio);

            // Set PWM frequency to 50Hz (20ms period)
            // f_pwm = clk_sys / (wrap + 1) / clkdiv
            // Typical: clk_sys = 125 MHz

            float clkdiv = 64.0f;
            uint32_t wrap = 39062; // ~50 Hz

            pwm_set_clkdiv(m_slice, clkdiv);
            pwm_set_wrap(m_slice, wrap);
            pwm_set_enabled(m_slice, true);

            m_wrap = wrap;
        }

        void SetAngle(float angle_deg) {
            if (angle_deg < 0.0f) angle_deg = 0.0f;
            if (angle_deg > 180.0f) angle_deg = 180.0f;

            // Map angle to pulse width
            float pulse_us = m_min_pulse + (angle_deg / 180.0f) * (m_max_pulse - m_min_pulse);

            SetPulseWidth(pulse_us);
        }

    private:
        
        void SetPulseWidth(float pulse_us) {
            // Convert µs to duty cycle level
            float period_us = 20000.0f; // 20ms

            float duty = pulse_us / period_us;
            uint16_t level = (uint16_t)(duty * m_wrap);

            pwm_set_chan_level(m_slice, m_channel, level);
        }

    private:
        uint m_gpio;
        uint m_slice;
        uint m_channel;
        uint16_t m_wrap;

        float m_min_pulse;
        float m_max_pulse;
    };

}