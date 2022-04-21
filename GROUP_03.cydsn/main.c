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
#include "UART.h"
#include "stdio.h"
#include "InterruptRoutines.h"
#include "InterruptRoutines.c"
#include "I2C_REG.h"

//Define structure of slave buffer
#define SLAVE_BUFFER_SIZE 6     //number of registers
#define CTRL_REG1 0             //position of control register 1
#define WHO_AM_I 1              //position of who am i register
#define MSB_LDR 2                  //position for Most Significant Byte of the first sensor average
#define LSB_LDR 3                  //position for Less Significant Byte of the first sensor average
#define MSB_TMP 4                  //position for Most Significant Byte of the second sensor average
#define LSB_TMP 5                  //position for Less Significant Byte of the second sensor average

uint8 average_sample;
uint8_t bit_status;
uint8 LED_modality;
uint8 colors;
uint8 tot_LDR;
uint8 count_samples=0;
uint8 sum_LDR=0;
uint8 sum_TMP=0;
uint8 average_LDR=0;
uint8 average_TMP=0;

//Define slaveBuffer of the EZI2C
uint8 slaveBuffer[SLAVE_BUFFER_SIZE];


int main(void)
{
    CyGlobalIntEnable; /* Enable global interrupts. */

    
    /* Place your initialization/startup code here (e.g. MyInst_Start()) */
    AMux_ADC_Init();
    AMux_ADC_Start();
    AMux_ADC_Select(0);
    UART_Start();
    ADC_DelSig_Start();
    //Timer_1_Start();
    
    isr_ADC_StartEx(Custom_ISR_ADC);
   
    PacketReadyFlag = 0;
    // Start the ADC conversion
    ADC_DelSig_StartConvert();

    slaveBuffer[WHO_AM_I] = I2C_WHO_AM_I_REG_VALUE;         // Set who am i register value
    slaveBuffer[CTRL_REG1] = SLAVE_MODE_OFF_CTRL_REG1;  //set control reg 1 with all bits = 0 
    
    
    for(;;)
    {
        //reading of the registers
        average_sample = (slaveBuffer[CTRL_REG1] & 0x18) >> 3; // I check the number of samples to extract
        bit_status = slaveBuffer[CTRL_REG1] & 0x03; //I check the bit status configration
        LED_modality = (slaveBuffer[CTRL_REG1] & 0x04); //I check the LedMod of the CTRL REG 1
        colors = (slaveBuffer[CTRL_REG1] & 0xE0) >> 5;
        
        
        switch (bit_status){
            
            case SLAVE_MODE_OFF_CTRL_REG1:
                Pin_RED_Write(0);
                Pin_GREEN_Write(0);
                Pin_BLUE_Write(0);
                slaveBuffer[MSB_LDR]= 0x00;                                
                slaveBuffer[LSB_LDR]= 0x00;                                
                slaveBuffer[MSB_TMP]= 0x00;                                
                slaveBuffer[LSB_TMP]= 0x00;                                

            break;
                
            case SLAVE_LDR_ON_CTRL_REG1:
                if(flagData==1)
                {                                       
                   sum_LDR+=value_digit_LDR;
                   count_samples++;
                
                   if (count_samples==average_sample)
                   {
                       average_LDR=sum_LDR/average_sample;
                    
                       slaveBuffer[MSB_LDR]=value_digit_LDR >> 8;
                       slaveBuffer[LSB_LDR]=value_digit_LDR & 0xFF;
                       slaveBuffer[MSB_TMP]=value_digit_TMP & 0x00;
                       slaveBuffer[LSB_TMP]=value_digit_TMP & 0x00;
                       if(LED_modality==LED_MOD_TMP)
                        {
                                Pin_RED_Write(0);
                                Pin_GREEN_Write(0);
                                Pin_BLUE_Write(0);
                        }
                       else if (LED_modality==LED_MOD_LDR)
                        {
                           
                            PWM_WriteCompare(65535-average_LDR);
                            CyDelay(100);
                        }
                                
                       count_samples=0;
                       sum_LDR=0;         
                    }
                }

                
                break;
 
            case SLAVE_TMP_ON_CTRL_REG1:
                if(flagData==1)
                {                    
                   
                   sum_TMP+=value_digit_TMP;
                   count_samples++;
                
                   if (count_samples==average_sample)
                   {
                       average_LDR=sum_LDR/average_sample;
                    
                       slaveBuffer[MSB_LDR]=value_digit_LDR & 0x00;
                       slaveBuffer[LSB_LDR]=value_digit_LDR & 0x00;
                       slaveBuffer[MSB_TMP]=value_digit_TMP >> 8;
                       slaveBuffer[LSB_TMP]=value_digit_TMP & 0xFF;
                    
                    if(LED_modality==LED_MOD_LDR)
                        {
                                Pin_RED_Write(0);
                                Pin_GREEN_Write(0);
                                Pin_BLUE_Write(0);
                        }
                       else if (LED_modality==LED_MOD_TMP)
                        {
                           
                            PWM_WriteCompare(average_TMP);
                           
                        }
                    
                        sum_TMP=0;
                        count_samples=0;
                    }
                }
                
                break;
                
                
            case SLAVE_BOTH_ON_CTRL_REG1:
                if (flagData==1)
                {
                   sum_LDR+=value_digit_LDR;
                   sum_TMP+=value_digit_TMP;
                   count_samples++;                    
                
                  if (count_samples==average_sample)
                 {
                    average_LDR=sum_LDR/average_sample;
                    average_LDR=sum_LDR/average_sample;
                    
                    slaveBuffer[MSB_LDR]=value_digit_LDR >> 8;
                    slaveBuffer[LSB_LDR]=value_digit_LDR & 0xFF;
                    slaveBuffer[MSB_TMP]=value_digit_TMP >>8;
                    slaveBuffer[LSB_TMP]=value_digit_TMP & 0xFF;
                    
                    average_LDR=0;
                    average_TMP=0;
                    sum_LDR=0;
                    sum_TMP=0;
                    count_samples=0;
                    
                    if(LED_modality==LED_MOD_TMP)
                        {
                            PWM_WriteCompare(average_TMP);
                        }
                       else if (LED_modality==LED_MOD_LDR)
                        {
                           
                            PWM_WriteCompare(65535-average_LDR);
                            
                        }
                 }
                
                }
                break;
                
                
                
            
       
        
        
            
            
        }
        
        
    }
    
 
    
}

/* [] END OF FILE */
