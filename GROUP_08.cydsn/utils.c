/* ========================================
 *
 * Authors: Alfonzo Massimo, Mazzucchelli Umberto
 * Date: 27/04/2022
 * 
 * Source file for general functions and macros
 * ========================================
*/

/*Include necessary headers and libraries*/

#include "utils.h"
#include "project.h"

// Declaration for functions to set RGB values 
static void RGBLed_WriteRed(uint8_t red);
static void RGBLed_WriteGreen(uint8_t green);
static void RGBLed_WriteBlue(uint8_t blue);

// Variables declaration
char value;
char newPeriod;
char red, green, blue;
char colors[3];
char offColors[3] = {0,0,0};
 

// Update LED function
void updateLed(uint8_t modulator,uint16_t rgb_value_ldr,uint16_t rgb_value_tmp,char LEDs[3])
{   
    // Switch case based on the sensor chosen by the user
    switch (modulator)
    {
        // If the user has chosen to modulate the led using TMP sensor
        case TMP_mod:
        value = ((rgb_value_tmp-tAmb)*255)/900;    // Transformation of the value read by the sensor 
        
        // Setting limits
        if(value>255) value=255;
        if(value<0) value=0;    
    
        // Lights only the colors selected
        for(int i=0;i<3;i++)
        {
            if(LEDs[i]==1) colors[i]=value;
            else colors[i]=0;
        }
        
        if(rgb_value_tmp<10300) RGBLed_WriteColor(offColors);  // Turns off the LED in case it's under the room temperature threshold
        else RGBLed_WriteColor(colors);
        break;
        
        // If the user has chosen to modulate the led using LDR sensor
        case LDR_mod:
        value = -((rgb_value_ldr-20000)*255)/30000; // Transformation of the value read by the sensor 
        
        // Setting limits
        if(value>255) value=255;
        if(value<0) value=0;    
        // Setting the colors to their maximus value if they have been chosen by the user
        for(int i=0;i<3;i++)
        {
            if(LEDs[i]==1) colors[i]=value;
            else colors[i]=0;
        }
        if(rgb_value_ldr<45000) RGBLed_WriteColor(colors);  // Turns off the LED in case it's under the ambient light threshold
        else RGBLed_WriteColor(offColors);
        break;
    }
    
}

// Starts all the PWMs and thei shared clock
void RGBLed_Start()
{
    PWM_R_Start();
    
    PWM_G_Start();
    
    PWM_B_Start();
    
    Clock_Start();
}

// Turns off the LED
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

// Updates the timer based on the number of samles selected by the user
void updateTimer(char samples,char defaultPeriod)
{
    Timer_Stop();
    newPeriod = defaultPeriod/samples;
    Timer_WritePeriod(newPeriod);
    Timer_Start();
}

/* [] END OF FILE */
