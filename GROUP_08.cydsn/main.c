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
#include "InterruptRoutines.h"
#include "stdio.h"
#include "utils.h"
#include "math.h"


uint8_t nSamples;

uint8_t slaveBuffer[SLAVE_BUFFER_SIZE]={0,WHO_AM_I_Def,0,0,0,0};



int main(void)
{
    CyGlobalIntEnable; /* Enable global interrupts. */
    
    EZI2C_Start();
    
    EZI2C_SetBuffer1(SLAVE_BUFFER_SIZE,SLAVE_BUFFER_SIZE-4,slaveBuffer);
    

    AMux_Start();
    ADC_Start();
    Timer_Start();
    isr_ADC_StartEx(Custom_ISR_ADC);
    
    nSamples = ((slaveBuffer[CTRL_REG] & 0b00011000)>>3)+1;
    TMP_avg=0;
    LDR_avg=0;
    TMP_sum=0;
    LDR_sum=0;
    count=0;
    dataReady=0;
    
    channel = 0;
    double LDR;
    
    for(;;)
    {
        nSamples = ((slaveBuffer[CTRL_REG] & 0b00011000)>>3)+1;
        readMode = slaveBuffer[CTRL_REG] & 0b00000011;
        
        if(dataReady==1)
        {
            switch (readMode)
            {
                case readBoth:
                    //convert in temperature
                    TMP_avg = ((TMP_avg - TMP_INTERCEPT)/TMP_SLOPE)*100;   //we multiply by 100 to have more resolution
                    
                    //convert in lux
                    LDR = SERIES_RESISTANCE * (ACTUAL_Vdd_mV / LDR_avg - 1.0);
                    LDR_avg = (int16) (pow(LDR/TEN_TO_LDR_INTERCEPT, 1/LDR_SLOPE));
                    slaveBuffer[MSB_LDR] = LDR_avg >> 8;
                    slaveBuffer[LSB_LDR] = LDR_avg & 0xFF;
                    slaveBuffer[MSB_TMP] = TMP_avg >>8;
                    slaveBuffer[LSB_TMP] = TMP_avg & 0xFF;
                    TMP_sum=0;
                    LDR_sum=0;
                    count=0;
                    dataReady=0;
                break;
                
                case readLDR:
                    //convert in lux
                    LDR = SERIES_RESISTANCE * (ACTUAL_Vdd_mV / LDR_avg - 1.0);
                    LDR_avg = (int16) (pow(LDR/TEN_TO_LDR_INTERCEPT, 1/LDR_SLOPE));
                    
                    slaveBuffer[MSB_LDR] = LDR_avg >> 8;
                    slaveBuffer[LSB_LDR] = LDR_avg & 0xFF;
                    slaveBuffer[MSB_TMP] = 0x00;
                    slaveBuffer[LSB_TMP] = 0x00;
                    LDR_sum=0;
                    count=0;
                    dataReady=0;
                break;
                
                case readTMP:
                    //convert in temperature
                    TMP_avg = ((TMP_avg - TMP_INTERCEPT)/TMP_SLOPE)*100;   //we multiply by 100 to have more resolution
                    slaveBuffer[MSB_LDR] = 0x00;
                    slaveBuffer[LSB_LDR] = 0x00;
                    slaveBuffer[MSB_TMP] = TMP_avg >>8;
                    slaveBuffer[LSB_TMP] = TMP_avg & 0xFF;
                    TMP_sum=0;
                    count=0;
                    dataReady=0;
                break;
                
                case readNone:
                    slaveBuffer[MSB_LDR] = 0x00;
                    slaveBuffer[LSB_LDR] = 0x00;
                    slaveBuffer[MSB_TMP] = 0x00;
                    slaveBuffer[LSB_TMP] = 0x00;
                break;
                
            }
        }
    }
}

/* [] END OF FILE */
