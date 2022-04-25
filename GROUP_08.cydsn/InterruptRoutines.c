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
#include "InterruptRoutines.h"
#include "project.h"
#include "utils.h"



int32 value_digit;
int16 TMP_sum; 
int16 LDR_sum;
int16 TMP_avg;
int16 LDR_avg;

int32 LDR_avg_digit;
int32 TMP_avg_digit;
int32 TMP_sum_digit; 
int32 LDR_sum_digit;

int channel;

int count=0;

CY_ISR(Custom_ISR_ADC)
{
    Timer_ReadStatusRegister();
    
    if(channel == 0 && dataReady==0)//read LDR
    {
        AMux_FastSelect(channel);
        ADC_StartConvert();
        value_digit=ADC_Read32();
        if(value_digit<0) value_digit=0;
        if(value_digit>65535) value_digit=65535;
        LDR_sum_digit += value_digit;
        LDR_sum += ADC_CountsTo_mVolts(value_digit);
        ADC_StopConvert();
        count++;
        channel = 1;
    }
    else if(channel == 1 && dataReady==0)//read TMP
    {
        AMux_FastSelect(channel);
        ADC_StartConvert();
        value_digit=ADC_Read32();
        if(value_digit<0) value_digit=0;
        if(value_digit>65535) value_digit=65535;
        TMP_sum_digit += value_digit;
        TMP_sum += ADC_CountsTo_mVolts(value_digit);
        ADC_StopConvert();
        count++;
        channel = 0;
    }
    
    if(count==(nSamples*2))
        {
            TMP_avg_digit = TMP_sum_digit/nSamples;
            LDR_avg_digit = LDR_sum_digit/nSamples;
            TMP_avg = TMP_sum/nSamples;
            LDR_avg = LDR_sum/nSamples;
            dataReady=1;
        }
        
    
}



/* [] END OF FILE */
