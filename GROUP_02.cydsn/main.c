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
uint8_t slaveBuffer[BUFFER_SIZE];                // Buffer for the slave device

// Define variable to turn on/off the RGB LED
#define HIGH            1                // To set logical value high to 1
#define LOW             0                // To set logical value low to 0
#define RED_ON          HIGH             // To set Red Channel of RGB LED to 1
#define RED_OFF         LOW              // To set Red Channel of RGB LED to 0
#define GREEN_ON        HIGH             // To set Green Channel of RGB LED to 1
#define GREEN_OFF       LOW              // To set Green Channel of RGB LED to 0
#define GREEN_ON        HIGH             // To set Blue Channel of RGB LED to 1
#define GREEN_OFF       LOW              // To set Blue Channel of RGB LED to 0


int main(void)
{
    CyGlobalIntEnable; /* Enable global interrupts. */

    //Starting functions
    Timer_Start(); 
    ADC_DelSig_Start();
    isr_ADC_StartEx(Custom_isr_ADC); 
    EZI2C_Start();
    AMux_ADC_Start(); 
    
    // Initialize send flag 
    PacketReadyFlag = 0;
    
      // Start the ADC conversion
    ADC_DelSig_StartConvert(); 

    for(;;)
    {
    
        
        
/* [] END OF FILE */