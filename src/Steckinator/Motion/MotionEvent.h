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
        G0,                     // Rapid positioning    [x, y, z, a]
        G1,                     // Linear Interpolation [x, y, z, a, f]
        G28,                    // Home all axes
        M10,                    // Enable gripper
        M11                     // Disable gripper
    };

    /**
     * @brief Motion event
     * 
     */
    struct MotionEvent {

        MotionCommand command = MotionCommand::INVALID;

        std::optional<float> x = std::nullopt;
        std::optional<float> y = std::nullopt;
        std::optional<float> z = std::nullopt;
        std::optional<float> a = std::nullopt;

        std::optional<float> f = std::nullopt;

    };

}