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

// Variables declaration - Average LDR and TMP measurements
int32 TMP_avg_value_digit;
int32 TMP_avg_value_mV;
int32 LDR_avg_value_digit;
int32 LDR_avg_value_mV;

#define TMP_CH_0 0  // Analog Channel 0: TMP
#define LDR_CH_1 1  // Analog Channel 1: LDR

CY_ISR(Custom_isr_ADC)
{
    // Read Timer status register to bring interrup line low
    Timer_ReadStatusRegister();
    
    //********** TMP **********//
    // Read data from ADC buffer
    AMux_ADC_Select(TMP_CH_0);     //Disconnection from all other channels, then connects the given channel (Analog Channel 0)
    TMP_avg_value_digit = ADC_DelSig_Read32();
    
    // Check consistency
    if (TMP_avg_value_digit<0)         TMP_avg_value_digit = 0;
    if (TMP_avg_value_digit>65535)     TMP_avg_value_digit = 65535;
    
    // Map to mV
    TMP_avg_value_mV = ADC_DelSig_CountsTo_mVolts(TMP_avg_value_digit);
    
    //********** LDR **********//
    // Read data from ADC buffer
    AMux_ADC_Select(LDR_CH_1);    //Disconnection from all other channels, then connects the given channel (Analog Channel 1)
    LDR_avg_value_digit = ADC_DelSig_Read32();
    
    // Check consistency
    if (LDR_avg_value_digit<0)         LDR_avg_value_digit = 0;
    if (LDR_avg_value_digit>65535)     LDR_avg_value_digit = 65535;

    // Map to mV
    LDR_avg_value_mV = ADC_DelSig_CountsTo_mVolts(LDR_avg_value_digit);
        
}

/* [] END OF FILE */