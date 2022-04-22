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

//extern uint8_t slave_buffer[SLAVE_BUFFER_SIZE];


int32 temp = 0, ldr = 0, value = 0;
_Bool send_data = 0;
uint8_t i = 0;

CY_ISR(Custom_ISR_ADC)
{   
    //status = 0, spengo tutto
    if (status == 0)
    {
        ADC_DelSig_PR_Stop();
        AMux_1_Stop();
        
        temp = 0;
        ldr = 0;
        
        sprintf(message, "tutto spento\r\n\n");
        UART_PutString(message);
        
    }
    
    //status = 1, campiono LDR
    if (status == 1)
    {
        //attivo ADC e inizio conversione
        ADC_DelSig_PR_Start();
        ADC_DelSig_PR_StartConvert();
        AMux_1_Start();
        
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
        slave_buffer[2] = ldr >> 8;
        slave_buffer[3] = ldr & 0xFF;
        
        sprintf(message, "ldr non in main %ld\r\n\n", ldr);
        UART_PutString(message);
        
        send_data = 1;
    }
   
    
    //status = 2, campiono TMP
    if (status == 2)
    {
        //attivo ADC e inizio conversione
        ADC_DelSig_PR_Start();
        ADC_DelSig_PR_StartConvert();
        AMux_1_Start();
        
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
        
        slave_buffer[4] = temp >> 8;
        slave_buffer[5] = temp & 0xFF;
        
        sprintf(message, "temp non in main %ld\r\n\n", temp);
        UART_PutString(message);
        
        send_data = 1;
    }
    
    
    // status = 3, campiono entrambi
    if (status == 3) 
    {
        //attivo ADC e inizio conversione
        ADC_DelSig_PR_Start();
        ADC_DelSig_PR_StartConvert();
        AMux_1_Start();
        
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
        
        sprintf(message, "ldr: %ld\r\n\n", ldr);
        UART_PutString(message);
        sprintf(message, "temp non in main %ld\r\n\n", temp);
        UART_PutString(message);
        
        send_data = 1;
    }
}

/* [] END OF FILE */
