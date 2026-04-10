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
#include "pico/stdlib.h"
#include "hardware/uart.h"
#include "hardware/gpio.h"
#include <string>

// *** NAMESPACE ***
namespace Steckinator {

    /**
     * @brief Uart
     * 
     */
    class Uart {
    public:
        Uart(uart_inst_t* uart, uint txPin, uint rxPin, uint baudrate);

        void begin();
        void writeLine(const std::string& s);
        int available();
        std::string readLine();
        void flush();

    private:
        uart_inst_t* m_uart;
        uint m_pinTx;
        uint m_pinRx;
        uint m_baudrate;
    };
}

