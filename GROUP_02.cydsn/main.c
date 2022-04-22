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
#define WHO_AM_I_DEFAULT_VALUE      0xBC         // Default value to set inside the Who Am I register (HEX = 0xBC, BIN = 10111100) 
uint8_t slaveBuffer[BUFFER_SIZE]; // Buffer for the slave device

uint8_t number_samples;
uint8_t sensor_status;
uint8_t sum_TMP_value;
int32_t count=0;  // contatore
uint_8 TMP_sample; //campione finale 
int32 TMP_avg_value_digit;
int32 TMP_avg_value_mV;
int32 LDR_avg_value_digit;
int32 LDR_avg_value_mV;

// Define variable to turn on/off the RGB LED
#define HIGH            1                // To set logical value high to 1
#define LOW             0                // To set logical value low to 0
#define RED_ON          HIGH             // To set Red Channel of RGB LED to 1
#define RED_OFF         LOW              // To set Red Channel of RGB LED to 0
#define GREEN_ON        HIGH             // To set Green Channel of RGB LED to 1
#define GREEN_OFF       LOW              // To set Green Channel of RGB LED to 0
#define GREEN_ON        HIGH             // To set Blue Channel of RGB LED to 1
#define GREEN_OFF       LOW              // To set Blue Channel of RGB LED to 0
#define TMP_ON_R        0x26             // To set LED modality on and Red channel on 

int main(void)
{
    CyGlobalIntEnable; /* Enable global interrupts. */

    //Starting functions
    Timer_Start(); 
    ADC_DelSig_Start();
    isr_ADC_StartEx(Custom_isr_ADC); 
    EZI2C_Start();
    AMux_ADC_Start(); 
    
    slaveBuffer[CTRL_REG_1]=EZI2C_STOP_REG_1;
    slaveBuffer[WHO_AM_I]=EZI2C_WHO_AM_I_DEFAULT_VALUE;
    EZI2C_SetBuffer1(BUFFER_SIZE, 0, slaveBuffer);
    // Initialize send flag 
    PacketReadyFlag = 0;
    
      // Start the ADC conversion
    ADC_DelSig_StartConvert(); 

    def
    for(;;)
    {
       number_samples = (slaveBuffer[CTRL_REG_1] & 0x3C) >> 3 ;// read and store the number of selected samples
       sensor_status = (slaveBuffer[CTRL_REG_1] & 0x03)      ; // read and store the status value of TMP
                  
       switch(sensor_status) {
        
        case EZI2C_TMP_REG_1 :
            
          slaveBuffer[CTRL_REG_1] = TMP_ON_R;              // cambio il registro di controllo per accendere il led e il controllo proporzionale 
        
           if (PacketReadyFlag == 1 ){
            
              sum_TMP_value = sum_TMP_value + TMP_avg_value_mV ;
                
              count++;
                
                if (count == number_samples){
                    TMP_sample = sum_TMP_value / number_samples ;           //faccio la media 
                    
                    slaveBuffer[MSB_TMP] = TMP_sample & FFx0;
                    slaveBuffer[LSB_TMP] = TMP_sample & 0xFF;
                    }
                
                PacketReadyFlag = 0;
               
                }
        
                
        


        
/* [] END OF FILE */