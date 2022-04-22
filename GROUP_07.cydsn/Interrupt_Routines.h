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

#ifndef __INTERRUPT_ROUTINES_H
    #define __INTERRUPT_ROUTINES_H
    #include "cytypes.h"
    #include "stdio.h"
    
    #define SLAVE_BUFFER_SIZE 6
    
    CY_ISR_PROTO(Custom_ISR_ADC); 
    
    uint8_t slave_buffer[SLAVE_BUFFER_SIZE];
    extern int status, samples;
    _Bool send_data; 
    char message[50];
    
 #endif

/* [] END OF FILE */
