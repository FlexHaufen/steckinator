/**
 * @file Uart.cpp
 * @brief Implementation of Uart
 * @version 0.1
 * @date 2025-11-12
 * 
 * @copyright Copyright (c) 2025
 * 
 */

// *** INCLUDES ***
#include "Steckinator/Driver/Uart/Uart.h"

// *** NAMESPACE ***
namespace Steckinator {

    Uart::Uart(uart_inst_t *uart, uint pinTx, uint pinRx, uint baudrate)
        : m_uart(uart), m_pinTx(pinTx), m_pinRx(pinRx), m_baudrate(baudrate) {}

    void Uart::begin() {
        uart_init(m_uart, m_baudrate);
        gpio_set_function(m_pinTx, GPIO_FUNC_UART);
        gpio_set_function(m_pinRx, GPIO_FUNC_UART);
        uart_set_hw_flow(m_uart, false, false);
        uart_set_format(m_uart, 8, 1, UART_PARITY_NONE);
        uart_set_fifo_enabled(m_uart, false);
    }

    void Uart::writeLine(const std::string &s) {
        uart_puts(m_uart, s.c_str());
        uart_puts(m_uart, "\r\n");
    }

    int Uart::available() {
        return uart_is_readable(m_uart);
    }

    std::string Uart::readLine() {
        std::string result;

        // TODO (flex): Dangerous this could be blocking.
        //              Maybe use callback timer in case of hangup
        while (true) {
            if (uart_is_readable(m_uart)) {
                char c = uart_getc(m_uart);
                if (c == '\n') {
                    break;
                } else if (c != '\r') { // ignore carriage return
                    result += c;
                }
            }
        }
        return result;
    }

    void Uart::flush() {
        // Wait until transmit buffer is empty

        // TODO (flex): Dangerous this could be blocking.
        //              Maybe use callback timer in case of hangup
        while (!uart_is_writable(m_uart)) {
            tight_loop_contents();
        }
    }
        
}
