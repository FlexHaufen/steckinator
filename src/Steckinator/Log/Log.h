/**
 * @file Log.h
 * @author flexhaufen
 * @brief Simple logger
 * @version 0.1
 * @date 2026-03-05
 * 
 * @copyright Copyright (c) 2026
 * 
 */



#pragma once
#include <stdio.h>

#define LOG_ENABLE      true


#if LOG_ENABLE

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