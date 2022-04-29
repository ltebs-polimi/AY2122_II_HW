/* =====================================================
 *
 * ELECTRONIC TECHNOLOGIES AND BIOSENSORS LABORATORY
 * Academic year 2021/22, II Semester
 * Assignment 1

 * Authors: Group 1
 *
 * ----------------------- MAIN ------------------------
 * Main source file for the assignement.
 * -----------------------------------------------------
 * 
 * =====================================================
*/

#include "project.h"
#include "InterruptRoutines.h"
#include "RGB_Led_driver.h"


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
    
    // Set values of parameters in the default configuration of the registers
    sampling_status=0;
    led_modality = 0;
    num_avg_samples=1;
    sum_TMP=0;
    sum_LDR=0;
    temp_celsius=0;
    light_lux=0;
    
    //Start with RGB led off
    RGBLed_WriteRed(RGB_LED_OFF);
    RGBLed_WriteGreen(RGB_LED_OFF);
    RGBLed_WriteBlue(RGB_LED_OFF);


    for(;;)
    {
        /* Place your application code here. */
    }
}

/* [] END OF FILE */

