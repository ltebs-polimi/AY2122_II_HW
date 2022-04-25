/* ========================================
 *
 * \ authors : Giorgio Allegri - Luca Pavirani
 * 
 * \ Header file for project defines
 *
 *
 * ========================================
*/

#ifndef __PROJDEF_H
    #define __PROJDEF_H
    
    // Define Buffer Register
    #define I2C_SLAVE_ADDRESS       0x08      //I2C slave address
    
    #define I2C_CTRL_REG1_ADDRESS   0x00  //I2C control register 1 address
    
    #define I2C_WHOAMI_ADDRESS      0x01     //I2C who am I address
    #define I2C_WHOAMI_VALUE        0xBC       //I2C who am I value
    
    #define SENSORS_OFF             0x00            //device stopped
    #define LDR_STATUS_READ         0x01        //sample LDR channel
    #define TMP_STATUS_READ         0x02        //sample TMP channel
    #define BOTH_CHANNELS_READ      0x03     //sample both channels
    
    #define LED_MOD_LDR 0x00    //LDR modulates RGB LED intensity
    #define LED_MOD_TMP 0x01    //TMP modulates RGB LED intensity
    
    // Define Slave Buffer
    #define SLAVE_BUFFER_SIZE 6 // number of registers
    #define CTRL_REG1         0 // position of control register 1
    #define WHO_AM_I          1 // position of who am i reg
    #define MSB_LDR           2 // position of MSB of LDR
    #define LSB_LDR           3 // position of LSB of LDR
    #define MSB_TMP           4 // position of MSB of TMP
    #define LSB_TMP           5 // position of LSB of TMP

    // Define samples for average
    #define SAMPLE_1 0x00
    #define SAMPLE_2 0x01
    #define SAMPLE_3 0x02
    #define SAMPLE_4 0x03

    // Define Led Status
    #define ON 1
    #define OFF 0

    // Define R/W boundary
    #define READ_REGISTERS 4

    // Define period
    #define DEFAULT_PERIOD 200 

    // Define masks
    #define MASK_AVERAGE_SAMPLES 0x18
    #define MASK_STATUS_REG      0x03
    #define MASK_LED_MODALITY    0x04
    
    //Constants for sample conversions
    #define TMP_SLOPE                   10      // (1000mV - 500mV) / (50°C - 25°C)  
    #define TMP_INTERCEPT               500.0   // 500mV is the intercept @25°C - RGB LED is turned for T > 25°C
    #define SERIES_RESISTANCE           990
    #define ACTUAL_Vdd_mV               4650.0
    #define TEN_TO_LDR_INTERCEPT        100000  // pow(10,q) = 100000 (q = 5)
    #define LDR_SLOPE                   -0.682


#endif
/* [] END OF FILE */
