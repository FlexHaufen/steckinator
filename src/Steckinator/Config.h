/**
 * @file Config.h
 * @brief Main configuration file
 * @version 0.1
 * @date 2026-03-12
 * 
 * @copyright Copyright (c) 2026
 * 
 */
#pragma once

// *** DEFINES ***

// ----------------------------------------------------------------------------------------------------
// DEBUG
// ----------------------------------------------------------------------------------------------------
#define STECKINATOR_RUN_DEBUG_PROGRAM               false           // Run a predefined program insted of parsing the commands from the MainController

// ----------------------------------------------------------------------------------------------------
// MOTION CONTROLLER
// ----------------------------------------------------------------------------------------------------
#define MOTION_CONTROLLER_STEPS_PER_MM_XY               40          // [steps/mm]   (motor_steps_per_rev * microsteps) / mm_per_rev = (200 * 8) / 40
#define MOTION_CONTROLLER_DEFAULT_FEED_RATE_G1         100          // [mm/s]
#define MOTION_CONTROLLER_DEFAULT_FEED_RATE_G28        100          // [mm/s]

#define MOTION_CONTROLLER_HOMING_DISTANCE              400          // Number of steps until homing position is guaranteed [mm]
#define MOTION_CONTROLLER_MOTION_QUEUE_SIZE             10          // How many events can be saved in the queue

// ----------------------------------------------------------------------------------------------------
// STEPPER MOTOR
// ----------------------------------------------------------------------------------------------------
#define STEPPER_MOTOR_ACCELERATION                 8000.0f          // acceleration of the stepper [mm/s^2]

// ----------------------------------------------------------------------------------------------------
// COMMUNICATION
// ----------------------------------------------------------------------------------------------------
#define COMMUNICATION_RESPONSE_QUEUE_SIZE               10          // How many responses can be saved in the queue


// ----------------------------------------------------------------------------------------------------
// GPIOs
// ----------------------------------------------------------------------------------------------------

// COM
#define GPIO_UART0_TX                       12
#define GPIO_UART0_RX                       13

#define GPIO_UART1_TX                       4
#define GPIO_UART1_RX                       5

// MOTORS
// Servo PWM
#define GPIO_SERVO0_PWM                     0
#define GPIO_SERVO1_PWM                     1

// DC Motor
#define GPIO_M0_DC_OUT1                     9
#define GPIO_M0_DC_OUT2                     10
#define GPIO_M_EN                           11

// Stepper
#define GPIO_M0_DIAG                        20
#define GPIO_M0_STEP                        21
#define GPIO_M0_DIR                         22

#define GPIO_M1_DIAG                        17
#define GPIO_M1_STEP                        18
#define GPIO_M1_DIR                         19

#define GPIO_M2_DIAG                        14
#define GPIO_M2_STEP                        15
#define GPIO_M2_DIR                         16

// SWITCHES
// End switches
#define GPIO_SW_0                           3
#define GPIO_SW_1                           6
#define GPIO_SW_2                           7
#define GPIO_SW_3                           8
// CTRL
#define GPIO_CTRL                           26

// LEDS
#define GPIO_DEBUG_LED                      25
#define GPIO_LED_0                          27
#define GPIO_LED_1                          28


// ----------------------------------------------------------------------------------------------------
// GCODE
// ----------------------------------------------------------------------------------------------------

#define GCODE_COMMAND_G   'G'
#define GCODE_COMMAND_M   'M'

#define GCODE_AXIS_X      'X'
#define GCODE_AXIS_Y      'Y'
#define GCODE_AXIS_Z      'Z'
#define GCODE_AXIS_A      'A'

#define GCODE_FEED_RATE   'F'