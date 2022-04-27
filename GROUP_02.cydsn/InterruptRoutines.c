/* ========================================
 *
 * LTEBS - A.Y. 2021/2022, II semester
 * Assignment, Group 02
 * Authors: Federico Petraccia, Francesca Terranova
 * 
 * Source file for interrupt routines
 *
 * ========================================
*/

// Include header
 #include "InterruptRoutines.h"

// Include required header files
#include "project.h"

extern volatile uint8_t PacketReadyFlag;

#define TMP_CH_0 0  // Analog Channel 0: TMP
#define LDR_CH_1 1  // Analog Channel 1: LDR


CY_ISR_PROTO(Custom_ISR_ADC){

    // Read Timer status register to bring interrup line low
    Timer_ReadStatusRegister();
    
    // Set flag to indicate the bytes are ready to be sent and to be read by the ADC
    PacketReadyFlag = 1;
    
    
    //********** TMP **********//
    // Read data from ADC buffer
    AMux_ADC_Select(TMP_CH_0);     //Disconnection from all other channels, then connects the given channel (Analog Channel 0)
    value_TMP_code = ADC_DeltaSigma_Read32();
    
   
    // Check consistency
    if (value_TMP_code < 0)           value_TMP_code = 0;
    if (value_TMP_code > 65535)       value_TMP_code = 65535;
    
    //********** LDR **********//
    // Read data from ADC buffer
    AMux_ADC_Select(LDR_CH_1);    //Disconnection from all other channels, then connects the given channel (Analog Channel 1)
    value_LDR_code = ADC_DeltaSigma_Read32();
    
     // Check consistency
    if (value_LDR_code < 0)           value_LDR_code = 0;
    if (value_LDR_code > 65535)       value_LDR_code = 65535;
   
}

/* [] END OF FILE */