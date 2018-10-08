/*******************************************************************************
  MPLAB Harmony Application Source File

  Company:
    Microchip Technology Inc.

  File Name:
    app_usart_usb_click_board.c

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

#include "app_usart_usb_click_board.h"
#include <string.h>

// *****************************************************************************
// *****************************************************************************
// Section: Global Data Definitions
// *****************************************************************************
// *****************************************************************************
static char usart0_StartMessage[192] = "**** USART Driver Multi-Instance Echo Demo "
"Application ****\r\n**** Type 10 characters and observe it echo back using "
"DMA***\r\n**** LED toggles each time the data is echoed ***\r\n";

// *****************************************************************************
/* Application Data

  Summary:
    Holds application data

  Description:
    This structure holds the application's data.

  Remarks:
    This structure should be initialized by the APP_USART_USB_CLICK_BOARD_Initialize function.

    Application strings and buffers are be defined outside this structure.
*/

APP_USART_USB_CLICK_BOARD_DATA appUsartClickBoardData;

// *****************************************************************************
// *****************************************************************************
// Section: Application Callback Functions
// *****************************************************************************
// *****************************************************************************

/* TODO:  Add any necessary callback functions.
*/

// *****************************************************************************
// *****************************************************************************
// Section: Application Local Functions
// *****************************************************************************
// *****************************************************************************


/* TODO:  Add any necessary local functions.
*/


// *****************************************************************************
// *****************************************************************************
// Section: Application Initialization and State Machine Functions
// *****************************************************************************
// *****************************************************************************

/*******************************************************************************
  Function:
    void APP_USART_USB_CLICK_BOARD_Initialize ( void )

  Remarks:
    See prototype in app_usart_usb_click_board.h.
 */

void APP_USART_USB_CLICK_BOARD_Initialize ( void )
{
    /* Place the App state machine in its initial state. */
    appUsartClickBoardData.state = APP_USART_USB_CLICK_BOARD_STATE_INIT;



    /* TODO: Initialize your application's state machine and other
     * parameters.
     */
}


/******************************************************************************
  Function:
    void APP_USART_USB_CLICK_BOARD_Tasks ( void )

  Remarks:
    See prototype in app_usart_usb_click_board.h.
 */

void APP_USART_USB_CLICK_BOARD_Tasks ( void )
{
    /* Check the application's current state. */
    switch ( appUsartClickBoardData.state )
    {
        case APP_USART_USB_CLICK_BOARD_STATE_INIT:
            /* Open USART Driver Instance 1 (USART 1)*/
            appUsartClickBoardData.usartHandle = DRV_USART_Open(DRV_USART_INDEX_0, 0);

            if (appUsartClickBoardData.usartHandle == DRV_HANDLE_INVALID)
            {
                appUsartClickBoardData.state = APP_USART_USB_CLICK_BOARD_STATE_ERROR;
            }
            else
            {
                appUsartClickBoardData.state = APP_USART_USB_CLICK_BOARD_STATE_SEND_MESSAGE;
            }
            break;

        case APP_USART_USB_CLICK_BOARD_STATE_SEND_MESSAGE:
            if (DRV_USART_WriteBuffer( appUsartClickBoardData.usartHandle, usart0_StartMessage, strlen(usart0_StartMessage)) == true)
            {
                appUsartClickBoardData.state = APP_USART_USB_CLICK_BOARD_STATE_LOOPBACK;
            }
            else
            {
                appUsartClickBoardData.state = APP_USART_USB_CLICK_BOARD_STATE_ERROR;
            }
            break;

        case APP_USART_USB_CLICK_BOARD_STATE_LOOPBACK:
            /* Submit a read request and block until read completes */
            if (DRV_USART_ReadBuffer( appUsartClickBoardData.usartHandle, appUsartClickBoardData.receiveBuffer, APP_LOOPBACK_DATA_SIZE) == true)
            {

                /* Copy receive buffer to transmit buffer */
                memcpy(appUsartClickBoardData.transmitBuffer, appUsartClickBoardData.receiveBuffer, APP_LOOPBACK_DATA_SIZE);

            }
            else
            {
                appUsartClickBoardData.state = APP_USART_USB_CLICK_BOARD_STATE_ERROR;
                break;
            }

            /* Write the received data back and block until write completes */
            if (DRV_USART_WriteBuffer( appUsartClickBoardData.usartHandle, appUsartClickBoardData.transmitBuffer, APP_LOOPBACK_DATA_SIZE) == true)
            {
                /* Toggle LED to indicate success */
                LED1_Toggle();
            }
            else
            {
                appUsartClickBoardData.state = APP_USART_USB_CLICK_BOARD_STATE_ERROR;
                break;
            }
            break;

        case APP_USART_USB_CLICK_BOARD_STATE_ERROR:
        default:
            /* If error then suspend the task thereby allowing other threads to run */
            vTaskSuspend(NULL);
            break;
    }
}


/*******************************************************************************
 End of File
 */