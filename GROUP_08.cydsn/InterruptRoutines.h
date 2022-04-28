/* ========================================
 *
 * Authors: Alfonzo Massimo, Mazzucchelli Umberto
 * Date: 27/04/2022
 * 
 * Header file for interrupt routines
 *
 * ========================================
*/

#ifndef __INTERRUPT_ROUTINES_H
    #define __INTERRUPT_ROUTINES_H
    
    /*Include necessary headers*/
    #include "cytypes.h"
    #include "stdio.h"
    // Definition of Slave Buffer size
    #define SLAVE_BUFFER_SIZE 6
    
    CY_ISR_PROTO (Custom_ISR_ADC);
    
    // Variables declaration
    uint8_t slaveBuffer[SLAVE_BUFFER_SIZE]; 
    uint8_t readMode;
    uint8_t nSamples;    
    int channel;    // Used to select the channel to read
    int count;
    
    int16 TMP_avg,LDR_avg,LDR_sum,TMP_sum;      // Variables used to store the sum and the average of the values
                                                // read from the sensors
    
    // Variables used to store the sum and the average of the values read from the sensors in digit format
    int32 TMP_avg_digit, LDR_avg_digit, TMP_sum_digit, LDR_sum_digit;
                                                                    
    
    int dataReady; //Variabile flag used to denote if the data is ready to be sent
    
#endif 
/* [] END OF FILE */
