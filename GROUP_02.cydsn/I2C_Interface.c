/*
* This file includes all the required source code to interface
* the I2C peripheral.
*
* This file needs to be completed.
*/

/**
*   \brief Value returned if device present on I2C bus.
*/
#ifndef DEVICE_CONNECTED
    #define DEVICE_CONNECTED 1
#endif

/**
*   \brief Value returned if device not present on I2C bus.
*/
#ifndef DEVICE_UNCONNECTED
    #define DEVICE_UNCONNECTED 0
#endif

#include "I2C_Interface.h" 
#include "I2C_Master.h"

ErrorCode I2C_Peripheral_Start(void) 
{
    // Start I2C peripheral
    I2C_Master_Start();  
    
    // Return no error since start function does not return any error
    return NO_ERROR;
}

// --------------------------------------------------------------------------------- //

ErrorCode I2C_Peripheral_Stop(void)
{
    // Stop I2C peripheral
    I2C_Master_Stop();
    // Return no error since stop function does not return any error
    return NO_ERROR;
}

// --------------------------------------------------------------------------------- //

ErrorCode I2C_Peripheral_ReadRegister(uint8_t device_address, 
                                      uint8_t register_address,
                                      uint8_t* data) {
    
    // Start Condition
    uint8_t error = I2C_Master_MasterSendStart(device_address, I2C_Master_WRITE_XFER_MODE);                                        
    if (error == I2C_Master_MSTR_NO_ERROR)
    {
        //Write the register address to be read
        error = I2C_Master_MasterWriteByte(register_address);
        if (error == I2C_Master_MSTR_NO_ERROR)
        {
            // Send a restart condition
            error = I2C_Master_MasterSendRestart(device_address, I2C_Master_READ_XFER_MODE);
            if (error == I2C_Master_MSTR_NO_ERROR)
            {
                *data = I2C_Master_MasterReadByte(I2C_Master_NAK_DATA);
            }
        }
    }
    // Stop Comunication
    I2C_Master_MasterSendStop();
    //Return
    return error ? ERROR : NO_ERROR;
    
}
                                    
// --------------------------------------------------------------------------------- //

ErrorCode I2C_Peripheral_ReadRegisterMulti(uint8_t device_address,
                                           uint8_t register_address,
                                           uint8_t register_count,
                                           uint8_t* data) {
    
    // Send start condition
    uint8_t error = I2C_Master_MasterSendStart(device_address, I2C_Master_WRITE_XFER_MODE);
    if (error == I2C_Master_MSTR_NO_ERROR)
    {
        // Write register address to be read with the MSB = 1
        register_address |= 0x80; //Datasheet indication for multi read -- autoincrement
        error = I2C_Master_MasterWriteByte(register_address);
        if (error == I2C_Master_MSTR_NO_ERROR)
        {
            // Send Restart condition
            error = I2C_Master_MasterSendRestart(device_address, I2C_Master_READ_XFER_MODE);
            if (error == I2C_Master_MSTR_NO_ERROR)
            {
                // Continue to read until we have register to be read
                uint8_t counter = register_count;
                while (counter > 1)
                {
                    data[register_count - counter] = 
                        I2C_Master_MasterReadByte(I2C_Master_ACK_DATA);
                        counter --;
                }
                // Read last data without acknolowledgment
                data[register_count - 1] = 
                    I2C_Master_MasterReadByte(I2C_Master_NAK_DATA);
            }
        }
    }
    // Send Stop Condition
    I2C_Master_MasterSendStop();
    // Return Error Code
    return error ? ERROR : NO_ERROR;
                                              
}
                                        
// --------------------------------------------------------------------------------- //

ErrorCode I2C_Peripheral_WriteRegister(uint8_t device_address,
                                       uint8_t register_address,
                                       uint8_t data) {
    
    
    // Send Start Condition
    uint8_t error = I2C_Master_MasterSendStart(device_address, I2C_Master_WRITE_XFER_MODE);
    if (error == I2C_Master_MSTR_NO_ERROR)
    {
        //Write Register Address (to be overwritten)
        error = I2C_Master_MasterWriteByte(register_address);
        if (error == I2C_Master_MSTR_NO_ERROR)
        {
            // Write Byte
            error = I2C_Master_MasterWriteByte(data);
        }
    }
    // Close Communication
    I2C_Master_MasterSendStop();
    // Return
    return error ? ERROR : NO_ERROR;
}
                                    

                                            
// --------------------------------------------------------------------------------- //

uint8_t I2C_Peripheral_IsDeviceConnected(uint8_t device_address) {
    
    // Send a start condition followed by a stop condition
    uint8_t error = I2C_Master_MasterSendStart(device_address, I2C_Master_WRITE_XFER_MODE);
    I2C_Master_MasterSendStop();
    // If no error generated during stop, device is connected
    return (error == I2C_Master_MSTR_NO_ERROR ? DEVICE_CONNECTED : DEVICE_UNCONNECTED);
    
}

/* [] END OF FILE */
