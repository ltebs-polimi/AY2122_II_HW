/* ========================================
 *
 * LTEBS - A.Y. 2021/2022, II semester
 * Assignment, Group 02
 * Authors: Federico Petraccia, Francesca Terranova
 * 
 * Header file containing addresses and values of I1C Slave Buffer
 *
 * ========================================
*/

#ifndef __EZI2C_H
    #define __EZI2C_H

    /**
    *   \brief 7-bit I2C address of the slave device
    */
    #define EZI2C_DEVICE_ADDRESS        0x08
    
    /**
    *   \brief Address of the Control Register 1
    */
    #define EZI2C_CTRL_REG_1_ADDRESS    0x00
    
    /**
    *   \brief Address of the WHO AM I register
    */
    #define EZI2C_WHO_AM_I_REG_ADDR     0x01
    
    /**
    *   \brief WHOAMI defaul value
    */
    #define EZI2C_WHO_AM_I_DEFAULT_VALUE      0xBC 

    /**
    *   \status set to 0b00 in Control Register 1 --> to stop the device
    */
    #define EZI2C_STOP_REG_1 0x00 
    
    /**
    *   \status set to 0b01 in Control Register 1 --> to sample the first channel (LDR)
    */    
    #define EZI2C_LDR_REG_1 0x01  
    
    /**
    *   \status set to 0b10 in Control Register 1 --> to sample the second channel (TMP)
    */      
    #define EZI2C_TMP_REG_1 0x02
    
    /**
    *   \status set to 0b11 in Control Register 1 --> to sample both channels (LDR and TMP)
    */      
    #define EZI2C_LDR_TMP_REG_1 0x03    

#endif

/* [] END OF FILE */