/* =================================================
 
Electronic Technologies and Biosensors Laboratory
Academic Year 2021/2022 - II Semester
Assignment
Alfonzo Massimo, Mazzucchelli Umberto

file to define registers values
 
 * =================================================
*/
#ifndef __UTILS_H
    #define __UTILS_H

    /**
    *   \brief 7-bit I2C address of the slave device.
    */
    #define SLAVE_DEVICE_ADDRESS 0x08

    /**
    *   \WHO AM I register address
    */
    #define SLAVE_WHO_AM_I_REG_ADDR 0x01
    
    /**
    *   \WHO AM I return value
    */
    #define SLAVE_WHOAMI_RETVAL     0xBC

    /**

    *   \Control register 1 Address
    */
    #define SLAVE_CTRL_REG1 0x00

    /**
    
    *   \OFF
    **/
    #define SLAVE_MODE_OFF 0x00
    
    /**
    *   \LDR
    **/
    #define SLAVE_LDR 0x01
    
    /**
    *   \TMP
    **/
    #define SLAVE_TMP 0x02
    
    /**
    *   \LDR and TMP
    **/
    #define SLAVE_BOTH 0x03
    
    /*
        \LDR modulates LED
    */
    #define MOD_LDR 0x00
    /*
        \TMP modulates LED
    */
    #define MOD_TMP 0x01
    
    /*
        \TMP LED ON - ON while reading TMP only
    */
    #define LED_TMP 0x01
    
    /*
        \LDR LED ON - ON while reading LDR only
    */
    #define LED_LDR 0x02
    /*
        \BOTH CHANNELS LED ON - ON while reading both
    */
    #define LED_BOTH 0x04
    
    /*
        \LDR channel
    */
    #define LDR_channel 0
    
    /*
        \TMP channel
    */
    #define TMP_channel 1
    
    extern int channel;

#endif
/* [] END OF FILE */
