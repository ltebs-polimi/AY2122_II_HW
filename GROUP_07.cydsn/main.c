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
#include "project.h"
#include "Interrupt_Routines.h"
#include "Device_Driver.h"

#define T_AMB 10000 

uint8_t slave_buffer[SLAVE_BUFFER_SIZE];

int status = 0, samples = 0;
_Bool red = 0, green = 0, blue = 0;
int32 control = 0;
_Bool led_modality = 0;

extern int32 temp, ldr;

int main(void)
{
    CyGlobalIntEnable;
    
    //set Timer
    Timer_Start();
    
    //set custom Interrupt
    isr_ADC_StartEx(Custom_ISR_ADC);
    
    //set Slave
    EZI2C_Start();
    EZI2C_Enable();

    
    //set who_am_i register
    slave_buffer[1] = 0xBC;
    //set control register = 0
    slave_buffer[0] = 0x00; 
    
    EZI2C_SetBuffer1(SLAVE_BUFFER_SIZE, SLAVE_BUFFER_SIZE - 5, slave_buffer);

    for(;;)
    {     
        /*check status and set the ON/OFF of the devices 
            to reduce power consumption when not used.*/
        status = slave_buffer[0] & 0x03;
        if (status != 0) Device_Start(); 
        if (status == 0) Device_Stop(); 
        
        //read the number of samples defined in the bridge control panel
        samples = ((slave_buffer[0] & 0b00011000)>>3);
        //allows to have 1 to 4 samples, instead of 0 to 3
        samples = samples + 1;
        
        /*set the timer to send the signal always at 50Hz
            (for example, if we set 4 samples we need a period of 12/4 = 3,
            which @600Hz is 50ms*/
        Timer_WritePeriod(12/samples);

        
        //read the registers to determine the color of the LED
        red = ((slave_buffer[0] & 0b00100000)>>5);
        green = ((slave_buffer[0] & 0b01000000)>>6);
        blue = ((slave_buffer[0] & 0b10000000)>>7);

        
        //read the register for determine the LED modulation
        led_modality = ((slave_buffer[0] & 0b00000100)>>2);
 
        //allows the modulation only if at least one of the color is selected
        if (red || green || blue)
        {
            //modulate LED with ldr
            if (led_modality == 0)  control = 65535 - ldr;
            
            //modulate LED with temperature
            else 
            {
                //determine control value for modulation
                control = (temp - T_AMB)*(65535/55535);
                if (control < 0)    control = 0;
            }
        }

        
        //color modulation
        if (red == 1)    PWM_RG_WriteCompare1(control);
        else    PWM_RG_WriteCompare1(0);
        
        if (green == 1)    PWM_RG_WriteCompare2(control);
        else    PWM_RG_WriteCompare2(0);
        
        if (blue == 1)   PWM_B_WriteCompare(control);
        else    PWM_B_WriteCompare(0);
        
    }
}
/* [] END OF FILE */
