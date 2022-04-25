/**
* \brief Main source file for the assignement.
*
*
* \authors Group 1
* \date April 14, 2022
*/


#include "project.h"
#include "InterruptRoutines.h"
#include "RGB_led_driver.h"


#define SLAVE_BUFFER_SIZE 6 ///< Size of the buffer of the I2C slave
#define BUFFER_RW_SIZE 1 ///< Only the first register allows R/W 
#define WHO_AM_I_REGISTER_VALUE 0xBC ///< Default value for the identification register 

uint8_t slaveBuffer[SLAVE_BUFFER_SIZE]; ///< Buffer for the slave device

int main(void)
{
    CyGlobalIntEnable; /* Enable global interrupts. */

    //Timer and isr initializations
    Timer_ADC_Start();
    isr_ADC_StartEx(Custom_Timer_ADC_ISR);
    Timer_50Hz_Start();
    isr_50Hz_StartEx(Custom_Timer_50Hz_ISR);
    
    // Start the Delta-Sigma ADC and the analog MUX
    ADC_DelSig_Start();
    AMux_Start();    
    
    // Start the EZI2C component
    EZI2C_Start();
    
    //Initization for the control register 1
    slaveBuffer[0] = 0b00000000; 
    
    
    // Set the read only registers
    slaveBuffer[1] = WHO_AM_I_REGISTER_VALUE;
    slaveBuffer[2] = 0; ///< Ch1 Bit 15-8
    slaveBuffer[3] = 0; ///< Ch1 Bit 07-0
    slaveBuffer[4] = 0; ///< Ch0 Bit 15-8
    slaveBuffer[5] = 0; ///< Ch0 Bit 07-0
    
    
    // Set up EZI2C buffer
    EZI2C_SetBuffer1(SLAVE_BUFFER_SIZE, BUFFER_RW_SIZE ,slaveBuffer);
    
    // Set up the PWM for the R, G, B channels
    RGBLed_Start();
    
    // Start with the RGB led switched off
    RGBLed_WriteRed(RGB_LED_OFF);
    RGBLed_WriteGreen(RGB_LED_OFF);
    RGBLed_WriteBlue(RGB_LED_OFF);


    for(;;)
    {
        /* Place your application code here. */
    }
}

/* [] END OF FILE */

