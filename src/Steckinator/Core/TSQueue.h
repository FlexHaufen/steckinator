/**
 * @file TSQueue.h
 * @brief Cpp wrapper for the native queue_t 
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

// *** NAMESPACE ***
namespace Steckinator {

    template<typename T, size_t Capacity>
    class TSQueue {
    public:
        static TSQueue& Instance() {
            static TSQueue instance;
            return instance;
        }

        /**
         * @brief Push a new item to the queue
         * 
         * @param item      The item to be pushed
         * @return true     The push was successful, else false
         */
        bool Push(const T& item) {
            return queue_try_add(&m_queue, &item);
        }

        /**
         * @brief Pop an item from the queue
         * 
         * @return std::optional<MotionEvent>       The item if, else `std::nullopt`
         */
        std::optional<T> Pop() {
            T item;
            if (queue_try_remove(&m_queue, &item)) {
                return item;
            }
            return std::nullopt;
        }

        /**
         * @brief Block and pop an item from the queue
         * 
         * @return  The popped item
         */
        T PopBlocking() {
            T item;
            queue_remove_blocking(&m_queue, &item);
            return item;
        }

    private:
        TSQueue() {
            queue_init(&m_queue, sizeof(T), Capacity);
        }

    private:
        // ** Members **
        queue_t m_queue;            // the underlying queue
    };


}  