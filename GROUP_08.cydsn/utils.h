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


//Constants for sample conversions
#define TMP_SLOPE 10.0
#define TMP_INTERCEPT 500.0
#define SERIES_RESISTANCE           1000
#define ACTUAL_Vdd_mV               4702.0
#define TEN_TO_LDR_INTERCEPT        100000 //q = 5 -> pow(10,q) = 100000
#define LDR_SLOPE                   -0.682
/* [] END OF FILE */
