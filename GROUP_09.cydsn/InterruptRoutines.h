/* ========================================
 *
 * \ Authors : Giorgio Allegri - Luca Pavirani
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
    int32 mv_LDR;
    int32 average_TMP;
    int32 average_LDR;
    int32 sum_TMP;
    int32 sum_LDR;
    
    // Define the flag to read the value from the ADC and set it to 0
    uint8_t AvgReady;
    
    uint8 num_samples;
    uint16_t average_samples;

    CY_ISR_PROTO (Custom_ISR_ADC);

#endif

/* [] END OF FILE */
