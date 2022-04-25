/* ========================================
 *
 * \ authors : Giorgio Allegri - Luca Pavirani
 * 
 * \ Header file for Interrupt Routines
 *
 *
 * ========================================
*/
#ifndef __INTERRUPT_ROUTINES_H
    #define __INTERRUPT_ROUTINES_H
    
    #include "cytypes.h"
    #include "stdio.h"
    
    int32 digit_TMP;  //value in digit of TMP sensor from ADC
    int32 digit_LDR;  //value in digit of LDR sensor from ADC
    int32 mv_TMP;

    CY_ISR_PROTO (Custom_ISR_ADC);

#endif

/* [] END OF FILE */
