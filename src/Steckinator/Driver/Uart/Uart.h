/**
 * @file Uart.h
 * @brief Implementation of the Uart consol
 * @version 0.1
 * @date 2025-11-12
 * 
 * @copyright Copyright (c) 2025
 * 
 */
#pragma once

// *** INCLUDES ***
#include <pico/stdlib.h>
#include <hardware/uart.h>
#include <hardware/gpio.h>
#include <string>

// *** NAMESPACE ***
namespace Steckinator {

    /**
     * @brief Uart
     * 
     */
    class Uart {
    public:

        /**
         * @brief Create a new uart interface
         * 
         * @param uart          The instance
         * @param txPin         TX
         * @param rxPin         RX
         * @param baudrate      baudrate. E.g. `115200`
         */
        Uart(uart_inst_t* uart, uint txPin, uint rxPin, uint baudrate);

        /**
         * @brief Start the uart
         * 
         */
        void begin();

        /**
         * @brief Sent a string over uart
         * 
         * @param s     The string to be sent
         */
        void writeLine(const std::string& s);

        /**
         * @brief Read a string from the uart
         * 
         * @warning This function is blocking
         * 
         * @return std::string  The string read
         */
        std::string readLine();

    private:

        // ** Members **
        uart_inst_t* m_uart;        // uart instance
        uint m_pinTx;
        uint m_pinRx;
        uint m_baudrate;
    };
}

