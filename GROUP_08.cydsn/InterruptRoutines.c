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
        TMP_sum += ADC_CountsTo_mVolts(value_digit);
        ADC_StopConvert();
        count++;
        channel = 0;
    }
    
    if(count==(nSamples*2))
        {
            TMP_avg = TMP_sum/nSamples;
            LDR_avg = LDR_sum/nSamples;
            dataReady=1;
        }
        
    
}



/* [] END OF FILE */
