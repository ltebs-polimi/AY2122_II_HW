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
#include "AMUX.h"

/*DECLARATIONS*/

//definition of the number of registers of the I2C Slave Buffer
#define BUFFERSIZE 6

//definition of the I2C Slave Buffer's registers' addresses
#define CR1 0
#define WhoAmI 1
#define MSB_LDR 2
#define LSB_LDR 3
#define MSB_TEMP 4
#define LSB_TEMP 5

//definitions of the slave bufferand number of samples to be averaged
uint8_t buffer[BUFFERSIZE]; 
uint8_t average_samples = 4; 

int main(void)
{
    /*STARTING INTERRUPT AND USEFUL COMPONENTS FOR SAMPLING*/
    
    CyGlobalIntEnable; 
    EZI2C_Start();
    ADC_DelSig_Start();
    AMUX_Start();
    Timer_Start();
    isr_ADC_StartEx(My_ISR);
    PWM_RG_Enable();
    PWM_B_Enable();
    
    /*SLAVE BUFFER SETTING*/
    
    //setting the initial values of the slave buffer register
    buffer[CR1] = 00011001; //RGB off, 4 samples to be avg, LDR readout, sampling ldr
    buffer[WhoAmI] = 0xBC; //fixed
    buffer[MSB_TEMP] = 0x00;
    buffer[LSB_TEMP]= 0x00;
    buffer[MSB_LDR]= 0x00;
    buffer[LSB_LDR] = 0x00;
    
    //creating the EZI2C buffer 
    EZI2C_SetBuffer1 (BUFFERSIZE, 1, buffer);  //of the 6 bytes, only the first 1 is R/W 
    
    for(;;)
    { 
    
    }
}

/* [] END OF FILE */


