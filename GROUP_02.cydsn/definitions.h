/* ========================================
 *
 * LTEBS - A.Y. 2021/2022, II semester
 * Assignment, Group 02
 * Authors: Federico Petraccia, Francesca Terranova
 * 
 * definitions file
 *
 * ========================================
*/

#include "project.h"

// Define I2C Slave Buffer Structure
#define CTRL_REG_1          0            // Address of Control Register 1
#define WHO_AM_I            1            // Address of Who Am I
#define MSB_LDR             2            // Address of Most Significant Byte of LDR sensor
#define LSB_LDR             3            // Address of Least Significant Byte of LDR sensor
#define MSB_TMP             4            // Address of Most Significant Byte of TMP sensor
#define LSB_TMP             5            // Address of Most Significant Byte of TMP sensor

#define BUFFER_SIZE         6            // Number of addressed registers inside the Slave Buffer
#define RW_SIZE             1            // Number of data bytes starting from the beginning of the buffer with read and write access 
                                            // (data bytes located at offset rwBoundary or greater are read only) 

// Pointer to the data buffer for I2C communication + Initialization of cells where the measurements will be inserted (default values set to 0)
// uint8_t slave_pointer_Buffer[BUFFER_SIZE] = {0,EZI2C_WHO_AM_I_DEFAULT_VALUE,0,0,0,0};   
uint8_t slave_pointer_Buffer[BUFFER_SIZE] = {0};

// Define variable to turn on/off the RGB LED
#define LOW               0                // To set logical value low to 0
#define HIGH              1                // To set logical value high to 1
#define LDR_readout       0                // To use in order to control the LED intensity using the LDR 
#define TMP_readout       1                // To use in order to control the LED intensity using the TMP
                                               // (see also definition above of "uint8_t LED_modality")
#define LED_CHECK_DELAY 600               //Blink delay in ms of LED CHECK, which is used to visually verify that 
                                           //an active channel is controlling the LED

// Computing averaged values
volatile uint8_t PacketReadyFlag = 0;      // Initialize flag to indicate if bytes are to send or not 
volatile uint32 sum_TMP;                    // Summation of binary values sampled from TMP
volatile uint32 sum_LDR;                    // Summation of binary values sampled from LDR

uint16 average_TMP_code;                  // Averaged values of the X samples considered from TMP (binary)
uint16 average_LDR_code;                  // Averaged values of the X samples considered from LDR (binary)
uint16 average_TMP_mV;                    // Averaged values of the X samples considered from TMP (mV)
uint16 average_LDR_mV;                    // Averaged values of the X samples considered from LDR (mV)
 
uint8_t counter = 0;                     // Counter to be incremented in order to keep track of number of samples
uint8_t LED_modality;                    // LED modality that indicates the sensor that modulates RGB LED intensity
                                            // 0 = LDR readout
                                            // 1 = TMP readout
volatile uint8_t samples = 4;            // Number of samples to be used to compute the average
                                         // Initialization choosing number of samples equal to 4
volatile uint8_t status;                    // bits 0 and 1; status can be set to:
                                            // a. 00 (device stopped)
                                            // b. 01 (to sample LDR channel)
                                            // c. 10 (to sample TMP channel) 
                                            // d. 11 (to sample both channel)

// Timer period
uint8_t timer_period = 0;                   // Period of transmission 
int32 clock_frequency = 6000;               // Timer clock frequency 
                                                  // By choosing a value equal to 6000 Hz we avoid to have a problem of overloading
                                                  // derived from the division computed for obtaining the timer period, beacuse 6000 Hz
                                                  // divided by transmission_datarate*samples (for samples from 1 to 4) is always an integer
int32 transmission_datarate = 50;           // Required transmission data rate

// PWM
uint16_t compareValueLDR = 50;     // PWM initialization to 50%; 8 bit PWM in order to insert 8 bit values of average data realted to LDR
uint16_t compareValueTMP = 50;     // PWM initialization to 50%; 8 bit PWM in order to insert 8 bit values of average data realted to TMP
char value;                        // Value that has to be written inside the PWM

// Conversion to lux and temperature
#define SERIES_RESISTANCE           10000 
#define ACTUAL_Vdd_mV               4869.0
#define TEN_TO_LDR_INTERCEPT        100000
#define LDR_SLOPE                   -0.7
#define q_TMP                       545
#define sensibility_TMP             10
uint16 average_LDR_lux;             // Averaged values of the X samples considered from LDR (lux)
uint16 average_TMP_temp;            // Averaged values of the X samples considered from TMP (°C)