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
#include "project.h"
#include "utils.h"
#include "InterruptRoutines.h"

int32 avg_LDR;
int32 avg_TMP;
int32 sum_TMP;
int32 sum_LDR;

uint8_t nSamples;
uint8_t NewNSamples;

uint8_t channel;
uint8_t timerPeriod;
uint8_t modulator;

int main(void)
{
    CyGlobalIntEnable; /* Enable global interrupts. */
    
    
    DataBuffer[SLAVE_WHO_AM_I_REG_ADDR]=SLAVE_WHOAMI_RETVAL;
    DataBuffer[SLAVE_CTRL_REG1]=SLAVE_MODE_OFF;
    
    nSamples = DataBuffer[SLAVE_CTRL_REG1] &= 00001100 >> 2; //Set #samples = 1
    timerPeriod = Timer_ReadPeriod(); //read default period
    
    
    Timer_Start(); 
    ADC_DelSig_Start();
    isr_ADC_StartEx(Custom_ISR_ADC);
    AMux_Start();
    EZI2C_Start();
    ADC_DelSig_StartConvert();
    
    EZI2C_SetBuffer1(TRANSMIT_BUFFER_SIZE,TRANSMIT_BUFFER_SIZE-4,DataBuffer);
    
    
    for(;;)
    {
        channel = DataBuffer[SLAVE_CTRL_REG1] & 0x03;
        NewNSamples = DataBuffer[SLAVE_CTRL_REG1] &= 0x0C >> 3;
        if((DataBuffer[SLAVE_CTRL_REG1] |= 0x0C) != nSamples)
        {
            Timer_Stop();
            timerPeriod /= NewNSamples;
            Timer_WritePeriod(timerPeriod);
            Timer_Start();
            nSamples = NewNSamples;
        }
        modulator = (DataBuffer[SLAVE_CTRL_REG1] & 0x04) >> 2;
        switch(channel)
        {
            case SLAVE_BOTH:
                Pin_B_Write(1);
                Pin_G_Write(1);
                Pin_R_Write(1);
                sum_LDR += LDR_meas;
                sum_TMP += TMP_meas;
                
                
            
        }
    }
}



/* [] END OF FILE */
