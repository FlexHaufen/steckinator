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

    using Deg = float;


    class Servo {
    public:
    
        /**
         * @brief Construct and fully initialise a servo motor.
         * 
         * @param gpio      gpio
         * @param max       max range of the servo 
         * @param min       min range of the servo
         */
        void Init(uint gpio, Deg max, Deg min) {
            m_gpio = gpio;
            m_max = max;
            m_min = min;

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

        void SetAngle(Deg angle_deg) {
            
            if (angle_deg < m_min) { angle_deg = m_min; }
            if (angle_deg > m_max) { angle_deg = m_max; }

            // Map angle to pulse width
            float pulse_us = m_min_pulse + (angle_deg / m_max) * (m_max_pulse - m_min_pulse);
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

        const float m_min_pulse =  500.0f;  // [us]
        const float m_max_pulse = 2500.0f;  // [us]

        Deg m_max = 0;
        Deg m_min = 0;
    };

}