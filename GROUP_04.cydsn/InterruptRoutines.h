/* ========================================
 
Electronic Technologies and Biosensors Laboratory
Academic Year 2020/2021 - II Semester
Assignment 03
GROUP_01 

interrupt routines header file
 
- Custom_ISR_ADC: isr for sampling sensor value from ADC

 * ========================================
*/

#ifndef __INTERRUPT_ROUTINES_H
    #define __INTERRUPT_ROUTINES_H
    
    #include "cytypes.h"
    #include "stdio.h"
    
    int32 value_digit_TMP;      //value in digit of TMP sensor from ADC
    int32 value_digit_LDR;      //value in digit of LDR sensor from ADC

    CY_ISR_PROTO(Custom_ISR_ADC);
    
    
#endif

/* [] END OF FILE */
