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

#define WHO_AM_I_Def 0xBC
#define CTRL_REG 0x00
#define WHO_AM_I 0x01
#define MSB_LDR 0x02
#define LSB_LDR 0x03
#define MSB_TMP 0x04
#define LSB_TMP 0x05

#define readTMP 2
#define readLDR 1
#define readBoth 3
#define readNone 0

#define TMP_mod 1
#define LDR_mod 0

//Constants for sample conversions
#define TMP_SLOPE                   11.0
#define TMP_INTERCEPT               552.0
#define SERIES_RESISTANCE           1000
#define ACTUAL_Vdd_mV               4681.0
#define TEN_TO_LDR_INTERCEPT        100000 //q = 5 -> pow(10,q) = 100000
#define LDR_SLOPE                   -0.682

#define tAmb 13000


// Brief Start driver
void RGBLed_Start(void);

// Stop 
void RGBLed_Stop(void);       

// Brief Write new color (c) 
void RGBLed_WriteColor(uint8_t red, uint8_t green, uint8_t blue);

void updateLed(uint8_t modulator,uint16_t rgb_value);

/* [] END OF FILE */
