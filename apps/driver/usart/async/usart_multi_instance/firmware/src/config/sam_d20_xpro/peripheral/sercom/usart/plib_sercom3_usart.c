/*******************************************************************************
  SERCOM Universal Synchronous/Asynchrnous Receiver/Transmitter PLIB

  Company
    Microchip Technology Inc.

  File Name
    plib_sercom3_usart.c

  Summary
    USART peripheral library interface.

  Description
    This file defines the interface to the USART peripheral library. This
    library provides access to and control of the associated peripheral
    instance.

  Remarks:
    None.
*******************************************************************************/

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

// *****************************************************************************
// *****************************************************************************
// Section: Included Files
// *****************************************************************************
// *****************************************************************************

#include "plib_sercom3_usart.h"

// *****************************************************************************
// *****************************************************************************
// Section: Global Data
// *****************************************************************************
// *****************************************************************************

/* SERCOM3 USART baud value for 115200 Hz baud rate */
#define SERCOM3_USART_INT_BAUD_VALUE			(63017U)

SERCOM_USART_OBJECT sercom3USARTObj;

// *****************************************************************************
// *****************************************************************************
// Section: SERCOM3 USART Interface Routines
// *****************************************************************************
// *****************************************************************************

void SERCOM3_USART_Initialize( void )
{
    /*
     * Configures USART Clock Mode
     * Configures TXPO and RXPO
     * Configures Data Order
     * Configures Standby Mode
     * Configures sampling rate
     * Configures IBON
     * Configures Parity
     * Configures Stop bits
     */
    SERCOM3_REGS->USART_INT.SERCOM_CTRLA = SERCOM_USART_INT_CTRLA_MODE_USART_INT_CLK | SERCOM_USART_INT_CTRLA_RXPO_PAD3 | SERCOM_USART_INT_CTRLA_TXPO_PAD2 | SERCOM_USART_INT_CTRLA_DORD_Msk | SERCOM_USART_INT_CTRLA_IBON_Msk | SERCOM_USART_INT_CTRLA_FORM(0x0) ;

    /* Configure Baud Rate */
    SERCOM3_REGS->USART_INT.SERCOM_BAUD = SERCOM_USART_INT_BAUD_BAUD(SERCOM3_USART_INT_BAUD_VALUE);

    /*
     * Configures RXEN
     * Configures TXEN
     * Configures CHSIZE
     * Configures Parity
     * Configures Stop bits
     */
    SERCOM3_REGS->USART_INT.SERCOM_CTRLB = SERCOM_USART_INT_CTRLB_CHSIZE_8_BIT | SERCOM_USART_INT_CTRLB_SBMODE_1_BIT | SERCOM_USART_INT_CTRLB_RXEN_Msk | SERCOM_USART_INT_CTRLB_TXEN_Msk;

    /* Wait for sync */
    while((SERCOM3_REGS->USART_INT.SERCOM_STATUS & SERCOM_USART_INT_STATUS_SYNCBUSY_Msk) & SERCOM_USART_INT_STATUS_SYNCBUSY_Msk);

    /* Enable the UART after the configurations */
    SERCOM3_REGS->USART_INT.SERCOM_CTRLA |= SERCOM_USART_INT_CTRLA_ENABLE_Msk;

    /* Wait for sync */
    while((SERCOM3_REGS->USART_INT.SERCOM_STATUS & SERCOM_USART_INT_STATUS_SYNCBUSY_Msk) & SERCOM_USART_INT_STATUS_SYNCBUSY_Msk);

    /* Initialize instance object */
    sercom3USARTObj.rxBuffer = NULL;
    sercom3USARTObj.rxSize = 0;
    sercom3USARTObj.rxProcessedSize = 0;
    sercom3USARTObj.rxBusyStatus = false;
    sercom3USARTObj.rxCallback = NULL;
    sercom3USARTObj.txBuffer = NULL;
    sercom3USARTObj.txSize = 0;
    sercom3USARTObj.txProcessedSize = 0;
    sercom3USARTObj.txBusyStatus = false;
    sercom3USARTObj.txCallback = NULL;
}

uint32_t SERCOM3_USART_FrequencyGet( void )
{
    return (uint32_t) (47972352UL);
}

bool SERCOM3_USART_SerialSetup( USART_SERIAL_SETUP * serialSetup, uint32_t clkFrequency )
{
    bool setupStatus       = false;
    uint32_t baudValue     = 0;

    if((sercom3USARTObj.rxBusyStatus == true) || (sercom3USARTObj.txBusyStatus == true))
    {
        /* Transaction is in progress, so return without updating settings */
        return setupStatus;
    }

    if((serialSetup != NULL) & (serialSetup->baudRate != 0))
    {
        if(clkFrequency == 0)
        {
            clkFrequency = SERCOM3_USART_FrequencyGet();
        }

        if(clkFrequency >= (16 * serialSetup->baudRate))
        {
            baudValue = 65536 - ((uint64_t)65536 * 16 * serialSetup->baudRate) / clkFrequency;
        }

        if(baudValue != 0)
        {
            /* Disable the USART before configurations */
            SERCOM3_REGS->USART_INT.SERCOM_CTRLA &= ~SERCOM_USART_INT_CTRLA_ENABLE_Msk;

            /* Wait for sync */
            while((SERCOM3_REGS->USART_INT.SERCOM_STATUS & SERCOM_USART_INT_STATUS_SYNCBUSY_Msk) & SERCOM_USART_INT_STATUS_SYNCBUSY_Msk);

            /* Configure Baud Rate */
            SERCOM3_REGS->USART_INT.SERCOM_BAUD = SERCOM_USART_INT_BAUD_BAUD(baudValue);

            /* Configure Parity Options */
            if(serialSetup->parity == USART_PARITY_NONE)
            {
                SERCOM3_REGS->USART_INT.SERCOM_CTRLA |= SERCOM_USART_INT_CTRLA_FORM(0x0) ;

                SERCOM3_REGS->USART_INT.SERCOM_CTRLB |= serialSetup->dataWidth | serialSetup->stopBits;
            }
            else
            {
                SERCOM3_REGS->USART_INT.SERCOM_CTRLA |= SERCOM_USART_INT_CTRLA_FORM(0x1) ;

                SERCOM3_REGS->USART_INT.SERCOM_CTRLB |= serialSetup->dataWidth | serialSetup->parity | serialSetup->stopBits;
            }

            /* Wait for sync */
            while((SERCOM3_REGS->USART_INT.SERCOM_STATUS & SERCOM_USART_INT_STATUS_SYNCBUSY_Msk) & SERCOM_USART_INT_STATUS_SYNCBUSY_Msk);

            /* Enable the USART after the configurations */
            SERCOM3_REGS->USART_INT.SERCOM_CTRLA |= SERCOM_USART_INT_CTRLA_ENABLE_Msk;

            /* Wait for sync */
            while((SERCOM3_REGS->USART_INT.SERCOM_STATUS & SERCOM_USART_INT_STATUS_SYNCBUSY_Msk) & SERCOM_USART_INT_STATUS_SYNCBUSY_Msk);

            setupStatus = true;
        }
    }

    return setupStatus;
}

bool SERCOM3_USART_Write( void *buffer, const size_t size )
{
    bool writeStatus      = false;
    uint8_t *pu8Data      = (uint8_t*)buffer;

    if(pu8Data != NULL)
    {
        if(sercom3USARTObj.txBusyStatus == false)
        {
            sercom3USARTObj.txBuffer = pu8Data;
            sercom3USARTObj.txSize = size;
            sercom3USARTObj.txProcessedSize = 0;
            sercom3USARTObj.txBusyStatus = true;

            if(size == 0)
            {
                writeStatus = true;
            }
            else
            {
                /* Initiate the transfer by sending first byte */
                if((SERCOM3_REGS->USART_INT.SERCOM_INTFLAG & SERCOM_USART_INT_INTFLAG_DRE_Msk) == SERCOM_USART_INT_INTFLAG_DRE_Msk)
                {
                    SERCOM3_REGS->USART_INT.SERCOM_DATA = sercom3USARTObj.txBuffer[sercom3USARTObj.txProcessedSize++];
                }

                SERCOM3_REGS->USART_INT.SERCOM_INTENSET = SERCOM_USART_INT_INTFLAG_DRE_Msk;

                writeStatus = true;
            }
        }
    }

    return writeStatus;
}

bool SERCOM3_USART_WriteIsBusy( void )
{
    return sercom3USARTObj.txBusyStatus;
}

size_t SERCOM3_USART_WriteCountGet( void )
{
    return sercom3USARTObj.txProcessedSize;
}

void SERCOM3_USART_WriteCallbackRegister( SERCOM_USART_CALLBACK callback, uintptr_t context )
{
    sercom3USARTObj.txCallback = callback;

    sercom3USARTObj.txContext = context;
}

bool SERCOM3_USART_Read( void *buffer, const size_t size )
{
    bool readStatus        = false;
    uint8_t *pu8Data       = (uint8_t*)buffer;
    uint8_t u8dummyData    = 0;

    if(pu8Data != NULL)
    {
        if(sercom3USARTObj.rxBusyStatus == false)
        {
            /* Checks for error before receiving */
            if(SERCOM3_USART_ErrorGet() != USART_ERROR_NONE)
            {
                /* Clear error statuses */
                SERCOM3_REGS->USART_INT.SERCOM_STATUS = SERCOM_USART_INT_STATUS_Msk;

                /* Flush existing error bytes from the RX FIFO */
                while((SERCOM3_REGS->USART_INT.SERCOM_INTFLAG & SERCOM_USART_INT_INTFLAG_RXC_Msk) == SERCOM_USART_INT_INTFLAG_RXC_Msk)
                {
                    u8dummyData = SERCOM3_REGS->USART_INT.SERCOM_DATA;
                }

                /* Ignore the warning */
                (void)u8dummyData;
            }

            sercom3USARTObj.rxBuffer = pu8Data;
            sercom3USARTObj.rxSize = size;
            sercom3USARTObj.rxProcessedSize = 0;
            sercom3USARTObj.rxBusyStatus = true;
            readStatus = true;

            /* Enable Receive Complete interrupt */
            SERCOM3_REGS->USART_INT.SERCOM_INTENSET = SERCOM_USART_INT_INTENSET_RXC_Msk;
        }
    }

    return readStatus;
}

bool SERCOM3_USART_ReadIsBusy( void )
{
    return sercom3USARTObj.rxBusyStatus;
}

size_t SERCOM3_USART_ReadCountGet( void )
{
    return sercom3USARTObj.rxProcessedSize;
}

void SERCOM3_USART_ReadCallbackRegister( SERCOM_USART_CALLBACK callback, uintptr_t context )
{
    sercom3USARTObj.rxCallback = callback;

    sercom3USARTObj.rxContext = context;
}

USART_ERROR SERCOM3_USART_ErrorGet( void )
{
    USART_ERROR errorStatus = USART_ERROR_NONE;

    errorStatus = SERCOM3_REGS->USART_INT.SERCOM_STATUS & (SERCOM_USART_INT_STATUS_PERR_Msk | SERCOM_USART_INT_STATUS_FERR_Msk | SERCOM_USART_INT_STATUS_BUFOVF_Msk);

    /* Clear Errors */
    SERCOM3_REGS->USART_INT.SERCOM_STATUS = SERCOM_USART_INT_STATUS_PERR_Msk | SERCOM_USART_INT_STATUS_FERR_Msk | SERCOM_USART_INT_STATUS_BUFOVF_Msk;

    return errorStatus;
}

void static SERCOM3_USART_ISR_RX_Handler( void )
{
    if(sercom3USARTObj.rxBusyStatus == true)
    {
        if(sercom3USARTObj.rxProcessedSize < sercom3USARTObj.rxSize)
        {
            sercom3USARTObj.rxBuffer[sercom3USARTObj.rxProcessedSize++] = SERCOM3_REGS->USART_INT.SERCOM_DATA;

            if(sercom3USARTObj.rxProcessedSize == sercom3USARTObj.rxSize)
            {
                sercom3USARTObj.rxBusyStatus = false;
                sercom3USARTObj.rxSize = 0;
                SERCOM3_REGS->USART_INT.SERCOM_INTENCLR = SERCOM_USART_INT_INTENCLR_RXC_Msk;

                if(sercom3USARTObj.rxCallback != NULL)
                {
                    sercom3USARTObj.rxCallback(sercom3USARTObj.rxContext);
                }
            }
        }
    }
}

void static SERCOM3_USART_ISR_TX_Handler( void )
{
    if(sercom3USARTObj.txBusyStatus == true)
    {
        if(sercom3USARTObj.txProcessedSize < sercom3USARTObj.txSize)
        {
            SERCOM3_REGS->USART_INT.SERCOM_DATA = sercom3USARTObj.txBuffer[sercom3USARTObj.txProcessedSize++];
        }

        if(sercom3USARTObj.txProcessedSize >= sercom3USARTObj.txSize)
        {
            sercom3USARTObj.txBusyStatus = false;
            sercom3USARTObj.txSize = 0;
            SERCOM3_REGS->USART_INT.SERCOM_INTENCLR = SERCOM_USART_INT_INTENCLR_DRE_Msk;

            if(sercom3USARTObj.txCallback != NULL)
            {
                sercom3USARTObj.txCallback(sercom3USARTObj.txContext);
            }
        }
    }
}

void SERCOM3_USART_InterruptHandler( void )
{
    if(SERCOM3_REGS->USART_INT.SERCOM_INTENSET != 0)
    {
        /* Checks for data register empty flag */
        if((SERCOM3_REGS->USART_INT.SERCOM_INTFLAG & SERCOM_USART_INT_INTFLAG_DRE_Msk) == SERCOM_USART_INT_INTFLAG_DRE_Msk)
        {
            SERCOM3_USART_ISR_TX_Handler();
        }

        /* Checks for receive complete empty flag */
        if((SERCOM3_REGS->USART_INT.SERCOM_INTFLAG & SERCOM_USART_INT_INTFLAG_RXC_Msk) == SERCOM_USART_INT_INTFLAG_RXC_Msk)
        {
            SERCOM3_USART_ISR_RX_Handler();
        }
    }
}
