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

#include "Steckinator/Config.h"
#include "Steckinator/Motion/MotionEvent.h"

// *** NAMESPACE ***
namespace Steckinator {

    class GCodeParser {
    public:

        MotionEvent ParseLine(const std::string& line) {
            
            MotionEvent event{};
            event.type = MotionType::INVALID;

            // We create a pointer to the string here
            // in order to parse each character separately
            const char* p_line =line.c_str();

            while(*p_line) {
                
                switch (*p_line) {

                    case GCODE_COMMAND_G:
                        {
                            uint8_t g = std::atoi(p_line + 1);
                            switch (g) {
                                case  0: event.type == MotionType::G0; break;
                                case  1: event.type == MotionType::G1; break;
                                case 28: event.type == MotionType::G28; break;
                                
                                default: 
                                    LOG_ERROR("Received invalid G__");
                                    break;
                            }                        
                        }
                        break;

                    case GCODE_COMMAND_M:
                        {
                            uint8_t m = std::atoi(p_line + 1);
                            switch (m) {
                                case 10: event.type == MotionType::M10; break;
                                case 11: event.type == MotionType::M11; break;
                                
                                default: 
                                    LOG_ERROR("Received invalid M__");
                                    break;
                            }  
                        }
                    
                    case GCODE_AXIS_X:
                        event.x = std::atof(p_line + 1);
                        break;
                    case GCODE_AXIS_Y:
                        event.y = std::atof(p_line + 1);
                        break;
                    case GCODE_AXIS_Z:
                        event.z = std::atof(p_line + 1);
                        break;
                    case GCODE_AXIS_A:
                        event.a = std::atof(p_line + 1);
                        break;
                    case GCODE_FEEDRATE:
                        event.f = std::atof(p_line + 1);
                        break;

                    default:
                        // any other character will be skipped
                        break;
                }
                p_line++;
            }
            return event;
        }

    };


}