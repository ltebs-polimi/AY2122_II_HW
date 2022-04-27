/* =====================================================
 *
 * ELECTRONIC TECHNOLOGIES AND BIOSENSORS LABORATORY
 * Academic year 2021/22, II Semester
 * Assignment 1

 * Authors: Group 1
 *
 * ------------- RGB LED DRIVER (header) ---------------
 * This file contains the definitions of the functions 
 * to drive the RGB LED. For functions implementation
 * and description, see RGB_Led_driver.c file.
 * -----------------------------------------------------
 * 
 * =====================================================
*/

#ifndef __RGB_LED_DRIVER_H__
    #define __RGB_LED_DRIVER_H__

    #include "project.h"
    
    typedef struct {
        uint8_t red;
        uint8_t blue;
        uint8_t green;
    } Color;
    
    void RGBLed_Start(void);
    
    void RGBLed_Stop(void);
    
    void RGBLed_WriteColor(Color c);
    
    void RGBLed_WriteRed(uint8_t red);
    
    void RGBLed_WriteGreen(uint8_t green);
    
    void RGBLed_WriteBlue(uint8_t blue);
    
#endif

/* [] END OF FILE */
