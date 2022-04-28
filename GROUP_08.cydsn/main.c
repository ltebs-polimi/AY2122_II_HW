/* ========================================
 *
 * Authors: Alfonzo Massimo, Mazzucchelli Umberto
 * Date: 27/04/2022
 * 
 * Main source file
 * 
 * This code handles the sampling of the data and related conversion 
 * into the measure of interest of two sensors in the form of °C for the
 * TMP sensor, and of lux for the LDR sensor.

 * The user is able to set 
 * - which sensor/s output will be read 
 * - the number of samples to be averaged 
 * - which sensor that will be used to modulate the RGB led
 * - the colors of the led
 * ========================================
*/

/*Include necessary headers and libraries*/

#include "project.h"
#include "InterruptRoutines.h"
#include "stdio.h"
#include "utils.h"
#include "math.h"

/*Definition of variables*/
uint8_t nSamples;       // Number of samples to be averaged
uint8_t modulator;      // Variable to choose the sensor used to modulate the led
char newSamples;        // Variable used to check if the number of samples was changed

//Declaration and inizialization of the EZI2C slave buffer
uint8_t slaveBuffer[SLAVE_BUFFER_SIZE]={0,WHO_AM_I_Def,0,0,0,0}; 

char defaultPeriod;                         // Variable used to store the timer period
int16 rgb_TMP,rgb_LDR;                      // Variables used to store the values read from the sensors
int16 LDR_avg_scaled, TMP_avg_scaled;       // Variables used to store the average values scaled 

char LEDs[3] = {0,0,0};        // Declaration and definition of the variables used to store which colors 
                               // of the led the user has chosen to use

int currMode;       // Variable used to store the status chosen by the user
char currLED;       // Variable used to store the current colors of the RGB chosen by the user
char newLED;        // Variable used to store the new colors of the RGB chosen by the user
char dataBuffer[16];

/***************************************
*                main
***************************************/
int main(void)
{
    CyGlobalIntEnable; // Enables global interrupts 
    
    EZI2C_Start();     // Enables the EZI2C
    
    // Set EZI2C buffer. SLAVE_BUFFER_SIZE - 4 is the WHO AM I register 
    EZI2C_SetBuffer1(SLAVE_BUFFER_SIZE,SLAVE_BUFFER_SIZE-5,slaveBuffer); 
    // Set the slave buffer to 0
    slaveBuffer[CTRL_REG]=0b00000000;

    // Enable various functions
    AMux_Start();
    ADC_Start();
    Timer_Start();
    isr_ADC_StartEx(Custom_ISR_ADC);

    // Inizialization of various variables
    newSamples = 1;
    defaultPeriod = Timer_ReadPeriod();
    TMP_avg=0;
    LDR_avg=0;
    TMP_sum=0;
    LDR_sum=0;
    count=0;
    dataReady=0;
   
    currMode = 0;
    
    channel = 0;
    double LDR;
    nSamples=1;
    currLED=0;
    
    for(;;)
    {
        // Storing data received by user
        newSamples = ((slaveBuffer[CTRL_REG] & 0b00011000)>>3)+1;   // Number of samples
        readMode = slaveBuffer[CTRL_REG] & 0b00000011;              // Status
        modulator = (slaveBuffer[CTRL_REG] & 0b00000100) >> 2;      // Choice of sensor
        newLED = (slaveBuffer[CTRL_REG] & 0b11100000) >> 5;         // Choice of colors
   
        // Checks if the colors have been changed and eventually stores the new one inside the LED array
        if(newLED != currLED)
        {
            LEDs[0] = (slaveBuffer[CTRL_REG] & 0b00100000)>>5;  //for red
            LEDs[1] = (slaveBuffer[CTRL_REG] & 0b01000000)>>6;  //for green
            LEDs[2] = (slaveBuffer[CTRL_REG] & 0b10000000)>>7;  //for blue
            currLED = newLED;
        }
        // Lights the led if the status allows it
        if(readMode != readNone) RGBLed_Start();
        
        // Checks if the number of samples has been changed and eventually updates it and the timer
        if(newSamples != nSamples) 
        {
            updateTimer(newSamples,defaultPeriod);
            nSamples = newSamples;
        }
        
        // Cleans all the variables when the status changes
        if(readMode != currMode) 
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
        
        if(dataReady==1)    // Checks if the ADC data is ready
        {
            // Switch case based on the status
            switch (readMode)
            {
                // Samples both sensors
                case readBoth:
                    // LED modulation
                    updateLed(modulator,LDR_avg_digit,TMP_avg_digit,LEDs);
                
                    // Converts in temperature
                    TMP_avg_scaled = ((TMP_avg - TMP_INTERCEPT)/TMP_SLOPE)*10;   //We multiply by 10 to have higher resolution
                    
                    // Convert in lux
                    LDR = SERIES_RESISTANCE * (ACTUAL_Vdd_mV / LDR_avg - 1.0);
                    LDR_avg_scaled = (int16) (pow(LDR/LDR_INTERCEPT, 1/LDR_SLOPE));
                    
                    
                    slaveBuffer[MSB_LDR] = LDR_avg_scaled >> 8;     // Saves the ldr sensor average scaled in the 4th register the MSB 
                    slaveBuffer[LSB_LDR] = LDR_avg_scaled & 0xFF;   // Saves the ldr sensor average scaled in the 5th register the LSB 
                    slaveBuffer[MSB_TMP] = TMP_avg_scaled >>8;      // Saves the tmp sensor average scaled in the 6th register the MSB 
                    slaveBuffer[LSB_TMP] = TMP_avg_scaled & 0xFF;   // Saves the tmp sensor average scaled in the 7th register the LSB 
                    
                    // Resets sums values, count and data flag
                    TMP_sum=0;
                    LDR_sum=0;
                    LDR_sum_digit=0;
                    TMP_sum_digit=0;
                    count=0;
                    dataReady=0;
                break;
                
                    // Only samples LDR sensor
                case readLDR:
                    
                    // LED modulation
                    updateLed(LDR_mod,LDR_avg_digit,0,LEDs);
                    
                    // Converts in lux and scales
                    LDR = SERIES_RESISTANCE * (ACTUAL_Vdd_mV / LDR_avg - 1.0);
                    LDR_avg_scaled = (int16) (pow(LDR/LDR_INTERCEPT, 1/LDR_SLOPE));
                    
                    slaveBuffer[MSB_LDR] = LDR_avg_scaled >> 8;     // Saves the ldr sensor average scaled in the 4th register the MSB
                    slaveBuffer[LSB_LDR] = LDR_avg_scaled & 0xFF;   // Saves the ldr sensor average scaled in the 5th register the LSB
                    slaveBuffer[MSB_TMP] = 0x00;                    // Saves in the 6th register 0 value
                    slaveBuffer[LSB_TMP] = 0x00;                    // Saves in the 7th register 0 value
                    
                    // Resets sums values, count and data flag
                    LDR_sum_digit=0;
                    LDR_sum=0;
                    count=0;
                    dataReady=0;
                break;
                // Only samples TMP sensor
                case readTMP:
                    
                    // LED modulation
                    updateLed(TMP_mod,0,TMP_avg_digit,LEDs);
                    
                    //convert in temperature
                    TMP_avg_scaled = ((TMP_avg - TMP_INTERCEPT)/TMP_SLOPE)*10;   // We multiply by 10 to have higher resolution
                    slaveBuffer[MSB_LDR] = 0x00;                    // Saves in the 4th 0 value
                    slaveBuffer[LSB_LDR] = 0x00;                    // Saves in the 5th 0 value
                    slaveBuffer[MSB_TMP] = TMP_avg_scaled >> 8;     // Saves the tmp sensor average scaled in the 6th register the MSB 
                    slaveBuffer[LSB_TMP] = TMP_avg_scaled & 0xFF;   // Saves the tmp sensor average scaled in the 7th register the LSB 
                    
                    // Resets sums values, count and data flag
                    TMP_sum_digit=0;
                    TMP_sum=0;
                    count=0;
                    dataReady=0;
                break;
                
                // Samples none of the sensors
                case readNone:
                    RGBLed_Stop();  // Stops the LED modulation
                    // Saves empty buffer
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
