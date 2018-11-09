/*******************************************************************************
  System Initialization File

  File Name:
    initialization.c

  Summary:
    This file contains source code necessary to initialize the system.

  Description:
    This file contains source code necessary to initialize the system.  It
    implements the "SYS_Initialize" function, defines the configuration bits,
    and allocates any necessary global system resources,
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
#include "configuration.h"
#include "definitions.h"


// ****************************************************************************
// ****************************************************************************
// Section: Configuration Bits
// ****************************************************************************
// ****************************************************************************


// *****************************************************************************
// *****************************************************************************
// Section: Driver Initialization Data
// *****************************************************************************
// *****************************************************************************
// <editor-fold defaultstate="collapsed" desc="DRV_USART Instance 0 Initialization Data">

USART_PLIB_API drvUsart0PlibAPI = {
        .readCallbackRegister = (USART_ReadCallbackRegister)USART1_ReadCallbackRegister,
        .read = (USART_Read)USART1_Read,
        .readIsBusy = (USART_ReadIsBusy)USART1_ReadIsBusy,
        .readCountGet = (USART_ReadCountGet)USART1_ReadCountGet,
        .writeCallbackRegister = (USART_WriteCallbackRegister)USART1_WriteCallbackRegister,
        .write = (USART_Write)USART1_Write,
        .writeIsBusy = (USART_WriteIsBusy)USART1_WriteIsBusy,
        .writeCountGet = (USART_WriteCountGet)USART1_WriteCountGet,
        .errorGet = (USART_ErrorGet)USART1_ErrorGet,
        .serialSetup = (USART_SerialSetup)USART1_SerialSetup
};

uint32_t drvUsart0remapDataWidth[] = { 0x0, 0x40, 0x80, 0xC0, 0x20000 };
uint32_t drvUsart0remapParity[] = { 0x800, 0x200, 0x0, 0x600, 0x400, 0xC00 };
uint32_t drvUsart0remapStopBits[] = { 0x0, 0x1000, 0x2000 };
uint32_t drvUsart0remapError[] = { 0x20, 0x80, 0x40 };

DRV_USART_INIT drvUsart0InitData =
{
    .usartPlib = &drvUsart0PlibAPI,
    .remapDataWidth = drvUsart0remapDataWidth,
    .remapParity = drvUsart0remapParity,
    .remapStopBits = drvUsart0remapStopBits,
    .remapError = drvUsart0remapError,

    .dmaChannelTransmit = SYS_DMA_CHANNEL_NONE,

    .dmaChannelReceive = SYS_DMA_CHANNEL_NONE,

    .queueSizeTransmit = DRV_USART_XMIT_QUEUE_SIZE_IDX0,

    .queueSizeReceive = DRV_USART_RCV_QUEUE_SIZE_IDX0,

    .interruptUSART = USART1_IRQn,

    .interruptDMA = XDMAC_IRQn,

};

// </editor-fold>


// *****************************************************************************
// *****************************************************************************
// Section: System Data
// *****************************************************************************
// *****************************************************************************
/* Structure to hold the object handles for the modules in the system. */
SYSTEM_OBJECTS sysObj;
// *****************************************************************************
// *****************************************************************************
// Section: Library/Stack Initialization Data
// *****************************************************************************
// *****************************************************************************


// *****************************************************************************
// *****************************************************************************
// Section: System Initialization
// *****************************************************************************
// *****************************************************************************


/*******************************************************************************
  Function:
    void SYS_Initialize ( void *data )

  Summary:
    Initializes the board, services, drivers, application and other modules.

  Remarks:
 */

void SYS_Initialize ( void* data )
{
    CLK_Initialize();
	PIO_Initialize();

    NVIC_Initialize();
	RSWDT_REGS->RSWDT_MR = RSWDT_MR_WDDIS_Msk;	// Disable RSWDT 

	WDT_REGS->WDT_MR = WDT_MR_WDDIS_Msk; 		// Disable WDT 

	BSP_Initialize();
	USART1_Initialize();


    sysObj.drvUsart0 = DRV_USART_Initialize(DRV_USART_INDEX_0, (SYS_MODULE_INIT *)&drvUsart0InitData);




    APP_Initialize();


}


/*******************************************************************************
 End of File
*/
