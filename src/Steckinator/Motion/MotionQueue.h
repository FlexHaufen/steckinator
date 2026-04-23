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
#include "Steckinator/Config.h"
#include "Steckinator/Motion/MotionEvent.h"
#include "Steckinator/Core/TSQueue.h"

// *** NAMESPACE ***
namespace Steckinator {

    using MotionQueue = TSQueue<MotionEvent, MOTION_CONTROLLER_MOTION_QUEUE_SIZE>;

}  