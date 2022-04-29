/* ========================================
 *
 * \ Authors : Giorgio Allegri - Luca Pavirani
 * 
 * \ Source file for project utilities
 *
 *
 * ========================================
*/
#include "project.h"
#include "ProjDef.h"
#include "RGBLedDriver.h"

Color rgb_color; 

static void RGBLed_WriteRed(uint8_t red);
static void RGBLed_WriteGreen(uint8_t green);
static void RGBLed_WriteBlue(uint8_t blue);

// Start the components
void StartComponents(void)
{
    Timer_Start();
    ADC_DelSig_Start();
    AMux_Start();
    EZI2C_Start();
    ADC_DelSig_StartConvert();
}

// Stop the components
void StopSensors(void)
{
    RGBLed_Stop();
                               
    slaveBuffer[MSB_LDR] = 0x00;
    slaveBuffer[LSB_LDR] = 0x00;
    slaveBuffer[MSB_TMP] = 0x00;
    slaveBuffer[LSB_TMP] = 0x00;
    
}

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

// sets the colors combination of the RGB Led
void Led_Control(uint8 led_channel, uint8 color)
{
    switch (led_channel){ // COLOR CODING DEPENDING ON COLOR CHANNEL CHOSEN: rgb_color = { B, G, R}
        case (RED):
            rgb_color.green = 0;
            rgb_color.blue  = 0;
            rgb_color.red   = color;
            break;
        case (GREEN):
            rgb_color.green = color;
            rgb_color.blue  = 0;
            rgb_color.red   = 0;
            break;
        case (BLUE):
            rgb_color.green = 0;
            rgb_color.blue  = color;
            rgb_color.red   = 0;
            break;
        case (GREENRED):
            rgb_color.green = color;
            rgb_color.blue  = 0;
            rgb_color.red   = color;
            break;
        case (BLUEGREEN):
            rgb_color.green = color;
            rgb_color.blue  = color;
            rgb_color.red   = 0;
            break;
        case (BLUERED):
            rgb_color.red   = color;
            rgb_color.blue  = color;
            rgb_color.green = 0;
            break;
        case (RGB):
            rgb_color.green = color;
            rgb_color.blue  = color;
            rgb_color.red   = color;
            break;
    }
    RGBLed_Start();
    RGBLed_WriteColor(rgb_color);  
}
/* [] END OF FILE */
