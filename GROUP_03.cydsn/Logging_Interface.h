/**
* \file Logging_Interface.h
* \brief Interface to be used for logging purposes.
*
* \author Davide Marzorati
* \date October 14, 2019
*/

#ifndef __LOGGING_INTERFACE_H
    #define __LOGGING_INTERFACE_H

    #include "cytypes.h"
    
    /**
    *   \brief Error codes for the Logging Interface.
    */
    typedef enum {
        LOGGING_OK,     ///< No error generated
        LOGGING_ERROR   ///< Error occurred
    } Logging_Error;
    
        /**
    *   \brief Start the interface.
    *
    *   This function takes care of starting the peripheral
    *   used for logging and debug purposes.
    */
    Logging_Error Logging_Interface_Start(void);
    
    /**
    *   \brief Stop the interface.
    *
    *   This function takes care of stopping the peripheral
    *   used for logging and debug purposes.
    */
    Logging_Error Logging_Interface_Stop(void);
    
    /**
    *   \brief Print out a string.
    *
    *   This function allows to print out a string.
    *   \param string pointer to the null terminated string 
    *       residing in ROM or RAM to be transmitted
    */
    Logging_Error Logging_Interface_PutString(const char8 string[]);
    
    /**
    *   \brief Print out a string.
    *
    *   This function allows to print out a string.
    *   \param string[] address of the memory array residing in RAM or ROM.
    *   \param byteCount number of bytes to be transmitted.
    */
    Logging_Error Logging_Interface_PutArray(const uint8 string[], uint8 byteCount);
    
    /**
    *   \brief Print out a number in int format.
    *
    *   This function allows to print out a number in int format.
    *   \param value the number to be printed
    */
    Logging_Error Logging_Interface_PutInt(int value);
    
    /**
    *   \brief Print out a char.
    *
    *   This function allows to print out a char.
    *   \param char the character to be printed
    */
    Logging_Error Logging_Interface_PutChar(char value);
    
    /**
    *   \brief Print out a number in float format.
    *
    *   This function allows to print out a number in float format.
    *   \param value the number to be printed
    */
    Logging_Error Logging_Interface_PutFloat(float value);
    
    
#endif

/* [] END OF FILE */
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

/* [] END OF FILE */
