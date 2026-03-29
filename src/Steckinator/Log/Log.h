/**
 * @file Log.h
 * @brief Simple logger
 * @version 0.1
 * @date 2026-03-05
 * 
 * @copyright Copyright (c) 2026
 * 
 */
#pragma once

// *** INCLUDES ***
#include <stdio.h>

// *** DEFINES ***
#define LOG_ENABLE      true        // true: logger enabled


#if LOG_ENABLE

    /**
     * @brief Wait for serial connection
     * 
     * Setting this in the code during startup
     * will block the program there and wait until
     * the host is connected over serial to the Pi
     * 
     * @note use this for serial debugging
     * 
     */
    #define LOG_WAIT_FOR_USB    { while (!stdio_usb_connected()) sleep_ms(10); }

    #define LOG_DEBUG(fmt, ...)     printf("[DEBUG  ] " fmt "\n", ##__VA_ARGS__)
    #define LOG_INFO(fmt, ...)      printf("[INFO   ] " fmt "\n", ##__VA_ARGS__)
    #define LOG_WARN(fmt, ...)      printf("[WARNING] " fmt "\n", ##__VA_ARGS__)
    #define LOG_ERROR(fmt, ...)     printf("[ERROR  ] " fmt "\n", ##__VA_ARGS__)
#else

    #define LOG_WAIT_FOR_USB

    #define LOG_DEBUG(x)
    #define LOG_INFO(x)
    #define LOG_WARN(x)
    #define LOG_ERROR(x)
#endif