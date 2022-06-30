/* ========================================
    AURORA PIERANTOZZI
    LETIZIA MOZZORECCHIA
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
uint8_t average_samples; 

int main(void)
{
    /*STARTING INTERRUPT AND USEFUL COMPONENTS FOR SAMPLING*/
    
     
    EZI2C_Start();
    ADC_DelSig_Start();
    AMUX_Start();
    Timer_Start();
    isr_ADC_StartEx(My_ISR);
    PWM_B_Start();
    PWM_B_WriteCompare(65000);
    PWM_RG_Start();
    PWM_RG_WriteCompare1(65000);
    PWM_RG_WriteCompare2(65000);
    
    CyGlobalIntEnable;
    
    /*SLAVE BUFFER SETTING*/
    
    buffer[CR1] = 0b00011001;   //initial value
    buffer[WhoAmI] = 0xBC;      //fixed
    buffer[MSB_TEMP] = 0x00;
    buffer[LSB_TEMP] = 0x00;
    buffer[MSB_LDR] = 0x00;
    buffer[LSB_LDR] = 0x00;
    
    //creating the EZI2C buffer 
    EZI2C_SetBuffer1 (BUFFERSIZE, 1, buffer);  //of the 6 bytes, only the first 1 is R/W 
    
    for(;;)
    { 
    }
}

/* [] END OF FILE */


