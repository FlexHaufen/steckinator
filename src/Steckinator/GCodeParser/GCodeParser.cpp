/**
 * @file GCodeParser.cpp
 * @brief Implementation of GCodeParser
 * @version 0.1
 * @date 2026-04-09
 * 
 * @copyright Copyright (c) 2026
 * 
 */

// *** INCLUDES ***
#include "Steckinator/GCodeParser/GCodeParser.h"

#include "Steckinator/Config.h"
#include "Steckinator/Log/Log.h"

#include <stdint.h>


// *** NAMESPACE ***
namespace Steckinator {


    MotionEvent GCodeParser::ParseLine(const std::string& line) {

        MotionEvent event{};
        event.command = MotionCommand::INVALID;

        // We create a pointer to the string here
        // in order to parse each character separately
        const char* p_line = line.c_str();

        // TODO (flex): This portion is blocking
        //              add upper bounds check, and move into separate task
        while(*p_line) {
            
            switch (*p_line) {

                case GCODE_COMMAND_G:
                    {
                        uint8_t g = std::atoi(p_line + 1);
                        switch (g) {
                            case  0: event.command = MotionCommand::G0; break;
                            case  1: event.command = MotionCommand::G1; break;
                            case 28: event.command = MotionCommand::G28; break;
                            
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
                            case 0:  event.command = MotionCommand::M0;  break;
                            case 1:  event.command = MotionCommand::M1;  break;
                            case 10: event.command = MotionCommand::M10; break;
                            case 11: event.command = MotionCommand::M11; break;
                            
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
                case GCODE_FEED_RATE:
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


}