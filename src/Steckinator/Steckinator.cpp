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
#include "Config.h"
#include "Log/Log.h"

#include "GCodeParser/GCodeParser.h"
#include "Motion/MotionController.h"
#include "Driver/Uart/Uart.h"

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
        /*
        Uart uart(uart1, GPIO_UART1_TX, GPIO_UART1_RX, 115200);
        uart.begin();

        while (true) {
            auto c = uart.readLine();
            MotionQueue::Instance().Push(GCodeParser::ParseLine(c));

            sleep_ms(10000);

            LOG_DEBUG("ok");
            uart.writeLine("ok");

        }
        */

        MotionQueue::Instance().Push(GCodeParser::ParseLine("G28"));
        MotionQueue::Instance().Push(GCodeParser::ParseLine("G1 X200 F800"));
        MotionQueue::Instance().Push(GCodeParser::ParseLine("G1 Y200 F800"));
        MotionQueue::Instance().Push(GCodeParser::ParseLine("G1 X10 F800"));
        MotionQueue::Instance().Push(GCodeParser::ParseLine("G1 Y10 F800"));
        MotionQueue::Instance().Push(GCodeParser::ParseLine("G1 X300 Y300 F800"));
        MotionQueue::Instance().Push(GCodeParser::ParseLine("G1 X10 Y10 F800"));


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