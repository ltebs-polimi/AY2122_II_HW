/* ========================================
    AURORA PIERANTOZZI
    LETIZIA MOZZORECCHIA
*/

#include "InterruptRoutines.h"
#include "project.h"

/*DECLARATIONS*/

//definition MUX channels 
#define LDR 0
#define TEMP 1 

//definition I2C Slave Buffer registers' addresses
#define CR1 0
#define WhoAmI 1
#define MSB_LDR 2
#define LSB_LDR 3
#define MSB_TEMP 4
#define LSB_TEMP 5

//definition of device's states (bit 0 and 1 of CR1 with address 0x00), read with status_mask
#define OFF 0       //value 00 to stop 
#define LDR_CH0 1   //value 01 to sample LDR
#define TEMP_CH1 2  //value 10 to sample TEMP
#define BOTH 3      //value 11 to sample both 

//definition of modality state (bit 2 of CR1 with address 0x00), read with readout
#define TEMP_READOUT 1
#define LDR_READOUT 0

//declaration of global variables 
extern uint8_t average_samples; //number of samples to be averaged
extern uint8_t buffer[];        //slave buffer
uint8_t status = LDR_CH0;       //which MUX channel activate during sampling 
uint8_t readout = TEMP_READOUT; //reading modality
uint8_t RGB_channel;            //RGB colors to switch on

//declaration of masks to control the modifications of registers 
uint8_t status_mask = 3;     //00000011 to read bit 0 and 1 of CR1 (00)
uint8_t average_mask = 0x18; //00011000 to read bit 3 and 4 of CR1 (00)
uint8_t color_mask = 0xE0;   //11100000 to read bit 5,6,7 of CR1 (00)
uint8_t check_status;
uint8_t check_average;
uint8_t check_color;

//declaration of variables useful for sampling and for calculating the mean
int32_t value_digit_LDR=0;
int32_t value_digit_temp=0;
int32_t value_digit;
uint16_t temp_mean;
uint16_t ldr_mean;
int i=0;

//declaration of variables and functions to activate the RGB led
uint16_t comp_value; 
void setPWM_RGB(uint8);
void activate_colors(uint8_t, uint16_t); 

//defines to set sensors' thresholds
#define TEMP_AMB 15000 
/* TEMP_AMB set by oursevles after calibration (retrieved from BCP) 
   * range for setting ambient temperature: 10000 - 17000 -> 17°C-18°C
   * maximum temperature (for setting maximum intensity on the RGB led): 40000 -> 40°C-45°C  
*/

#define REF_LIGHT_MAX 13351  
/* REF_LIGHT_MAX set by oursevles after calibration (retrieved from BCP) 
*/


/***************************************************************************************************************
ISR FOR THE MANAGEMENT OF SAMPLING, AVERAGING AND WRITING OF THE 16 DATA BITS IN THE RESPECTED BUFFER REGISTERS
****************************************************************************************************************/
/*  The MUX channel to activate during sampling and the number of samples to average can be modified by writing on 
    the respective registers from Bridge Control Panel.
*/


CY_ISR(My_ISR) {
 
    Timer_ReadStatusRegister();
    
    /*SAMPLING CHANNEL CH0 (LDR)*/
    
    if(status == LDR_CH0 || status == BOTH) {//it occurs only when status is 01 or 11
        
        AMUX_Select(LDR); //connect the MUX to the right channel 
        ADC_DelSig_StartConvert(); //switch on the ADC
                                           
        value_digit = ADC_DelSig_Read32(); //sampling the signal
    
        if (value_digit<0) value_digit=0;
        if (value_digit>65535) value_digit=65535;
    
        value_digit_LDR += value_digit; //monitor the summation of samples to be averaged
        ADC_DelSig_StopConvert(); //switch off the ADC
        //for reasons of conversion stability, from datasheet, it is suggested to keep it off during channel changing
    }
    
    /*SAMPLING CHANNEL CH1 (TEMPERATURE)*/
    
    if(status == TEMP_CH1 || status == BOTH) { //it occurs only when status is 10 or 11
    
        AMUX_Select(TEMP);
        ADC_DelSig_StartConvert();
        value_digit = ADC_DelSig_Read32();
    
        if (value_digit<0) value_digit=0;
        if (value_digit>65535) value_digit=65535;
    
        value_digit_temp += value_digit;
        ADC_DelSig_StopConvert();
    }
    
    if (status != OFF) i++; //if the sampling is on, update the count of number of samples in the summation
 
    
    /*CALCULATING THE MEAN AND WRITING OF THE 16 DATA BITS IN THE RESPECTED BUFFER REGISTERS*/
    
    if (i>=average_samples) { //it occurs if the number of samples to be averaged has been reached
    
        if(status == LDR_CH0 || status == BOTH){
            ldr_mean = value_digit_LDR/average_samples; //reading digit (16 bit)
            buffer[MSB_LDR] = ldr_mean >>8;       
            buffer[LSB_LDR] = ldr_mean & 0xFF;  
        }
        if(status == TEMP_CH1 || status == BOTH) {
            temp_mean = value_digit_temp/average_samples; //reading digit (16 bit)
            buffer[MSB_TEMP] = temp_mean >> 8;
            buffer[LSB_TEMP] = temp_mean & 0xFF;
        }    

        //calling the function that checks bit 2 of CR1 and consequently activates RGB led
        setPWM_RGB(readout);            
    
        value_digit_LDR=0;  //reset the summation in order to prepare for the next acquisition 
        value_digit_temp=0;
        i=0;                //reset the counts of samples to be averaged
    }
}


/*************************************************************
FUNCTION EXECUTED AT THE OUTPUT OF EACH CALLBACK OF THE MASTER
*************************************************************/
/*
    Parameters of the code (status, samples to be averaged and RGB led channels to be activated) are modified if 
    modified by the user in the registers (by writing on the BCP).
*/

void EZI2C_ISR_ExitCallback(void) {

    //variables to check for any changes in the registers
    check_status = (buffer[CR1] & status_mask);   //reading from CR1 the device' status  
    check_average = (buffer[CR1] & average_mask); //reading from CR1 the number of samples to be averaged  
    check_color = (buffer[CR1] & color_mask);     //reading from CR1 which colors of the RGB led have to be activated  
    
    if (status != check_status) { //if the users has changed the device' status (bit 1-0 of CR1)
        status = check_status; //update the status variable 
        //reset the count of the samples to be averaged
        i=0;                 
        //reset the summation in order to prepare for the next acquisition
        value_digit_LDR=0;   
        value_digit_temp=0;
        //at each status change the data registers are reset to zero
        buffer[MSB_LDR]=0;   
        buffer[LSB_LDR]=0;
        buffer[MSB_TEMP]=0;
        buffer[LSB_TEMP]=0;
    }
    
    if (average_samples != check_average) { //if the user has changed the number of samples to be averaged (bit 3-4 of CR1)                                   
        average_samples = check_average;   //update the average_samples variable
    }
    
    if (RGB_channel != check_color) { //if the user has changed the colors of the RGB led to activate (bit 5-6-7 of CR1)                                    
        RGB_channel = check_color;   //update the RGB_channel variable
    }
    
}

/*************************************************************************************************
FUNCTION THAT LOOKS FOR SENSORS' THRESHOLDS AND ACCORDINGLY SET THE DC OF THE PWM DRIVING RGB LED
**************************************************************************************************/
/*  Input Parameter: 
    * readout: 
      modality used to modulate RGB led intensity.
*/

void setPWM_RGB (uint8 readout) {

    if (readout == LDR_READOUT) { 
        if (ldr_mean < REF_LIGHT_MAX) { //when in full light, the RGB is switched OFF
            PWM_RG_WriteCompare1(0);
            PWM_RG_WriteCompare2(0);
            PWM_B_WriteCompare(0); 
        } 
        else { //when in darkness, the RGB is switched ON and its intensity is modulated according to the light
            activate_colors(RGB_channel, (65535-ldr_mean));
        }  
    }
    
    if (readout == TEMP_READOUT) {
            if (temp_mean < TEMP_AMB) { //when below ambient temperature, the RGB is switched OFF
                PWM_RG_WriteCompare1(0);
                PWM_RG_WriteCompare2(0);
                PWM_B_WriteCompare(0);             
            }
            else if (temp_mean > TEMP_AMB) { //when over the ambient temperature, the RGB is switched ON and its intensity is modulated according to the temperature 
                comp_value = (temp_mean * 2.521) - 35288.08; //to scale the operating digit temperature range (15000-40000) into 0-65535
                if (comp_value > 65535) comp_value = 65535;
                activate_colors(RGB_channel, comp_value);
            }
    }
}

/***********************************************************************************
FUNCTION THAT ACTIVATES THE COLORS OF THE RGB LED ACCORDING TO THE CHANNELS SELECTED
************************************************************************************/

/*  Input Parameters: 
    * RGB_channel: 
      equal to color_check obtained by masking the buffer reading with color_mask (11100000):
      if the bit 5 is on -> Red PWM is activated
      if the bit 6 is on -> Green PWM is activated
      if the bit 7 is on -> Blue PWM is activated
    * value:
      compare value used to modulates the PWM driving RGB channels.
*/

void activate_colors(uint8 RGB_channel,uint16 value){
    if (((RGB_channel) & (00100000)) == 20) PWM_RG_WriteCompare1(value);
    else PWM_RG_WriteCompare1(65535);
    
    if (((RGB_channel) & (01000000)) == 40) PWM_RG_WriteCompare2(value);
    else PWM_RG_WriteCompare2(65535);
    
    if (((RGB_channel) & (10000000)) == 80) PWM_B_WriteCompare(value);
    else PWM_B_WriteCompare(65535);
} 


/* [] END OF FILE */
