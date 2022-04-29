/* ========================================
 *
 * ADELAIDE STUCCHI - NATALIA GINGHINI
 * GROUP 5 
 * 
 * ========================================
*/
#include "project.h"
#include "InterruptRoutines.h"

/*DECLARATIONS*/

#define BUFFERSIZE 6 

//Project specifications

#define CR1 0
#define WhoAmI 1
#define MSB_LDR 2
#define LSB_LDR 3
#define MSB_TEMP 4
#define LSB_TEMP 5

//declaring buffer and number of samples

uint8_t buffer[BUFFERSIZE];
uint8_t average_samples = 4; //started at 4 but is possible to modify it from the Bridge Control Panel 


int main(void)
{
    //initializing functions 
    
    EZI2C_Start();
    
    ADC_DelSig_Start();
   
    AMUX_Start();
    
    Timer_Start();
    
    isr_ADC_StartEx(My_ISR);
    
    Clock_PWM_Start();
    
    PWM_RG_Start();
    PWM_B_Start(); 
    
    CyGlobalIntEnable; 
    
    
    //setting the initial values of the buffer register 
    buffer[CR1] = 0b00011001; //initialized at 4 samples, the device is stopped, LDR modality 
    buffer[WhoAmI] = 0xBC;
    buffer [MSB_TEMP] = 0x00;
    buffer [LSB_TEMP]= 0x00;
    buffer[MSB_LDR]= 0x00;
    buffer[LSB_LDR] = 0x00;
    
    //creazione EZI2C buffer
    EZI2C_SetBuffer1 (BUFFERSIZE, 1, buffer);
    
    
    for(;;)
    { 
      
    }
}

/* [] END OF FILE */

