/* ========================================
 *
 * LTEBS - A.Y. 2021/2022, II semester
 * Assignment, Group 02
 * Authors: Federico Petraccia, Francesca Terranova
 * 
 * Header file for interrupt routines
 *
 * ========================================
*/

#ifndef __INTERRUPT_ROUTINES_H
    #define __INTERRUPT_ROUTINES_H
    
    #include "cytypes.h"
    #include "stdio.h"
    
    int32 TMP_avg_value_digit ;       // Value in digit of TMP sensor from ADC
    int32 TMP_avg_value_mV;           // Value in digit of LDR sensor from ADC

    CY_ISR_PROTO(Custom_ISR_ADC);
    
    
#endif

/* [] END OF FILE */