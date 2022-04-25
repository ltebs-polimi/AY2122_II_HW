/* ========================================
 *
 * \ authors : Giorgio Allegri - Luca Pavirani
 * 
 * \ Source file for Interrupt Routines
 *
 *
 * ========================================
*/

// Include required header files
#include "InterruptRoutines.h"
#include "project.h"

#define LDR_CHANNEL 0
#define TMP_CHANNEL 1

extern volatile uint8_t ReadFlag;

CY_ISR(Custom_ISR_ADC)
{
    // Read Timer status register to bring interrupt line low
    Timer_ReadStatusRegister();
    
    ReadFlag = 1; 
    
    AMux_FastSelect(LDR_CHANNEL);              // select the value of the LDR sensor 
    digit_LDR = ADC_DelSig_Read32();     // read the value of the LDR sensor
    
    if (digit_LDR < 0) digit_LDR = 0;
    if (digit_LDR > 65535) digit_LDR = 65535;
    //value_mv = ADC_DelSig_CountsTo_mVolts(value_digit);
    
    AMux_FastSelect(TMP_CHANNEL);           // select the value of the TMP sensor 
    digit_TMP = ADC_DelSig_Read32();  // read the value of the TMP sensor
    
    if (digit_TMP < 0) digit_TMP = 0;
    if (digit_TMP > 65535) digit_TMP = 65535;
    mv_TMP = ADC_DelSig_CountsTo_mVolts(digit_TMP);
    
}

/* [] END OF FILE */
