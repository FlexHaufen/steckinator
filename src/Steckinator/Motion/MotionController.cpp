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

        LOG_DEBUG("init");
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
                // wait until are motors are idle again
                if (AllIdle()) {
                    m_state = State::IDLE;
                    //LOG_INFO("Idle");
                }
                break;

            default:
                break;
        }
    }

    void MotionController::ExecuteMove(const MotionEvent& e) {

        switch (e.type) {
            case MotionType::G0:
                StartMove(e, 200);
                break;
            
            case MotionType::G28:
                StartHome();
                break;
            default:
                break;
        }
    }

    void MotionController::StartMove(const MotionEvent& ev, float speedMmS) {
        float dX = ev.x - m_posX;
        float dY = ev.y - m_posY;

        // CoreXY: A = ΔX + ΔY,  B = ΔX - ΔY
        Steps stepsA = ToSteps(dY + dX);
        Steps stepsB = ToSteps(dY - dX);

        float speedSteps = speedMmS * STEPS_PER_MM_XY;
        m_motorA.SetSpeed(speedSteps);
        m_motorB.SetSpeed(speedSteps);

        if (stepsA != 0) m_motorA.MoveRelative(stepsA);
        if (stepsB != 0) m_motorB.MoveRelative(stepsB);

        m_posX = ev.x;
        m_posY = ev.y;

    }

    void MotionController::StartHome() {
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