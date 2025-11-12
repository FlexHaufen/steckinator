/**
 * @file main.cpp
 * @author flexhaufen
 * @brief Entrypoint
 * @version 0.1
 * @date 2025-11-12
 * 
 * @copyright Copyright (c) 2025
 * 
 */


// *** INCLUDES ***
#include <stdio.h>
#include "pico/stdlib.h"

#include "Uart/Uart.h"

// *** DEFINES ***
#define GPIO_DEBUG_LED          25


/**
 * @brief Main
 * 
 */
int main() {

    stdio_init_all();

    gpio_init(GPIO_DEBUG_LED);
    gpio_set_dir(GPIO_DEBUG_LED, GPIO_OUT);
    gpio_put(GPIO_DEBUG_LED, true);

    // I dont know why the f**k but uart 1 is not working
    Uart uart(uart1, 4, 5 ,9600);
    uart.begin();

    while (true) {
        uart.writeLine(uart.readLine());
    }

    return 0;
}
