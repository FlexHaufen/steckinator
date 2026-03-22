/**
 * @file main.cpp
 * @author flexhaufen
 * @brief Entrypoint
 * @version 0.1
 * @date 2025-11-12
 * 
 * @copyright Copyright (c) 2025
 * 
 */


// *** INCLUDES ***
#include "Steckinator/Steckinator.h"

/**
 * @brief Main
 * 
 */
int main() {

    Steckinator::Steckinator* steckinator = new Steckinator::Steckinator();
    steckinator->Run();
    delete steckinator;
    
    return 0;
}
