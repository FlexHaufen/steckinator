/**
 * @file MotionController.cpp
 * @brief Implementation of MotionController
 * @version 0.2
 * @date 2026-03-28
 *
 * @copyright Copyright (c) 2026
 */

// *** INCLUDES ***
#include "Steckinator/Motion/MotionController.h"
#include "Steckinator/Log/Log.h"

#include <cmath>

// *** NAMESPACE ***
namespace Steckinator {


    void MotionController::Init() {
        m_state      = State::IDLE;

        m_motorA.Init(pio0, 0, GPIO_M0_STEP, GPIO_M0_DIR);
        m_motorB.Init(pio0, 1, GPIO_M1_STEP, GPIO_M1_DIR);

        m_swX.Init(GPIO_SW_0);
        m_swY.Init(GPIO_SW_1);
    }


    void MotionController::Update() {
        switch (m_state) {

            case State::IDLE:
            {
                auto e = m_queue.Pop();
                if (!e) {
                    break;
                }

                ExecuteMove(e.value());                
                m_state = State::EXECUTING;
                break;
            }

            case State::EXECUTING:
                // wait until the fat ass motors are idle again
                if (AllIdle()) {
                    m_state = State::IDLE;
                }
                break;

            default:
                break;
        }
    }

    void MotionController::ExecuteMove(const MotionEvent& e) {

        switch (e.type) {
            case MotionType::G0:        // intended fallthrough
            case MotionType::G1:
                StartLinearMove(e);
                break;
            


            case MotionType::G28:
                StartHome();
                break;

            case MotionType::INVALID:   // intended fallthrough
            default:
                break;
        }
    }

    void MotionController::StartLinearMove(const MotionEvent& e) {

        float dX = e.x.value_or(m_posX) - m_posX;
        float dY = e.y.value_or(m_posY) - m_posY;
        
        // CoreXY: A = ΔX + ΔY,  B = ΔX - ΔY
        Steps stepsA = ToSteps(dY + dX);
        Steps stepsB = ToSteps(dY - dX);
        
        // calculate the speed for the stepper motors
        float steps_per_second = MOTION_CONTROLLER_STEPS_PER_MM_XY * e.f.value_or(MOTION_CONTROLLER_DEFAULT_FEED_RATE_G1); 
        m_motorA.SetSpeed(steps_per_second);
        m_motorB.SetSpeed(steps_per_second);

        if (stepsA != 0) { m_motorA.MoveRelative(stepsA); }
        if (stepsB != 0) { m_motorB.MoveRelative(stepsB); }

        if (e.x.has_value()) { m_posX = e.x.value(); }
        if (e.y.has_value()) { m_posY = e.y.value(); }
    }

    void MotionController::StartHome() {

        // TODO (flex): These parameters need to be adjusted
        //              Also put them into the Config.h
        m_motorA.SetSpeed(1000);
        m_motorB.SetSpeed(1000);

        while (!m_swY.Get()) {
            m_motorA.MoveRelative(-10);
            m_motorB.MoveRelative( 10);
        }
        
        while (!m_swX.Get()) {
            m_motorA.MoveRelative(-10);
            m_motorB.MoveRelative(-10);
        }

        m_motorA.SetPosition(0);
        m_motorB.SetPosition(0);

    }

    bool MotionController::AllIdle() {
        return (!m_motorA.IsBusy() && !m_motorB.IsBusy());
    }

} 