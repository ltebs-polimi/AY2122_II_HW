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
    #define SLAVE_BUFFER_SIZE 7
    
    CY_ISR_PROTO (Custom_ISR_ADC);
    
    //variables declaration
    uint8_t slaveBuffer[SLAVE_BUFFER_SIZE]; 
    
    uint8_t readMode;
    
    uint8_t nSamples;    
    int channel; //to select the channel to read
   
    int count;
    
    int16 TMP_avg,LDR_avg,LDR_sum,TMP_sum;
    
    int32 TMP_avg_digit, LDR_avg_digit,TMP_sum_digit,LDR_sum_digit;
    
    int dataReady; //variabile flag
    
#endif 
/* [] END OF FILE */
