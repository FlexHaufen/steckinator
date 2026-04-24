/**
 * @file Steckinator.cpp
 * @brief Implementation of Steckinator
 * @version 0.1
 * @date 2026-03-27
 * 
 * @copyright Copyright (c) 2026
 * 
 */

// *** INCLUDES ***
#include "Steckinator/Steckinator.h"

#include <pico/stdlib.h>
#include <pico/multicore.h>

#include "Steckinator/Config.h"
#include "Steckinator/Log/Log.h"

#include "Steckinator/GCodeParser/GCodeParser.h"
#include "Steckinator/Motion/MotionController.h"
#include "Steckinator/Motion/MotionQueue.h"
#include "Steckinator/Driver/Uart/Uart.h"
#include "Steckinator/Communication/ResponseQueue.h"

// *** NAMESPACE ***
namespace Steckinator {



    Steckinator::Steckinator() {
        stdio_init_all();
        //LOG_WAIT_FOR_USB;
        LOG_INFO("Setup");

        m_led_power.Init(GPIO_DEBUG_LED);
        m_led_power.On();
    }

    void Steckinator::Run() {

        multicore_launch_core1(Steckinator::Core1Run);
        Core0Run(); // does not return
        
    }

    void Steckinator::Core0Run() {

        #if STECKINATOR_RUN_DEBUG_PROGRAM
            
            MotionQueue::Instance().Push(GCodeParser::ParseLine("G28"));
            //MotionQueue::Instance().Push(GCodeParser::ParseLine("M10"));
            MotionQueue::Instance().Push(GCodeParser::ParseLine("G1 X200 F1000"));
            MotionQueue::Instance().Push(GCodeParser::ParseLine("G1 Y200 F1000"));
            //MotionQueue::Instance().Push(GCodeParser::ParseLine("M11"));
            MotionQueue::Instance().Push(GCodeParser::ParseLine("G1 X10 F1000"));
            MotionQueue::Instance().Push(GCodeParser::ParseLine("G1 Y10 F1000"));
            MotionQueue::Instance().Push(GCodeParser::ParseLine("G1 X300 Y300 F1000"));
            MotionQueue::Instance().Push(GCodeParser::ParseLine("G1 X10 Y10 F1000"));
        
        #else
        
            Uart uart(uart1, GPIO_UART1_TX, GPIO_UART1_RX, 115200);
            uart.begin();

            while (true) {

                // wait for command
                auto c = uart.readLine();           // blocking
                MotionQueue::Instance().Push(GCodeParser::ParseLine(c));

                // wait for execution to finish
                auto response = ResponseQueue::Instance().PopBlocking();    // blocking
                uart.writeLine(( response == Response::OK) ? "ok" : "error");
            }

        #endif

        // never leave
        for (;;) {}
    }

    void Steckinator::Core1Run() {

        // TODO (flex): Move this shit to the StepperMotor driver
        // currently this is used for enabling the drivers
        // as the enable pin is common
        gpio_init(GPIO_M_EN);
        gpio_set_dir(GPIO_M_EN, GPIO_OUT);
        gpio_put(GPIO_M_EN, false);

        MotionController mc;
        mc.Init();
        
        while (true) {
            mc.Update();
            sleep_ms(10);
        }

        // never leave
        for (;;) {}
    }
}