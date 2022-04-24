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

void Device_Start(){
    ADC_DelSig_PR_Start(); 
    AMux_1_Start(); 
    ADC_DelSig_PR_StartConvert(); 
    UART_Start(); 
    Timer_Start(); 
    PWM_RG_Start(); 
    PWM_B_Start(); 
}

void Device_Stop(){
    ADC_DelSig_PR_Stop(); 
    AMux_1_Stop();
    UART_Stop(); 
    PWM_RG_Stop(); 
    PWM_B_Stop(); 

}


/* [] END OF FILE */
