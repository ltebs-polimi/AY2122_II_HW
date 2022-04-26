/* =====================================================
 *
 * ELECTRONIC TECHNOLOGIES AND BIOSENSORS LABORATORY
 * Academic year 2021/22, II Semester
 * Assignment 1

 * Authors: Group 1
 *
 * ----------- INTERROUPT ROUTINE (source) -------------
 * This file contains all the source code required
 * for the interrupt service routines.
 * -----------------------------------------------------
 * 
 * =====================================================
*/

// Include required header files
#include "InterruptRoutines.h"
#include "project.h"
#include "math.h"
#include "RGB_Led_driver.h"

// variables
extern uint8_t slaveBuffer[];

uint8 num_avg_samples = 0;
uint8 RGB_channel = 0;

int red=0;
int green=0;
int blu=0;
int stable=0;
int16 previous_T=0;
int16 new_T=0;



CY_ISR(Custom_Timer_50Hz_ISR)
{
    // Read timer status register to pull interrupt line low
    Timer_50Hz_ReadStatusRegister();
    
    if(flag_send == 1)
    {
        if(temp_celsius > 255 || light_lux > 255) {
            if(temp_celsius > 255) {
                slaveBuffer[5] = temp_celsius & 0xFF;
                slaveBuffer[4] = temp_celsius >> 8;
            }
            if(light_lux > 255) {
                slaveBuffer[3] = light_lux & 0xFF;
                slaveBuffer[2] = light_lux >> 8;
            }  
        } else {
        slaveBuffer[2] = 0x00;
        slaveBuffer[3] = light_lux;
        slaveBuffer[4] = 0x00;
        slaveBuffer[5] = temp_celsius;
        }
        
        flag_send = 0;
        
        counter_samples = 0;
        sum_TMP = 0;
        sum_LDR = 0;
        
        if(led_modality){
            
            //modulate RGB led with TMP readout
            if(temp_celsius <= AMBIENT_T){

                if(red)     RGBLed_WriteRed(RGB_LED_OFF);
                if(green)   RGBLed_WriteGreen(RGB_LED_OFF);
                if(blu)     RGBLed_WriteBlue(RGB_LED_OFF);
                
                
            }else{
                
                if(red)     RGBLed_WriteRed((temp_celsius-AMBIENT_T)*LIGHT_INCREMENT);
                if(green)   RGBLed_WriteGreen((temp_celsius-AMBIENT_T)*LIGHT_INCREMENT);
                if(blu)     RGBLed_WriteBlue((temp_celsius-AMBIENT_T)*LIGHT_INCREMENT);
                             
            }
            
            
        }else{
            
            //modulate RGB led with LDR readout
            if(light_lux<=DARK_CONDITION){
                
                if(red)     RGBLed_WriteRed(RGB_LED_ON);
                if(green)   RGBLed_WriteGreen(RGB_LED_ON);
                if(blu)     RGBLed_WriteBlue(RGB_LED_ON);
                
            }else{
             
                if(red)     RGBLed_WriteRed(RGB_LED_OFF);
                if(green)   RGBLed_WriteGreen(RGB_LED_OFF);
                if(blu)     RGBLed_WriteBlue(RGB_LED_OFF);
                
            }
            
        }
   
    }
    
}

CY_ISR(Custom_Timer_ADC_ISR)
{
    // Read timer status register to pull interrupt line low
    Timer_ADC_ReadStatusRegister();

    switch(sampling_status) ///< Switch case based on the required sampling status
    {
        case 0x00:
            ADC_DelSig_StopConvert();
            AMux_DisconnectAll();
        break;
        
    
        case 0x01: ///< Temperature readout
            
            AMux_FastSelect(TMP_CH);
            if(counter_samples < num_avg_samples) ///< data are sampled until the number of samples to compute the average is reached
            {
                TMP_value = ADC_DelSig_Read16();
                if(TMP_value < 0) TMP_value = 0;
                if(TMP_value > 65535) TMP_value = 65535;
                TMP_mv = ADC_DelSig_CountsTo_mVolts(TMP_value);
            
                sum_TMP = (sum_TMP + TMP_mv);
                
                counter_samples ++;
            } 
           
            break;
        case 0x02: ///< Light readout
            AMux_FastSelect(LDR_CH);
            if(counter_samples < num_avg_samples) ///< data are sampled until the number of samples to compute the average is reached
            {
                LDR_value = ADC_DelSig_Read32();
                if(LDR_value < 0) LDR_value = 0;
                if(LDR_value > 65535) LDR_value = 65535;
                LDR_mv = ADC_DelSig_CountsTo_mVolts(LDR_value);
            
                sum_LDR = (sum_LDR + LDR_mv);
                
                counter_samples ++;
            }
            break;
        case 0x03: ///< Temperature and light readout
            if(counter_samples < num_avg_samples) ///< data are sampled until the number of samples to compute the average is reached
            {
                
                ADC_DelSig_StopConvert(); ///<AMUX datasheet recommends to stop conversion during channel input selection
                AMux_FastSelect(TMP_CH);
                TMP_value = ADC_DelSig_Read32();
                if(TMP_value < 0) TMP_value = 0;
                if(TMP_value > 65535) TMP_value = 65535;
                TMP_mv = ADC_DelSig_CountsTo_mVolts(TMP_value);
            
                sum_TMP = (sum_TMP + TMP_mv);
                
                ADC_DelSig_StopConvert();
                AMux_FastSelect(LDR_CH);
                LDR_value = ADC_DelSig_Read32();
                if(LDR_value < 0) LDR_value = 0;
                if(LDR_value > 65535) LDR_value = 65535;
                LDR_mv = ADC_DelSig_CountsTo_mVolts(LDR_value);
            
                sum_LDR = (sum_LDR + LDR_mv);
                
                counter_samples ++;
            }
            break;
            
        default:   ///< Rest condition
            break;
    }
    
    if(counter_samples == num_avg_samples) ///< After num_avg_samples has been reached, the buffer can be updated
    {
        avg_TMP = sum_TMP / num_avg_samples;
        avg_LDR = sum_LDR / num_avg_samples;
        
        // Save previous value of temperature
        previous_T=temp_celsius;
        
        // Conversion from mV to degrees Celsius
        new_T = (avg_TMP - 500)/10;
        
        // Check if the temperature reading is stable: it's a sort of debouncing
        if(temp_celsius==previous_T){
            
            stable++;
            
        }else{
            
            stable=0;

        }
        
        if(stable==250){
            temp_celsius=new_T;
            stable=0;
        }
        
        // Conversion from mV to resistance (kOhm) considering the voltage divider 
        resistance_kohm= (avg_LDR*100)/(5-avg_LDR);
        
        // COnversion from resistance to lux
        light_lux= 10*pow(100/avg_LDR, 1.25);
        
        // Data are available 
        flag_send = 1;
        
        // counter_samples is increased in order to avoid to perform a further readout 
        counter_samples ++;
        
    }  
    
}

/**
*   This function is called when exiting the EZI2C_ISR. Here we
*   perform all the tasks based on the requests.
*/
void EZI2C_ISR_ExitCallback(void)
{
    
    // Check if the status bits (0 and 1) of control status register 1 have been changed
    if(sampling_status != (slaveBuffer[0] & 0b00000011)){
     
        sampling_status = slaveBuffer[0] & 0b00000011;
        
        
        // The data registers are cleared 
        slaveBuffer[2] = 0;
        slaveBuffer[3] = 0;
        slaveBuffer[4] = 0;
        slaveBuffer[5] = 0;
        
        // The conversion is stopped
        ADC_DelSig_StopConvert();
   
    }
    
    // Check if the led modality bit (2) of control status register 1 has been changed    
    if(led_modality != (slaveBuffer[0] & 0b00000100)>>2){
        
        led_modality = (slaveBuffer[0] & 0b00000100)>>2;
        
    }
    
    // Check if the average samples bits (3 and 4) of control status register 1 have been changed 
    if(num_avg_samples != (slaveBuffer[0] & 0b00011000)>>3){
     
        num_avg_samples = ((slaveBuffer[0] & 0b00011000)>>3) + 1;
        
        
    }
    
    // Check if the led channel bits (5, 6 and 7) of control status register 1 have been changed 
    if(RGB_channel != (slaveBuffer[0]&0b11100000)>>5){
     
        RGB_channel = (slaveBuffer[0]&0b11100000)>>5;
        
        switch(RGB_channel){ // Switch case based on the possible combinations of channels

            case 0x00:
                
                red=0;
                green=0;
                blu=0;
            
                RGBLed_WriteRed(RGB_LED_OFF);
                RGBLed_WriteGreen(RGB_LED_OFF);
                RGBLed_WriteBlue(RGB_LED_OFF);
                    
                break;
            
            case 0x01:
                
                red=1;
                green=0;
                blu=0;
                    
                break;

            case 0x02:
                
                red=0;
                green=1;
                blu=0;
                    
                break;
            
            case 0x03:
                
                red=1;
                green=1;
                blu=0;
                    
                break;
            
            case 0x04:
                
                red=0;
                green=0;
                blu=1;
                    
                break;

            case 0x05:
                
                red=1;
                green=0;
                blu=1;
                    
                break;

            case 0x06:
            
                red=1;
                green=1;
                blu=0;
                    
                break;
            
            
            case 0x07:
            
                red=1;
                green=1;
                blu=1;
                    
                break;
            
            default:
                break;
            
           
        }
        
    }
    
}
/* [] END OF FILE */