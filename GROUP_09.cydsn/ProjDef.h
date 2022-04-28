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
    #define DEFAULT_PERIOD 200 // period defined in the top design 

    // Define masks
    #define MASK_AVERAGE_SAMPLES 0x18
    #define MASK_STATUS_REG      0x03
    #define MASK_LED_MODALITY    0x04
    #define MASK_LED_CHANNEL     0xE0
    
    // Define LED channels
    #define RED         1 //001
    #define GREEN       2 //010
    #define GREENRED    3 //011
    #define BLUE        4 //100
    #define BLUERED     5 //101
    #define BLUEGREEN   6 //110
    #define RGB         7 //111
    
    //Constants for sample conversions
    #define TMP_SLOPE                   10.0    // (1000mV - 500mV) / (50°C - 25°C)  
    #define TMP_INTERCEPT               500.0   // 500mV is the intercept @25°C - RGB LED is turned on for T > 25°C
    #define SERIES_RESISTANCE           1000    // R2 = 1kOhm
    #define ACTUAL_Vdd_mV               4820.0  
    #define LDR_INTERCEPT               100000  // pow(10,q) = 100000 (q = 5)
    #define LDR_SLOPE                   -0.682
    #define T_AMBIENT                   20
    #define LDR_TH                      50      // dark room = 50 lux
    #define OFFSET_TMP                  8       // offset of the sensor


#endif
/* [] END OF FILE */
