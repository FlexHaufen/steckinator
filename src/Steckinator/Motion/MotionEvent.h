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

#include <string>

namespace Steckinator {




    enum class MotionType {
        INVALID = 0,
        G0,
        G1
    };


    struct MotionEvent {

        MotionType type = MotionType::INVALID;

        float x = 0;
        float y = 0;
        float z = 0;
        float f = 0;    // feedrate

    };


}