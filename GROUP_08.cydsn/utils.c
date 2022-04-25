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
#include "utils.h"
#include "project.h"

static void RGBLed_WriteRed(uint8_t red);
static void RGBLed_WriteGreen(uint8_t green);
static void RGBLed_WriteBlue(uint8_t blue);

char value;

void updateLed(uint8_t modulator,uint16_t rgb_value)
{   

    switch (modulator)
    {
        case TMP_mod:
        value = ((rgb_value-10300)*255)/900;
        if(value<0) value=0;
        if(rgb_value<10300) RGBLed_WriteColor(0, 0, 0);
        else RGBLed_WriteColor(value,0,0);
        
        /*
        PWM_R_WriteCompare((TMP_avg_digit*255)/65535);
        PWM_G_WriteCompare((TMP_avg_digit*255)/65535);
        */
        break;
        
        case LDR_mod:
        if(rgb_value<30000) RGBLed_WriteColor(0, 0, 255);
        else RGBLed_WriteColor(0, 0, 0);
        /*
        PWM_B_WriteCompare((LDR_avg_digit*255)/65535);
        PWM_G_WriteCompare((LDR_avg_digit*255)/65535);
        */
        break;
    }
    
}


void RGBLed_Start()
{
    PWM_R_Start();
    
    PWM_G_Start();
    
    PWM_B_Start();
    
    Clock_Start();
}

void RGBLed_Stop()
{
    PWM_R_Stop();
    Clock_Stop();
    
    PWM_G_Stop();

    
    PWM_B_Stop();

    
    Pin_R_Write(0);
    Pin_G_Write(0);
    Pin_B_Write(0);
}

// LED colors definition
void RGBLed_WriteColor(uint8_t red, uint8_t green, uint8_t blue)
{
    RGBLed_WriteRed(red);
    RGBLed_WriteGreen(green);
    RGBLed_WriteBlue(blue);
}

// Assigning the DC to modulate LED intensity

static void RGBLed_WriteRed(uint8_t red)
{
    PWM_R_WriteCompare(red);
}

static void RGBLed_WriteGreen(uint8_t green)
{
    PWM_G_WriteCompare(green);
}

static void RGBLed_WriteBlue(uint8_t blue)
{
    PWM_B_WriteCompare(blue);
}
/* [] END OF FILE */
