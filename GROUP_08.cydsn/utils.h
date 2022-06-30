/* ========================================
 *
 * Authors: Alfonzo Massimo, Mazzucchelli Umberto
 * Date: 27/04/2022
 * 
 * Header file for general functions and macros
 * ========================================
*/

/*Include necessary headers*/
#include "project.h"

// Definition of the registers 
#define WHO_AM_I_Def 0xBC
#define CTRL_REG 0x00
#define WHO_AM_I 0x01
#define MSB_LDR 0x02
#define LSB_LDR 0x03
#define MSB_TMP 0x04
#define LSB_TMP 0x05

// Definition of the possible status
#define readTMP 2
#define readLDR 1
#define readBoth 3
#define readNone 0

// Definition of the possible modulator
#define TMP_mod 1
#define LDR_mod 0

//Constants for sample conversions
#define TMP_SLOPE                   11.0
#define TMP_INTERCEPT               552.0
#define SERIES_RESISTANCE           10000
#define ACTUAL_Vdd_mV               4900.0
#define LDR_INTERCEPT        100000 //q = 5 -> pow(10,q) = 100000
#define LDR_SLOPE                   -0.682

#define tAmb 10300


// Brief Start driver
void RGBLed_Start(void);

// Stop RGB
void RGBLed_Stop(void);       

// Writes new colors
void RGBLed_WriteColor(char colors[3]);

// Updates LED modulation
void updateLed(uint8_t modulator,uint16_t rgb_value_ldr,uint16_t rgb_value_tmp,char LEDs[3]);

// Updates timer based on number of samples selected by the user
void updateTimer(char samples,char defaultPeriod);

/* [] END OF FILE */
