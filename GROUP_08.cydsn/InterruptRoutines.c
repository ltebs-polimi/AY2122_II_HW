/* =================================================
 
Electronic Technologies and Biosensors Laboratory
Academic Year 2021/2022 - II Semester
Assignment
Alfonzo Massimo, Mazzucchelli Umberto

Interrupt Routines source file
 
 * =================================================
*/
#include "InterruptRoutines.h"
#include "project.h"
#include "utils.h"

int32 TMP_meas;
int32 LDR_meas;



uint8 SendFlag = 0;

CY_ISR (Custom_ISR_ADC)
{
    Timer_ReadStatusRegister();
    if (ReadADC_Flag==1)
    {
        
        switch (channel)
        {
            case 1:
                AMux_FastSelect(LDR_channel);
                LDR_meas = ADC_DelSig_Read32();
                if (LDR_meas<0) LDR_meas = 0;
                if (LDR_meas>65535) LDR_meas = 65535;
                break;
                
            case 2:
                AMux_FastSelect(TMP_channel);
                TMP_meas = ADC_DelSig_Read32();
                if (TMP_meas<0) TMP_meas = 0;
                if (TMP_meas>65535) TMP_meas = 65535;
            break;
                
            case 3: //Read both channels and save the variable
                AMux_FastSelect(LDR_channel);
                LDR_meas = ADC_DelSig_Read32();
                if (LDR_meas<0) LDR_meas = 0;
                if (LDR_meas>65535) LDR_meas = 65535;
                AMux_FastSelect(TMP_channel);
                TMP_meas = ADC_DelSig_Read32();
                if (TMP_meas<0) TMP_meas = 0;
                if (TMP_meas>65535) TMP_meas = 65535;
            break;
        }
        
        ReadADC_Flag = 0;
        SendFlag = 1;
    }
    
}


/* [] END OF FILE */
