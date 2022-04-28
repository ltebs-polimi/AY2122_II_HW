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

uint8_t num_samples = 0;   //counter to count the number of sample to consider for the average

uint8_t average_old;
uint8_t status_bits;            //values of the status bit to activate the right channel
uint8_t led_modality;
uint8_t timer_period;           //value to set the period of the timer
uint8_t led_channel;

uint8_t rgb_value_LDR;          //MSB of the value sampled from LDR  
uint8_t rgb_value_TMP;          //MSB of the value sampled from TMP
uint8_t timer_default;

Color rgb_color;
uint8 color;

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
    
    slaveBuffer[WHO_AM_I]  = I2C_WHOAMI_VALUE;  // Set who am i register value
    slaveBuffer[CTRL_REG1] = SENSORS_OFF;       // Set control reg 1 with all bits = 0 
    

    // set EZI2C buffer
    EZI2C_SetBuffer1(SLAVE_BUFFER_SIZE, SLAVE_BUFFER_SIZE - READ_REGISTERS, slaveBuffer);
    average_old = 1;
    timer_default = Timer_ReadPeriod();
    for(;;)
    
    {
        // Number of samples extracted from the bit 3 and 4 of the control register
        average_samples = ((slaveBuffer[CTRL_REG1] & MASK_AVERAGE_SAMPLES)>>3)+1; //0x18 = 0001 1000 mask
                       
        // Set the timer period
        if (average_old != average_samples) {
            average_old = average_samples;  
            Timer_Stop();
            timer_period = timer_default / average_samples;        
            //Write the period of the timer 
            Timer_WritePeriod(timer_period);
            Timer_Start();
        }
        
       
        // Check only the status bits of the control register
        status_bits = (slaveBuffer[CTRL_REG1] & MASK_STATUS_REG);           // 0x03 = 0000 0011
        
         // Check only the LED Modality
        led_modality = (slaveBuffer[CTRL_REG1] & MASK_LED_MODALITY) >> 2;   // 0x04 = 0000 0100
        
        // Check only the LED channel
        led_channel = (slaveBuffer[CTRL_REG1] & MASK_LED_CHANNEL) >> 5;     // 0xE0 = 1110 0000
        
        Color rgb_color = {0, 0, 0};
        
        // Switch case to check the LDR and TMP status from the values of the status bits 
        switch(status_bits){
            
            case BOTH_CHANNELS_READ: // status = 0b11 = 0x03
                                       
                if (AvgReady == 1){ // enters here when the average is calculated and ready

                    average_TMP = ((average_TMP - TMP_INTERCEPT)/TMP_SLOPE)-OFFSET_TMP ;  // multiply by 100 to have more resolution
                    
                    //convert in lux
                    double LDR = SERIES_RESISTANCE * (ACTUAL_Vdd_mV / average_LDR - 1.0);
                    average_LDR = (int16) (pow(LDR/LDR_INTERCEPT, 1/LDR_SLOPE));

                    int TMP_mod = average_TMP - T_AMBIENT;
                    int LDR_mod = 255-average_LDR*(255/LDR_TH);
                                     
                    // If - else to check the led modality
                    if (led_modality){ // led_modality = 1 --> tmp modulation
                        color = TMP_mod;
                        switch (led_channel){ // COLOR CODING DEPENDING ON COLOR CHANNEL CHOSEN: rgb_color = { B, G, R}
                            case (RED):
                                rgb_color.green = 0;
                                rgb_color.blue  = 0;
                                rgb_color.red   = color;
                                break;
                            case (GREEN):
                                rgb_color.green = color;
                                rgb_color.blue  = 0;
                                rgb_color.red   = 0;
                                break;
                            case (BLUE):
                                rgb_color.green = 0;
                                rgb_color.blue  = color;
                                rgb_color.red   = 0;
                                break;
                            case (GREENRED):
                                rgb_color.green = color;
                                rgb_color.blue  = 0;
                                rgb_color.red   = color;
                                break;
                            case (BLUEGREEN):
                                rgb_color.green = color;
                                rgb_color.blue  = color;
                                rgb_color.red   = 0;
                                break;
                            case (BLUERED):
                                rgb_color.red   = color;
                                rgb_color.blue  = color;
                                rgb_color.green = 0;
                                break;
                            case (RGB):
                                rgb_color.green = color;
                                rgb_color.blue  = color;
                                rgb_color.red   = color;
                                break;
                        }
                        RGBLed_Start();
                        RGBLed_WriteColor(rgb_color);    
                    }
                    else { // led modaility = 0 --> ldr modulation
                        color = LDR_mod;
                        switch (led_channel){ // COLOR CODING DEPENDING ON COLOR CHANNEL CHOSEN: rgb_color = { B, G, R}
                            case (RED):
                                rgb_color.green = 0;
                                rgb_color.blue  = 0;
                                rgb_color.red   = color;
                                break;
                            case (GREEN):
                                rgb_color.green = color;
                                rgb_color.blue  = 0;
                                rgb_color.red   = 0;
                                break;
                            case (BLUE):
                                rgb_color.green = 0;
                                rgb_color.blue  = color;
                                rgb_color.red   = 0;
                                break;
                            case (GREENRED):
                                rgb_color.green = color;
                                rgb_color.blue  = 0;
                                rgb_color.red   = color;
                                break;
                            case (BLUEGREEN):
                                rgb_color.green = color;
                                rgb_color.blue  = color;
                                rgb_color.red   = 0;
                                break;
                            case (BLUERED):
                                rgb_color.red   = color;
                                rgb_color.blue  = color;
                                rgb_color.green = 0;
                                break;
                            case (RGB):
                                rgb_color.green = color;
                                rgb_color.blue  = color;
                                rgb_color.red   = color;
                                break;
                        }
                        RGBLed_Start();
                        RGBLed_WriteColor(rgb_color);
                        
                    }
                    
                    slaveBuffer[MSB_LDR]= average_LDR >> 8;      // save in the 3th register the MSB of the ldr sensor average (8 = 0x08 = 1000)
                    slaveBuffer[LSB_LDR]= average_LDR & 0xFF;    // save in the 4th register the LSB of the ldr sensor average
                    slaveBuffer[MSB_TMP]= average_TMP >> 8;      // save in the 5th register the MSB of the tmp sensor average
                    slaveBuffer[LSB_TMP]= average_TMP & 0xFF;    // save in the 6th register the LSB of the tmp sensor average

                    // reset the sum and sample count
                    sum_LDR = 0;
                    sum_TMP = 0;
                    num_samples = 0;
                    AvgReady = 0; // reset the flag for average calculation   
                }
                
                break;
                
            case  TMP_STATUS_READ: // 
                   
                if (AvgReady == 1){ 
                        
                    //convert in temperature
                    average_TMP = ((average_TMP - TMP_INTERCEPT)/TMP_SLOPE)-OFFSET_TMP ;  // multiply by 100 to have more resolution  
                    int TMP_mod = average_TMP - T_AMBIENT;
                    
                    //rgb_value_TMP = (average_TMP) >> 8;
                    RGBLed_Start();
                    color = TMP_mod; // modulation with tmp is needed
                    
                    switch (led_channel){ // COLOR CODING DEPENDING ON COLOR CHANNEL CHOSEN: rgb_color = { B, G, R}
                        case (RED):
                            rgb_color.green = 0;
                            rgb_color.blue  = 0;
                            rgb_color.red   = color;
                            break;
                        case (GREEN):
                            rgb_color.green = color;
                            rgb_color.blue  = 0;
                            rgb_color.red   = 0;
                            break;
                        case (BLUE):
                            rgb_color.green = 0;
                            rgb_color.blue  = color;
                            rgb_color.red   = 0;
                            break;
                        case (GREENRED):
                            rgb_color.green = color;
                            rgb_color.blue  = 0;
                            rgb_color.red   = color;
                            break;
                        case (BLUEGREEN):
                            rgb_color.green = color;
                            rgb_color.blue  = color;
                            rgb_color.red   = 0;
                            break;
                        case (BLUERED):
                            rgb_color.red   = color;
                            rgb_color.blue  = color;
                            rgb_color.green = 0;
                            break;
                        case (RGB):
                            rgb_color.green = color;
                            rgb_color.blue  = color;
                            rgb_color.red   = color;
                            break;
                    }

                    if (average_TMP > T_AMBIENT){ // turn on LED only if T > ambient temperature
                        RGBLed_Start();
                        RGBLed_WriteColor(rgb_color);
                    } else RGBLed_Stop();
                    slaveBuffer[MSB_LDR] = 0x00;
                    slaveBuffer[LSB_LDR] = 0x00; // no value is saved in 0x03 e 0x04 (sono riservati per i valori di Ch0)
                    slaveBuffer[MSB_TMP] = average_TMP >> 8; //0b1000 = 0x08 = 8 (shift a dx di 8 bit)
                    slaveBuffer[LSB_TMP] = average_TMP & 0xFF; // nel LSB si salva
                    
                    //reset
                    sum_TMP = 0;
                    num_samples = 0;
                    ReadFlag = 0;
                    AvgReady = 0;
                    
                }
                
                break;
                
            case LDR_STATUS_READ: 
                       
                if (AvgReady == 1){
                    
                    double LDR = SERIES_RESISTANCE * (ACTUAL_Vdd_mV / average_LDR - 1.0);
                    average_LDR = (int16) (pow(LDR/LDR_INTERCEPT, 1/LDR_SLOPE));
                    int LDR_mod = 255-average_LDR*(255/LDR_TH);
                    
                                        
                    color = LDR_mod;
                    switch (led_channel){ // COLOR CODING DEPENDING ON COLOR CHANNEL CHOSEN
                        case (RED):
                            rgb_color.red   = color;
                            rgb_color.blue  = 0;
                            rgb_color.green = 0;
                            break;
                        case (GREEN):
                            rgb_color.green = color;
                            rgb_color.blue  = 0;
                            rgb_color.red   = 0;
                            break;
                        case (BLUE):
                            rgb_color.green = 0;
                            rgb_color.blue  = color;
                            rgb_color.red   = 0;
                            break;
                        case (GREENRED):
                            rgb_color.green = color;
                            rgb_color.blue  = 0;
                            rgb_color.red   = color;
                            break;
                        case (BLUEGREEN):
                            rgb_color.green = color;
                            rgb_color.blue  = color;
                            rgb_color.red   = 0;
                            break;
                        case (BLUERED):
                            rgb_color.red   = color;
                            rgb_color.blue  = color;
                            rgb_color.green = 0;
                            break;
                        case (RGB):
                            rgb_color.green = color;
                            rgb_color.blue  = color;
                            rgb_color.red   = color;
                            break;
                    }
                    
                    // turn on LED only in dark environment
                    if (average_LDR < LDR_TH){ 
                        RGBLed_Start();
                        RGBLed_WriteColor(rgb_color);
                    } else RGBLed_Stop();
                    
                    slaveBuffer[MSB_LDR] = average_LDR >> 8;
                    slaveBuffer[LSB_LDR] = average_LDR & 0xFF;
                    slaveBuffer[MSB_TMP] = 0x00;
                    slaveBuffer[LSB_TMP] = 0x00;
                    
                    //reset
                    num_samples=0;
                    sum_LDR=0;
                    AvgReady = 0;
                }
                
                break;
                
            case SENSORS_OFF:  
                
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
