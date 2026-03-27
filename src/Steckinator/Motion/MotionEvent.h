/**
 * @file MotionEvent.h
 * @author flexhaufen
 * @brief Types for Motion
 * @version 0.1
 * @date 2026-03-05
 * 
 * @copyright Copyright (c) 2026
 * 
 */
#pragma once

#include <deque>
#include <optional>

namespace Steckinator {




    enum class MotionType {
        INVALID = 0,
        G0,                     // Rapid movement       [x, y, z, a]
        G1,                     //                      [x, y, z, a, f]
        G28,                    // Home all axes
        M10,                    // Enable gripper
        M11                     // Disable gripper
    };


    struct MotionEvent {

        MotionType type = MotionType::INVALID;

        float x = 0;
        float y = 0;
        float z = 0;
        float a = 0;
        float f = 0;    // GCODE_FEEDRATE

    };

    class MotionQueue {
    public:
        MotionQueue()  {}


        void Push(const MotionEvent& motionEvent) { m_motionQueue.emplace_back(); }
        //std::optional<MotionEvent> Pop() { return m_motionQueue.front(); }

    private:

        // ** Members **
        std::deque<MotionEvent> m_motionQueue;


    };

}