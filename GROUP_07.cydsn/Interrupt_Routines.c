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

#include "Interrupt_Routines.h"
#include "project.h"

#define LDR_channel 0
#define TMP_channel 1


int32 temp = 0, ldr = 0, value_ldr = 0, value_temp = 0;

uint8_t i = 0;

CY_ISR(Custom_ISR_ADC)
{   
    //status = 0, everything is turned OFF
    if (status == 0)
    {
        //set variables to 0
        temp = 0;
        ldr = 0;
    }
    
    //status = 1, LDR sampling
    if (status == 1)
    {
        //sample LDR channel
        AMux_1_FastSelect(LDR_channel);
        
        ldr = 0;
        //loop to sample the correct amount of times
        for(i=0; i<samples; i++)
        {
            value_ldr = ADC_DelSig_PR_Read32();
            
            if (value_ldr < 0)  value_ldr = 0;
            if (value_ldr > 65535)  value_ldr  = 65535;
            
            ldr += value_ldr;
            value_ldr = 0;
        }
        
        //average of the sampled values
        ldr = ldr/samples;
        
        //transmission of the sampled values
        slave_buffer[2] = ldr >> 8;
        slave_buffer[3] = ldr & 0xFF;
        
        //set the unused registers to 0
        slave_buffer[4]=0; 
        slave_buffer[5]=0; 
    }
   
    
    //status = 2, TEMP sampling
    if (status == 2)
    {
        
        //sample LDR channel
        AMux_1_FastSelect(TMP_channel);
        
        temp = 0;
        
        //loop to sample the correct amount of times
        for(i=0; i<samples; i++)
        {
            value_temp = ADC_DelSig_PR_Read32();
            
            if (value_temp < 0) value_temp = 0;
            if (value_temp > 65535) value_temp  = 65535;
            
            temp += value_temp;
            value_temp = 0;
        }
        
        //average of the sampled values
        temp = temp/samples;
        
        //set the unused registers to 0
        slave_buffer[2]=0; 
        slave_buffer[3]=0;
        
        //transmission of the sampled values
        slave_buffer[4] = temp >> 8;
        slave_buffer[5] = temp & 0xFF;
    }
    
    
    //status = 3, sample of both channels
    if (status == 3) 
    {
        temp = 0;
        ldr = 0;
        
        //loop to sample the correct amount of times
        for(i=0; i<samples; i++)
        {
            //LDR sampling
            AMux_1_FastSelect(LDR_channel);
            value_ldr = ADC_DelSig_PR_Read32();
            
            if (value_ldr < 0)  value_ldr = 0;
            if (value_ldr > 65535)  value_ldr  = 65535;
            
            ldr += value_ldr;
            value_ldr = 0;
            
            //TEMP sampling
            AMux_1_FastSelect(TMP_channel);
            value_temp = ADC_DelSig_PR_Read32();
            if (value_temp < 0) value_temp = 0;
            if (value_temp > 65535) value_temp  = 65535;
            
            temp += value_temp;
            value_temp = 0;
        }
        
        ldr = ldr/samples;
        temp = temp/samples;     
        
        //transmission of the ldr values
        slave_buffer[2] = ldr >> 8;
        slave_buffer[3] = ldr & 0xFF;
        
        //transmission of the temperature values
        slave_buffer[4] = temp >> 8;
        slave_buffer[5] = temp & 0xFF;
    }
}

/* [] END OF FILE */
