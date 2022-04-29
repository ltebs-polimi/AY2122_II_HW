/*
GROUP_05
ADELAIDE STUCCHI 
NATALIA GINGHINI
*/
#include "InterruptRoutines.h"
#include "project.h"

//DECLARATIONS

//MUX channels definition: CH0->LDR, CH1->temperature
#define TEMP 1
#define LDR 0

//definition of buffer register indices
#define CR1 0
#define WhoAmI 1
#define MSB_LDR 2
#define LSB_LDR 3
#define MSB_TEMP 4
#define LSB_TEMP 5

//definition of device states(bit 1-0 of the control register 0x00)
#define OFF 0   //00
#define CH_LDR 1   //01 -> sampling LDR sensor
#define CH_TEMP 2   //10 -> sampling TEMPERATURE sensor
#define BOTH 3  //11 -> sampling both sensors

//definition of led modality
#define TEMP_READ_OUT 1
#define LDR_READ_OUT 0

//threshold values
#define TEMP_AMB 11000 //retrived by experiments with BCP, enviromental temperature 
#define LIGHT_MAX 1335 //retrived by experiments with BCP, when LDR exposed to flashlight

//global variables declaration 
extern uint8_t average_samples; //number of samples for the average
extern uint8_t  buffer[]; //slave buffer

//variable used to set the color of the RGB
uint8_t RGB_channel;

//variable declaration for sampling and for the calculation of the average
int32_t value_digit_LDR=0;
int32_t value_digit_temp=0;
int32_t value_digit;
int i=0; //counter of samples
uint16_t temp_mean; 
uint16_t ldr_mean;
uint8_t read_out=LDR_READ_OUT; //initialization
uint16_t LDR_digit;
uint16_t compared_value; //to control the PWMs

//masks and variables to control the modification of registers from the Bridge Control Panel
uint8_t status_mask = 3; //0000 0011
uint8_t average_mask = 0b00011000; 
uint8_t color_mask = 0b11100000; 
uint8_t check_status;
uint8_t check_average;
uint8_t check_color;

//state variable declaration: current status of the device 
uint8_t status = CH_TEMP; //initialization 

//functions declaration
void set_PWM_RGB(uint8_t);
void activate_colors(uint8_t, uint16_t);
void set_PWM_RGB (uint8_t);

/*ISR for the management of the sampling, the calculation of the average 
and the writing of the (16) data bit of the two channels in the registers of the buffer */

CY_ISR(My_ISR)
{   
    Timer_ReadStatusRegister();
    
    /*Sampling channel CH0 (LDR)*/
    
    if(status == CH_LDR || status == BOTH)    //if the state is 01 or 11
    {
        AMUX_Select(LDR);                  //connect the multiplexer to the LDR channel
        ADC_DelSig_StartConvert();         //switch on the ADC 
        value_digit = ADC_DelSig_Read32();  //sampling of the signal
    
        if (value_digit<0) value_digit=0;
        if (value_digit>65535) value_digit=65535;
    
        value_digit_LDR += value_digit;   //summation of the samples
        ADC_DelSig_StopConvert();          //switch of the ADC
    }
    
    /*Sampling channel CH1 (TEMP)*/
    
    if(status == CH_TEMP || status == BOTH)    //if the state is 10 or 11
    {
        AMUX_Select(TEMP);
        ADC_DelSig_StartConvert();
        value_digit = ADC_DelSig_Read32();
    
        if (value_digit<0) value_digit=0;
        if (value_digit>65535) value_digit=65535;
    
        value_digit_temp += value_digit;
        ADC_DelSig_StopConvert();
    }
    
    if (status != OFF) i++; // if sampling is on, update the count of the number of samples in the summation
    
    /* Average calculation and writing of the data bits on the appropriate registers*/
    
    if (i>=average_samples) //if the number of samples to be averaged has been reached
    {
        if(status == CH_TEMP || status == BOTH)
        {
            temp_mean=value_digit_temp/average_samples;
            buffer[MSB_TEMP] = temp_mean >>8; //to separate the most and least significant byte
            buffer[LSB_TEMP] = temp_mean & 0xFF;
        }
        if(status == CH_LDR || status == BOTH)
        {
            ldr_mean=value_digit_LDR/average_samples;
            buffer[MSB_LDR] = ldr_mean >>8;        //to separate the most and least significant byte
            buffer[LSB_LDR] = ldr_mean & 0xFF;
        }    
        
        set_PWM_RGB(read_out);
        
        value_digit_LDR=0;  //reset summation, preparation for the next acquisition
        value_digit_temp=0;
        i=0;                //reset the count of samples to be averaged
    }
    
}

//Function for the RGB management 

void set_PWM_RGB (uint8_t read_out) 
{
        if (read_out == LDR_READ_OUT)
        {
            if (ldr_mean < LIGHT_MAX) //full light -> RGB OFF 
            {
                PWM_RG_WriteCompare1(0);
                PWM_RG_WriteCompare2(0);
                PWM_B_WriteCompare(0); 
            }
            else
            {
                activate_colors(RGB_channel, (65535-ldr_mean));
            }
            LDR_digit = ADC_DelSig_CountsTo_mVolts(ldr_mean);            
        }
        
        if (read_out == TEMP_READ_OUT)
        {
            if (temp_mean < TEMP_AMB)
            {
                PWM_B_WriteCompare(0);
                PWM_RG_WriteCompare1(0);
                PWM_RG_WriteCompare2(0);
            }
            else if (temp_mean >= TEMP_AMB)
            {
                compared_value = (temp_mean * 2.521) - 35288.08;
                if (compared_value > 65535) compared_value=65535;
                activate_colors(RGB_channel, compared_value);
            }             
        }
}

//Function to set the RGB colors 

void activate_colors(uint8 RGB_channel,uint16 compared_value)
{
    if ((RGB_channel) & (00100000))
    {
        PWM_RG_WriteCompare1(compared_value);
    }
    else 
    {
        PWM_RG_WriteCompare1(65535);
    }
    
    if ((RGB_channel) & (01000000)) 
    {
        PWM_RG_WriteCompare2(compared_value);
    }
    else 
    {
        PWM_RG_WriteCompare2(65535);
    }
    
    if ((RGB_channel) & (10000000))
    {
        PWM_B_WriteCompare(compared_value);
    }
    else 
    {
        PWM_B_WriteCompare(65535);
    }
}
    
/*Function called at the exit of each callback of the master, 
to modify the parameters (status and sample to be averaged) if modified by the user*/

void EZI2C_ISR_ExitCallback(void)
{
    check_status = (buffer[CR1] & status_mask); //variable to control the modification of the register CR1
    check_average = (buffer[CR1]& average_mask); //variable to control the modification of the register CR1
    check_color = (buffer[CR1] & color_mask);
    
    if (status != check_status) //if the user has changed the status of the device (bit 1-0 of control register 1)
    {
        status = check_status; //update the status variable according to the change 
        
        i=0;                 //reset the count of samples to be averaged
        value_digit_LDR=0;   //reset summation, preparation for the next acquisition
        value_digit_temp=0;  //reset summation, preparation for the next acquisition
        buffer[MSB_LDR]=0;   //at each change of state the data registers are reset
        buffer[LSB_LDR]=0;
        buffer[MSB_TEMP]=0;
        buffer[LSB_TEMP]=0;
    }
    
    if (average_samples != check_average) //if the user has changed the number of samples to average (bits 3-4 of CR1)
    { 
        average_samples = check_average;  //update the variable according to the change 
    }
    
    if (RGB_channel != check_color) //if the user has changed the RGB colors 
    {                                 
        RGB_channel = check_color; //update the status variable according to the change 
    }
    
}

/* [] END OF FILE */
