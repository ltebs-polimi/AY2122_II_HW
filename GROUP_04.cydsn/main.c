/* ==============================================
 
Electronic Technologies and Biosensors Laboratory
Academic Year 2021/2022 - II Semester
Assignment
GROUP_04 - Franke Patrick & Canavate Chloé 
 
    main file
============================================== */


//Include headers
#include "project.h"
#include "InterruptRoutines.h"
#include "RegAddress.h"

//Define structure of slave buffer
#define SLAVE_BUFFER_SIZE 6     //Number of registers
#define CTRL_REG1 0             //Position of control register 1
#define WHO_AM_I 1              //Position of who am i register
#define MSB_LDR 2               //Position for Most Significant Byte of the LDR average
#define LSB_LDR 3               //Position for Less Significant Byte of the LDR average
#define MSB_TMP 4               //Position for Most Significant Byte of the TMP average
#define LSB_TMP 5               //position for Less Significant Byte of the TMP average

//Define slaveBuffer of the EZI2C
uint8 slaveBuffer[SLAVE_BUFFER_SIZE];

//Define the flag to read the value from the ADC and set it to 0
volatile uint8 ReadValue = 0; 


int32 sum_LDR;                  //Sum of LDR sensor samples to compute the average
int32 sum_TMP;                  //Sum of TMP sensor samples to compute the average
int32 average_LDR;              //Average of LDR sensor
int32 average_TMP;              //Average of LDR sensor
uint8_t nb_samples = 0;         //Counter to count the number of samples to consider for the average

//Define the variables to store readings from the control register 1
uint8_t nb_average_samples;     //Number of samples for the average  
uint8_t status_bits;            //Value of the status bits to control LDR and TMP on/off
uint8_t timer_period;           //Value to set the period of the timer according to the number of samples
uint8_t led_modality;           //Value of the led modality
uint8_t color_channel;          //Color combination of the RGB led

int32 PWM_val_LDR;
int32 PWM_val_TMP;
int32 cmp_val;




int main(void)
{
    CyGlobalIntEnable; //Enable global interrupts

    //Start the components
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
    
    ADC_DelSig_StartConvert();  //Start ADC conversion

    slaveBuffer[WHO_AM_I] = SLAVE_WHOAMI_VALUE;         //Set Who Am I register value
    slaveBuffer[CTRL_REG1] = SLAVE_MODE_OFF_CTRL_REG1;  //Set Control Register 1 to default value (0x00)
    
    //Set EZI2C buffer
    //SLAVE_BUFFER_SIZE - 5 is the second position (boundary of r/w cells)
    EZI2C_SetBuffer1(SLAVE_BUFFER_SIZE, SLAVE_BUFFER_SIZE - 5, slaveBuffer);
    
    char message[50];
    
    for (;;)
    {
        //Extract the number of samples according to the bits 3 and 4 of the Control Register 1
        nb_average_samples = ((slaveBuffer[CTRL_REG1] & 0x18) >> 3) + 1; 

        //Extract the status bits (bits 0 and 1) of the Control Register 1 
        status_bits = slaveBuffer[CTRL_REG1] & 0x03;
        
        //Extract the LED modality from the Control Register 1
        led_modality = (slaveBuffer[CTRL_REG1] & 0x04) >> 2; 
        
        //Extract the color combination for the RGB led
        color_channel = slaveBuffer[CTRL_REG1] & 0xE0;
        
        //Write the timer period to configure the timer to the right frequency according to the number of samples (nb_average_samples)
        //3000Hz is the clock frequency and 50Hz is the required transmission data rate
        timer_period = (3000/50) / nb_average_samples;
        Timer_WritePeriod(timer_period);
        
        
        //Switch case to check the value of the status bits and sample the right sensor(s)
        switch(status_bits)
        {
            case SLAVE_BOTH_ON_CTRL_REG1:   //Sampling asked for both LDR and TMP
                
                if (ReadValue == 1)         //Check if the value is read from the ADC
                {                       
                    //Sampling sums update
                    sum_LDR = sum_LDR + value_LDR;
                    sum_TMP = sum_TMP + value_TMP;
                    
                    nb_samples++;           //Increment nb of samples read
                    
                    if (nb_samples == nb_average_samples)   //Check if the number of samples already done corresponds to the asked nb of samples
                    {    
                        
                        sprintf(message, "LDR: %ld, TMP: %ld \r\n", average_LDR, average_TMP);
                        UART_PutString(message);
                        
                        //Compute the averages
                        average_LDR = sum_LDR / nb_average_samples;
                        average_TMP = sum_TMP / nb_average_samples;
                        
                        //Write in the buffer
                        slaveBuffer[MSB_LDR] = average_LDR >> 8;      //Write in the 3rd register the MSB of the LDR sensor average
                        slaveBuffer[LSB_LDR] = average_LDR & 0xFF;    //Write in the 4th register the LSB of the LDR sensor average
                        slaveBuffer[MSB_TMP] = average_TMP >> 8;      //Write in the 5th register the MSB of the TMP sensor average
                        slaveBuffer[LSB_TMP] = average_TMP & 0xFF;    //Write in the 6th register the LSB of the TMP sensor average
                        
                        //Reset sum and nb of samples counter
                        sum_LDR = 0;
                        sum_TMP = 0;
                        nb_samples = 0;
                    }
                    
                    ReadValue = 0;  //Reset the flag for reading value  
                }
                break;
                
                
            case SLAVE_TMP_ON_CTRL_REG1:    //Sampling asked for TMP only
                
                if (ReadValue == 1)         //Check if the value is read from the ADC
                {                       
                    //Sampling sum update
                    sum_TMP = sum_TMP + value_TMP;
                    
                    nb_samples++;           //Increment nb of samples read
                    
                    if (nb_samples == nb_average_samples)    //Check if the number of samples already done corresponds to the asked nb of samples
                    {    
                        //Compute the average
                        average_TMP = sum_TMP / nb_average_samples;
                        
                        //Write in the buffer
                        slaveBuffer[MSB_LDR] = 0x00;                  //Write 0 in the 3rd register (no LDR sampling)
                        slaveBuffer[LSB_LDR] = 0x00;                  //Write 0 in the 4th register (no LDR sampling)
                        slaveBuffer[MSB_TMP] = average_TMP >> 8;      //Write in the 5th register the MSB of the TMP sensor average
                        slaveBuffer[LSB_TMP] = average_TMP & 0xFF;    //Write in the 6th register the LSB of the TMP sensor average
                    
                        //Reset sum and nb of samples counter
                        sum_TMP = 0;
                        nb_samples = 0;
                    }
   
                    ReadValue = 0;  //Reset the flag for reading value
                }
                break;
                
                
            case SLAVE_LDR_ON_CTRL_REG1:    //Sampling asked for LDR only
                
                if (ReadValue == 1)         //Check if the value is read from the ADC
                {    
                    //Sampling sum update
                    sum_LDR = sum_LDR + value_LDR;
                    
                    nb_samples++;           //Increment nb of samples read
                    
                    if (nb_samples == nb_average_samples)    //Check if the number of samples already done corresponds to the asked nb of samples
                    {    
                        //Compute the average
                        average_LDR = sum_LDR / nb_average_samples;
                                 
                        //Write in the buffer
                        slaveBuffer[MSB_LDR] = average_LDR >> 8;      //Write in the 3rd register the MSB of the LDR sensor average
                        slaveBuffer[LSB_LDR] = average_LDR & 0xFF;    //Write in the 4th register the LSB of the LDR sensor average
                        slaveBuffer[MSB_TMP] = 0x00;                  //Write 0 in the 5th register (no TMP sampling)
                        slaveBuffer[LSB_TMP] = 0x00;                  //Write 0 in the 6th register (no TMP sampling)
                        
                        //Reset sum and nb of samples counter
                        sum_LDR = 0;
                        nb_samples = 0;
                    }

                    ReadValue = 0;  //Reset the flag for reading value 
                }
                break;
                
                
            case SLAVE_MODE_OFF_CTRL_REG1:      //No sampling asked
                
                //Write in the buffer
                slaveBuffer[MSB_LDR] = 0x00;                         //Write 0 in the 3rd register (no LDR sampling)
                slaveBuffer[LSB_LDR] = 0x00;                         //Write 0 in the 4th register (no LDR sampling)
                slaveBuffer[MSB_TMP] = 0x00;                         //Write 0 in the 5th register (no TMP sampling)
                slaveBuffer[LSB_TMP] = 0x00;                         //Write 0 in the 6th register (no TMP sampling)

                break;
        }    
        
        
        //Modulation of the RGB led
        
        //Switch case according to user led modality choice (either LDR or TMP)
        switch(led_modality)
        { 
            case SLAVE_LED_LDR:
                
                //Threshold below which the environment is said to be dark
                if (average_LDR < 400)     PWM_val_LDR = 0;    //The RGB led will be set to maximum intensity

                else    PWM_val_LDR = 20;                       //The RGB led will be turned off
                
                cmp_val = PWM_val_LDR;
                break;
            
            //TMP sensor values range mostly between 9000 and 11000
            case SLAVE_LED_TMP:
                
                //Check if value is below lower bound and set compare value to turn off RGB led later on
                if (average_TMP - 9000 <= 0)    PWM_val_TMP = 20;  
                
                //Check if value is above upper bound and set compare value to get RGB led max intensity later on
                else if (average_TMP - 9000 >= 2000)   PWM_val_TMP = 0;
                               
                else    //The TMP sensor outcome ranges between 9000 and 11000; the intensity will be modulated
                {
                    //Formula to normalize TMP sensor outcome between 0 and 1 and mulitiply with PWM's maximum compare value (20)
                    //To adapt compare value of the modulator
                    PWM_val_TMP = 20-((average_TMP-9000)/2000.0)*20;
                }
                
                cmp_val = PWM_val_TMP;
                break;   
        }

        
        //Switch case to light up the RGB led with the right color combination
        switch(color_channel){
            
            case SLAVE_RED_ON_CTRL_REG1:                //Red light only
                PWM_LED_R_WriteCompare(cmp_val);
                PWM_LED_G_WriteCompare(20);
                PWM_LED_B_WriteCompare(20);
            break;
            
            case SLAVE_GREEN_ON_CTRL_REG1:              //Green light only
                PWM_LED_R_WriteCompare(20);
                PWM_LED_G_WriteCompare(cmp_val);
                PWM_LED_B_WriteCompare(20);
            break;
                
            case SLAVE_BLUE_ON_CTRL_REG1:               //Blue light only
                PWM_LED_R_WriteCompare(20);
                PWM_LED_G_WriteCompare(20);
                PWM_LED_B_WriteCompare(cmp_val);
            break;
                
            case SLAVE_RED_GREEN_ON_CTRL_REG1:          //Both red and green lights
                PWM_LED_R_WriteCompare(cmp_val);
                PWM_LED_G_WriteCompare(cmp_val);
                PWM_LED_B_WriteCompare(20);
            break;
                
            case SLAVE_RED_BLUE_ON_CTRL_REG1:           //Both red and blue lights
                PWM_LED_R_WriteCompare(cmp_val);
                PWM_LED_G_WriteCompare(20);
                PWM_LED_B_WriteCompare(cmp_val);
            break;
                
            case SLAVE_GREEN_BLUE_ON_CTRL_REG1:         //Both green and blue lights
                PWM_LED_R_WriteCompare(20);
                PWM_LED_G_WriteCompare(cmp_val);
                PWM_LED_B_WriteCompare(cmp_val);
            break;
                
            case SLAVE_RED_GREEN_BLUE_ON_CTRL_REG1:     //All lights               
                PWM_LED_R_WriteCompare(cmp_val);
                PWM_LED_G_WriteCompare(cmp_val);
                PWM_LED_B_WriteCompare(cmp_val);
            break;
                
            case SLAVE_RED_GREEN_BLUE_OFF_CTRL_REG1:    //No light
                PWM_LED_R_WriteCompare(20);
                PWM_LED_G_WriteCompare(20);
                PWM_LED_B_WriteCompare(20);
            break;    
        }   
    } 
}
    
/* [] END OF FILE */