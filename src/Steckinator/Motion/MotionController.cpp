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

#include "Steckinator/Config.h"
#include "Steckinator/Log/Log.h"
#include "Steckinator/Helper/Types.h"

#include <cmath>
#include <algorithm>
#include "Steckinator/Communication/ResponseQueue.h"
#include "Steckinator/Core/GpioIrqManager.h"

// *** NAMESPACE ***
namespace Steckinator {

    std::atomic<bool> MotionController::s_emergencyStopRequested{false};


    void MotionController::Init() {

        m_led_status.Init(GPIO_LED_1);
        m_state = State::IDLE;

        auto offset = StepperMotor::GetStepperProgramOffset(pio0);
        m_motorA.Init(pio0, 0, offset, GPIO_M0_STEP, GPIO_M0_DIR, MOTION_CONTROLLER_STEPS_PER_MM_XY);
        m_motorB.Init(pio0, 1, offset, GPIO_M1_STEP, GPIO_M1_DIR, MOTION_CONTROLLER_STEPS_PER_MM_XY);
        m_motorC.Init(pio0, 2, offset, GPIO_M2_STEP, GPIO_M2_DIR, MOTION_CONTROLLER_STEPS_PER_DEG_C);

        m_servoZ.Init(GPIO_SERVO1_PWM, 180.f, 0.f);

        m_swX.Init(GPIO_SW_0);
        m_swY.Init(GPIO_SW_1);
        m_swC.Init(GPIO_SW_2);

        m_vacuumPump.Init(GPIO_M0_DC_OUT1, GPIO_M0_DC_OUT2);

        return;
    }


    void MotionController::Update() {
        if (s_emergencyStopRequested.exchange(false)) {
            ExecuteEmergencyStop();
            return;
        }

        switch (m_state) {

            case State::IDLE: {
                auto e = MotionQueue::Instance().Pop();
                if (!e) {
                    break;
                }

                m_led_status.On();
                EnableMotors();
                ExecuteCommand(e.value());                
                break;
            }

            case State::EXECUTING_MOVE:
                // wait until the fat ass motors are idle again
                if (AreMotorsIdle()) {
                    m_state = State::IDLE;
                    m_led_status.Off();
                    //DisableMotors();
                    ResponseQueue::Instance().Push(Response::OK);           // FIXME (inj): This is currently also done in ExecuteCommand_Homing
                                                                            //              could this be done once? and not twice?

                }
                break;

            case State::EXECUTING_HOMING:
                ContinueHomingXY();
                if (m_homingState.Homed() && AreMotorsIdle()) {
                    m_posX = 0.f;
                    m_posY = 0.f;

                    m_state = State::IDLE;

                    //DisableMotors();
                    GpioIrqManager::instance().unregisterCallback(m_swX.GetPin());
                    GpioIrqManager::instance().unregisterCallback(m_swY.GetPin());
                    GpioIrqManager::instance().unregisterCallback(m_swC.GetPin());
                    ResponseQueue::Instance().Push(Response::OK);
                    m_led_status.Off();
                }

                break;

            default:
                break;
        }
    }

    void MotionController::RequestEmergencyStop() {
        s_emergencyStopRequested.store(true);
    }

    void MotionController::ExecuteCommand(const MotionEvent& e) {

        switch (e.command) {
            case MotionCommand::G0:         // intended fallthrough
            case MotionCommand::G1:
                StartLinearMove(e);
                m_state = State::EXECUTING_MOVE;
                break;
            
            case MotionCommand::G28:
                StartHoming();
                m_state = State::EXECUTING_HOMING;
                break;

            case MotionCommand::M10:
                m_vacuumPump.On();
                m_state = State::EXECUTING_MOVE;     
                break;

            case MotionCommand::M11:
                m_vacuumPump.Off();
                m_state = State::EXECUTING_MOVE;
                break;

            case MotionCommand::INVALID:    // intended fallthrough
            default:
                break;
        }
    }
    void MotionController::ExecuteEmergencyStop() {
        m_motorA.Stop();
        m_motorB.Stop();
        m_motorC.Stop();

        MotionQueue::Instance().Clear();

        m_state = State::IDLE;
        m_homingState.Clear();
        m_led_status.Off();
        DisableMotors();

        LOG_ERROR("Emergency stop executed");
    }


    void MotionController::StartLinearMove(const MotionEvent& e) {

        // Calculate the relative movement
        float dX = e.x.value_or(m_posX) - m_posX;
        float dY = e.y.value_or(m_posY) - m_posY;
        float dC = e.c.value_or(m_posC) - m_posC;
        
        // Convert to steps and degrees
        Steps stepsA = m_motorA.ToSteps(dY + dX);                                                   // Core XY: A = ΔX + ΔY
        Steps stepsB = m_motorB.ToSteps(dY - dX);                                                   // Core XY: B = ΔX - ΔY
        Steps stepsC = m_motorC.ToSteps(dC);                                                        // rotation

        // Update absolute position
        m_posX += dX;
        m_posY += dY;
        m_posZ = std::clamp(MOTION_CONTROLLER_MIN_Z_ANGLE, e.z.value_or(m_posZ), MOTION_CONTROLLER_MAX_Z_ANGLE);
        m_posC += dC;
        
        // Queue movements
        if (stepsA != 0) { m_motorA.MoveRelative(stepsA, e.f.value_or(MOTION_CONTROLLER_DEFAULT_FEED_RATE_G1),  StepperMotor::AccelerationMethod::RAMP); }
        if (stepsB != 0) { m_motorB.MoveRelative(stepsB, e.f.value_or(MOTION_CONTROLLER_DEFAULT_FEED_RATE_G1),  StepperMotor::AccelerationMethod::RAMP); }
        m_servoZ.SetAngle(m_posZ);
        if (stepsC != 0) { m_motorC.MoveRelative(stepsC, MOTION_CONTROLLER_DEFAULT_FEED_RATE_C, StepperMotor::AccelerationMethod::NONE); }

        return;
    }

    void MotionController::StartHoming() {

        // Reset/initialize homing state from current switch levels.
        m_homingState.axis_x_homed = m_swX.Get();
        m_homingState.axis_y_homed = m_swY.Get();
        m_homingState.axis_c_homed = m_swC.Get();

        // unregister any existing callbacks for the switches, to avoid double registration.
        GpioIrqManager::instance().unregisterCallback(m_swX.GetPin());
        GpioIrqManager::instance().unregisterCallback(m_swY.GetPin());
        GpioIrqManager::instance().unregisterCallback(m_swC.GetPin());

        if (!m_homingState.axis_x_homed || !m_homingState.axis_y_homed) {
            RegisterHomingCallbacksXY();
        }

        // Home X/Y: first try diagonal move towards the corner.
        if (!m_homingState.axis_x_homed && !m_homingState.axis_y_homed) {
            StartHomingXYDiagonal();
        }
        else if (!m_homingState.axis_x_homed) {
            StartHomingXOnly();
        }
        else if (!m_homingState.axis_y_homed) {
            StartHomingYOnly();
        }
        else {
            m_homingPhase = HomingPhase::PHASE_DONE;
        }

        // Home C independent in parallel.
        if (!m_homingState.axis_c_homed) {
            m_motorC.MoveRelative(-m_motorC.ToSteps(MOTION_CONTROLLER_HOMING_DISTANCE), MOTION_CONTROLLER_DEFAULT_FEED_RATE_G28, StepperMotor::AccelerationMethod::NONE);
            GpioIrqManager::instance().registerCallback(
                m_swC.GetPin(), 
                GPIO_IRQ_EDGE_RISE | GPIO_IRQ_EDGE_FALL,
                [this](uint gpio, uint32_t events) {
                    UNUSED(gpio); UNUSED(events);

                    if (!m_swC.Get()) {
                        return;
                    }
                    m_motorC.Stop();
                    m_homingState.axis_c_homed = true;
                    GpioIrqManager::instance().unregisterCallback(m_swC.GetPin());
                }
            );
        }

        return;
    }

    bool MotionController::AreMotorsIdle() {
        return (!m_motorA.IsBusy() && !m_motorB.IsBusy() && !m_motorC.IsBusy());
    }

    void MotionController::ContinueHomingXY() {
        if (!AreMotorsIdle()) {
            return;
        }

        if (m_homingState.axis_x_homed && m_homingState.axis_y_homed) {
            m_homingPhase = HomingPhase::PHASE_DONE;
            return;
        }

        if (m_homingState.axis_x_homed && !m_homingState.axis_y_homed && m_homingPhase != HomingPhase::PHASE_Y_ONLY) {
            StartHomingYOnly();
            return;
        }

        if (!m_homingState.axis_x_homed && m_homingState.axis_y_homed && m_homingPhase != HomingPhase::PHASE_X_ONLY) {
            StartHomingXOnly();
            return;
        }
    }

    void MotionController::StartHomingXYDiagonal() {
        m_homingPhase = HomingPhase::PHASE_XY_DIAGONAL;

        // CoreXY mapping:
        // X- => A-, B-
        // Y- => A-, B+
        // Combined (X- and Y- simultaneously) => A-, B~0 (corner move)
        const Steps stepsA = -m_motorA.ToSteps(2.0f * MOTION_CONTROLLER_HOMING_DISTANCE);
        m_motorA.MoveRelative(stepsA, MOTION_CONTROLLER_DEFAULT_FEED_RATE_G28, StepperMotor::AccelerationMethod::NONE);
    }

    void MotionController::StartHomingXOnly() {
        m_homingPhase = HomingPhase::PHASE_X_ONLY;
        m_motorA.MoveRelative(-m_motorA.ToSteps(MOTION_CONTROLLER_HOMING_DISTANCE), MOTION_CONTROLLER_DEFAULT_FEED_RATE_G28, StepperMotor::AccelerationMethod::NONE);
        m_motorB.MoveRelative(-m_motorB.ToSteps(MOTION_CONTROLLER_HOMING_DISTANCE), MOTION_CONTROLLER_DEFAULT_FEED_RATE_G28, StepperMotor::AccelerationMethod::NONE);
    }

    void MotionController::StartHomingYOnly() {
        m_homingPhase = HomingPhase::PHASE_Y_ONLY;
        m_motorA.MoveRelative(-m_motorA.ToSteps(MOTION_CONTROLLER_HOMING_DISTANCE), MOTION_CONTROLLER_DEFAULT_FEED_RATE_G28, StepperMotor::AccelerationMethod::NONE);
        m_motorB.MoveRelative( m_motorB.ToSteps(MOTION_CONTROLLER_HOMING_DISTANCE), MOTION_CONTROLLER_DEFAULT_FEED_RATE_G28, StepperMotor::AccelerationMethod::NONE);
    }

    void MotionController::RegisterHomingCallbacksXY() {
        GpioIrqManager::instance().registerCallback(
            m_swX.GetPin(),
            GPIO_IRQ_EDGE_RISE | GPIO_IRQ_EDGE_FALL,
            [this](uint gpio, uint32_t events) {
                UNUSED(gpio); UNUSED(events);

                if (!m_swX.Get()) {
                    return;
                }

                if (m_homingState.axis_x_homed) {
                    return;
                }

                m_homingState.axis_x_homed = true;
                m_motorA.Stop();
                m_motorB.Stop();
            }
        );

        GpioIrqManager::instance().registerCallback(
            m_swY.GetPin(),
            GPIO_IRQ_EDGE_RISE | GPIO_IRQ_EDGE_FALL,
            [this](uint gpio, uint32_t events) {
                UNUSED(gpio); UNUSED(events);

                if (!m_swY.Get()) {
                    return;
                }

                if (m_homingState.axis_y_homed) {
                    return;
                }

                m_homingState.axis_y_homed = true;
                m_motorA.Stop();
                m_motorB.Stop();
            }
        );
    }

} 