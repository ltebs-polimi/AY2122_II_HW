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

char DataBuffer[16];

uint8_t nSamples;
uint8_t modulator;

uint8_t slaveBuffer[SLAVE_BUFFER_SIZE]={0,WHO_AM_I_Def,0,0,0,0};


int16 LDR_avg_scaled, TMP_avg_scaled,rgb_TMP,rgb_LDR;


int currMod;
int currMode;

uint8_t red;    //RED (0 - 255)
uint8_t green;  //GREEN (0 - 255)
uint8_t blue;   //BLUE (0 - 255)


int main(void)
{
    CyGlobalIntEnable; /* Enable global interrupts. */
    
    EZI2C_Start();
    
    EZI2C_SetBuffer1(SLAVE_BUFFER_SIZE,SLAVE_BUFFER_SIZE-4,slaveBuffer);
    
    UART_Start();
        
    AMux_Start();
    ADC_Start();
    Timer_Start();
    isr_ADC_StartEx(Custom_ISR_ADC);
    
    PWM_R_Start();
    Clock_Start();
    
    PWM_G_Start();

    
    PWM_B_Start();

    
    nSamples = ((slaveBuffer[CTRL_REG] & 0b00011000)>>3)+1;
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
    
    slaveBuffer[CTRL_REG] = 0b00000110;
    
    for(;;)
    {
        nSamples = ((slaveBuffer[CTRL_REG] & 0b00011000)>>3)+1;
        readMode = slaveBuffer[CTRL_REG] & 0b00000011;
        modulator = (slaveBuffer[CTRL_REG] & 0b00000100) >> 2;
        /*
        if(currMod != modulator)
        {
            switch (modulator)
            {
                case LDR_mod:
                PWM_R_Sleep();
                PWM_B_Wakeup();
                PWM_G_Wakeup();
                currMod = modulator;
                break;
                
                case TMP_mod:
                PWM_B_Sleep();
                PWM_R_Wakeup();
                PWM_G_Wakeup();
                currMod = modulator;
            }
        } 
        */
        if(readMode != readNone) RGBLed_Start();
        
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
                    
                    rgb_LDR = LDR_avg_digit >> 8;
                    rgb_TMP = TMP_avg_digit >> 8;
                
                    //updateLed(modulator,red,green,blue,rgb_LDR,rgb_TMP);
                    
                    //convert in temperature
                    TMP_avg_scaled = ((TMP_avg - TMP_INTERCEPT)/TMP_SLOPE)*10;   //we multiply by 100 to have more resolution
                    
                    //convert in lux
                    LDR = SERIES_RESISTANCE * (ACTUAL_Vdd_mV / LDR_avg - 1.0);
                    LDR_avg_scaled = (int16) (pow(LDR/TEN_TO_LDR_INTERCEPT, 1/LDR_SLOPE));
                    
                    slaveBuffer[MSB_LDR] = LDR_avg_scaled >> 8;
                    slaveBuffer[LSB_LDR] = LDR_avg_scaled & 0xFF;
                    slaveBuffer[MSB_TMP] = TMP_avg_scaled >>8;
                    slaveBuffer[LSB_TMP] = TMP_avg_scaled & 0xFF;
                    TMP_sum=0;
                    LDR_sum=0;
                    LDR_sum_digit=0;
                    TMP_sum_digit=0;
                    count=0;
                    dataReady=0;
                break;
                
                case readLDR:
                    //LED modulation
                    
                    
                    
                    //convert in lux
                    LDR = SERIES_RESISTANCE * (ACTUAL_Vdd_mV / LDR_avg - 1.0);
                    LDR_avg_scaled = (int16) (pow(LDR/TEN_TO_LDR_INTERCEPT, 1/LDR_SLOPE));
                    
                    slaveBuffer[MSB_LDR] = LDR_avg_scaled >> 8;
                    slaveBuffer[LSB_LDR] = LDR_avg_scaled & 0xFF;
                    slaveBuffer[MSB_TMP] = 0x00;
                    slaveBuffer[LSB_TMP] = 0x00;
                    rgb_LDR = slaveBuffer[MSB_LDR];
                    sprintf(DataBuffer, "\n%ld", LDR_avg_digit);
                    UART_PutString(DataBuffer);
                    
                    updateLed(LDR_mod,LDR_avg_digit);
                    LDR_sum_digit=0;
                    LDR_sum=0;
                    count=0;
                    dataReady=0;
                break;
                
                case readTMP:
                    
                    //rgb_TMP = TMP_avg_digit;
                    updateLed(TMP_mod,TMP_avg_digit);
                    
                    //convert in temperature
                    TMP_avg_scaled = ((TMP_avg - TMP_INTERCEPT)/TMP_SLOPE)*10;   //we multiply by 100 to have more resolution
                    slaveBuffer[MSB_LDR] = 0x00;
                    slaveBuffer[LSB_LDR] = 0x00;
                    slaveBuffer[MSB_TMP] = TMP_avg_scaled >> 8;
                    slaveBuffer[LSB_TMP] = TMP_avg_scaled & 0xFF;
                    sprintf(DataBuffer, "\n%ld", TMP_avg_digit);
                    UART_PutString(DataBuffer);
                    
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
