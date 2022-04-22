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

#define BUFFER_SIZE                 6            // Number of addressed registers inside the Slave Buffer
#define RW_SIZE                     2            // Number of data bytes starting from the beginning of the buffer with read and write access 
                                                    // (data bytes located at offset rwBoundary or greater are read only)
#define WHO_AM_I_DEFAULT_VALUE      0xBC         // Default value to set inside the Who Am I register (HEX = 0xBC, BIN = 10111100) 

// Pointer to the data buffer for I2C communication + Initialization of cells where the measurements will be inserted
uint8_t slave_pointer_Buffer[BUFFER_SIZE] = {0,EZI2C_WHO_AM_I_DEFAULT_VALUE,0,0,0,0};   

// Define variable to turn on/off the RGB LED
#define HIGH              1                // To set logical value high to 1
#define LOW               0                // To set logical value low to 0
#define RED_ON            HIGH             // To set Red Channel of RGB LED to 1
#define RED_OFF           LOW              // To set Red Channel of RGB LED to 0
#define GREEN_ON          HIGH             // To set Green Channel of RGB LED to 1
#define GREEN_OFF         LOW              // To set Green Channel of RGB LED to 0
#define GREEN_ON          HIGH             // To set Blue Channel of RGB LED to 1
#define GREEN_OFF         LOW              // To set Blue Channel of RGB LED to 0
#define TMP_ON_R          0xE4

// Variables declaration
volatile uint8 PacketReadyFlag = 0;      // Initialize flag to indicate if bytes are to send or not 
int32 sum_TMP;                           // Summation of binary values sampled from TMP
int32 sum_LDR;                           // Summation of binary values sampled from LDR
int32 TMP_value;                         // Averaged values of the X samples considered from TMP (binary)
int32 LDR_value;                         // Averaged values of the X samples considered from LDR (binary)
int32 average_mV_TMP;                    // Averaged values of the X samples considered from TMP (mV)
int32 average_mV_LDR;                    // Averaged values of the X samples considered from LDR (mV)
uint8_t counter = 0;                     // Counter to be incremented in order to keep track of number of samples
uint8_t LED_modality;                    // LED modality that indicates the sensor that modulates RGB LED intensity
                                            // 0 = LDR readout
                                            // 1 = TMP readout
uint8_t samples;                         // Number of samples to be used to compute the average
uint8_t status;                          // bits 0 and 1; status can be set to:
                                            // a. 00 (device stopped)
                                            // b. 01 (to sample LDR channel)
                                            // c. 10 (to sample TMP channel) 
                                            // d. 11 (to sample both channel)
uint8 timer_period; /////

int main(void)
{
    CyGlobalIntEnable; /* Enable global interrupts. */

    //Starting functions
    Timer_Start(); 
    ADC_DelSig_Start();
    isr_ADC_StartEx(Custom_isr_ADC); 
    EZI2C_Start();
    AMux_ADC_Start(); 
       
    // Start the ADC conversion
    ADC_DelSig_StartConvert(); 
    
    // Timer period
    timer_period = 1000/50*samples; /// definire valori!!! 
    Timer_WritePeriod(timer_period);
    
    // Initialize of EZI2C slave
    // This allows to claim which are the cells that can be used for read and write (R/W) for I2C protocol 
    EZI2C_SetBuffer1(BUFFER_SIZE, RW_SIZE, slave_pointer_Buffer);
        
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
           
     
    if (status == EZI2C_STOP_REG_1) {
       
        slave_pointer_Buffer[MSB_TMP]  = 0x00;
        slave_pointer_Buffer[MSB_LDR]  = 0x00;
        slave_pointer_Buffer[LSB_TMP]  = 0x00;
        slave_pointer_Buffer[LSB_LDR]  = 0x00;
    
    }    

    if (status == EZI2C_LDR_REG_1) {
        
    
    }    
        
    if (status == EZI2C_TMP_REG_1) {       
   
        slave_pointer_Buffer[CTRL_REG_1] = TMP_ON_R;              // cambio il registro di controllo per accendere il led e il controllo proporzionale 
        // mettere a uno la modalità del led e mettere a 1 tutti i canali del led
        //E4
           if (PacketReadyFlag == 1 ){
            
              sum_TMP = sum_TMP + average_mV_TMP;
                
              counter++;
                
                if (counter == samples){
                    TMP_value = sum_TMP / samples ;           //faccio la media 
                    
                    slave_pointer_Buffer[MSB_TMP] = TMP_value >> 8; //1111111100000000 
                    slave_pointer_Buffer[LSB_TMP] = TMP_value & 0xFF; // in binario  11111111 
                    
                    sum_TMP = 0;
                    counter = 0;
                    TMP_value=0;
                    }
                
                PacketReadyFlag = 0;
                
                }    
    }
    
    if (status == EZI2C_LDR_TMP_REG_1) {
        
    
    }     
                
/* [] END OF FILE */