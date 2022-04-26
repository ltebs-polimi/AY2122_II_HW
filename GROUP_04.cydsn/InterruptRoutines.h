/* ======================================================
 
Electronic Technologies and Biosensors Laboratory
Academic Year 2021/2022 - II Semester
Assignment
GROUP_04 - Franke Patrick & Canavate Chloé 

    interrupt routines header file: 
      Custom_ISR_ADC: isr to sample sensor value from ADC

====================================================== */
 

#ifndef __INTERRUPT_ROUTINES_H
    #define __INTERRUPT_ROUTINES_H
    
    #include "cytypes.h"
    #include "stdio.h"
    
    int32 value_TMP;      //Digit value of TMP sensor from ADC
    int32 value_LDR;      //Digit value of LDR sensor from ADC

    CY_ISR(Custom_ISR_ADC);
    
#endif

/* [] END OF FILE */