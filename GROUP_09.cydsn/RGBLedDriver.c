/* ========================================
 *
 * \ authors : Giorgio Allegri - Luca Pavirani
 * 
 * \ Source file for RGB Led Driver
 *
 *
 * ========================================
*/

#include "RGBLedDriver.h"

static void RGBLed_WriteRed(uint8_t red);
static void RGBLed_WriteGreen(uint8_t green);
static void RGBLed_WriteBlue(uint8_t blue);

void RGBLed_Start()
{
    PWM_GB_Start();
    PWM_R_Start();
}

void RGBLed_Stop()
{
    PWM_GB_Stop();
    PWM_R_Stop();
}

// LED colors definition
void RGBLed_WriteColor(Color c)
{
    RGBLed_WriteRed(c.red);
    RGBLed_WriteGreen(c.green);
    RGBLed_WriteBlue(c.blue);
}

// Assigning the DC to modulate LED intensity

static void RGBLed_WriteRed(uint8_t red)
{
    PWM_R_WriteCompare(red);
}

static void RGBLed_WriteGreen(uint8_t green)
{
    PWM_GB_WriteCompare2(green);
}

static void RGBLed_WriteBlue(uint8_t blue)
{
    PWM_GB_WriteCompare1(blue);
}
/* [] END OF FILE */
