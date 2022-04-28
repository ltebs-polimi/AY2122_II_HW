/* ====================================================
 
Electronic Technologies and Biosensors Laboratory
Academic Year 2021/2022 - II Semester
Assignment
GROUP_04 - Franke Patrick & Canavate Chloé 

    interrupt routines source file: 
      sensor sampling and conversion into digital value

==================================================== */


#include "InterruptRoutines.h"
#include "project.h"

#define LDR_CHANNEL 1
#define TMP_CHANNEL 0

extern volatile uint8_t ReadValue;


CY_ISR(Custom_ISR_ADC)

{
    Timer_ReadStatusRegister();
    
    ReadValue = 1;  //Set flag to 1 to attest ADC reading
    
    
    AMux_FastSelect(LDR_CHANNEL);           //Select LDR sensor channel
    value_LDR = ADC_DelSig_Read32();        //Read LDR sensor value
    
    //Keep values inside the valid range
    if (value_LDR < 0)     value_LDR = 0;
    if (value_LDR > 65535) value_LDR = 65535;
   

    AMux_FastSelect(TMP_CHANNEL);           //Select TMP sensor channel
    value_TMP = ADC_DelSig_Read32();        //Read TMP sensor value
    
    //Keep values inside the valid range
    if (value_TMP < 0)     value_TMP = 0;
    if (value_TMP > 65535) value_TMP = 65535;   
}


/* [] END OF FILE */
