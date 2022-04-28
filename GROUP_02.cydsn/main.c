/* ========================================
 *
 * LTEBS - A.Y. 2021/2022, II semester
 * Assignment, Group 02
 * Authors: Federico Petraccia, Francesca Terranova
 * 
 * main source file
 *
 * ========================================
*/

//Include necessary headers and libraries
#include "project.h"
#include "InterruptRoutines.h"
#include "EZI2C.h"
#include "math.h"
#include "stdio.h"
#include "stdlib.h"
#include "definitions.h"

int main(void)
{
    CyGlobalIntEnable; /* Enable global interrupts. */

    //Starting functions
    Timer_Start(); 
    ADC_DeltaSigma_Start();
    isr_ADC_StartEx(Custom_ISR_ADC); 
    EZI2C_Start();   
    PWM_Start();
        
    AMux_ADC_Start(); 

    // Initialize of EZI2C slave
    EZI2C_SetBuffer1(BUFFER_SIZE, RW_SIZE, slave_pointer_Buffer); // This allows to claim which are the cells that can be used for read and write (R/W) for I2C protocol 
    slave_pointer_Buffer[WHO_AM_I] = EZI2C_WHO_AM_I_DEFAULT_VALUE;
    slave_pointer_Buffer[CTRL_REG_1] = EZI2C_STOP_REG_1;
       
    // Start the ADC conversion
    ADC_DeltaSigma_StartConvert(); 
        
    timer_period = clock_frequency/(transmission_datarate*samples);         // Expression to compute the timer period
    Timer_WritePeriod(timer_period-1);                                      // Writes the period register of the timer                  
    // Note: The Period minus one is the initial value loaded into the period register. The software can change this register
    // at any time with the Timer_WritePeriod() API. To get the equivalent result using this API, the Period value from the
    // customizer, minus one, must be used as the argument in the function. (Datasheet Timer 2.80, pag 5)
   
    
    
    for(;;)
    {
        
        // Since the "Average samples bit 1" and the "Average sample bit 2" are positioned on bit 4 and 3, 
        // the mask to apply to the Control Reg 1 will be 0b00011000 which corresponds to 0x18 in hexadecimal notation.
        // In order to obtain the only the 2 bits of interest I need to shift to the right by 3 positions.
        samples = (slave_pointer_Buffer[CTRL_REG_1] & 0x18) >> 3;
        
        // In order to obtain the 2 bit indicating the status (i.e. if we want to sample LDR channel, TMP channel,
        // both of them or stop the acquisition), it is necessary to mask the value contained in the Control Register 1.
        // The mask to be used for this operation is 0b00000011, which corresponds to 0x03. This mask allow to take
        // the first 2 bits of the Control Register 1, containing TMP status and LDR status. 
        status =  slave_pointer_Buffer[CTRL_REG_1] & 0x03;
               
        // In order to obtain LED modality, which is on the third bit, the mask to be used is 0b00000100, corresponding to 
        // 0x04 in hexadecimal notation. The result of the masking has to be right-shifted by 2 bit.
        LED_modality = (slave_pointer_Buffer[CTRL_REG_1] & 0x04) >> 2;
    
        //  STOP - Code executed when no sensors is active
        if (status == EZI2C_STOP_REG_1) 
        {
            
            LED_CHECK_Write(LOW);   
            Pin_LED_R_Write(LOW);
            Pin_LED_B_Write(LOW);
            Pin_LED_G_Write(LOW);            
           
            slave_pointer_Buffer[MSB_TMP]  = 0x00;
            slave_pointer_Buffer[MSB_LDR]  = 0x00;
            slave_pointer_Buffer[LSB_TMP]  = 0x00;
            slave_pointer_Buffer[LSB_LDR]  = 0x00;
        
        }    
        
        // LDR - Code executed when LDR sensor is active 
        else if (status == EZI2C_LDR_REG_1) 
        {
            
           if (PacketReadyFlag == 1 ){
            
              sum_LDR += value_LDR_code; 
                             
              counter++;
                
                if (counter == samples)
                {
                    average_LDR_code = sum_LDR / samples;                 
                    
                    // Conversion to lux
                    average_LDR_mV = ADC_DeltaSigma_CountsTo_mVolts(average_LDR_code);
                    double LDR = SERIES_RESISTANCE * (ACTUAL_Vdd_mV / average_LDR_mV- 1.0);
                    average_LDR_lux = (int16) (pow(LDR/TEN_TO_LDR_INTERCEPT, 1/LDR_SLOPE));

                    // Save into I2C buffer                       
                    // Right shift in order to memorize the Most Significant Byte (MSB) in the address 0x02 of the I2C Slave Buffer 
                    slave_pointer_Buffer[MSB_LDR] = average_LDR_lux >> 8;    
                    // Application of mask 0b11111111 (0xFF in hexadecimal notation) in order to memorize the Least Significant Byte (LSB)
                    // in the address 0x03 of the I2C Slave Buffer 
                    slave_pointer_Buffer[LSB_LDR] = average_LDR_lux & 0xFF;
                   
                    // Control of LED using PWM
                    if(LED_modality==TMP_readout)
                    {
                        
                        // Use of internal LED to say to the user he/she is trying to control the LED using an inactive channel 
                        LED_CHECK_Write(!LED_CHECK_Read());
                        CyDelay(LED_CHECK_DELAY);// Add delay
                        
                        // Since the only channel that is read in this modality is the LDR, we decided not to activate the PWM 
                        // when the user choose that the LED has to be controlled by the TMP sensor.
                                                                         
                    }
                    else if (LED_modality==LDR_readout)
                    {
                        
                        LED_CHECK_Write(LOW); 
                        Pin_LED_R_Write(HIGH);  // red channel is active
                        Pin_LED_B_Write(LOW);
                        Pin_LED_G_Write(LOW); 
                        
                        compareValueLDR = average_LDR_code;
                          
                        value = -((compareValueLDR-20000)*255)/30000;
                        if(value>255) value=255;
                        if(value<0) value=0; 
                        
                        PWM_WriteCompare(value);                      
                        
                    }
                        
                    // Reset counter and variables related to LDR sensor
                    sum_LDR = 0;
                    counter = 0;
                    average_LDR_code = 0;
                    average_LDR_mV = 0;

                }
                
                PacketReadyFlag = 0;
                
            }    
        
        }    
        
        // TMP - Code executed when TMP sensor is active 
        else if (status == EZI2C_TMP_REG_1) {           
        
           if (PacketReadyFlag == 1 )
           {
            
              sum_TMP += value_TMP_code; 
                                     
              counter++;
                
                if (counter == samples)
                {
                    
                    average_TMP_code = sum_TMP / samples; 
                    
                    // Conversion to temperature (°C)                    
                    average_TMP_mV = ADC_DeltaSigma_CountsTo_mVolts(average_TMP_code);
                    average_TMP_temp = (ADC_DeltaSigma_CountsTo_mVolts(average_TMP_code)-q_TMP)/sensibility_TMP;
                    
                    // Save into I2C buffer          
                    // Right shift in order to memorize the Most Significant Byte (MSB) in the address 0x04 of the I2C Slave Buffer 
                    slave_pointer_Buffer[MSB_TMP] = average_TMP_temp >> 8;    
                    // Application of mask 0b11111111 (0xFF in hexadecimal notation) in order to memorize the Least Significant Byte (LSB)
                    // in the address 0x05 of the I2C Slave Buffer 
                    slave_pointer_Buffer[LSB_TMP] = average_TMP_temp & 0xFF;
                    
                    // Control of LED using PWM
                    if(LED_modality==LDR_readout){
                        
                        // Use of internal LED to say to the user he/she is trying to control the LED using the switched-off channel 
                        LED_CHECK_Write(!LED_CHECK_Read());
                        CyDelay(LED_CHECK_DELAY);// Add delay
                        
                        // Since the only channel that is read in this modality is the TMP, we decided not to activate the PWM 
                        // when the user choose that the LED has to be controlled by the LDR sensor.                 
                                
                    }
                    else if (LED_modality==TMP_readout){
                        
                        LED_CHECK_Write(LOW);
                       
                        compareValueTMP = average_TMP_code;
                        
                        value = ((compareValueTMP-10430)*255)/810;      // between 23°C e 29°C  
                        if(value>255) value=255;
                        if(value<0) value=0; 
                        
                        PWM_WriteCompare(value);
                        
                        if(compareValueTMP<10430)
                        {
                            Pin_LED_R_Write(LOW);
                            Pin_LED_B_Write(LOW);
                            Pin_LED_G_Write(LOW);
                        }
                        else
                        {
                            Pin_LED_R_Write(LOW);
                            Pin_LED_B_Write(HIGH);  // blue channel is active
                            Pin_LED_G_Write(LOW);
                        }                           
                        
                    }
                    
                    // Reset counter and variables related to TMP sensor
                    sum_TMP = 0;
                    counter = 0;
                    average_TMP_code = 0;
                    average_TMP_mV = 0; 

                }
                
                PacketReadyFlag = 0;
                
            }    
        }
       
       // TMP and LDR - Code executed when both TMP and LDR sensors are active 
       else if (status == EZI2C_LDR_TMP_REG_1) {
        
            LED_CHECK_Write(LOW);
           
           if (PacketReadyFlag == 1 ){
            
              sum_LDR += value_LDR_code; 
              sum_TMP += value_TMP_code;
                                
              counter++;
                
                if (counter == samples){
                    average_LDR_code = sum_LDR / samples; 
                    average_TMP_code = sum_TMP / samples; 
                    
                    // Conversion to lux
                    average_LDR_mV = ADC_DeltaSigma_CountsTo_mVolts(average_LDR_code);
                    double LDR = SERIES_RESISTANCE * (ACTUAL_Vdd_mV / average_LDR_mV- 1.0);
                    average_LDR_lux = (int16) (pow(LDR/TEN_TO_LDR_INTERCEPT, 1/LDR_SLOPE));
                    
                    // Conversion to temperature (°C)                    
                    average_TMP_mV = ADC_DeltaSigma_CountsTo_mVolts(average_TMP_code);
                    average_TMP_temp = (ADC_DeltaSigma_CountsTo_mVolts(average_TMP_code)-q_TMP)/sensibility_TMP;
                                        
                    // Save into I2C buffer          
                    // Right shift in order to memorize the Most Significant Byte (MSB) of the LDR and TMP sensors in the corresponding addresses 
                    slave_pointer_Buffer[MSB_LDR] = average_LDR_lux >> 8;    
                    slave_pointer_Buffer[MSB_TMP] = average_TMP_temp >> 8;
                    // Application of mask 0b11111111 (0xFF in hexadecimal notation) in order to memorize the Least Significant Byte (LSB)
                    // of the LDR and TMP sensors in the corresponding addresses 
                    slave_pointer_Buffer[LSB_LDR] = average_LDR_lux & 0xFF;
                    slave_pointer_Buffer[LSB_TMP] = average_TMP_temp & 0xFF;
                    
                    // Control of LED using PWM
                    if(LED_modality==TMP_readout)
                    {
                        
                        compareValueTMP = average_TMP_code;
                        
                        value = ((compareValueTMP-10430)*255)/810;      // between 23°C e 29°C  
                        if(value>255) value=255;
                        if(value<0) value=0; 
                        
                        PWM_WriteCompare(value);
                        
                        if(compareValueTMP<10430)
                        {
                            Pin_LED_R_Write(LOW);
                            Pin_LED_B_Write(LOW);
                            Pin_LED_G_Write(LOW);
                        }
                        else
                        {
                            Pin_LED_R_Write(LOW);
                            Pin_LED_B_Write(LOW);  
                            Pin_LED_G_Write(HIGH);    // green channel is active 
                        }  
                                
                    }
                    else if (LED_modality==LDR_readout)
                    {
                            Pin_LED_R_Write(HIGH);  // red and blue channel are active, resulting in a purple light
                            Pin_LED_B_Write(HIGH);
                            Pin_LED_G_Write(LOW);     
                            
                        compareValueLDR = average_LDR_code;
                          
                        value = -((compareValueLDR-20000)*255)/30000;
                        if(value>255) value=255;
                        if(value<0) value=0; 
                            
                        PWM_WriteCompare(value);

                    }
                    
                    // Reset counter and variables related to TMP and LDR sensors
                    sum_LDR = 0;
                    sum_TMP = 0;
                    counter = 0;
                    average_LDR_code = 0;
                    average_TMP_code = 0;
                    average_LDR_mV = 0; 
                    average_TMP_mV = 0; 

                }
                
                PacketReadyFlag = 0;
                
            }  
        }  
    }
}
                
/* [] END OF FILE */