/**
 * @file GCodeParser.h
 * @author flexhaufen
 * @brief GCodeParser
 * @version 0.1
 * @date 2026-03-05
 * 
 * @copyright Copyright (c) 2026
 * 
 */
#pragma once


#include <string>
#include "Motion/MotionEvent.h"

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

                    case 'G':
                        {
                            // TODO (inj):  Maybe this can also be done by
                            //              just casting the number into "MotionType"
                            uint8_t g = std::atoi(p_line + 1);
                            switch (g) {
                                case 0: event.type == MotionType::G0; break;
                                case 1: event.type == MotionType::G1; break;
                                
                                default: 
                                    LOG_ERROR("Received invalid G__");
                                    break;
                            }                        
                        }
                        break;
                    
                    case 'X':
                        event.x = std::atof(p_line + 1);
                        break;
                    case 'Y':
                        event.y = std::atof(p_line + 1);
                        break;
                    case 'Z':
                        event.z = std::atof(p_line + 1);
                        break;
                    case 'F':
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