/* 
 *
 * Copyright YOUR COMPANY, THE YEAR
 * All Rights Reserved
 * UNPUBLISHED, LICENSED SOFTWARE.
 *
 * CONFIDENTIAL AND PROPRIETARY INFORMATION
 * WHICH IS THE PROPERTY OF your company.
 *
 */
#include "project.h"
#include "UART.h"
#include "InterruptRoutines.h"
#include "I2C_REG.h"
#include "Logging.h"


//DEINITION OF VARIABLES
uint8_t average_sample; // number of samples chosen by the user
uint8_t bit_status; // variable used to choose the sampling configuration (OFF, LDR ON, TMP ON, BOTH ON)
uint8_t LED_modality; //used to choose the LED blinking (depending on TMP sampling or LDR sampling)
uint8_t blue; //pin BLUE    
uint8_t green; // pin GREEN
uint8_t red; // pin RED
int16 count_samples; // number of samples 
uint32 sum_LDR=0; // integration of LDR samples
uint32 sum_TMP=0; // integration of TMP samples
uint32 average_LDR=0; //average of the LDR samples (inizialized to 0)
uint32 average_TMP=0; //average of the TMP samples (inizialized to 0)
uint16_t ldr_tot; //used for the expression of LDR average samples in a 16bit form
uint16_t tmp_tot; //used for the expression of TMP average samples in a 16bit form
uint16_t tmp_V; //used for the conversion from TMP_digit to TMP_mV form
uint16_t tmp_C; //used for the conversion from TMP_mV to TMP_Celsius form
uint16_t ldr_V; //used for the conversion from LDR_Digit to LDR_mV form


uint16 color_int=0; //varaibale declared to set the PWM


char message[50] = {'\0'};

volatile uint8 flagData=0;

//Define slaveBuffer of the EZI2C
uint8 slaveBuffer[SLAVE_BUFFER_SIZE];


int main(void)
{
                            
    CyGlobalIntEnable; /* Enable global interrupts. */

    //Inizialization of our functions
    Timer_Start();
    Logging_Start();
    ADC_DelSig_Start();   
    isr_ADC_StartEx(ISR_ADC);
    AMux_ADC_Start();
    EZI2C_Start();   
    UART_Start();
    PWM_RED_Start();
    PWM_BLUE_Start();
    PWM_GREEN_Start();
    Clock_PWM_Start();
    

    // Start the ADC conversion
    UART_PutString("Inizio Campionamento!\r\n");
    ADC_DelSig_StartConvert();

    slaveBuffer[WHO_AM_I] = I2C_WHO_AM_I_REG_VALUE;         // Set who am i register value
    slaveBuffer[CTRL_REG1] = SLAVE_MODE_OFF_CTRL_REG1;  //set control reg 1 with all bits = 0 

    //5 is the 4th position, the boundary of the r/w cells 
    EZI2C_SetBuffer1(SLAVE_BUFFER_SIZE, SLAVE_BUFFER_SIZE - 5 ,slaveBuffer);
    
    
    for(;;)
    {
        //reading of the registers
        average_sample = (slaveBuffer[CTRL_REG1] & 0x18) >> 3; // I check the number of samples to extract
        average_sample += 1; //We set +1 since the binary values are 0b00=0, 0b01=1, 0b10=2 and 0b11=3. In order to have a range 1-4 we have to increase by one unit
        
        // We write the period raccording to the number of average samples to obtain 50Hz transmission rate
        //We have a timer clock of 5kHz. This means that with 4 average samples, we obtain a period equal to 25, equal to 5ms (25/5000Hz)
        //5ms correspond to 200Hz, but we have 4 samples. Since the freqeuncy required for the data trasmission is 50Hz,
        // we comply with tbis requirement: 200Hz/4=50Hz. The same concept can be translated to the other values of average samples.
        Timer_WritePeriod((0.02/average_sample)*5000);  

        bit_status = slaveBuffer[CTRL_REG1] & 0x03; //I check the bit status configuration

        LED_modality = (slaveBuffer[CTRL_REG1] & 0x04)>>2; //I check the LedMod of the CTRL REG 1

        red = (slaveBuffer[CTRL_REG1] & 0b00100000)>>5; //Checking the red pin
        green = (slaveBuffer[CTRL_REG1] & 0b01000000)>>6; //Checking the green pin
        blue = (slaveBuffer[CTRL_REG1] & 0b10000000)>>7; //Checking the blue pin
        
        switch (bit_status){
            
            case SLAVE_MODE_OFF_CTRL_REG1: //LDR AND TMP ARE BOTH OFF
                //LED is off
                PWM_RED_Sleep();
                PWM_BLUE_Sleep();
                PWM_GREEN_Sleep();

                //We don't save anything in the slave buffer
                slaveBuffer[MSB_LDR]= 0x00;                                
                slaveBuffer[LSB_LDR]= 0x00;                                
                slaveBuffer[MSB_TMP]= 0x00;                                
                slaveBuffer[LSB_TMP]= 0x00; 
                flagData=0;

            break;
                
            case SLAVE_LDR_ON_CTRL_REG1: //LDR IS ON, TMP IS OFF
                UART_PutString("LDR ON!\r\n");
                if(flagData==1) //We want to be sure about the sampling made by the isr (flag=1)
                {                                       
                    sum_LDR+=value_digit_LDR;
                    count_samples++; //We increase the number of samples
                    flagData=0;
                
                    if (count_samples==average_sample) //When i reach the number of samples chosen by the user
                    {
                        average_LDR=sum_LDR/average_sample; //computation of the average
                    
                        slaveBuffer[MSB_LDR]= average_LDR >> 8; //We save in the 3rd register the MSB of average_LDR
                        slaveBuffer[LSB_LDR]= average_LDR & 0xFF; //We save in the 4th register the LSB of average_LDR
                        slaveBuffer[MSB_TMP]= 0x00;
                        slaveBuffer[LSB_TMP]= 0x00;
                        ldr_tot= (slaveBuffer[LSB_LDR] | slaveBuffer[MSB_LDR] << 8 ); //definition of the average_ldr in 16bit

                        ldr_V = ADC_DelSig_CountsTo_mVolts(ldr_tot); //ldr_average in mV form

                        sprintf(message, "LDR Output: %d\r\n", ldr_tot ); //we have decided to print and save ldr in the digit form such that we 
                                                                          //can make a comparison between LDR and TMP, as the scale is the same
                        UART_PutString(message);                                                                                                                                                                                                        
                        
                        sum_LDR=0; //sum goes back to 0
                        average_LDR=0; //same thing for the average

                        //We turn on the 3 PWM                        
                        PWM_RED_Start();
                        PWM_GREEN_Start();
                        PWM_BLUE_Start();
                    }
                }
                break;
 
            //The same thing is repeated for the TMP sensor as before     
            case SLAVE_TMP_ON_CTRL_REG1: //TMP ON, LDR OFF
                UART_PutString("TMP ON!\r\n");
                if(flagData==1)
                {
                    sum_TMP+=value_digit_TMP;
                    count_samples++;
                    flagData=0;
                
                    if (count_samples==average_sample)
                    {
                        average_TMP=sum_TMP/average_sample;
                        average_TMP=(average_TMP-BASE_LINE_TMP)*GAIN_TMP;
                    
                        slaveBuffer[MSB_LDR]=average_LDR & 0x00;
                        slaveBuffer[LSB_LDR]=average_LDR & 0x00;
                        slaveBuffer[MSB_TMP]=average_TMP >>8; //We save in the 5th register the MSB of avrage_TMP
                        slaveBuffer[LSB_TMP]=average_TMP & 0xFF; // WE save in the 6th rgister the LSB of average_TMP
                        tmp_tot= (slaveBuffer[LSB_TMP] | slaveBuffer[MSB_TMP] << 8 );
                        tmp_V = ADC_DelSig_CountsTo_mVolts(tmp_tot); //We convert the digit signal into mV
                        tmp_C = (tmp_V-TMP_OFF)/TMP_M; //Convertion from mV into Celsius 
                        sprintf(message, "Temp Output: %d\r\n", tmp_tot);   // we print and save the digit form for the same reason in the previous case                 
                        UART_PutString(message);
                        
                        sum_TMP=0;
                        average_TMP=0;

                        //We turn on the 3 PWM                     
                        PWM_RED_Start();
                        PWM_GREEN_Start();
                        PWM_BLUE_Start();
                    }
                }
                break;
            
                case SLAVE_BOTH_ON_CTRL_REG1: //BOTH ON
                UART_PutString("LDR and TMP ON!\r\n");
                if (flagData==1)
                {
                    sum_LDR+=value_digit_LDR;
                    sum_TMP+=value_digit_TMP;
                    count_samples++; 
                    flagData=0;
                
                    if (count_samples==average_sample)
                    {
                        average_LDR=sum_LDR/average_sample;
                        average_TMP=sum_TMP/average_sample;
                        average_TMP=(average_TMP-BASE_LINE_TMP)*GAIN_TMP;
                         
                        slaveBuffer[MSB_LDR]=average_LDR >> 8;
                        slaveBuffer[LSB_LDR]=average_LDR & 0xFF;
                        slaveBuffer[MSB_TMP]=average_TMP >>8;
                        slaveBuffer[LSB_TMP]=average_TMP & 0xFF;                         
                        
                        ldr_tot= (slaveBuffer[LSB_LDR] | slaveBuffer[MSB_LDR] << 8 );
                        ldr_V = ADC_DelSig_CountsTo_mVolts(ldr_tot);
                        sprintf(message, "LDR Output: %d\r\n", ldr_tot );
                        UART_PutString(message);
                    
                        tmp_tot= (slaveBuffer[LSB_TMP] | slaveBuffer[MSB_TMP] << 8 );
                        tmp_V = ADC_DelSig_CountsTo_mVolts(tmp_tot);
                        tmp_C = (tmp_V-TMP_OFF)/TMP_M;
                        sprintf(message, "Temp Output: %dC\r\n",tmp_tot); //We use int16 since the sensor accuracy is +/-1C                        UART_PutString(message);    
                    
                        average_LDR=0;
                        average_TMP=0;
                        sum_LDR=0;
                        sum_TMP=0;    
                        //count_samples = 0;
                        PWM_RED_Start();
                        PWM_GREEN_Start();
                        PWM_BLUE_Start();
                    }
                }
                break;
                
            }
        
            if (count_samples == average_sample) { //This is the condition to TURN ON the LED
                if (LED_modality==LED_MOD_LDR) 
                {
                    if (bit_status == SLAVE_TMP_ON_CTRL_REG1) { //If we are sampling with TMP but the LED_MOD is on LDR we TURN OFF the 3 PWM
                        PWM_RED_Sleep();
                        PWM_GREEN_Sleep();
                        PWM_BLUE_Sleep();
                    }
                    if(ldr_tot > MID_RANGE_LDR) color_int=0; //Since the curve of the LDR is very steep, we have decided to define a mid_range
                                                            //equal to 25000 (within a 0-65535 range): if we give an higher light intensity, the LED is turned OFF
                    else color_int = 65535;                 //otherwise is turned ON with its maximum intensity
                }
            
                else if (LED_modality==LED_MOD_TMP)
                {
                    if (bit_status == SLAVE_LDR_ON_CTRL_REG1) { //If we are sampling with LDR but the LED_MOD is on TMP we TURN OFF the 3 PWM
                        PWM_RED_Sleep();
                        PWM_GREEN_Sleep();
                        PWM_BLUE_Sleep();
                    }
                    color_int = (tmp_tot - BASE_LINE_TMP)*GAIN_TMP; //color intensity is computed based on the range around 24C (10300 digit)
                    if (color_int > 65535) color_int = 65535;
                    else if (color_int < 0) color_int = 0;
                }
                
                //If the color pins are ON i can set the PWM according to the color intesity set before
                if(red) {
                    PWM_RED_WriteCompare(color_int);
                    color_int = 0;
                }
                else PWM_RED_WriteCompare(0);
                
                if (green) {
                    PWM_GREEN_WriteCompare(color_int);
                    color_int = 0;
                }
                else PWM_GREEN_WriteCompare(0);
                
                if(blue) { 
                    PWM_BLUE_WriteCompare(color_int);
                    color_int = 0;
                }
                else PWM_BLUE_WriteCompare(0);
                
                count_samples = 0;
            }       
    }
}
                
/* [] END OF FILE */
