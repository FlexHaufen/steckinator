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
#include "pico/stdlib.h"
#include "Log/Log.h"

// *** DEFINES ***
#define GPIO_DEBUG_LED          25


/**
 * @brief Main
 * 
 */
int main() {
    stdio_init_all();


    LOG_WAIT_FOR_USB;
    LOG_INFO("Initializing");


    gpio_init(GPIO_DEBUG_LED);
    gpio_set_dir(GPIO_DEBUG_LED, GPIO_OUT);
    gpio_put(GPIO_DEBUG_LED, true);



    while (true) {


        sleep_ms(1000);
    }

    return 0;
}
