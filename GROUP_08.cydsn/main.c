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
#include "utils.h"
#include "InterruptRoutines.h"

int main(void)
{
    CyGlobalIntEnable; /* Enable global interrupts. */
    
    Timer_Start(); 
    ADC_DelSig_Start();
    isr_ADC_StartEx(Custom_ISR_ADC);
    AMux_Start();
    EZI2C_Start();

    for(;;)
    {
        /* Place your application code here. */
    }
}

/* [] END OF FILE */
