/* =====================================================
 *
 * ELECTRONIC TECHNOLOGIES AND BIOSENSORS LABORATORY
 * Academic year 2021/22, II Semester
 * Assignment 1

 * Authors: Group 1
 *
 * ------------- RGB LED DRIVER (source) ---------------
 * This file contains the implementation of the 
 * functions to drive the RGB LED.
 * -----------------------------------------------------
 * 
 * =====================================================
*/

#include <RGB_Led_driver.h> 

void RGBLed_WriteRed (uint8_t red);
void RGBLed_WriteGreen (uint8_t green);
void RGBLed_WriteBlue (uint8_t blue);

/*-------------------------------------------
 *\brief: PWM to drive the RGB LED is started
*/
void RGBLed_Start() 
{
    PWM_RG_Start();
    PWM_B_Start();
}

/*------------------------------------------
 *\brief: PWM to drive the RGB LED is stopped
*/
void RGBLed_Stop()
{
    PWM_RG_Stop();
    PWM_B_Stop();
}

/*------------------------------------------
 *\brief: RGB LED color setting
*/
void RGBLed_WriteColor(Color c)
{
    RGBLed_WriteRed(c.red);
    RGBLed_WriteGreen(c.green);
    RGBLed_WriteBlue(c.blue);
}

/*------------------------------------------
 *\brief: color set to red
*/
void RGBLed_WriteRed(uint8_t red)
{
    PWM_RG_WriteCompare1(red);
}

/*------------------------------------------
 *\brief: color set to green
*/
void RGBLed_WriteGreen(uint8_t green)
{
    PWM_RG_WriteCompare2(green);
}

/*------------------------------------------
 *\brief: color set to blue
*/
void RGBLed_WriteBlue(uint8_t blue)
{
    PWM_B_WriteCompare(blue);
}

/* [] END OF FILE */
