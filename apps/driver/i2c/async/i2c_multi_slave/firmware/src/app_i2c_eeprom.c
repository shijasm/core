/*******************************************************************************
  MPLAB Harmony Application Source File

  Company:
    Microchip Technology Inc.

  File Name:
    app_i2c_eeprom.c

  Summary:
    This file contains the source code for the MPLAB Harmony application.

  Description:
    This file contains the source code for the MPLAB Harmony application.  It
    implements the logic of the application's state machine and it may call
    API routines of other MPLAB Harmony modules in the system, such as drivers,
    system services, and middleware.  However, it does not call any of the
    system interfaces (such as the "Initialize" and "Tasks" functions) of any of
    the modules in the system or make any assumptions about when those functions
    are called.  That is the responsibility of the configuration-specific system
    files.
 *******************************************************************************/

// DOM-IGNORE-BEGIN
/*******************************************************************************
* Copyright (C) 2018 Microchip Technology Inc. and its subsidiaries.
*
* Subject to your compliance with these terms, you may use Microchip software
* and any derivatives exclusively with Microchip products. It is your
* responsibility to comply with third party license terms applicable to your
* use of third party software (including open source software) that may
* accompany Microchip software.
*
* THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS". NO WARRANTIES, WHETHER
* EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE, INCLUDING ANY IMPLIED
* WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS FOR A
* PARTICULAR PURPOSE.
*
* IN NO EVENT WILL MICROCHIP BE LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE,
* INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND
* WHATSOEVER RELATED TO THE SOFTWARE, HOWEVER CAUSED, EVEN IF MICROCHIP HAS
* BEEN ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE FORESEEABLE. TO THE
* FULLEST EXTENT ALLOWED BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL CLAIMS IN
* ANY WAY RELATED TO THIS SOFTWARE WILL NOT EXCEED THE AMOUNT OF FEES, IF ANY,
* THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR THIS SOFTWARE.
*******************************************************************************/
// DOM-IGNORE-END

// *****************************************************************************
// *****************************************************************************
// Section: Included Files
// *****************************************************************************
// *****************************************************************************

#include "app_i2c_eeprom.h"
#include "app_i2c_temp_sensor.h"

// *****************************************************************************
// *****************************************************************************
// Section: Global Data Definitions
// *****************************************************************************
// *****************************************************************************
#define APP_EEPROM_I2C_CLOCK_SPEED                  400000
#define APP_EEPROM_SLAVE_ADDR                       0x0050
#define APP_EEPROM_START_MEMORY_ADDR                0x00

// *****************************************************************************
/* Application Data

  Summary:
    Holds application data

  Description:
    This structure holds the application's data.

  Remarks:
    This structure should be initialized by the APP_I2C_EEPROM_Initialize function.

    Application strings and buffers are be defined outside this structure.
*/

APP_EEPROM_DATA appEEPROMData;

// *****************************************************************************
// *****************************************************************************
// Section: Application Callback Functions
// *****************************************************************************
// *****************************************************************************
void APP_EEPROM_Notify(uint16_t temperature)
{
    appEEPROMData.temperature = temperature;
    appEEPROMData.isTemperatureReady = true;
}

/* TODO:  Add any necessary callback functions.
*/

// *****************************************************************************
// *****************************************************************************
// Section: Application Local Functions
// *****************************************************************************
// *****************************************************************************
static void appEEPROMEventHandler( 
    DRV_I2C_TRANSFER_EVENT event, 
    DRV_I2C_TRANSFER_HANDLE transferHandle, 
    uintptr_t context 
)
{
    appEEPROMData.transferStatus = event;
}

/* TODO:  Add any necessary local functions.
*/

// *****************************************************************************
// *****************************************************************************
// Section: Application Initialization and State Machine Functions
// *****************************************************************************
// *****************************************************************************

/*******************************************************************************
  Function:
    void APP_I2C_EEPROM_Initialize ( void )

  Remarks:
    See prototype in app_i2c_eeprom.h.
 */

void APP_I2C_EEPROM_Initialize ( void )
{
    /* Initialize the EEPROM Sensor Application data */
    appEEPROMData.state          = APP_EEPROM_STATE_INIT;
    appEEPROMData.i2cHandle      = DRV_HANDLE_INVALID;
    appEEPROMData.transferHandle = DRV_I2C_TRANSFER_HANDLE_INVALID;
}

/******************************************************************************
  Function:
    void APP_I2C_EEPROM_Tasks ( void )

  Remarks:
    See prototype in app_i2c_eeprom.h.
 */

void APP_I2C_EEPROM_Tasks ( void )
{
    uint8_t dummyData = 0;   
    
    switch (appEEPROMData.state)
    {
        case APP_EEPROM_STATE_INIT:
        {
            /* Open I2C driver client */
            appEEPROMData.i2cHandle = DRV_I2C_Open( DRV_I2C_INDEX_0, DRV_IO_INTENT_READWRITE);

            if(appEEPROMData.i2cHandle != DRV_HANDLE_INVALID)
            {
                /* Register the I2C Driver client event callback */
                DRV_I2C_TransferEventHandlerSet(appEEPROMData.i2cHandle, appEEPROMEventHandler, 0);
                appEEPROMData.isTemperatureReady = true;
                appEEPROMData.state = APP_EEPROM_STATE_WRITE;
            }
            else
            {
                appEEPROMData.state = APP_EEPROM_STATE_ERROR;
            }               
            break;
        }
        
        case APP_EEPROM_STATE_WRITE:
        {
            /* checks if temperature data is ready */
            if (true == appEEPROMData.isTemperatureReady)
            {
                appEEPROMData.isTemperatureReady = false;
                appEEPROMData.transferStatus = DRV_I2C_TRANSFER_EVENT_PENDING;

                appEEPROMData.txBuffer[0] = APP_EEPROM_START_MEMORY_ADDR;        
                appEEPROMData.txBuffer[1] = appEEPROMData.temperature;

                /* Write temperature data to EEPROM */
                DRV_I2C_WriteTransferAdd(appEEPROMData.i2cHandle, 
                                         APP_EEPROM_SLAVE_ADDR, 
                                         (void *)appEEPROMData.txBuffer, 
                                         2, 
                                         &appEEPROMData.transferHandle);
                
                appEEPROMData.state = APP_EEPROM_STATE_WAIT_TRANSFER_COMPLETE;                
            }
            break;
        }
        
        case APP_EEPROM_STATE_WAIT_TRANSFER_COMPLETE:
        {
            if (appEEPROMData.transferStatus == DRV_I2C_TRANSFER_EVENT_COMPLETE)
            {
                /* Add a dummy write to verify whether transfer is complete */
                appEEPROMData.transferStatus = DRV_I2C_TRANSFER_EVENT_PENDING;
                DRV_I2C_WriteTransferAdd(appEEPROMData.i2cHandle, 
                                         APP_EEPROM_SLAVE_ADDR, 
                                         (void *)&dummyData, 
                                         1, 
                                         &appEEPROMData.transferHandle );
                                
                appEEPROMData.state = APP_EEPROM_STATE_WAIT_WRITE_COMPLETE;                   
            }            
            break;
        }
        
        case APP_EEPROM_STATE_WAIT_WRITE_COMPLETE:
        {
            if (appEEPROMData.transferStatus == DRV_I2C_TRANSFER_EVENT_COMPLETE)
            {
                appEEPROMData.state = APP_EEPROM_STATE_READ;                
            }
            else if (appEEPROMData.transferStatus == DRV_I2C_TRANSFER_EVENT_ERROR)
            {
                /* Add a dummy write to verify whether write cycle is complete */
                appEEPROMData.transferStatus = DRV_I2C_TRANSFER_EVENT_PENDING;
                DRV_I2C_WriteTransferAdd(appEEPROMData.i2cHandle, 
                                         APP_EEPROM_SLAVE_ADDR, 
                                         (void *)&dummyData, 
                                         1, 
                                         &appEEPROMData.transferHandle );                
            }                
            break;
        }
        
        case APP_EEPROM_STATE_READ:
        {
            /* Add read transfer to read data from EEPROM */
            appEEPROMData.transferStatus = DRV_I2C_TRANSFER_EVENT_PENDING;
            DRV_I2C_WriteReadTransferAdd(appEEPROMData.i2cHandle, 
                                         APP_EEPROM_SLAVE_ADDR, 
                                         (void *)&dummyData, 
                                         1, 
                                         (void *)&appEEPROMData.rxBuffer,
                                         1,
                                         &appEEPROMData.transferHandle );
            
            appEEPROMData.state = APP_EEPROM_STATE_WAIT_READ_COMPLETE;
            break;
        }
        
        case APP_EEPROM_STATE_WAIT_READ_COMPLETE:
        {
            /* Print the results */
            if (appEEPROMData.transferStatus == DRV_I2C_TRANSFER_EVENT_COMPLETE)
            {
                printf("Temperature = %d C\r\n", appEEPROMData.rxBuffer);
                appEEPROMData.state = APP_EEPROM_STATE_WRITE;
            }
            else if (appEEPROMData.transferStatus == DRV_I2C_TRANSFER_EVENT_ERROR)
            {
                printf("Error!!! while reading EEPROM \r\n");
                appEEPROMData.state = APP_EEPROM_STATE_ERROR;
            }
        }
        
        case APP_EEPROM_STATE_ERROR:
        {
            break;
        }
    }
}

/*******************************************************************************
 End of File
 */