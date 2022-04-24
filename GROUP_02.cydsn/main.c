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

// Define I2C Slave Buffer Structure
#define CTRL_REG_1          0            // Address of Control Register 1
#define WHO_AM_I            1            // Address of Who Am I
#define MSB_LDR             2            // Address of Most Significant Byte of LDR sensor
#define LSB_LDR             3            // Address of Least Significant Byte of LDR sensor
#define MSB_TMP             4            // Address of Most Significant Byte of TMP sensor
#define LSB_TMP             5            // Address of Most Significant Byte of TMP sensor

#define BUFFER_SIZE         6            // Number of addressed registers inside the Slave Buffer
#define RW_SIZE             2            // Number of data bytes starting from the beginning of the buffer with read and write access 
                                            // (data bytes located at offset rwBoundary or greater are read only) 

// Pointer to the data buffer for I2C communication + Initialization of cells where the measurements will be inserted (default values set to 0)
// uint8_t slave_pointer_Buffer[BUFFER_SIZE] = {0,EZI2C_WHO_AM_I_DEFAULT_VALUE,0,0,0,0};   
uint8_t slave_pointer_Buffer[BUFFER_SIZE] = {0};

// Define variable to turn on/off the RGB LED
#define LOW               0                // To set logical value low to 0
#define HIGH              1                // To set logical value high to 1
#define LDR_readout       0                // To use in order to control the LED intensity using the LDR 
#define TMP_readout       1                // To use in order to control the LED intensity using the TMP
                                               // (see also definition above of "uint8_t LED_modality")
#define LED_CHECK_DELAY 1000               //Blink delay in ms of LED CHECK, which is used to visually verify that 
                                           //an active channel is controlling the LED

// Variables declaration
volatile uint8_t PacketReadyFlag = 0;      // Initialize flag to indicate if bytes are to send or not 
volatile int32 sum_TMP;                    // Summation of binary values sampled from TMP
volatile int32 sum_LDR;                    // Summation of binary values sampled from LDR

int16 average_TMP_code;                  // Averaged values of the X samples considered from TMP (binary)
int16 average_LDR_code;                  // Averaged values of the X samples considered from LDR (binary)
int16 average_TMP_mV;                    // Averaged values of the X samples considered from TMP (mV)
int16 average_LDR_mV;                    // Averaged values of the X samples considered from LDR (mV)

uint8_t counter = 0;                     // Counter to be incremented in order to keep track of number of samples
uint8_t LED_modality;                    // LED modality that indicates the sensor that modulates RGB LED intensity
                                            // 0 = LDR readout
                                            // 1 = TMP readout
volatile uint8_t samples = 4;            // Number of samples to be used to compute the average
                                         // Initialization choosing number of samples equal to 4
volatile uint8_t status;                    // bits 0 and 1; status can be set to:
                                            // a. 00 (device stopped)
                                            // b. 01 (to sample LDR channel)
                                            // c. 10 (to sample TMP channel) 
                                            // d. 11 (to sample both channel)

// Timer period
uint8_t timer_period = 0;                   // Period of transmission 
int32 clock_frequency = 6000;               // Timer clock frequency 
                                                  // By choosing a value equal to 6000 Hz we avoid to have a problem of overloading
                                                  // derived from the division computed for obtaining the timer period, beacuse 6000 Hz
                                                  // divided by transmission_datarate*samples (for samples from 1 to 4) is always an integer
int32 transmission_datarate = 50;           // Required transmission data rate// questa variabile dove la utilizziamo?

// PWM
uint16_t PWM_compare = 127;     //SISTEMARE COMMENTO!!!// Compare of the PWM 
                                // Initialization of PWM to 50% (set to 127 since since PWM works at 8 bit)
//uint8_t PWM_period;            // Period of the PWM   
                                 
int main(void)
{
    CyGlobalIntEnable; /* Enable global interrupts. */

    //Starting functions
    Timer_Start(); 
    ADC_DeltaSigma_Start();
    isr_ADC_StartEx(Custom_ISR_ADC); 
    EZI2C_Start();
    PWM_LED_Start(); 
    
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
    
    /* DA CODICE PROF!!!
    // Set up variables for PWM component
    PWM_compare  = PWM_LED_ReadCompare();
    PWM_period  = PWM_LED_ReadPeriod();
    
    // Set up Slave Buffer
    slaveBuffer[0] = PWM_compare;
    slaveBuffer[1] = PWM_period; */
    
        
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
        
        //status = 0x03;
               
        // In order to obtain LED modality, which is on the third bit, the mask to be used is 0b00000100, corresponding to 
        // 0x04 in hexadecimal notation. The result of the masking has to be right-shifted by 2 bit.
        LED_modality = (slave_pointer_Buffer[CTRL_REG_1] & 0x04) >> 2;
           
    
        if (status == EZI2C_STOP_REG_1) {
            
            // By setting one pin of each AND port to 0, all the channels of the RGB led will be off
            Pin_REG_B_Write(LOW);
            Pin_REG_G_Write(LOW);
            Pin_REG_R_Write(LOW);
            
           
           
            slave_pointer_Buffer[MSB_TMP]  = 0x00;
            slave_pointer_Buffer[MSB_LDR]  = 0x00;
            slave_pointer_Buffer[LSB_TMP]  = 0x00;
            slave_pointer_Buffer[LSB_LDR]  = 0x00;
        
        }    

        else if (status == EZI2C_LDR_REG_1) {
            
       
            
           if (PacketReadyFlag == 1 ){
            
              sum_LDR += value_LDR_code; 
                             
              counter++;
                
                if (counter == samples){
                    average_LDR_code = sum_LDR / samples; 
                    
                    // Map to mV
                    average_LDR_mV = ADC_DeltaSigma_CountsTo_mVolts(average_LDR_code);
                    
                    // Save into I2C buffer           
                    // Right shift in order to memorize the Most Significant Byte (MSB) in the address 0x02 of the I2C Slave Buffer 
                    slave_pointer_Buffer[MSB_LDR] = average_LDR_mV >> 8;    
                    // Application of mask 0b11111111 (0xFF in hexadecimal notation) in order to memorize the Least Significant Byte (LSB)
                    // in the address 0x03 of the I2C Slave Buffer 
                    slave_pointer_Buffer[LSB_LDR] = average_LDR_mV & 0xFF;
                    
                    // Control of LED using PWM
                    if(LED_modality==TMP_readout){
                        
                       // Pin_REG_B_Write(LOW);
                       // Pin_REG_G_Write(LOW);
                        Pin_REG_R_Write(LOW);
                        
                        PWM_LED_WriteCompare(average_LDR_code);
                        CyDelay(10);
                        
                        
                       
                        // Use of internal LED to say to the user he/she is trying to control the LED using an inactive channel 
                        LED_CHECK_Write(!LED_CHECK_Read());
                        CyDelay(LED_CHECK_DELAY);// Add delay
                                
                    }
                    else if (LED_modality==LDR_readout){
                        
                       // Pin_REG_B_Write(HIGH);
                        //Pin_REG_G_Write(LOW);
                        Pin_REG_R_Write(HIGH);
                        
                        PWM_LED_WriteCompare(average_LDR_code);
                        CyDelay(10);
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
            
        else if (status == EZI2C_TMP_REG_1) {      
           
          
           
        
           if (PacketReadyFlag == 1 ){
            

              sum_TMP += value_TMP_code; 
                            
            
              counter++;
                
                if (counter == samples){
                    
                    average_TMP_code = sum_TMP / samples; 
                    
                    // Map to mV
                    average_TMP_mV = ADC_DeltaSigma_CountsTo_mVolts(average_TMP_code);
                    
                    // Save into I2C buffer          
                    // Right shift in order to memorize the Most Significant Byte (MSB) in the address 0x04 of the I2C Slave Buffer 
                    slave_pointer_Buffer[MSB_TMP] = average_TMP_mV >> 8;    
                    // Application of mask 0b11111111 (0xFF in hexadecimal notation) in order to memorize the Least Significant Byte (LSB)
                    // in the address 0x05 of the I2C Slave Buffer 
                    slave_pointer_Buffer[LSB_TMP] = average_TMP_mV & 0xFF;
                    
                    // Control of LED using PWM
                    if(LED_modality==LDR_readout){
 
                      //  Pin_REG_B_Write(LOW);
                      //  Pin_REG_G_Write(LOW);
                        Pin_REG_R_Write(LOW);
                        
                     
                       
                        // Use of internal LED to say to the user he/she is trying to control the LED using the switched-off channel 
                        LED_CHECK_Write(!LED_CHECK_Read());
                        CyDelay(LED_CHECK_DELAY);// Add delay
                                
                    }
                    else if (LED_modality==TMP_readout){
                        
                       // Pin_REG_B_Write(HIGH);
                       // Pin_REG_G_Write(HIGH);
                        Pin_REG_R_Write(LOW); 
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
        
       else if (status == EZI2C_LDR_TMP_REG_1) {
          
        //
            
           if (PacketReadyFlag == 1 ){
            
              sum_LDR += value_LDR_code; 
              sum_TMP += value_TMP_code;
                                
              counter++;
                
                if (counter == samples){
                    average_LDR_code = sum_LDR / samples; 
                    average_TMP_code = sum_TMP / samples; 
                    
                    // Map to mV
                    average_LDR_mV = ADC_DeltaSigma_CountsTo_mVolts(average_LDR_code);
                    average_TMP_mV = ADC_DeltaSigma_CountsTo_mVolts(average_TMP_code);
                    
                    // Save into I2C buffer          
                    // Right shift in order to memorize the Most Significant Byte (MSB) of the LDR and TMP sensors in the corresponding addresses 
                    slave_pointer_Buffer[MSB_LDR] = average_LDR_mV >> 8;    
                    slave_pointer_Buffer[MSB_TMP] = average_TMP_mV >> 8;
                 
                    // Application of mask 0b11111111 (0xFF in hexadecimal notation) in order to memorize the Least Significant Byte (LSB)
                    // of the LDR and TMP sensors in the corresponding addresses 
                    slave_pointer_Buffer[LSB_LDR] = average_LDR_mV & 0xFF;
                    slave_pointer_Buffer[LSB_TMP] = average_TMP_mV & 0xFF;
                    
                    // Control of LED using PWM
                    if(LED_modality==TMP_readout){
                        
                        //Pin_REG_B_Write(HIGH);
                       // Pin_REG_G_Write(LOW);
                        Pin_REG_R_Write(HIGH);                                 
                    }
                    else if (LED_modality==LDR_readout){
                        
                       // Pin_REG_B_Write(LOW);
                        //Pin_REG_G_Write(LOW);
                        Pin_REG_R_Write(HIGH);   

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