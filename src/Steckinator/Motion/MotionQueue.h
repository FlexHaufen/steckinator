/**
 * @file MotionQueue.h
 * @brief Queue for storing the MotionEvents
 * @version 0.1
 * @date 2026-04-23
 * 
 * @copyright Copyright (c) 2026
 * 
 */
#pragma once

// *** INCLUDES ***
#include <optional>
#include <pico/util/queue.h>

#include "Steckinator/Config.h"
#include "Steckinator/Motion/MotionEvent.h"

// *** NAMESPACE ***
namespace Steckinator {

  class MotionQueue {
    public:

        /**
         * @brief Get the class instance
         * 
         * @return MotionQueue& 
         */
        static MotionQueue& Instance() {
            static MotionQueue instance;   // created once, thread-safe (C++11)
            return instance;
        }

        /**
         * @brief Push a new event to the queue
         * 
         * @param event     The event to be pushed
         * @return true     The push was successful, else false
         */
        bool Push(const MotionEvent& event) {
            if (!queue_try_add(&m_queue, &event)) {
                // TODO handle overflow (log, drop, etc.)
                return false;
            }
            return true;
        }

        /**
         * @brief Pop an event from the queue
         * 
         * @return std::optional<MotionEvent>       the event if, else `std::nullopt`
         */
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
        queue_t m_queue;            // the underlying queue
    };


}  