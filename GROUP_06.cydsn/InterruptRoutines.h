/* ========================================
 *
 * Copyright YOUR COMPANY, THE YEAR
 * All Rights Reserved
 * UNPUBLISHED, LICENSED SOFTWARE.
 *
 * CONFIDENTIAL AND PROPRIETARY INFORMATION
 * WHICH IS THE PROPERTY OF your company.
 *
 * ========================================
*/

/* ========================================

*/
#include "InterruptRoutines.h"
#include "project.h"

/*DECLARATIONS*/

//definition MUX channels (CH0 --> LDR, CH1 --> TEMP)
#define LDR 0
#define TEMP 1 

//definition I2C Slave Buffer registers' addresses
#define CR1 0
#define WhoAmI 1
#define MSB_LDR 2
#define LSB_LDR 3
#define MSB_TEMP 4
#define LSB_TEMP 5

//definition of device's states (bit 0 and 1 of CR1 with address 0x00)
//read with status_mask
#define OFF 0   //value 00 to stop 
#define LDR_CH0 1   //value 01 to sample LDR
#define TEMP_CH1 2   //value 10 to sample TEMP
#define BOTH 3  //value 11 to sample both 

//definition of modality state (bit 2 of CR1 with address 0x00)
//read with readout
#define TEMP_READOUT 1
#define LDR_READOUT 0

//definitino of number of samples to be averaged (bit 3 and 4 of CR1 with address 0x00)
//read with average_mask
#define ONE_SAMP 0 //value 00 to sample 1 item 
#define TWO_SAMP 1 //valore 01 to sample 2 items
#define THREE_SAMP 2 //valore 10 to sample 3 items
#define FOUR_SAMP 3 //valore 11 to sample 4 items


//declaration of global variables 
extern uint8_t average_samples; //number of samples to be averaged
extern uint8_t status;  //what to sample 
extern uint8_t readout; //reading modality
extern uint8_t RGB_channel; //RGB color to switch on
extern uint8_t  buffer[]; //slave buffer

//declaration of masks to control the modifications of registers 
uint8_t status_mask = 3; //00000011 to read bit 0 and 1 of CR1 (00)
uint8_t average_mask = 24; //00011000 to read bit 3 and 4 of CR1 (00)
uint8_t color_mask = 7; //00000111 to read bit 5,6,7 of CR1 (00)
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

//declaration of variables to read temperature sensor
uint16_t temp_mv; 
#define TEMP_AMB 100 //in mV (25°C -> 750mV = 0.75V)
#define TEMP_MAX 5500 //in mV (125°C -> 5500mV = 5.5V)

//declaration of variables to read LDR sensor
uint16_t ldr_mv;
uint16_t delta_perc;
#define TH_PERC //to be defined
#define REF //set by ourselves after calibration (in mV)

//declaration of variables and functions to activate the RGB led
uint16_t comp_value; 
void setPWM_RGB(uint8);


/*ISR FOR THE MANAGEMENT OF SAMPLING, AVERAGING AND WRITING OF THE 16 DATA BITS OF THE TWO CHANNELS 
IN THE RESPECTED BUFFER REGISTERS (number of samples to average can be modified by writing on the 
respective registers from Bridge Control Panel; 
in the code the values are initialized according to the project specifications*/

CY_ISR(My_ISR) {
 
    Timer_ReadStatusRegister();
    
    /*SAMPLING CHANNEL CH0 (LDR)*/
    
    if(status == LDR_CH0 || status == BOTH) {   //it occurs only when status is 01 or 11
    
        AMUX_Select(LDR);                   //connect the MUX to the right channel 
        ADC_DelSig_StartConvert();         //switch on the ADC
                                           
        value_digit = ADC_DelSig_Read32(); //sampling the signal
    
        if (value_digit<0) value_digit=0;
        if (value_digit>65535) value_digit=65535;
    
        value_digit_LDR += value_digit;   //monitor the summation of samples to be averaged
        ADC_DelSig_StopConvert();         //switch off the ADC
        //for reasons of conversion stability, from datasheet, it is suggested to keep it off during channel changing
    }
    
    /*SAMPLING CHANNEL CH1 (TEMPERATURE)*/
    
    if(status == TEMP_CH1 || status == BOTH) {    //it occurs only when status is 10 or 11
    
        AMUX_Select(TEMP);
        ADC_DelSig_StartConvert();
        value_digit = ADC_DelSig_Read32();
    
        if (value_digit<0) value_digit=0;
        if (value_digit>65535) value_digit=65535;
    
        value_digit_temp += value_digit;
        ADC_DelSig_StopConvert();
    }
    
    if (status != OFF) i++; 
    //if the sampling is on, update the count of number of samples in the summatino 
    
    /*CALCULATING THE MENA AND WRITING OF THE 16 DATA BITS IN THE RESPECTED BUFFER REGISTERS*/
    
    if (i>=average_samples) { //it occurs if the number of samples to be averaged has been reached
    
        if(status == LDR_CH0 || status == BOTH)
        {
            ldr_mean = value_digit_LDR/average_samples; //reading digit
            ldr_mv = ADC_DelSig_CountsTo_mVolts(ldr_mean); //reading mV
            
            buffer[MSB_LDR] = ldr_mean >>8;       
            buffer[LSB_LDR] = ldr_mean & 0xFF;   
            
        }
        if(status == TEMP_CH1 || status == BOTH){
            temp_mean = value_digit_temp/average_samples; //reading °C
            buffer[MSB_TEMP] = temp_mean >> 8;
            buffer[LSB_TEMP] = temp_mean & 0xFF;

        }    

        //call the function that checks the 2° bit of CR1 and consequently activates the RGB led
        setPWM_RGB(readout);            
    }
        
    value_digit_LDR=0;  //reset the summation in order to prepare for the next acquisition
    value_digit_temp=0;
    i=0;                //reset the counts of samples to be averaged

}


/*UNCTION EXECUTED AT THE OUTPUT OF EACH CALLBACK OF THE MASTER TO MODIFY THE PARAMETERS OF THE CODE 
(STATUS, SAMPLES TO BE AVERAGED AND RGB LED CHANNELS TO BE ACTIVATED) IF MODIFIED BY THE USER AND MANAGE THE LED) */

void EZI2C_ISR_ExitCallback(void)

{   //variables to check for any changes in the registers
    check_status = (buffer[CR1] & status_mask);   //reading from CR1 the device' status  
    check_average = (buffer[CR1] & average_mask); //reading from CR1 the number of samples to be averaged  
    check_color = (buffer[CR1] & color_mask);     //reading from CR1 which colors of the RGB led activate  
    
    if (status != check_status) //if the users has changed the device' status (bit 1-0 of CR1)
    {
        status = check_status; //update the status variable 
        
        if (status == BOTH) 
        
        //reset the count of the samples to be averaged
        i=0;                 
        //reset the summatinos in order to prepare for the next acquisitino 
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

/* [] END OF FILE */


/*FUNCTION THAT MODULATES WITH PWM THE INTENSITY 'OF THE LED RGB BASED ON THE READINGS OF THE SENSORS
....................add description of the different working principles ...........................*/

void setPWM_RGB (uint8 readout) {
    PWM_RG_Enable();
    PWM_RG_Stop();
    PWM_B_Enable();
    PWM_B_Stop();
    
    if (readout == LDR_READOUT) {
            /* When the LED is modulated by the LDR and the environment is dark, 
            the LED must be switched on with maximum intensity, 
            while when the environment is bright the LED must be switched off. 
            Note: the LDR characteristic curve is very steep (almost ON/OFF), 
            please consider this fact in order to better exploit the sensor dynamic, 
            either hardware or firmware solution. 
            */
        
        
            /*
            //controllo soglia LDR (luce/buio) con variazione percentuale del valore
            //non ricaviamo la resistenza LDR con cui poi ottenere retta di calibrazione lineare (in loglog) con i lux
            //stabiliamo soglia ON/OFF con i mV
            
            delta_perc = (ref - ldr_mv)*100; //ref stabiliamo noi quanto è leggendo cosa ci viene fuori quando settiamo luce/buio
            
            //TH_PERC definiamo noi quanto lo vogliamo (tipo 5%?))
            if (delta_perc < TH_PERC) { 
                flag_luce = 1;
            }
            else (delta_perc > TH_PERC) {
                flag_luce = 0;
            }

            //se luce
            if (flag_luce) {
            //spengo il LED RGB
            PWM_RG_Stop();
            PWM_B_Stop();
            }
        
            //se buio 
            else {
            //massima luminosità LED RGB
            PWM_RG_Start();
            PWM_B_Start();
            PWM_RG_WriteCompare1(255);
            PWM_RG_WriteCompare2(255);
            PWM_B_WriteCompare(255);
            }
            
            */
            
        }
    
    if (readout == TEMP_READOUT) {
            /*When the LED is modulated by the TMP, 
            the LED must be switched off at ambient temperature 
            and increase intensity with rising temperature 
            */

            //leggere output TEMP (media dei campioni -> temp_mean)
            temp_mv = 500 + 10*((float)temp_mean/65535); //scale factor = 10mV/°C 
            
            //controllo soglia temperatura ambiente TEMP_AMB
            
            if ((temp_mv <= TEMP_AMB) || (temp_mv > TEMP_MAX)) { //se sto sotto la temperatura ambiente o se sto superando quella massima
                //spengo il LED RGB
                
                
                PWM_RG_Stop();
                PWM_RG_WriteCompare1(0);
                PWM_RG_WriteCompare2(0);
                PWM_B_WriteCompare(0);
                PWM_B_Stop();
            }
            
            if ((temp_mv > TEMP_AMB) && (temp_mv < TEMP_MAX)) { //se sto sopra la temperatura ambiente
                //aumento intensità del LED RGB fino alla temperatura massima
                comp_value = 2.55*(temp_mv) - 63.75; 
                PWM_RG_Enable();
                PWM_RG_Start();
                PWM_B_Enable();
                PWM_B_Start();
                PWM_RG_WriteCompare1(comp_value);
                PWM_RG_WriteCompare2(comp_value);
                PWM_B_WriteCompare(comp_value);
            }
    }
}



/* [] END OF FILE */



/* [] END OF FILE */
