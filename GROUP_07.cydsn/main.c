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
uint8_t count = 0;
int status = 0, samples = 0;
_Bool red = 0, green = 0, blue = 0;
int32 control = 0;
_Bool led_modality = 0;

extern int32 temp, ldr;

int main(void)
{
    CyGlobalIntEnable; /* Enable global interrupts. */
    
    //abilito timer
    Timer_Start();
    
    //abilito interrupt
    isr_ADC_StartEx(Custom_ISR_ADC);
    
    //abilito lo slave
    EZI2C_Start();
    EZI2C_Enable();

    
    //set registro who_am_i
    slave_buffer[1] = 0xBC;
    //setto control register inizialmente a 0 con solo average = 1
    slave_buffer[0] = 0x00; 
    
    EZI2C_SetBuffer1(SLAVE_BUFFER_SIZE, SLAVE_BUFFER_SIZE - 5, slave_buffer);

    for(;;)
    {     
        //controllo lo status e del numero di samples
        status = slave_buffer[0] & 0x03;
        if (status != 0) Device_Start(); 
        if (status == 0) Device_Stop(); 
        
        samples = ((slave_buffer[0] & 0b00011000)>>3);
        samples = samples + 1; //altrimenti con 00 mi campiona 0 valori e con 11 ne campiona 3
        
        //Setto il timer in modo da inviare i dati sempre a 50 Hz
        Timer_WritePeriod(12/samples);

        
        //campiono il registro per il led
        red = ((slave_buffer[0] & 0b00100000)>>5);
        green = ((slave_buffer[0] & 0b01000000)>>6);
        blue = ((slave_buffer[0] & 0b10000000)>>7);

        
        //campiono il registro per il led drive
        led_modality = ((slave_buffer[0] & 0b00000100)>>2);
 
        if (red || green || blue)
        {
            //regolo led con ldr
            if (led_modality == 0)  control = 65535 - ldr;
            
            //regolo led con temp
            else 
            {
                //determino valore
                control = (temp - T_AMB)*(65535/55535);
                if (control < 0)    control = 0;
            }
        }

        
        //aggiusto i colori
        if (red == 1)    PWM_RG_WriteCompare1(control);
        else    PWM_RG_WriteCompare1(0);
        
        if (green == 1)    PWM_RG_WriteCompare2(control);
        else    PWM_RG_WriteCompare2(0);
        
        if (blue == 1)   PWM_B_WriteCompare(control);
        else    PWM_B_WriteCompare(0);
        
    }
}
/* [] END OF FILE */
