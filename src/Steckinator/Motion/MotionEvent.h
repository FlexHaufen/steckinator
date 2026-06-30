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
#include <optional>


// *** NAMESPACE ***
namespace Steckinator {

    /**
     * @brief Motion commands that can be used
     * 
     */
    enum class MotionCommand {
        INVALID = 0,
        G0,                     // Rapid positioning    [x, y, z, c]
        G1,                     // Linear Interpolation [x, y, z, c, f]
        G28,                    // Home all axes
        G29,                    // Home C axis
        M10,                    // Enable gripper
        M11                     // Disable gripper
    };

    /**
     * @brief Motion event
     * 
     */
    struct MotionEvent {

        MotionCommand command = MotionCommand::INVALID;     // GCode Command

        std::optional<float> x = std::nullopt;              // Absolute position X  
        std::optional<float> y = std::nullopt;              // Absolute position Y
        std::optional<float> z = std::nullopt;              // Absolute position Z
        std::optional<float> c = std::nullopt;              // Absolute position C  (rotation)

        std::optional<float> f = std::nullopt;              // Feedrate

    };

}