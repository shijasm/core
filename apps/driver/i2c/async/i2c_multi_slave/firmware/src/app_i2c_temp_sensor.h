/*******************************************************************************
  MPLAB Harmony Application Header File

  Company:
    Microchip Technology Inc.

  File Name:
    app_i2c_temp_sensor.h

  Summary:
    This header file provides prototypes and definitions for the application.

  Description:
    This header file provides function prototypes and data type definitions for
    the application.  Some of these are required by the system (such as the
    "APP_I2C_TEMP_SENSOR_Initialize" and "APP_I2C_TEMP_SENSOR_Tasks" prototypes) and some of them are only used
    internally by the application (such as the "APP_I2C_TEMP_SENSOR_STATES" definition).  Both
    are defined here for convenience.
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

#ifndef _APP_I2C_TEMP_SENSOR_H
#define _APP_I2C_TEMP_SENSOR_H

// *****************************************************************************
// *****************************************************************************
// Section: Included Files
// *****************************************************************************
// *****************************************************************************

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>
#include "configuration.h"
#include "driver/i2c/drv_i2c.h"

// DOM-IGNORE-BEGIN
#ifdef __cplusplus  // Provide C++ Compatibility

extern "C" {

#endif
// DOM-IGNORE-END

// *****************************************************************************
// *****************************************************************************
// Section: Type Definitions
// *****************************************************************************
// *****************************************************************************

// *****************************************************************************
// *****************************************************************************
/* Temperature Sensor Application states

  Summary:
    Temperature Sensor Application states enumeration

  Description:
    This enumeration defines the valid temperature sensor application states.  
    These states determine the behavior of the application at various times.
*/

typedef enum
{
    /* Initial state. */
    APP_TEMP_STATE_INIT,
            
    /* Read Temperature state */
    APP_TEMP_STATE_READ_TEMPERATURE,
            
    /* Wait for read transfer to complete state */
    APP_TEMP_STATE_WAIT_TRANSFER_COMPLETE,
    
    /* Error state */
    APP_TEMP_STATE_ERROR,

} APP_TEMP_STATES;

// *****************************************************************************
/* Temperature Sensor Application Data

  Summary:
    Holds temperature sensor application data

  Description:
    This structure holds the temperature sensor application's data.

  Remarks:
    Application strings and buffers are be defined outside this structure.
 */

typedef struct
{
    /* Application's current state */
    APP_TEMP_STATES  state;
     
    /* I2C driver client handle */
    DRV_HANDLE i2cHandle;
     
    /* I2C driver transfer handle */
    DRV_I2C_TRANSFER_HANDLE transferHandle;
     
    /* buffer to hold temperature queried from sensor */
    uint8_t rxBuffer[2];
     
    /* flag to check whether read transfer is done */
    volatile bool isTransferDone;
     
    /* temperature value in degree celcius */
    uint8_t temperature;
     
    /* flag to indicate timer expiry*/
    volatile bool tmrExpired;

    uint8_t registerAddr;
} APP_TEMP_DATA;

// *****************************************************************************
// *****************************************************************************
// Section: Application Callback Routines
// *****************************************************************************
// *****************************************************************************
/* These routines are called by drivers when certain events occur.
*/

// *****************************************************************************
// *****************************************************************************
// Section: Application Initialization and State Machine Functions
// *****************************************************************************
// *****************************************************************************
/*******************************************************************************
  Function:
    void APP_I2C_TEMP_SENSOR_Initialize ( void )

  Summary:
     MPLAB Harmony application initialization routine.

  Description:
    This function initializes the Harmony application.  It places the
    application in its initial state and prepares it to run so that its
    APP_Tasks function can be called.

  Precondition:
    All other system initialization routines should be called before calling
    this routine (in "SYS_Initialize").

  Parameters:
    None.

  Returns:
    None.

  Example:
    <code>
    APP_I2C_TEMP_SENSOR_Initialize();
    </code>

  Remarks:
    This routine must be called from the SYS_Initialize function.
*/

void APP_I2C_TEMP_SENSOR_Initialize ( void );

/*******************************************************************************
  Function:
    void APP_I2C_TEMP_SENSOR_Tasks ( void )

  Summary:
    MPLAB Harmony Demo application tasks function

  Description:
    This routine is the Harmony Demo application's tasks function.  It
    defines the application's state machine and core logic.

  Precondition:
    The system and application initialization ("SYS_Initialize") should be
    called before calling this.

  Parameters:
    None.

  Returns:
    None.

  Example:
    <code>
    APP_I2C_TEMP_SENSOR_Tasks();
    </code>

  Remarks:
    This routine must be called from SYS_Tasks() routine.
 */

void APP_I2C_TEMP_SENSOR_Tasks( void );

void APP_EEPROM_Notify(uint8_t temperature);



#endif /* _APP_I2C_TEMP_SENSOR_H */

//DOM-IGNORE-BEGIN
#ifdef __cplusplus
}
#endif
//DOM-IGNORE-END

/*******************************************************************************
 End of File
 */
