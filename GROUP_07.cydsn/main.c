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

#define T_AMB 10000 //calcolato un po' troppo brutalmente

uint8_t slave_buffer[SLAVE_BUFFER_SIZE];
uint8_t count = 0;
int status = 0, samples = 0;
_Bool red = 0, green = 0, blue = 0;
int16 control = 0;
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
    //abilito comunicazione seriale (da togliere)
    UART_Start();
    //abilito PWM
    PWM_RG_Start();
    PWM_B_Start();
    
    //set registro who_am_i
    slave_buffer[1] = 0xBC;
    //setto control register inizialmente a 0 con solo average = 1
    slave_buffer[0] = 0x00; 
    
    EZI2C_SetBuffer1(SLAVE_BUFFER_SIZE, SLAVE_BUFFER_SIZE - 4, slave_buffer);

    for(;;)
    {
        //control=0; 
        
        //controllo lo status e del numero di samples
        status = slave_buffer[0] & 0x03;
        samples = ((slave_buffer[0] & 0b00011000)>>3);
        samples = samples + 1; //altrimenti con 00 mi campiona 0 valori e con 11 ne campiona 3
        
        Timer_WritePeriod(samples*255/4);
        
        ////////////////////////////////////////
        ////        DEFINIRE LED RGB        ////
        ////////////////////////////////////////
        
        //campiono il registro per il led
        red = ((slave_buffer[0] & 0b00100000)>>5);
        if (red)
        {
            sprintf(message, "rosso\r\n\n");
            UART_PutString(message);
        }
        green = ((slave_buffer[0] & 0b01000000)>>6);
        if (green)
        {
            sprintf(message, "verde\r\n\n");
            UART_PutString(message);
        }
        blue = ((slave_buffer[0] & 0b10000000)>>7);
        if (blue)
        {
            sprintf(message, "blu\r\n\n");
            UART_PutString(message);
        }
        
        //campiono il registro per il led drive
        led_modality = ((slave_buffer[0] & 0b00000100)>>2);
        


        
        if (led_modality == 0) //regolo led con ldr
        {
            control = 65535 - ldr;
            
            sprintf(message, "valore di temp: %li\r\n\n", ldr);
            UART_PutString(message);
            
            sprintf(message, "valore di controllo del LED: %d\r\n\n", control);
            UART_PutString(message);
        }
        
        else //regolo led con temp
        {
            //determino valore
            control = (temp - T_AMB)*(65535/55535);
            if (control < 0)
                control = 0;
        }
        

        
        //aggiusto i colori
        if (red == 1)    PWM_RG_WriteCompare1(control);
        else    PWM_RG_WriteCompare1(0);
        
        if (green == 1)    PWM_RG_WriteCompare2(control);
        else    PWM_RG_WriteCompare2(0);
        
        if (blue == 1)   PWM_B_WriteCompare(control);
        else    PWM_B_WriteCompare(0);

/* [] END OF FILE */
