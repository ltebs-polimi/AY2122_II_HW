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
#include "Device_Driver.h"
#include "project.h"

//turn ON all the elements only when needed
void Device_Start()
{
    ADC_DelSig_PR_Start(); 
    AMux_1_Start(); 
    ADC_DelSig_PR_StartConvert(); 
    PWM_RG_Start(); 
    PWM_B_Start(); 
}

//set all the elements OFF when not needed
void Device_Stop()
{
    ADC_DelSig_PR_Stop(); 
    AMux_1_Stop(); 
    PWM_RG_Stop(); 
    PWM_B_Stop(); 
}


/* [] END OF FILE */
