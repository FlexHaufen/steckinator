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

#include <stdio.h>
#include "pico/stdlib.h"


#define GPIO_DEBUG_LED          25



int main() {
    stdio_init_all();


    gpio_init(GPIO_DEBUG_LED);
    gpio_set_dir(GPIO_DEBUG_LED, GPIO_OUT);
    gpio_put(GPIO_DEBUG_LED, true);



    while (true) {
        printf("Hello, world!\n");
        sleep_ms(1000);
    }

    return 0;
}
