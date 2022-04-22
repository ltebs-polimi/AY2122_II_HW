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
uint8_t color_channel;          //color combination of the RGB led

int32 PWM_val_LDR;
int32 PWM_val_TMP;
int32 cmp_val;


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
    PWM_LED_R_Start();    
    PWM_LED_G_Start();    
    PWM_LED_B_Start();
    CyDelay(5);
    
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
    
    CyDelay(10);
    
    for (;;)

    {
        
        //Extract the number of samples according to the bits 3 and 4 of the control register 1
        average_samples = ((slaveBuffer[CTRL_REG1] & 0x18) >> 3) + 1; 

        //Check only the status bits of the control register 1
        status_bits = slaveBuffer[CTRL_REG1] & 0x03;
        
        //Check the LED modality of the control register 1
        led_modality = (slaveBuffer[CTRL_REG1] & 0x04) >> 2; 
        
        //Extract the color combination for the RGB led
        color_channel = slaveBuffer[CTRL_REG1] & 0xE0;
        
        //Write the timer period value to configure the timer to the right frequency
        
        timer_period = (3000 / 50 ) / average_samples;
        
        Timer_WritePeriod(timer_period);
        
              
        
        
        // PWM_LED_WriteCompare(cmp_val)
        

        
                
        
        
        //Switch case to check the values of the status bits and sample the right signal(s)
        switch(status_bits){
            
            case SLAVE_BOTH_ON_CTRL_REG1:
                
                if (ReadValue == 1) {                       //check if the value is read from the ADC
                    
                    // sprintf(message, "case: both on, ldr_val: %ld, tmp_val: %ld \r\n", value_digit_LDR, value_digit_TMP);
                    // UART_PutString(message); 
                    
                    //summing values for the average computation
                    sum_digit_LDR = sum_digit_LDR + value_digit_LDR;
                    sum_digit_TMP = sum_digit_TMP + value_digit_TMP;
                    
                    
                    num_samples++;                          //increase the number of samples to compute the average
                    
                    if (num_samples == average_samples){    //check if the number of samples is the right one 
                        
                        sprintf(message, "avg_ldr_val: %ld, avg_tmp_val: %ld \r\n", average_digit_LDR, average_digit_TMP);
                        UART_PutString(message);
                        
                        
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
                
                sprintf(message, "case: tmp on\r\n");
                UART_PutString(message);
                
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
                
                sprintf(message, "case: ldr on\r\n");
                UART_PutString(message); 
                
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
                
                sprintf(message, "case: both off\r\n");
                UART_PutString(message);  
                
                // Pin_LED_Write(OFF);                         //switch the led off
                
                slaveBuffer[MSB_LDR]= 0x00;                                // save no value in the register
                slaveBuffer[LSB_LDR]= 0x00;                                // save no value in the register
                slaveBuffer[MSB_TMP]= 0x00;                                // save no value in the register
                slaveBuffer[LSB_TMP]= 0x00;                                // save no value in the register

                break;
        
            
        }    
        
        // MODULATION OF THE LED
        
        // Standardize the TMP and LDR data between 0 and 1 and mulitiply with PWM's maximum compare value
        //PWM_val_TMP = 10 - (average_digit_TMP / 65535) * 20;
        //average_digit_TMP = 50000;
        
        
        if (average_digit_TMP - 46000 <= 0) {
            PWM_val_TMP = 20;
        } else if (average_digit_TMP - 46000 >= 4000) {
            PWM_val_TMP = 0;
        } else {
            PWM_val_TMP = 20 - ((average_digit_TMP - 46000) / 4000.0) * 20;
        }
                
        
        //sprintf(message, "PWM_val_TMP: %li\r\n", PWM_val_TMP);
        //UART_PutString(message); 
        
        
        if (average_digit_LDR < 1000) {
            PWM_val_LDR = 0;
        } else {
            PWM_val_LDR = 20;
        
        }
            
            
        
        //Switch case to light up the RGB led with the right color combination
        
        switch(led_modality){
            case SLAVE_LED_LDR:
                cmp_val = PWM_val_LDR;
                //cmp_val = 0;
                break;
            case SLAVE_LED_TMP:
                cmp_val = PWM_val_TMP;
                //cmp_val = 0;
                break;
            
        }
        
        switch(color_channel){
            
            case SLAVE_RED_ON_CTRL_REG1:
                PWM_LED_R_WriteCompare(cmp_val);
                PWM_LED_G_WriteCompare(20);
                PWM_LED_B_WriteCompare(20);
            
            break;
            
            case SLAVE_GREEN_ON_CTRL_REG1:
                PWM_LED_R_WriteCompare(20);
                PWM_LED_G_WriteCompare(cmp_val);
                PWM_LED_B_WriteCompare(20);
            
            break;
                
            case SLAVE_BLUE_ON_CTRL_REG1:
                PWM_LED_R_WriteCompare(20);
                PWM_LED_G_WriteCompare(20);
                PWM_LED_B_WriteCompare(cmp_val);
                //PWM_LED_G_Stop();
                //PWM_LED_R_Stop();    
                //PWM_LED_B_Start();
                //LED_R_Write(OFF_R);
                //LED_G_Write(OFF_G);
                //LED_B_Write(ON_B);
            
            break;
                
            case SLAVE_RED_GREEN_ON_CTRL_REG1:
                PWM_LED_R_WriteCompare(cmp_val);
                PWM_LED_G_WriteCompare(cmp_val);
                PWM_LED_B_WriteCompare(20);
            
            break;
                
            case SLAVE_RED_BLUE_ON_CTRL_REG1:
                PWM_LED_R_WriteCompare(cmp_val);
                PWM_LED_G_WriteCompare(20);
                PWM_LED_B_WriteCompare(cmp_val);
            
            break;
                
            case SLAVE_GREEN_BLUE_ON_CTRL_REG1:
                PWM_LED_R_WriteCompare(20);
                PWM_LED_G_WriteCompare(cmp_val);
                PWM_LED_B_WriteCompare(cmp_val);
            
            break;
                
            case SLAVE_RED_GREEN_BLUE_ON_CTRL_REG1:                
                PWM_LED_R_WriteCompare(cmp_val);
                PWM_LED_G_WriteCompare(cmp_val);
                PWM_LED_B_WriteCompare(cmp_val);
            
            break;
                
            case SLAVE_RED_GREEN_BLUE_OFF_CTRL_REG1:
                PWM_LED_R_WriteCompare(20);
                PWM_LED_G_WriteCompare(20);
                PWM_LED_B_WriteCompare(20);
            
            break;
                
        }
        
        
        
    }
    
    
    
}
    
/* [] END OF FILE */
