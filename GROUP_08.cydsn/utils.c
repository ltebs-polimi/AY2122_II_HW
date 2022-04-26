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
char newPeriod;

char red, green, blue;

char colors[3];
char offColors[3] = {0,0,0};

void updateLed(uint8_t modulator,uint16_t rgb_value_ldr,uint16_t rgb_value_tmp,char LEDs[3])
{   

    switch (modulator)
    {
        case TMP_mod:
        value = ((rgb_value_tmp-10300)*255)/900;
        if(value<0) value=0;
        red = LEDs[0];
        green = LEDs[1];
        blue = LEDs[2];
        for(int i=0;i<3;i++)
        {
            if(LEDs[i]==1) colors[i]=value;
            else colors[i]=0;
        }
        
        if(rgb_value_tmp<10300) RGBLed_WriteColor(offColors);
        else RGBLed_WriteColor(colors);
        
        /*
        PWM_R_WriteCompare((TMP_avg_digit*255)/65535);
        PWM_G_WriteCompare((TMP_avg_digit*255)/65535);
        */
        break;
        
        case LDR_mod:
        for(int i=0;i<3;i++)
        {
            if(LEDs[i]==1) colors[i]=255;
            else colors[i]=0;
        }
        if(rgb_value_ldr<30000) RGBLed_WriteColor(colors);
        else RGBLed_WriteColor(offColors);
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
    Pin_R_Write(0);
    Pin_G_Write(0);
    Pin_B_Write(0);
}

// LED colors definition
void RGBLed_WriteColor(char colors[3])
{
    RGBLed_WriteRed(colors[0]);
    RGBLed_WriteGreen(colors[1]);
    RGBLed_WriteBlue(colors[2]);
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

void updateTimer(char samples,char defaultPeriod)
{
    Timer_Stop();
    newPeriod = defaultPeriod/samples;
    Timer_WritePeriod(newPeriod);
    Timer_Start();
}

/* [] END OF FILE */
