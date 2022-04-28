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

uint8_t channel = 0;

extern volatile uint8_t ReadFlag;

CY_ISR(Custom_ISR_ADC)
{
    // Read Timer status register to bring interrupt line low
    Timer_ReadStatusRegister();
    //if (ReadFlag == 0)
    
    //ReadFlag = 1;
    
    if ((channel == LDR_CHANNEL)&&(AvgReady == 0)){
        
        AMux_FastSelect(LDR_CHANNEL);              // select the value of the LDR sensor 
        ADC_DelSig_StartConvert(); 
        digit_LDR = ADC_DelSig_Read32();     // read the value of the LDR sensor
        if (digit_LDR < 0) digit_LDR = 0;
        if (digit_LDR > 65535) digit_LDR = 65535;
        mv_LDR = ADC_DelSig_CountsTo_mVolts(digit_LDR);
        sum_LDR += mv_LDR;
        num_samples++;
        
        ADC_DelSig_StopConvert();
        
        channel = TMP_CHANNEL;
    
    }
    
    if ((channel == LDR_CHANNEL) && (AvgReady == 0)){
    
        AMux_FastSelect(TMP_CHANNEL);           // select the value of the TMP sensor 
        ADC_DelSig_StartConvert(); 
        digit_TMP = ADC_DelSig_Read32();  // read the value of the TMP sensor
        if (digit_TMP < 0) digit_TMP = 0;
        if (digit_TMP > 65535) digit_TMP = 65535;
        mv_TMP = ADC_DelSig_CountsTo_mVolts(digit_TMP);
        sum_TMP += mv_TMP;
        num_samples++;
        
        ADC_DelSig_StopConvert();
        
        channel = LDR_CHANNEL;
    }
    
    if (num_samples == average_samples * 2){
        // average calculation
        average_LDR = sum_LDR / average_samples;
        average_TMP = sum_TMP / average_samples;
        AvgReady = 1;
     } 
}



/* [] END OF FILE */
