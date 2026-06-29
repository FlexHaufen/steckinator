/**
 * @file MotionController.h
 * @brief MotionController
 * @version 0.1
 * @date 2026-03-28
 * 
 * @copyright Copyright (c) 2026
 * 
 */

#pragma once

// *** INCLUDES ***

#include <functional>
#include <atomic>

#include "Steckinator/Config.h"

#include "Steckinator/Motion/MotionEvent.h"
#include "Steckinator/Motion/MotionQueue.h"
#include "Steckinator/Driver/Stepper/StepperMotor.h"
#include "Steckinator/Driver/Servo/Servo.h"
#include "Steckinator/Driver/Switch/Switch.h"
#include "Steckinator/Driver/Led/Led.h"
#include "Steckinator/Driver/VacuumPump/VacuumPump.h"


// *** NAMESPACE ***
namespace Steckinator {
    
    class MotionController {
    public:
  
        /**
         * @brief Initialize the controller
         * 
         */
        void Init();

        /**
         * @brief Update MotorController
         * 
         * This needs to be called as fast as
         * possible in the super loop
         * 
         * @note This function is non-blocking
         * 
         */
        void Update();

        /**
         * @brief Enable all motors
         * 
         * The enable pin is shared
         * 
         */
        static void EnableMotors() { gpio_put(GPIO_M_EN, false); /* false means enable */ }

        /**
         * @brief Disable all motors
         * 
         * The enable pin is shared
         * 
         */
        static void DisableMotors() { gpio_put(GPIO_M_EN, true); }

        /**
         * @brief Request an emergency stop from any thread/core
         *
         * The stop itself is executed in Update() on the motion core.
         */
        static void RequestReset();
    
    private:
    
        void StartLinearMove(const MotionEvent& e);
        void StartHoming();

        /**
         * @brief Checks if all motors are idle
         * 
         * Idle means the motor is currently not 
         * executing a move.
         * 
         * @return true: motors are idle, else false
         */
        bool AreMotorsIdle();
    
        /**
         * @brief Execute a move
         * 
         * @param e     The MotionEvent to be executed
         */
        void ExecuteCommand(const MotionEvent& e);

        /**
         * @brief Execute the homing command
         * 
         */
        void ExecuteCommand_Homing();
    
        /**
         * @brief Abort all active motion immediately
         *
         */
        void ExecuteReset();

    private:

        // ** Members **

        enum class State {
            IDLE,
            EXECUTING_MOVE,
            EXECUTING_HOMING
        } m_state = State::IDLE;            // state of the motion controller


        enum class HomingPhase {
            PHASE_Y,                        // moving toward Y endstop
            PHASE_X,                        // moving toward X endstop  
            PHASE_Z,                        // moving toward Z endstop
            PHASE_C,                        // moving toward C endstop
            PHASE_DONE
        } m_homingPhase = HomingPhase::PHASE_Y;


        StepperMotor   m_motorA;            // Motor A (core xy)
        StepperMotor   m_motorB;            // Motor B (core xy)
        StepperMotor   m_motorC;            // Motor C (rotation)
    
        Servo          m_servoZ;            // Servo Z

        Switch         m_swX;
        Switch         m_swY;
        Switch         m_swC;

        VacuumPump     m_vacuumPump;        // Vacuum Pump

    
        Led m_led_status;

        float m_posX = 0.0f;                // [mm]
        float m_posY = 0.0f;                // [mm]
        float m_posC = 0.0f;                // [deg]
        float m_posZ = MOTION_CONTROLLER_MAX_Z_ANGLE;                // [deg]

        static std::atomic<bool> s_resetRequested;
    };

}
