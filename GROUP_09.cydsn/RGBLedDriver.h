/* ========================================
 *
 * \ authors : Giorgio Allegri - Luca Pavirani
 * 
 * \ Header file for RGB Led Driver
 *
 *
 * ========================================
*/

#ifndef __RGB_LED_DRIVER_H__
    #define __RGB_LED_DRIVER_H__
    
    #include "project.h"
    
    //  Brief Struct holding color data 
    typedef struct {
        uint8_t red;    //RED (0 - 255)
        uint8_t green;  //GREEN (0 - 255)
        uint8_t blue;   //BLUE (0 - 255)
    } Color;
    
    // Brief Start driver
    void RGBLed_Start(void);
    
    // Stop 
    void RGBLed_Stop(void);       
    
    // Brief Write new color (c) 
    void RGBLed_WriteColor(Color c);
   
#endif

/* [] END OF FILE */
