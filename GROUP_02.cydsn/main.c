/* ========================================
 *
 * LTEBS - A.Y. 2021/2022, II semester
 * Assignment, Group 02
 * Authors: Federico Petraccia, Francesca Terranova
 * 
 * main source file
 *
 * ========================================
*/

//Include necessary headers and libraries
#include "project.h"
#include "InterruptRoutines.h"
#include "EZI2C.h"
#include "math.h"
#include "stdio.h"
#include "stdlib.h"

// Define I2C Slave Buffer Structure
#define CTRL_REG_1          0            // Address of Control Register 1
#define WHO_AM_I            1            // Address of Who Am I
#define MSB_LDR             2            // Address of Most Significant Byte of LDR sensor
#define LSB_LDR             3            // Address of Least Significant Byte of LDR sensor
#define MSB_TMP             4            // Address of Most Significant Byte of TMP sensor
#define LSB_TMP             5            // Address of Most Significant Byte of TMP sensor

#define BUFFER_SIZE         6            // Number of addressed registers inside the Slave Buffer
#define RW_SIZE             2            // Number of data bytes starting from the beginning of the buffer with read and write access 
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

// Variables declaration
volatile uint8_t PacketReadyFlag = 0;      // Initialize flag to indicate if bytes are to send or not 
volatile uint32 sum_TMP;                    // Summation of binary values sampled from TMP
volatile uint32 sum_LDR;                    // Summation of binary values sampled from LDR

uint16 average_TMP_code;                  // Averaged values of the X samples considered from TMP (binary)
uint16 average_LDR_code;                  // Averaged values of the X samples considered from LDR (binary)
uint16 average_TMP_mV;                    // Averaged values of the X samples considered from TMP (mV)
uint16 average_LDR_mV;                    // Averaged values of the X samples considered from LDR (mV)
uint16 average_LDR_lux;
uint16 average_TMP_temp;
 
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

u_int16_t compareValueLDR = 50;     // PWM initialization to 50%; 16 bit PWM in order to insert 16 bit values of average data realted to LDR
u_int16_t compareValueTMP = 50;     // PWM initialization to 50%; 16 bit PWM in order to insert 16 bit values of average data realted to TMP
u_int16_t periodValuePWM= 100;

// Conversion to lux and temperature
//#define R_pull_down      1000
//#define Val_mV         4869.0 // it depends from the PC you're using
//#define gamma_LDR             -0.7  
// double gamma_LDR =  -0.7;
// uint32 intercept_LDR = 100000;
// int16 arg_LDR;
#define SERIES_RESISTANCE           10000
#define ACTUAL_Vdd_mV               4869.0
#define TEN_TO_LDR_INTERCEPT        100000  // pow(10,q) = 100000 (q = 5)
#define LDR_SLOPE                   -0.7


//#define q_TMP             500
//#define sensibility_TMP    10
#define q_TMP             545
#define sensibility_TMP    10

//INTENSITY
char value;
char DataBuffer[16];

// PWM settings
u_int16_t MAX_Value_LDR=0xD7D3;  // It corresponds to the minimum brightness value which can be measured:  with LDR sensors at 10 lux it is equal to 72,6 kohm (if the ADC
// is set with a dynamic range from 0V to 5V and gain = 1); Rpd (pull down resistor) = 10kohm, 
// DA SISTEMARE!!!! 
//corrisponde al minimo valore di luminosità misurabile: con RLDR a 10 lux pari a 72,6kohm, ADC con dinamica da 0V a 5V e guadagno 1; Rpd=10kohm, a 0,1 lux abbiamo 4,973V corrispondente ad un codice 0xFE9A 
                         // lo usiamo come periodo dei PWM
u_int16_t MAX_Value_TMP=0x4CCC; // corrisponde al codice massimo corrispondente alla temperatura più alta misurabili con il TMP (125°C) con stesse impostazioni ADC di LDR. A 125°C Vtmp=1750mV a cui corrisponde un codice HEX 0x5999 
//Start PWM   -----forse va fatto partire all'interno degli IF
//PWM_LED_Start(); //start PWM

// Conversion Function to Lux
/*double ConvertToLux(int16 digit_Value, int16 R_pd, int16 Val, int16 intercept, double slope){
    double LDR;
    int16 mV_Value;
    mV_Value = ADC_DeltaSigma_CountsTo_mVolts(digit_Value);
    LDR = (int16) (pow((R_pd * (Val/mV_Value-1.0))/intercept,1/slope));
    return LDR;
}*/

//Conversion Function to °C
double ConvertToTMP(int16 digit_Value, double intercept, double slope) {
    //average_TMP_temp = (average_TMP_mV - q_TMP)/sensibility_TMP;
    double A;
    A = (ADC_DeltaSigma_CountsTo_mVolts(digit_Value)-intercept)/slope;
    return A;
} 

int main(void)
{
    CyGlobalIntEnable; /* Enable global interrupts. */

    //Starting functions
    Timer_Start(); 
    ADC_DeltaSigma_Start();
    isr_ADC_StartEx(Custom_ISR_ADC); 
    EZI2C_Start();   
    PWM_Start();

    UART_Start(); // UART!!! 
        
    AMux_ADC_Start(); 

    // Initialize of EZI2C slave
    EZI2C_SetBuffer1(BUFFER_SIZE, RW_SIZE, slave_pointer_Buffer); // This allows to claim which are the cells that can be used for read and write (R/W) for I2C protocol 
    slave_pointer_Buffer[WHO_AM_I] = EZI2C_WHO_AM_I_DEFAULT_VALUE;
    slave_pointer_Buffer[CTRL_REG_1] = EZI2C_STOP_REG_1;
       
    // Start the ADC conversion
    ADC_DeltaSigma_StartConvert(); 
        
    timer_period = clock_frequency/(transmission_datarate*samples);         // Expression to compute the timer period
    Timer_WritePeriod(timer_period-1);                                      // Writes the period register of the timer                  
    // Note: The Period minus one is the initial value loaded into the period register. The software can change this register
    // at any time with the Timer_WritePeriod() API. To get the equivalent result using this API, the Period value from the
    // customizer, minus one, must be used as the argument in the function. (Datasheet Timer 2.80, pag 5)
   
    
    
    for(;;)
    {
        
        // Since the "Average samples bit 1" and the "Average sample bit 2" are positioned on bit 4 and 3, 
        // the mask to apply to the Control Reg 1 will be 0b00011000 which corresponds to 0x18 in hexadecimal notation.
        // In order to obtain the only the 2 bits of interest I need to shift to the right by 3 positions.
        samples = (slave_pointer_Buffer[CTRL_REG_1] & 0x18) >> 3;
        
        // In order to obtain the 2 bit indicating the status (i.e. if we want to sample LDR channel, TMP channel,
        // both of them or stop the acquisition), it is necessary to mask the value contained in the Control Register 1.
        // The mask to be used for this operation is 0b00000011, which corresponds to 0x03. This mask allow to take
        // the first 2 bits of the Control Register 1, containing TMP status and LDR status. 
        status =  slave_pointer_Buffer[CTRL_REG_1] & 0x03;
        
        //status = 0x03;
               
        // In order to obtain LED modality, which is on the third bit, the mask to be used is 0b00000100, corresponding to 
        // 0x04 in hexadecimal notation. The result of the masking has to be right-shifted by 2 bit.
        LED_modality = (slave_pointer_Buffer[CTRL_REG_1] & 0x04) >> 2;
           
    
        if (status == EZI2C_STOP_REG_1) {
            
            LED_CHECK_Write(LOW);                                   
           
            slave_pointer_Buffer[MSB_TMP]  = 0x00;
            slave_pointer_Buffer[MSB_LDR]  = 0x00;
            slave_pointer_Buffer[LSB_TMP]  = 0x00;
            slave_pointer_Buffer[LSB_LDR]  = 0x00;
        
        }    

        else if (status == EZI2C_LDR_REG_1) {
            
           if (PacketReadyFlag == 1 ){
            
              sum_LDR += value_LDR_code; 
                             
              counter++;
                
                if (counter == samples){
                    average_LDR_code = sum_LDR / samples;                 
                    
                    // Map to mV
                    average_LDR_mV = ADC_DeltaSigma_CountsTo_mVolts(average_LDR_code);
                    //average_LDR_lux = ConvertToLux(average_LDR_code, R_pull_down, Val_mV, intercept_LDR, gamma_LDR);
              //      arg_LDR = R_pull_down*(Val_mV/average_LDR_mV -1.0);   
              //      average_LDR_lux = (int16) (pow(arg_LDR/intercept_LDR, 1/gamma_LDR));
                    double LDR = SERIES_RESISTANCE * (ACTUAL_Vdd_mV / average_LDR_mV- 1.0);
                    average_LDR_lux = (int16) (pow(LDR/TEN_TO_LDR_INTERCEPT, 1/LDR_SLOPE));

                    // Save into I2C buffer                       
                    // Right shift in order to memorize the Most Significant Byte (MSB) in the address 0x02 of the I2C Slave Buffer 
                    slave_pointer_Buffer[MSB_LDR] = average_LDR_lux >> 8;    
                    // Application of mask 0b11111111 (0xFF in hexadecimal notation) in order to memorize the Least Significant Byte (LSB)
                    // in the address 0x03 of the I2C Slave Buffer 
                    slave_pointer_Buffer[LSB_LDR] = average_LDR_lux & 0xFF;
                    
                    // UART CHECK!!!!!!
                    sprintf(DataBuffer, "\n%hd", average_LDR_code);
                    UART_PutString(DataBuffer);
                    
                    // Control of LED using PWM
                    if(LED_modality==TMP_readout){
                        
                        // Use of internal LED to say to the user he/she is trying to control the LED using an inactive channel 
                        LED_CHECK_Write(!LED_CHECK_Read());
                        CyDelay(LED_CHECK_DELAY);// Add delay
                        
                        // Since the only channel that is read in this modality is the LDR, we decided not to activate the PWM 
                        // when the user choose that the LED has to be controlled by the TMP sensor.
                                                                         
                    }
                    else if (LED_modality==LDR_readout){
                        
                        LED_CHECK_Write(LOW);
                        Pin_LED_R_Write(HIGH);
                        Pin_LED_B_Write(HIGH);
                        Pin_LED_G_Write(HIGH);
                        
                        compareValueLDR = average_LDR_code;
                        // PWM_WritePeriod(MAX_Value_LDR);
                        // PWM_WriteCompare(MAX_Value_LDR-compareValueLDR);
                          
                        value = -((compareValueLDR-20000)*255)/30000;
                        if(value>255) value=255;
                        if(value<0) value=0; 
                            
                        PWM_WriteCompare(value);
                        
                    }
                        
                    // Reset counter and variables related to LDR sensor
                    sum_LDR = 0;
                    counter = 0;
                    average_LDR_code = 0;
                    average_LDR_mV = 0;

                    // bisogna inserire PWM_Stop()???
                    // se esco da questo IF i LED devono in ogni caso lampeggiare anche se si entra nell'IF dell'altro sensore
                    //quindi non devo azzerare i compareValueLDR e MAXValueLDR ??
                    // ma devono lampeggiare anche se PacketReadyFlag è 0??
                }
                
                PacketReadyFlag = 0;
                
            }    
        
        }    
            
        else if (status == EZI2C_TMP_REG_1) {           
        
           if (PacketReadyFlag == 1 ){
            

              sum_TMP += value_TMP_code; 
                            
            
              counter++;
                
                if (counter == samples){
                    
                    average_TMP_code = sum_TMP / samples; 
                    
                    // Map to mV
                    //average_TMP_mV = ADC_DeltaSigma_CountsTo_mVolts(average_TMP_code);
                    
                    average_TMP_temp = ConvertToTMP(average_TMP_code, q_TMP, sensibility_TMP);
                    // Save into I2C buffer          
                    // Right shift in order to memorize the Most Significant Byte (MSB) in the address 0x04 of the I2C Slave Buffer 
                    slave_pointer_Buffer[MSB_TMP] = average_TMP_temp >> 8;    
                    // Application of mask 0b11111111 (0xFF in hexadecimal notation) in order to memorize the Least Significant Byte (LSB)
                    // in the address 0x05 of the I2C Slave Buffer 
                    slave_pointer_Buffer[LSB_TMP] = average_TMP_temp & 0xFF;
                    
                    // UART CHECK!!!!!!
                    sprintf(DataBuffer, "\n%hd", average_TMP_code);
                    UART_PutString(DataBuffer);
                    
                    // Control of LED using PWM
                    if(LED_modality==LDR_readout){
                        
                        // Use of internal LED to say to the user he/she is trying to control the LED using the switched-off channel 
                        LED_CHECK_Write(!LED_CHECK_Read());
                        CyDelay(LED_CHECK_DELAY);// Add delay
                        
                        // Since the only channel that is read in this modality is the TMP, we decided not to activate the PWM 
                        // when the user choose that the LED has to be controlled by the LDR sensor.                 
                                
                    }
                    else if (LED_modality==TMP_readout){
                        
                        LED_CHECK_Write(LOW);

                        
                       /* PWM_B_Stop(); // è proprio necessario fermarlo???
                        PWM_G_Stop(); // è proprio necessario fermarlo???
                        PWM_R_Stop(); // è proprio necessario fermarlo???*/
                       
                        compareValueTMP = average_TMP_code;
                       /* PWM_WritePeriod(MAX_Value_TMP);
                        PWM_WriteCompare(compareValueTMP);*/
                        
                        value = ((compareValueTMP-10430)*255)/810;      // tra 23°C e 29*C   
                        if(value>255) value=255;
                        if(value<0) value=0; 
                        
                        PWM_WriteCompare(value);
                        
                        if(compareValueTMP<10300){
                            Pin_LED_R_Write(LOW);
                            Pin_LED_B_Write(LOW);
                            Pin_LED_G_Write(LOW);
                        }
                        else{
                            Pin_LED_R_Write(HIGH);
                            Pin_LED_B_Write(HIGH);
                            Pin_LED_G_Write(HIGH);
                        }
                            

                        /*
                        PWM_B_Start();
                        PWM_G_Start();
                        PWM_R_Start();*/
                        
                    }
                    
                    // Reset counter and variables related to TMP sensor
                    sum_TMP = 0;
                    counter = 0;
                    average_TMP_code = 0;
                    average_TMP_mV = 0; 

                    // vedi commenti precedenti
                }
                
                PacketReadyFlag = 0;
                
            }    
        }
        
       else if (status == EZI2C_LDR_TMP_REG_1) {
           
           
           if (PacketReadyFlag == 1 ){
            
              sum_LDR += value_LDR_code; 
              sum_TMP += value_TMP_code;
                                
              counter++;
                
                if (counter == samples){
                    average_LDR_code = sum_LDR / samples; 
                    average_TMP_code = sum_TMP / samples; 
                    
                    // Map to mV
                    //average_LDR_mV = ADC_DeltaSigma_CountsTo_mVolts(average_LDR_code);
                   // average_LDR_lux = ConvertToLux(average_LDR_code, R_pull_down, Val_mV, intercept_LDR, gamma_LDR);
                    average_TMP_temp = ConvertToTMP(average_TMP_code, q_TMP, sensibility_TMP);
                    
                    // Save into I2C buffer          
                    // Right shift in order to memorize the Most Significant Byte (MSB) of the LDR and TMP sensors in the corresponding addresses 
                    slave_pointer_Buffer[MSB_LDR] = average_LDR_lux >> 8;    
                    slave_pointer_Buffer[MSB_TMP] = average_TMP_temp >> 8;
                    // Application of mask 0b11111111 (0xFF in hexadecimal notation) in order to memorize the Least Significant Byte (LSB)
                    // of the LDR and TMP sensors in the corresponding addresses 
                    slave_pointer_Buffer[LSB_LDR] = average_LDR_lux & 0xFF;
                    slave_pointer_Buffer[LSB_TMP] = average_TMP_temp & 0xFF;
                    
                    // Control of LED using PWM
                    if(LED_modality==TMP_readout){
                        
                        LED_CHECK_Write(LOW);
                        Pin_LED_R_Write(HIGH);
                        Pin_LED_B_Write(HIGH);
                        Pin_LED_G_Write(HIGH);
                        
                       /* PWM_B_Stop(); // è proprio necessario fermarlo???
                        PWM_G_Stop(); // è proprio necessario fermarlo???
                        PWM_R_Stop(); // è proprio necessario fermarlo???*/
                        compareValueTMP = average_TMP_code;
                        PWM_WritePeriod(MAX_Value_TMP);
                        PWM_WriteCompare(compareValueTMP);
                       

                        /*
                        PWM_B_Start();
                        PWM_G_Start();
                        PWM_R_Start();*/
                                
                    }
                    else if (LED_modality==LDR_readout){
                        /*PWM_B_Stop(); // è proprio necessario fermarlo???
                        PWM_G_Stop(); // è proprio necessario fermarlo???
                        PWM_R_Stop(); // è proprio necessario fermarlo???*/
                        
                        LED_CHECK_Write(LOW);
                        Pin_LED_R_Write(HIGH);
                        Pin_LED_B_Write(HIGH);
                        Pin_LED_G_Write(HIGH);
                        
                        compareValueLDR = average_LDR_code;
                        PWM_WritePeriod(MAX_Value_LDR);
                        PWM_WriteCompare(MAX_Value_LDR-compareValueLDR);

                        /*
                        PWM_B_Start();
                        PWM_G_Start();
                        PWM_R_Start();*/
 
                        }
                    
                    // Reset counter and variables related to TMP and LDR sensors
                    sum_LDR = 0;
                    sum_TMP = 0;
                    counter = 0;
                    average_LDR_code = 0;
                    average_TMP_code = 0;
                    average_LDR_mV = 0; 
                    average_TMP_mV = 0; 

                }
                
                PacketReadyFlag = 0;
                
            }  
        }  
    }
}
                
/* [] END OF FILE */