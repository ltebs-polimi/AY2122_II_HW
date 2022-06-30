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

#ifndef __I2C_REG_H
    #define __I2C_REG_H

    
    // 6bit I2C address of the slave device.
    
    #define I2C_DEVICE_ADDRESS 0x08

    
    //Address of the WHO AM I register return value
    
    #define I2C_WHO_AM_I_REG_VALUE 0xBC
    
    //Who i am register address

    #define I2C_WHO_I_AM_REG_ADDR 0x01
   
    
    //Address of the Control register 1
    
    #define I2C_CTRL_REG1 0x00
     

    //8 bit to set no sampling (status bit 1 = 0, status bit 0 = 0)
    
    #define SLAVE_MODE_OFF_CTRL_REG1 0x00
    
    
    //8 bit to set the LDR sampling on (status bit 1 = 0, status bit 0 = 1)
    
    #define SLAVE_LDR_ON_CTRL_REG1 0x01
    
    
    // 8 bit to set the TMP sampling on (status bit 1 = 1, status bit 0 = 0)
    
    #define SLAVE_TMP_ON_CTRL_REG1 0x02
    
    
      // 8 bit to sampling both LDR and TMP (status bit 1 = 1, status bit 0 = 1)
    
    #define SLAVE_BOTH_ON_CTRL_REG1 0x03
    
    //LED MODALITY
       
    #define LED_MOD_TMP 0x01
    
    #define LED_MOD_LDR 0x00
    
     #define SLAVE_BUFFER_SIZE 6     //number of registers

     #define CTRL_REG1 0             //position of control register 1

     #define WHO_AM_I 1              //position of who am i register

     #define MSB_LDR 2                  //position for Most Significant Byte of the first sensor average

     #define LSB_LDR 3                  //position for Less Significant Byte of the first sensor average

     #define MSB_TMP 4                  //position for Most Significant Byte of the second sensor average

     #define LSB_TMP 5                 //position for Less Significant Byte of the second sensor average
     
     #define TMP_M 10 // mV/C

     #define TMP_OFF 500 // mV

     #define MID_RANGE_LDR 25000 //according to the values printed on the UART

     #define BASE_LINE_TMP 10000 //corresponds to the room temperaure in digit

     #define GAIN_TMP 26 //factor used to convert the considered TMP range into a 0-65355 intervall
    
#endif


/* [] END OF FILE */
