/**
 * @file MotionEvent.h
 * @brief Types for Motion
 * @version 0.1
 * @date 2026-03-05
 * 
 * @copyright Copyright (c) 2026
 * 
 */
#pragma once

// *** INCLUDES ***
#include <deque>
#include <optional>

#include <pico/util/queue.h>

#include "Steckinator/Config.h"

// *** NAMESPACE ***
namespace Steckinator {


    enum class MotionCommand {
        INVALID = 0,
        G0,                     // Rapid positioning    [x, y, z, a]
        G1,                     // Linear Interpolation [x, y, z, a, f]
        G28,                    // Home all axes
        M10,                    // Enable gripper
        M11                     // Disable gripper
    };


    struct MotionEvent {

        MotionCommand command = MotionCommand::INVALID;

        std::optional<float> x = std::nullopt;
        std::optional<float> y = std::nullopt;
        std::optional<float> z = std::nullopt;
        std::optional<float> a = std::nullopt;

        std::optional<float> f = std::nullopt;

    };

    
    class MotionQueue {
    public:
     

        // Global access point
        static MotionQueue& Instance() {
            static MotionQueue instance;   // created once, thread-safe (C++11)
            return instance;
        }

        bool Push(const MotionEvent& event) {
            if (!queue_try_add(&m_queue, &event)) {
                // TODO handle overflow (log, drop, etc.)
                return false;
            }
            return true;
        }

        std::optional<MotionEvent> Pop() {
            MotionEvent event;
            if (queue_try_remove(&m_queue, &event)) {
                return event;              // value present
            }
            return std::nullopt;           // queue empty
        }

    private:

        MotionQueue() {
            queue_init(&m_queue, sizeof(MotionEvent), MOTION_CONTROLLER_QUEUE_SIZE);
        }

    private:
        // ** Members **
        queue_t m_queue;
    };
    

}