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
       
    // Variables declaration - single LDR and TMP measurements expressed in digit
    int32 value_TMP_code;
    int32 value_LDR_code;

    CY_ISR_PROTO(Custom_ISR_ADC);
    
    
#endif

/* [] END OF FILE */