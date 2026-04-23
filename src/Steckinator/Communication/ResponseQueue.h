/**
 * @file ResponseQueue.h
 * @brief Queue for storing the Responses to the MainController
 * @version 0.1
 * @date 2026-04-23
 * 
 * @copyright Copyright (c) 2026
 * 
 */
#pragma once

// *** INCLUDES ***
#include "Steckinator/Config.h"
#include "Steckinator/Core/TSQueue.h"

// *** NAMESPACE ***
namespace Steckinator {

    enum class Response {
        OK = 0,
        ERROR = 1
    };


    using ResponseQueue = TSQueue<Response, COMMUNICATION_RESPONSE_QUEUE_SIZE>;

}  