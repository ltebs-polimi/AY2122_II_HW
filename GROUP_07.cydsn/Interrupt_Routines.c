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


int32 temp = 0, ldr = 0, value = 0;
_Bool send_data = 0;
uint8_t i = 0;

CY_ISR(Custom_ISR_ADC)
{   
    //status = 0, spengo tutto
    if (status == 0)
    {
        temp = 0;
        ldr = 0;
    }
    
    //status = 1, campiono LDR
    if (status == 1)
    {
        //campiono il canale
        AMux_1_FastSelect(LDR_channel);
        
        ldr = 0;
        
        for(i=0; i<samples; i++)
        {
            value = ADC_DelSig_PR_Read32();
            if (value < 0)  value = 0;
            if (value > 65535)  value  = 65535;
            
            ldr += value;
            value = 0;
        }
        
        ldr = ldr/samples;
        
        //trasmetto il valore letto
        slave_buffer[2] = ldr >> 8;
        slave_buffer[3] = ldr & 0xFF;
        
        //setto a 0 i registri non usati
        slave_buffer[4]=0; 
        slave_buffer[5]=0; 
    }
   
    
    //status = 2, campiono TMP
    if (status == 2)
    {
        //campiono il canale
        AMux_1_FastSelect(TMP_channel);
        
        temp = 0;
        
        for(i=0; i<samples; i++)
        {
            value = ADC_DelSig_PR_Read32();
            if (value < 0)  value = 0;
            if (value > 65535)  value  = 65535;
            
            temp += value;
            value = 0;
        }
        
        temp = temp/samples;
        
        //setto a 0 i registri non usati
        slave_buffer[2]=0; 
        slave_buffer[3]=0;
        
        //trasmetto il valore letto
        slave_buffer[4] = temp >> 8;
        slave_buffer[5] = temp & 0xFF;
    }
    
    
    // status = 3, campiono entrambi
    if (status == 3) 
    {
        temp = 0;
        ldr = 0;
        
        for(i=0; i<samples; i++)
        {
            //campiono ldr
            AMux_1_FastSelect(LDR_channel);
            value = ADC_DelSig_PR_Read32();
            if (value < 0)  value = 0;
            if (value > 65535)  value  = 65535;
            
            ldr += value;
            value = 0;
            
            //campiono temp
            AMux_1_FastSelect(TMP_channel);
            value = ADC_DelSig_PR_Read32();
            if (value < 0)  value = 0;
            if (value > 65535)  value  = 65535;
            
            temp += value;
            value = 0;
        }
        
        ldr = ldr/samples;
        temp = temp/samples;
        
        //mando temperatura
        slave_buffer[4] = temp >> 8;
        slave_buffer[5] = temp & 0xFF;
        
        //mando LDR
        slave_buffer[2] = ldr >> 8;
        slave_buffer[3] = ldr & 0xFF;
    }
}

/* [] END OF FILE */
