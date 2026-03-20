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
#include <pico/stdlib.h>
#include "Log/Log.h"

#include "Driver/TMC2209Driver.h"
#include "Config.h"

/**
 * @brief Main
 * 
 */
int main() {
    stdio_init_all();


    //LOG_WAIT_FOR_USB;
    //LOG_INFO("Initializing");


    TMC2209Driver m_0({
        GPIO_M0_STEP,
        GPIO_M0_DIR,
        GPIO_M0_DIAG,
        GPIO_M_EN,
        1
    });

    //TMC2209Driver m_1({
    //    GPIO_M1_STEP,
    //    GPIO_M1_DIR,
    //    GPIO_M1_DIAG,
    //    GPIO_M_EN,
    //    1
    //});


    m_0.setPosition(10);

    while (true){}
    

    return 0;
}
