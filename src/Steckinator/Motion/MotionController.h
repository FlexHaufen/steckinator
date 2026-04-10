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

#include <cmath>

#include "Steckinator/Config.h"

#include "Steckinator/Motion/MotionEvent.h"
#include "Steckinator/Driver/Stepper/StepperMotor.h"
#include "Steckinator/Driver/Switch/Switch.h"
#include "Steckinator/Driver/Led/Led.h"


// *** NAMESPACE ***
namespace Steckinator {
    
    class MotionController {
    public:
  
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
         * @brief Push a new motion event to the queue
         * 
         * @param event the event to be pushed
         */
        void Push(const MotionEvent& event) {m_queue.Push(event); };
    
    private:
    
        void StartLinearMove(const MotionEvent& e);
        void StartHome();

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
        void ExecuteMove(const MotionEvent& e);

        Steps ToSteps(float mm) const { return static_cast<Steps>(std::roundf(mm * MOTION_CONTROLLER_STEPS_PER_MM_XY)); }
    
    private:

        // ** Members **

        MotionQueue    m_queue;             // underlying queue

        enum class State {
            IDLE,
            EXECUTING,
            HOMING,
            HOMING_BACKOFF
        } m_state = State::IDLE;            // state of the motion controller

        StepperMotor   m_motorA;            // Motor A
        StepperMotor   m_motorB;            // Motor B
    
        Switch         m_swX;
        Switch         m_swY;
    
        Led m_led_status;

        float m_posX = 0.0f;
        float m_posY = 0.0f;
        //float m_posZ = 0.0f;
        //float m_posE = 0.0f;
    };

}
