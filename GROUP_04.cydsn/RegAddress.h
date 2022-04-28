/* ==============================================
 
Electronic Technologies and Biosensors Laboratory
Academic Year 2021/2022 - II Semester
Assignment
GROUP_04 - Franke Patrick & Canavate Chloé 

    headers with registers' values
============================================== */


#ifndef __REGADDRESS_H
    #define __REGADDRESS_H

    //6-bit I2C address of the slave device
    #define SLAVE_DEVICE_ADDRESS 0x08

    //Who Am I register address
    #define SLAVE_WHO_AM_I_REG_ADDR 0x01
    
    //Who Am I return value
    #define SLAVE_WHOAMI_VALUE 0xBC

    //Control Register 1 address
    #define SLAVE_CTRL_REG1_ADDRESS 0x00
    
    //8 bits to set no sampling (status bit 1 = 0, status bit 0 = 0)
    #define SLAVE_MODE_OFF_CTRL_REG1 0x00
    
    //8 bits to sample LDR only (status bit 1 = 0, status bit 0 = 1)
    #define SLAVE_LDR_ON_CTRL_REG1 0x01
    
    //8 bits to sample TMP only (status bit 1 = 1, status bit 0 = 0)
    #define SLAVE_TMP_ON_CTRL_REG1 0x02
    
    //8 bits to sample both LDR and TMP (status bit 1 = 1, status bit 0 = 1)
    #define SLAVE_BOTH_ON_CTRL_REG1 0x03
    
    //8 bits to light up red channel only (status bit 5 = 1, status bit 6 = 0, status bit 7 = 0)
    #define SLAVE_RED_ON_CTRL_REG1 0x20
    
    //8 bits to light up green channel only (status bit 5 = 0, status bit 6 = 1, status bit 7 = 0)
    #define SLAVE_GREEN_ON_CTRL_REG1 0x40
    
    //8 bits to light up blue channel only (status bit 5 = 0, status bit 6 = 0, status bit 7 = 1)
    #define SLAVE_BLUE_ON_CTRL_REG1 0x80
    
    //8 bits to light up both red and green channels (status bit 5 = 1, status bit 6 = 1, status bit 7 = 0)
    #define SLAVE_RED_GREEN_ON_CTRL_REG1 0x60
    
    //8 bits to light up both red and blue channels (status bit 5 = 1, status bit 6 = 0, status bit 7 = 1)
    #define SLAVE_RED_BLUE_ON_CTRL_REG1 0xA0
    
    //8 bits to light up both green and blue channels (status bit 5 = 0, status bit 6 = 1, status bit 7 = 1)
    #define SLAVE_GREEN_BLUE_ON_CTRL_REG1 0xC0
    
    //8 bits to light up all channels (status bit 5 = 1, status bit 6 = 1, status bit 7 = 1)
    #define SLAVE_RED_GREEN_BLUE_ON_CTRL_REG1 0xE0
    
    //8 bits to turn off all channels (status bit 5 = 0, status bit 6 = 0, status bit 7 = 0)
    #define SLAVE_RED_GREEN_BLUE_OFF_CTRL_REG1 0x00
    
    //Change LED modality to use the LDR
    #define SLAVE_LED_LDR 0x00
    
    //Change LED modality to use the TMP
    #define SLAVE_LED_TMP 0x01

#endif


/* [] END OF FILE */