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
/*
*   Source file for the logging interface.
*/


#define UART_Name(fn) UART_ ## fn
#define UART_Name_Header_File "UART.h"

// Include required libraries
#include "Logging_Interface.h"
#include "stdio.h"
#include UART_Name_Header_File

Logging_Error Logging_Interface_Start(void)
{
    UART_Name(Start)();
    
    return LOGGING_OK;
}

Logging_Error Logging_Interface_Stop(void)
{
    UART_Name(Stop)();
    
    return LOGGING_OK;
}
    

Logging_Error Logging_Interface_PutString(const char8 string[])
{
    UART_Name(PutString)(string);
    
    return LOGGING_OK;
}
    
    
Logging_Error Logging_Interface_PutArray(const uint8 string[], 
                                            uint8 byteCount)
{
    UART_Name(PutArray)(string, byteCount);
    return LOGGING_OK;
}


Logging_Error Logging_Interface_PutInt(int value) 
{
    char message[30];
    sprintf(message,"%d",value);
    return Logging_Interface_PutString(message);
}

Logging_Error Logging_Interface_PutChar(char value)
{
    UART_Name(PutChar)(value);
    return LOGGING_OK;
}

Logging_Error Logging_Interface_PutFloat(float value)
{
    char message[30];
    sprintf(message,"%f",value);
    return Logging_Interface_PutString(message);
}
/* [] END OF FILE */


