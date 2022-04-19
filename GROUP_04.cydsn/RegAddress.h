/* ========================================
 
Electronic Technologies and Biosensors Laboratory
Academic Year 2020/2021 - II Semester
Assignment 03
GROUP_01 

file to define values of registers
 
 * ========================================
*/

#ifndef __REGADDRESS_H
    #define __REGADDRESS_H

    /**
    *   \brief 7-bit I2C address of the slave device.
    */
    #define SLAVE_DEVICE_ADDRESS 0x08

    /**
    *   \brief Address of the WHO AM I register
    */
    #define SLAVE_WHO_AM_I_REG_ADDR 0x01
    
    /**
    *   \brief WHOAMI return value
    */
    #define SLAVE_WHOAMI_VALUE 0xBC

    /**
    *   \brief Address of the Control register 1
    */
    #define SLAVE_CTRL_REG1_ADDRESS 0x00
    
    /**
    *   \brief 8 bit to set no sampling (status bit 1 = 0, status bit 0 = 0)
    **/
    #define SLAVE_MODE_OFF_CTRL_REG1 0x00
    
    /**
    *   \brief 8 bit to set the LDR sampling on (status bit 1 = 0, status bit 0 = 1)
    **/
    #define SLAVE_LDR_ON_CTRL_REG1 0x01
    
    /**
    *   \brief 8 bit to set the TMP sampling on (status bit 1 = 1, status bit 0 = 0)
    **/
    #define SLAVE_TMP_ON_CTRL_REG1 0x02
    
    /**
    *   \brief 8 bit to sampling both LDR and TMP (status bit 1 = 1, status bit 0 = 1)
    **/
    #define SLAVE_BOTH_ON_CTRL_REG1 0x03
    
    /**
    *   \brief 8 bit to light up only red channel (status bit 5 = 1, status bit 6 = 0, status bit 7 = 0)
    **/
    #define SLAVE_RED_ON_CTRL_REG1 0x20
    
    /**
    *   \brief 8 bit to light up only green channel (status bit 5 = 0, status bit 6 = 1, status bit 7 = 0)
    **/
    #define SLAVE_GREEN_ON_CTRL_REG1 0x40
    
    /**
    *   \brief 8 bit to light up only blue channel (status bit 5 = 0, status bit 6 = 0, status bit 7 = 1)
    **/
    #define SLAVE_BLUE_ON_CTRL_REG1 0x80
    
    /**
    *   \brief 8 bit to light up both red and green channels (status bit 5 = 1, status bit 6 = 1, status bit 7 = 0)
    **/
    #define SLAVE_RED_GREEN_ON_CTRL_REG1 0x60
    
    /**
    *   \brief 8 bit to light up both red and blue channels (status bit 5 = 1, status bit 6 = 0, status bit 7 = 1)
    **/
    #define SLAVE_RED_BLUE_ON_CTRL_REG1 0xA0
    
    /**
    *   \brief 8 bit to light up both green and blue channels (status bit 5 = 0, status bit 6 = 1, status bit 7 = 1)
    **/
    #define SLAVE_GREEN_BLUE_ON_CTRL_REG1 0xC0
    
    /**
    *   \brief 8 bit to light up all channels (status bit 5 = 1, status bit 6 = 1, status bit 7 = 1)
    **/
    #define SLAVE_RED_GREEN_BLUE_ON_CTRL_REG1 0xE0
    
        /**
    *   \brief 8 bit to turn off all channels (status bit 5 = 0, status bit 6 = 0, status bit 7 = 0)
    **/
    #define SLAVE_RED_GREEN_BLUE_OFF_CTRL_REG1 0x00
    
    /**
    *   \ Change LED modality to use the LDR
    **/
    //#define SLAVE_LED_LDR 0x00
    
    /**
    *   \ Change LED modality to use the TMP
    **/
    //#define SLAVE_LED_TMP 0x01
    
 
  
#endif


/* [] END OF FILE */
