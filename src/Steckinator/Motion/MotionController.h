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
    
    private:
    
        void StartLinearMove(const MotionEvent& e);
        void StartHoming();
        void ContinueHomingXY();
        void StartHomingXYDiagonal();
        void StartHomingXOnly();
        void StartHomingYOnly();
        void RegisterHomingCallbacksXY();

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

    
    private:

        // ** Members **

        enum class State {
            IDLE,
            EXECUTING_MOVE,
            EXECUTING_HOMING
        } m_state = State::IDLE;            // state of the motion controller


        enum class HomingPhase {
            PHASE_XY_DIAGONAL,              // home X and Y simultaneously (towards corner)
            PHASE_X_ONLY,                   // X remaining
            PHASE_Y_ONLY,                   // Y remaining
            PHASE_DONE
        } m_homingPhase = HomingPhase::PHASE_DONE;

        struct HomingState {
            bool axis_x_homed = false;
            bool axis_y_homed = false;
            bool axis_c_homed = false;

            bool Homed() { return axis_x_homed && axis_y_homed && axis_c_homed; }
        } m_homingState;



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

    };

}
