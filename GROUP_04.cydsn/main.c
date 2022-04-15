/* ========================================
 
Electronic Technologies and Biosensors Laboratory
Academic Year 2020/2021 - II Semester
Assignment 03
GROUP_01 

main source file: It starts the general functionality of the project. 
                  The average is calculated and stored adequately.  
 
 * ========================================
*/

//Include necessary headers and libraries
#include "project.h"
#include "InterruptRoutines.h"
#include "RegAddress.h"

//Define structure of slave buffer
#define SLAVE_BUFFER_SIZE 6     //number of registers
#define CTRL_REG1 0             //position of control register 1
#define WHO_AM_I 1              //position of who am i register
#define MSB_LDR 2               //position for Most Significant Byte of the first sensor average
#define LSB_LDR 3               //position for Less Significant Byte of the first sensor average
#define MSB_TMP 4               //position for Most Significant Byte of the second sensor average
#define LSB_TMP 5               //position for Less Significant Byte of the second sensor average

//Define led status - RGB
#define ON_R 1
#define OFF_R 0
#define ON_G 1
#define OFF_G 0
#define ON_B 1
#define OFF_B 0

//Define slaveBuffer of the EZI2C
uint8 slaveBuffer[SLAVE_BUFFER_SIZE];


//Define the flag to read the value from the ADC and set it to 0
volatile uint8 ReadValue = 0; 

int32 sum_digit_LDR;            //sum in digit of LDR sensor
int32 sum_digit_TMP;            //sum in digit of TMP sensor
int32 average_digit_LDR;        //average in digit of LDR sensor
int32 average_digit_TMP;        //average in digit of LDR sensor
uint8_t num_samples = 0;        //counter to count the number of sample to consider for the average

uint8_t average_samples;        //number of samples for the average  
uint8_t status_bits;            //values of the status bit to activate the right channel
uint8_t timer_period;           //value to set the period of the timer
uint8_t led_modality;           //value of the led modality


/***************************************
*                main
***************************************/

int main(void)
{
    CyGlobalIntEnable; /* Enable global interrupts. */

    //Starting functions
    Timer_Start(); 
    ADC_DelSig_Start();
    isr_StartEx(Custom_ISR_ADC);
    AMux_Start();
    EZI2C_Start();
    UART_Start();
    // maybe a PWM start ?
    
    
    ADC_DelSig_StartConvert(); // start ADC conversion

    slaveBuffer[WHO_AM_I] = SLAVE_WHOAMI_VALUE;         // Set who am i register value
    slaveBuffer[CTRL_REG1] = SLAVE_MODE_OFF_CTRL_REG1;  //set control reg 1 with all bits = 0 
    
    //set EZI2C buffer
    //SLAVE_BUFFER_SIZE - 4 is the third position, the boundary of the r/w cells 
    EZI2C_SetBuffer1(SLAVE_BUFFER_SIZE, SLAVE_BUFFER_SIZE - 4, slaveBuffer);

    char message[50] = {'\0'};
    
    UART_PutString("**************\r\n");
    UART_PutString("** I2C Scan **\r\n");
    UART_PutString("**************\r\n");
    
    for (;;)

    {
        
        //Extract the number of samples according to the bits 3 and 4 of the control register 1
        average_samples = (slaveBuffer[CTRL_REG1] & 0x18) >> 3; 

        //Check only the status bits of the control register 1
        status_bits = slaveBuffer[CTRL_REG1] & 0x03;
        
        //Check the LED modality of the control register 1
        led_modality = (slaveBuffer[CTRL_REG1] & 0x04) >> 2; 
        
        //Read from the Control register 2 the timer period
        // timer_period = slaveBuffer[CTRL_REG2];
        
        //Write the timer period value to configure the timer to the right frequency
        Timer_WritePeriod(timer_period);
        
        //Switch case to check the values of the status bits and sample the right signal(s)
        switch(status_bits){
            
            case SLAVE_BOTH_ON_CTRL_REG1:
                
                // Pin_LED_Write(ON);                          // Switch the led on 
                
                if (ReadValue == 1) {                       //check if the value is read from the ADC
                    
                    //summing values for the average computation
                    sum_digit_LDR = sum_digit_LDR + value_digit_LDR;
                    sum_digit_TMP = sum_digit_TMP + value_digit_TMP;
                    
                    sprintf(message, "\r\naverage digit ldr: 0x%2.2ld\r\n", value_digit_LDR);
                    UART_PutString(message); 
                    
                    num_samples++;                          //increase the number of samples to compute the average
                    
                    if (num_samples == average_samples){    //check if the number of samples is the right one 
                        
                        // calculate the average
                        average_digit_LDR = sum_digit_LDR / average_samples;
                        average_digit_TMP = sum_digit_TMP / average_samples;

                        slaveBuffer[MSB_LDR]= average_digit_LDR >> 8;      // save in the 4th register the MSB of the ldr sensor average
                        slaveBuffer[LSB_LDR]= average_digit_LDR & 0xFF;    // save in the 5th register the LSB of the ldr sensor average
                        slaveBuffer[MSB_TMP]= average_digit_TMP >> 8;      // save in the 6th register the MSB of the tmp sensor average
                        slaveBuffer[LSB_TMP]= average_digit_TMP & 0xFF;    // save in the 7th register the LSB of the tmp sensor average
                        
                        // reset the sum and sample count
                        sum_digit_LDR = 0;
                        sum_digit_TMP = 0;
                        num_samples = 0;
                    }
                  
                    
                    ReadValue = 0;                          // Reset the flag for reading value
                    
                }
                    
            
                break;
                
            case SLAVE_TMP_ON_CTRL_REG1:
                
                // Pin_LED_Write(OFF);                         //switch the led off
                
                if (ReadValue == 1) {                       //check if the value is read from the ADC
                
                    //summing values for the average computation
                    sum_digit_TMP = sum_digit_TMP + value_digit_TMP;
                    
                    num_samples++;                          //increase the number of samples to compute the average
                    
                    if (num_samples == average_samples){    //check if the number of samples is the right one
                        
                        // calculate the average
                        average_digit_TMP = sum_digit_TMP / average_samples;
                                              
                        slaveBuffer[MSB_LDR]= 0x00;                        // save no value in the register
                        slaveBuffer[LSB_LDR]= 0x00;                        // save no value in the register
                        slaveBuffer[MSB_TMP]= average_digit_TMP >> 8;      // save in the 6th register the MSB of the tmp sensor average
                        slaveBuffer[LSB_TMP]= average_digit_TMP & 0xFF;    // save in the 7th register the LSB of the tmp sensor average
                    
                        // reset the sum and sample count
                        sum_digit_TMP = 0;
                        num_samples = 0;
                    }
   
                    ReadValue = 0;                          // Reset the flag for reading value
                    
                }
                
                break;
                
            case SLAVE_LDR_ON_CTRL_REG1:
                
                // Pin_LED_Write(OFF);                         //switch the led off
                
                if (ReadValue == 1) {                       //check if the value is read from the ADC
                    
                    //summing values for the average computation
                    sum_digit_LDR = sum_digit_LDR + value_digit_LDR;
                    
                    num_samples++;                          //increase the number of samples to compute the average
                    
                    if (num_samples == average_samples){    //check if the number of samples is the right one
                        
                        // calculate the average
                        average_digit_LDR = sum_digit_LDR / average_samples;
                                             
                        slaveBuffer[MSB_LDR]= average_digit_LDR >> 8;      // save in the 4th register the MSB of the ldr sensor average
                        slaveBuffer[LSB_LDR]= average_digit_LDR & 0xFF;    // save in the 5th register the LSB of the ldr sensor average
                        slaveBuffer[MSB_TMP]= 0x00;                        // save no value in the register
                        slaveBuffer[LSB_TMP]= 0x00;                        // save no value in the register
                        
                        // reset the sum and sample count
                        sum_digit_LDR = 0;
                        num_samples = 0;
                    }
                    
                    
                    ReadValue = 0;                          // Reset the flag for reading value
                    
                }
                
                break;
                
            case SLAVE_MODE_OFF_CTRL_REG1:
                
                // Pin_LED_Write(OFF);                         //switch the led off
                
                slaveBuffer[MSB_LDR]= 0x00;                                // save no value in the register
                slaveBuffer[LSB_LDR]= 0x00;                                // save no value in the register
                slaveBuffer[MSB_TMP]= 0x00;                                // save no value in the register
                slaveBuffer[LSB_TMP]= 0x00;                                // save no value in the register

                break;
        
            
        }
        
        // sprintf(message, "average digit ldr: 0x%02X", average_digit_LDR);
        // average_digit_TMP
        
    }
}
    
/* [] END OF FILE */
