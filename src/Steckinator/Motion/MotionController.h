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
    
     
    struct MotionControllerConfig {
        float stepsPerMm    = 80.0f;   ///< Applies to all axes
        float rapidSpeedMmS = 100.0f;  ///< G0 speed
        float feedSpeedMmS  = 30.0f;   ///< G1 default (overridden by MotionEvent::f)
        float homeSpeedMmS  = 15.0f;   ///< Homing approach speed
        float homeBackoffMm = 3.0f;    ///< Back-off after endstop triggers
    };

    class MotionController {
    public:
  
        void Init();
    
        /// Call this as fast as possible from the main loop. Non-blocking.
        void Update();
    
        bool  IsIdle()      const { return m_state == State::IDLE; }
        bool  IsExecuting() const { return m_state != State::IDLE; }

        void Push(const MotionEvent& event) {m_queue.Push(event); };
    
    private:
        enum class State { IDLE, EXECUTING, HOMING, HOMING_BACKOFF };
    
        void StartMove(const MotionEvent& ev, float speedMmS);
        void StartHome();
        bool AllIdle();
    
        void ExecuteMove(const MotionEvent& ev);

        Steps ToSteps(float mm) const {
            return static_cast<Steps>(std::roundf(mm * STEPS_PER_MM_XY));
        }
    
        MotionQueue    m_queue;
        StepperMotor   m_motorA;
        StepperMotor   m_motorB;
    
        Switch         m_swX;
        Switch         m_swY;
    
    
        State m_state = State::IDLE;
    
        float m_posX = 0.0f;
        float m_posY = 0.0f;
        float m_posZ = 0.0f;
        float m_posE = 0.0f;
    };

}
