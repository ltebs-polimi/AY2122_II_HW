/* =================================================
 
Electronic Technologies and Biosensors Laboratory
Academic Year 2021/2022 - II Semester
Assignment
Alfonzo Massimo, Mazzucchelli Umberto

Interrupt Routines header file
 
 * =================================================
*/
#ifndef __INTERRUPT_ROUTINES_H
    #define __INTERRUPT_ROUTINES_H
    
    #include "cytypes.h"
    #include "stdio.h"
    
    #define BYTE_TO_SEND 2
    #define TRANSMIT_BUFFER_SIZE 7
    
    uint8_t DataBuffer[TRANSMIT_BUFFER_SIZE];
    
    volatile uint8 ReadADC_Flag; 

    CY_ISR_PROTO(Custom_ISR_ADC);
    
    
#endif

/* [] END OF FILE */
