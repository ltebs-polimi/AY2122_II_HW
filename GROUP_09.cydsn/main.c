/* ========================================
 *
 * \ authors : Giorgio Allegri - Luca Pavirani
 * 
 * \ Main
 *
 *
 * ========================================
*/

#include "project.h"
#include "InterruptRoutines.h"
#include "ProjDef.h"
#include "RGBLedDriver.h"
#include <stdio.h>
#include "math.h"

// Define slaveBuffer of the EZI2C 
uint8 slaveBuffer[SLAVE_BUFFER_SIZE];

// Define the flag to read the value from the ADC and set it to 0
volatile uint8 ReadFlag = 0;

int32 sum_LDR;            //sum in digit of LDR sensor
int32 sum_TMP;            //sum in digit of TMP sensor
int32 average_LDR;        //average in digit of LDR sensor
int32 average_TMP;        //average in digit of LDR sensor
uint8_t num_samples = 0;        //counter to count the number of sample to consider for the average

uint8_t average_samples;        //number of samples for the average  
uint8_t status_bits;            //values of the status bit to activate the right channel
uint8_t led_modality;
uint8_t timer_period;           //value to set the period of the timer

uint8_t rgb_value_LDR;          //MSB of the value sampled from LDR  
uint8_t rgb_value_TMP;          //MSB of the value sampled from TMP

Color tmp_color;                //LED color assigned for tmp measurements
Color ldr_color;                //LED color assigned for ldr measurements


int main(void)
{
    CyGlobalIntEnable; // Enable global interrupts.
    
    // Start the components and functions
    Timer_Start();
    ADC_DelSig_Start();
    isr_StartEx(Custom_ISR_ADC); 
    AMux_Start();
    EZI2C_Start();
    ADC_DelSig_StartConvert(); 
    
    slaveBuffer[WHO_AM_I] = I2C_WHOAMI_VALUE;  // Set who am i register value
    slaveBuffer[CTRL_REG1] = SENSORS_OFF;  // Set control reg 1 with all bits = 0 
    

    // set EZI2C buffer
    EZI2C_SetBuffer1(SLAVE_BUFFER_SIZE, SLAVE_BUFFER_SIZE - READ_REGISTERS, slaveBuffer);
    
    for(;;)
    
    {
        // Number of samples extracted from the bit 3 and 4 of the control register
        average_samples = (slaveBuffer[CTRL_REG1] & MASK_AVERAGE_SAMPLES) >> 3; //0x18 = 0001 1000 mask
        
        // Switch case to set the average samples according to user selection
        switch (average_samples) {
            case SAMPLE_1:
                average_samples = 1;
            break;
            case SAMPLE_2:
                average_samples = 2;
            break;
            case SAMPLE_3:
                average_samples = 3;
            break;
            case SAMPLE_4:
                average_samples = 4;
            break;
        }
                
        // Set the timer period
        timer_period = DEFAULT_PERIOD / (2*average_samples); 
        
        // Write the period of the timer 
        Timer_WritePeriod(timer_period);
        
        // Check only the status bits of the control register
        status_bits = (slaveBuffer[CTRL_REG1] & MASK_STATUS_REG); //0x03 = 0011
        
        // Check only the LED Modality
        led_modality = (slaveBuffer[CTRL_REG1] & MASK_LED_MODALITY) >> 2; // 0x04 = 0100
       
        
        // Switch case to check the LDR and TMP status from the values of the status bits 
        switch(status_bits){
            
            case BOTH_CHANNELS_READ: // status = 0b11 = 0x03
                                       
                if (ReadFlag == 1){
                    
                    // Sum of the values for average computation
                    sum_LDR = sum_LDR + digit_LDR;
                    sum_TMP = sum_TMP + mv_TMP;
                    
                    num_samples++;                          // increase the number of samples to compute the average
                    
                    if (num_samples == average_samples){    // check if the number of samples is correct
                        
                        //average calculation
                        average_LDR = sum_LDR / average_samples;
                        average_TMP = sum_TMP / average_samples;
                        
                        //convert in temperature
                        average_TMP = ((average_TMP - TMP_INTERCEPT)/TMP_SLOPE)*100;   // multiply by 100 to have more resolution
                        
                        //convert in lux
                        double LDR = SERIES_RESISTANCE * (ACTUAL_Vdd_mV / average_LDR - 1.0);
                        average_LDR = (int16) (pow(LDR/TEN_TO_LDR_INTERCEPT, 1/LDR_SLOPE));
                        
                        rgb_value_LDR = average_LDR >> 8;     //  MSB of the average
                        rgb_value_TMP = average_TMP >> 8;
                        
                        // If - else to check the led modality
                        if (led_modality){
                            
                            RGBLed_Start();
                            Color tmp_color = {0 , 0 , rgb_value_TMP};
                            RGBLed_WriteColor(tmp_color);
                            
                        }
                        else {
                            
                            RGBLed_Start();
                            Color ldr_color = {255-rgb_value_LDR, 0 , 0};
                            RGBLed_WriteColor(ldr_color);
                            
                        }
                        
                        slaveBuffer[MSB_LDR]= average_LDR >> 8;      // save in the 4th register the MSB of the ldr sensor average (8 = 0x08 = 1000)
                        slaveBuffer[LSB_LDR]= average_LDR & 0xFF;    // save in the 5th register the LSB of the ldr sensor average
                        slaveBuffer[MSB_TMP]= average_TMP >> 8;      // save in the 6th register the MSB of the tmp sensor average
                        slaveBuffer[LSB_TMP]= average_TMP & 0xFF;    // save in the 7th register the LSB of the tmp sensor average

                        // reset the sum and sample count
                        sum_LDR = 0;
                        sum_TMP = 0;
                        num_samples = 0;
                    }
                    
                    ReadFlag = 0; // reset the flag for reading value
                    
                }
                
                break;
                
            case  TMP_STATUS_READ: // 
                   
                if (ReadFlag == 1){ 
                    
                    // sum for average computation
                    sum_TMP = sum_TMP + digit_TMP;
                    num_samples++;
                    
                    if (num_samples == average_samples){
                        
                        //average
                        average_TMP = sum_TMP / average_samples;
                        
                         //convert in temperature
                        average_TMP = ((average_TMP - TMP_INTERCEPT)/TMP_SLOPE)*100; 
                        
                        rgb_value_TMP = average_TMP >> 8;
                        
                        RGBLed_Start();
                        Color tmp_color = {0, 0, rgb_value_TMP};
                        RGBLed_WriteColor(tmp_color);
                        
                        slaveBuffer[MSB_LDR] = 0x00;
                        slaveBuffer[LSB_LDR] = 0x00; // no value is saved in 0x03 e 0x04 (sono riservati per i valori di Ch0)
                        slaveBuffer[MSB_TMP] = average_TMP >> 8; //0b1000 = 0x08 = 8 (shift a dx di 8 bit)
                        slaveBuffer[LSB_TMP] = average_TMP & 0xFF; // nel LSB si salva
                        
                        //reset
                        sum_TMP = 0;
                        num_samples = 0;
                        
                    }
                    
                    ReadFlag = 0;
                    
                }
                
                break;
                
            case LDR_STATUS_READ: 
                       
                if (ReadFlag == 1){
                    
                    sum_LDR = sum_LDR + digit_LDR;
                    num_samples++;
                    
                    if (num_samples == average_samples){
                        average_LDR = sum_LDR / average_samples;
                        
                        //convert in lux
                        double LDR = SERIES_RESISTANCE * (ACTUAL_Vdd_mV / average_LDR - 1.0);
                        average_LDR = (int16) (pow(LDR/TEN_TO_LDR_INTERCEPT, 1/LDR_SLOPE));
                        
                        rgb_value_LDR = average_LDR >> 8;
                        
                        RGBLed_Start();
                        Color ldr_color = {255-rgb_value_LDR, 0, 0};
                        RGBLed_WriteColor(ldr_color);
                        
                        slaveBuffer[MSB_LDR] = average_LDR >> 8;
                        slaveBuffer[LSB_LDR] = average_LDR & 0xFF;
                        slaveBuffer[MSB_TMP] = 0x00;
                        slaveBuffer[LSB_TMP] = 0x00;
                        
                        //reset
                        num_samples=0;
                        sum_LDR=0;
                        
                    }
                    
                    ReadFlag=0;
                }
                
                break;
                
            case SENSORS_OFF:  
                
                RGB_RedLed_Write(OFF);
                RGB_BluLed_Write(OFF);
                RGB_GreenLed_Write(OFF);
                RGBLed_Stop();
                               
                slaveBuffer[MSB_LDR] = 0x00;
                slaveBuffer[LSB_LDR] = 0x00;
                slaveBuffer[MSB_TMP] = 0x00;
                slaveBuffer[LSB_TMP] = 0x00;
                
                break;
                
        }                   
    }
}


/* [] END OF FILE */
