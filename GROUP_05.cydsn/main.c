/*
GROUP_05
ADELAIDE STUCCHI 
NATALIA GINGHINI
*/
#include "project.h"
#include "InterruptRoutines.h"

#define BUFFERSIZE 6 //buffer dimension

//definition of buffer register indices
#define CR1 0
#define WhoAmI 1
#define MSB_LDR 2
#define LSB_LDR 3
#define MSB_TEMP 4
#define LSB_TEMP 5

//number of samples to be averaged initialized to 4, but modifiable from Bridge Control Panel between 1 and 4
uint8_t average_samples = 4; 
uint8_t buffer[BUFFERSIZE]; //buffer declaration


int main(void)
{
<<<<<<< HEAD
    //Start interrupt and components
=======
    //initializing functions 
>>>>>>> bfb5127c237b304d3e27c9e8dae2517eec80c818
    
    EZI2C_Start();
    
    ADC_DelSig_Start();
   
    AMUX_Start();
    
    Timer_Start();
    
    isr_ADC_StartEx(My_ISR);
    
    Clock_PWM_Start();
    
<<<<<<< HEAD
    PWM_B_Start();
    
    PWM_RG_Start();
    
    CyGlobalIntEnable; 
   
    //setting the initial values ​​of the buffer registers
    buffer[CR1] = 0b00011001; //initial value 
=======
    PWM_RG_Start();
    PWM_B_Start(); 
    
    CyGlobalIntEnable; 
    
    
    //setting the initial values of the buffer register 
    buffer[CR1] = 0b00011001; //initialized at 4 samples, the device is stopped, LDR modality 
>>>>>>> bfb5127c237b304d3e27c9e8dae2517eec80c818
    buffer[WhoAmI] = 0xBC;
    buffer [MSB_TEMP] = 0x00;
    buffer [LSB_TEMP]= 0x00;
    buffer[MSB_LDR]= 0x00;
    buffer[LSB_LDR] = 0x00;
    
    //EZI2C buffer creation
    EZI2C_SetBuffer1 (BUFFERSIZE, BUFFERSIZE-5, buffer);
    
    for(;;)
    { 
    }
}
/* [] END OF FILE */
