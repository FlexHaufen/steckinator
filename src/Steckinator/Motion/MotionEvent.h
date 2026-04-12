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
        MotionQueue() = default;

        // Push a new motion command
        void Push(MotionEvent event) {
            // TODO (flex): Invalid event types must not be pushed!
            m_motionQueue.push_back(event);
        }

        // Non-blocking pop - returns the next command if available
        std::optional<MotionEvent> Pop() {
            if (m_motionQueue.empty()) {
                return std::nullopt;
            }

            MotionEvent front = std::move(m_motionQueue.front());
            m_motionQueue.pop_front();
            return front;
        }

    private:

        // ** Members **
        
        std::deque<MotionEvent> m_motionQueue;
    };

    /*

    Use the below queue for core safety
    implement with the same architecture as above
    but use a underlying queue_t 

    class MotionQueue {
    public:
        MotionQueue() {
            queue_init(&m_queue, sizeof(MotionEvent), 32); // capacity = 32
        }

        void Push(const MotionEvent& event) {
            queue_add_blocking(&m_queue, &event);
        }

        bool Pop(MotionEvent& out) {
            return queue_try_remove(&m_queue, &out);
        }

    private:
        queue_t m_queue;
    };
    */

}