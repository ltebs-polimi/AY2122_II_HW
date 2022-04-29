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
#include "Logging.h"
#include "stdio.h"

Logging_Error Logging_Start(void)
{
    // Start the interface
    Logging_Error error = Logging_Interface_Start();
    if ( error == LOGGING_ERROR)
    {
        return LOGGING_ERROR;
    }
    
    // Print welcome string
    error = Logging_Interface_PutString(WELCOME_STRING);
    if ( error == LOGGING_ERROR)
    {
        return LOGGING_ERROR;
    }
    
    // Print debug string
    #ifdef DEBUG_ENABLED
        error = Logging_Interface_PutString(DEBUG_ENABLED_STRING);
        if ( error == LOGGING_ERROR)
        {
            return LOGGING_ERROR;
        }
    #endif
    
    return error;
}

/* [] END OF FILE */
