/* ========================================
 *
 * Authors: Alfonzo Massimo, Mazzucchelli Umberto
 * Date: 27/04/2022
 * 
 * Source file for interrupt routines
 *
 * ========================================
*/

/*Include necessary headers*/
#include "InterruptRoutines.h"
#include "project.h"
#include "utils.h"

// Variables declaration
int32 value_digit;
int16 TMP_sum; 
int16 LDR_sum;
int16 TMP_avg;
int16 LDR_avg;

int32 LDR_avg_digit;
int32 TMP_avg_digit;
int32 TMP_sum_digit; 
int32 LDR_sum_digit;

int channel;
int count= 0;

CY_ISR(Custom_ISR_ADC)
{
    Timer_ReadStatusRegister();
    
    if(channel == 0 && dataReady==0)// Reads LDR sensor
    {
        AMux_FastSelect(channel);   // Select the LDR sensor channel
        ADC_StartConvert();         // Starts the ADC conversion 
        value_digit=ADC_Read32();   // Stores the value
        
        // Setting limits for the valid range
        if(value_digit<0) value_digit=0;
        if(value_digit>65535) value_digit=65535;
        
        LDR_sum_digit += value_digit;   // Summing the values for the average computation in digits
        LDR_sum += ADC_CountsTo_mVolts(value_digit);    //Summing the values for lux computation
        ADC_StopConvert();
        count++;        // Increases the count
        channel = 1;    // Sets the channel to the TMP sensor channel  
    }
    else if(channel == 1 && dataReady==0)// Reads TMP sensor
    {
        AMux_FastSelect(channel);   // Select the TMP sensor channel
        ADC_StartConvert();         // Starts the ADC conversion 
        value_digit=ADC_Read32();   // Stores the value
        
        // Setting limits for the valid range
        if(value_digit<0) value_digit=0;
        if(value_digit>65535) value_digit=65535;
        
        TMP_sum_digit += value_digit;  // Summing the values for the average computation in digits
        TMP_sum += ADC_CountsTo_mVolts(value_digit);    //Summing the values for celsius degree computation 
        ADC_StopConvert();
        count++;        // Increases the count
        channel = 0;    // Sets the channel to the LDR sensor channel  
    }
    
    if(count==(nSamples*2)) //If we have reached the number of samples chosen by the user
    {
        // Calculation of the averages
        TMP_avg_digit = TMP_sum_digit/nSamples;
        LDR_avg_digit = LDR_sum_digit/nSamples;
        TMP_avg = TMP_sum/nSamples;
        LDR_avg = LDR_sum/nSamples;
        dataReady=1;    // Sets the data flag for the ADC to 1, so the data is ready to be sent
    }
}
/* [] END OF FILE */
