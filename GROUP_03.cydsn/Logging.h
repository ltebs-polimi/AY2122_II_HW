/* ========================================
 *
 * Copyright YOUR COMPANY, THE YEAR
 * All Rights Reserved
 * UNPUBLISHED, LICENSED SOFTWARE.
 *
 * CONFIDENTIAL AND PROPRIETARY INFORMATION
 * WHICH IS THE PROPERTY OF your company.
 *
 * ========================================
*/
/* ========================================
 *
 * Copyright YOUR COMPANY, THE YEAR
 * All Rights Reserved
 * UNPUBLISHED, LICENSED SOFTWARE.
 *
 * CONFIDENTIAL AND PROPRIETARY INFORMATION
 * WHICH IS THE PROPERTY OF your company.
 *
 * ========================================
*/
/**
*   \file Logging.h
*
*   This driver allows to log data for debug purposes.
*   Here you can find all the macros and function
*   declarations that can be used for debug purposes.
*
*   \author Davide Marzorati
*   \date September 23, 2019
*/


#ifndef __LOGGING_H
    #define __LOGGING_H

    #include "Logging_Interface.h"
    
    /**
    *   \brief Macro to enable debug messages.
    *
    *   If you want to disable debug messages, comment this out.
    */
    #ifndef DEBUG_ENABLED
        #define DEBUG_ENABLED
    #endif
        
    /**
    *   \brief Welcome string
    */
    #ifndef WELCOME_STRING
        #define WELCOME_STRING "\n************************\n* EZI2C Program Start *\n************************\n"
    #endif
    
    /**
    *   \brief Warning string for debug enabled
    */
    #ifndef DEBUG_ENABLED_STRING
        #define DEBUG_ENABLED_STRING "Warning: Debug is enabled.\n"
    #endif
    
    Logging_Error Logging_Start(void);

    Logging_Error Logging_Stop(void);
    
    
    #endif
/* [] END OF FILE */






/* [] END OF FILE */
