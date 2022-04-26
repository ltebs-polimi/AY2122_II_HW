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
uint8_t modulator;

uint8_t slaveBuffer[SLAVE_BUFFER_SIZE]={0,WHO_AM_I_Def,0,0,0,0};

char defaultPeriod;
char newSamples;
int16 LDR_avg_scaled, TMP_avg_scaled,rgb_TMP,rgb_LDR;

char LEDs[3] = {0,0,0};


int currMod;
int currMode;
char currLED;
char newLED;

int main(void)
{
    CyGlobalIntEnable; /* Enable global interrupts. */
    
    EZI2C_Start();
    
    EZI2C_SetBuffer1(SLAVE_BUFFER_SIZE,SLAVE_BUFFER_SIZE-4,slaveBuffer);
    slaveBuffer[CTRL_REG]=0b00000000;
    
    UART_Start();
        
    AMux_Start();
    ADC_Start();
    Timer_Start();
    isr_ADC_StartEx(Custom_ISR_ADC);

    
    newSamples = 1;
    defaultPeriod = Timer_ReadPeriod();
    TMP_avg=0;
    LDR_avg=0;
    TMP_sum=0;
    LDR_sum=0;
    count=0;
    dataReady=0;
    
    currMod = 0;
    currMode = 0;
    
    channel = 0;
    double LDR;
    nSamples=1;
    currLED=0;
    
    for(;;)
    {
        newSamples = ((slaveBuffer[CTRL_REG] & 0b00011000)>>3)+1;
        readMode = slaveBuffer[CTRL_REG] & 0b00000011;
        modulator = (slaveBuffer[CTRL_REG] & 0b00000100) >> 2;
        newLED = (slaveBuffer[CTRL_REG] & 0b11100000) >> 5;
   
        if(newLED != currLED)
        {
            LEDs[0] = (slaveBuffer[CTRL_REG] & 0b00100000)>>5;
            LEDs[1] = (slaveBuffer[CTRL_REG] & 0b01000000)>>6;
            LEDs[2] = (slaveBuffer[CTRL_REG] & 0b10000000)>>7;
            currLED = newLED;
        }
        if(readMode != readNone) RGBLed_Start();
        
        if(newSamples!= nSamples) 
        {
            updateTimer(newSamples,defaultPeriod);
            nSamples = newSamples;
        }

        if(readMode != currMode) //clean all the variables when the status changes
        {
            currMode = readMode;
            LDR_avg=0;
            TMP_avg=0;
            TMP_sum=0;
            LDR_sum=0;
            LDR_sum_digit=0;
            TMP_sum_digit=0;
            count=0;
            dataReady=0;
            slaveBuffer[MSB_LDR] = 0x00;
            slaveBuffer[LSB_LDR] = 0x00;
            slaveBuffer[MSB_TMP] = 0x00;
            slaveBuffer[LSB_TMP] = 0x00;
        }
        
        if(dataReady==1)
        {
            switch (readMode)
            {
                case readBoth:
                    //LED modulation
                    updateLed(modulator,LDR_avg_digit,TMP_avg_digit,LEDs);
                
                    //convert in temperature
                    TMP_avg_scaled = ((TMP_avg - TMP_INTERCEPT)/TMP_SLOPE)*10;   //*10 to have an higher resolution
                    
                    //convert in lux
                    LDR = SERIES_RESISTANCE * (ACTUAL_Vdd_mV / LDR_avg - 1.0);
                    LDR_avg_scaled = (int16) (pow(LDR/TEN_TO_LDR_INTERCEPT, 1/LDR_SLOPE));
                    slaveBuffer[MSB_LDR] = LDR_avg_scaled >> 8;
                    slaveBuffer[LSB_LDR] = LDR_avg_scaled & 0xFF;
                    slaveBuffer[MSB_TMP] = TMP_avg_scaled >>8;
                    slaveBuffer[LSB_TMP] = TMP_avg_scaled & 0xFF;
                    rgb_LDR = slaveBuffer[MSB_LDR];
                    TMP_sum=0;
                    LDR_sum=0;
                    LDR_sum_digit=0;
                    TMP_sum_digit=0;
                    count=0;
                    dataReady=0;
                break;
                
                case readLDR:
                    //LED modulation
                    updateLed(LDR_mod,LDR_avg_digit,0,LEDs);
                    
                    
                    //convert in lux
                    LDR = SERIES_RESISTANCE * (ACTUAL_Vdd_mV / LDR_avg - 1.0);
                    LDR_avg_scaled = (int16) (pow(LDR/TEN_TO_LDR_INTERCEPT, 1/LDR_SLOPE));
                    
                    slaveBuffer[MSB_LDR] = LDR_avg_scaled >> 8;
                    slaveBuffer[LSB_LDR] = LDR_avg_scaled & 0xFF;
                    slaveBuffer[MSB_TMP] = 0x00;
                    slaveBuffer[LSB_TMP] = 0x00;
                    
                    LDR_sum_digit=0;
                    LDR_sum=0;
                    count=0;
                    dataReady=0;
                break;
                
                case readTMP:
                    
                    //rgb_TMP = TMP_avg_digit;
                    updateLed(TMP_mod,0,TMP_avg_digit,LEDs);
                    
                    //convert in temperature
                    TMP_avg_scaled = ((TMP_avg - TMP_INTERCEPT)/TMP_SLOPE)*10;   //we multiply by 100 to have more resolution
                    slaveBuffer[MSB_LDR] = 0x00;
                    slaveBuffer[LSB_LDR] = 0x00;
                    slaveBuffer[MSB_TMP] = TMP_avg_scaled >> 8;
                    slaveBuffer[LSB_TMP] = TMP_avg_scaled & 0xFF;
                    TMP_sum_digit=0;
                    TMP_sum=0;
                    count=0;
                    dataReady=0;
                break;
                
                case readNone:
                    RGBLed_Stop();
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
