/**
 * @file GCodeParser.h
 * @brief GCodeParser
 * @version 0.1
 * @date 2026-03-05
 * 
 * @copyright Copyright (c) 2026
 * 
 */
#pragma once

// *** INCLUDES ***
#include <string>

#include "Steckinator/Motion/MotionEvent.h"

// *** NAMESPACE ***
namespace Steckinator {

    class GCodeParser {
    public:

        /**
         * @brief Creates a MotionEvent from a string
         * 
         * The command will be `INVALID` if no corresponding command is found
         * The axis parameters will be `std::nullopt` if no corresponding parameter is found
         * 
         * 
         * @param line              The string command e.g `"G01 X10 Y10 F10"`
         * @return MotionEvent      The resulting MotionEvent
         */
        MotionEvent ParseLine(const std::string& line);

    };


}