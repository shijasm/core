/*******************************************************************************
  USART Driver Implementation.

  Company:
    Microchip Technology Inc.

  File Name:
    drv_usart.c

  Summary:
    Source code for the USART driver dynamic implementation.

  Description:
    This file contains the source code for the dynamic implementation of the
    USART driver.
*******************************************************************************/

//DOM-IGNORE-BEGIN
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
//DOM-IGNORE-END

// *****************************************************************************
// *****************************************************************************
// Section: Included Files
// *****************************************************************************
// *****************************************************************************

#include "configuration.h"
#include "driver/usart/drv_usart.h"
#include "drv_usart_local.h"

//SYS_DEBUG is not available yet, hence commented for now.
//#include "system/debug/sys_debug.h"

// *****************************************************************************
// *****************************************************************************
// Section: Global Data
// *****************************************************************************
// *****************************************************************************

/* This is the driver instance object array. */
static DRV_USART_OBJ gDrvUSARTObj[DRV_USART_INSTANCES_NUMBER] ;

// *****************************************************************************
// *****************************************************************************
// Section: File scope functions
// *****************************************************************************
// *****************************************************************************

static inline uint32_t  _DRV_USART_MAKE_HANDLE(uint16_t token, uint8_t drvIndex, uint8_t clientIndex)
{
    return ((token << 16) | (drvIndex << 8) | clientIndex);
}

static inline uint16_t _DRV_USART_UPDATE_TOKEN(uint16_t token)
{
    token++;

    if (token >= DRV_USART_TOKEN_MAX)
    {
        token = 1;
    }

    return token;
}

static void _DRV_USART_TX_PLIB_CallbackHandler( uintptr_t context )
{
    DRV_USART_OBJ *dObj = (DRV_USART_OBJ *)context;

    dObj->txRequestStatus = DRV_USART_REQUEST_STATUS_COMPLETE;

    OSAL_SEM_PostISR(&dObj->txTransferDone);
}

static DRV_USART_ERROR _DRV_USART_GetErrorType(const uint32_t* remapError, uint32_t errorMask)
{
    DRV_USART_ERROR error = DRV_USART_ERROR_NONE;

    for (uint32_t i = 0; i < 3; i++)
    {
        if (remapError[i] == errorMask)
        {
            error = (DRV_USART_ERROR)(i+1);
            break;
        }
    }
    return error;
}

static void _DRV_USART_RX_PLIB_CallbackHandler( uintptr_t context )
{
    DRV_USART_OBJ *dObj = (DRV_USART_OBJ *)context;
    DRV_USART_CLIENT_OBJ* clientObj = (DRV_USART_CLIENT_OBJ*)dObj->currentRxClient;
    uint32_t errorMask;

    errorMask = dObj->usartPlib->errorGet();

    if(errorMask == (uint32_t) DRV_USART_ERROR_NONE)
    {
        clientObj->errors = DRV_USART_ERROR_NONE;
        dObj->rxRequestStatus = DRV_USART_REQUEST_STATUS_COMPLETE;
    }
    else
    {
        clientObj->errors = _DRV_USART_GetErrorType(dObj->remapError, errorMask);
        dObj->rxRequestStatus = DRV_USART_REQUEST_STATUS_ERROR;
    }

    OSAL_SEM_PostISR(&dObj->rxTransferDone);
}

<#if core.DMA_ENABLE?has_content>
static void _DRV_USART_TX_DMA_CallbackHandler(SYS_DMA_TRANSFER_EVENT event, uintptr_t context)
{
    DRV_USART_OBJ *dObj = (DRV_USART_OBJ *)context;

    if(event == SYS_DMA_TRANSFER_COMPLETE)
    {
        dObj->txRequestStatus = DRV_USART_REQUEST_STATUS_COMPLETE;
    }
    else if(event == SYS_DMA_TRANSFER_ERROR)
    {
        dObj->txRequestStatus = DRV_USART_REQUEST_STATUS_ERROR;
    }

    OSAL_SEM_PostISR(&dObj->txTransferDone);
}

static void _DRV_USART_RX_DMA_CallbackHandler(SYS_DMA_TRANSFER_EVENT event, uintptr_t context)
{
    DRV_USART_OBJ *dObj = (DRV_USART_OBJ *)context;

    if(event == SYS_DMA_TRANSFER_COMPLETE)
    {
        dObj->rxRequestStatus = DRV_USART_REQUEST_STATUS_COMPLETE;
    }
    else if(event == SYS_DMA_TRANSFER_ERROR)
    {
        dObj->rxRequestStatus = DRV_USART_REQUEST_STATUS_ERROR;
    }

    OSAL_SEM_PostISR(&dObj->rxTransferDone);
}

</#if>
static DRV_USART_CLIENT_OBJ* _DRV_USART_DriverHandleValidate(DRV_HANDLE handle)
{
    /* This function returns the pointer to the client object that is
       associated with this handle if the handle is valid. Returns NULL
       otherwise.
    */
    uint32_t drvInstance = 0;
    DRV_USART_CLIENT_OBJ* clientObj = NULL;

    if((handle != DRV_HANDLE_INVALID) && (handle != 0))
    {
        /* Extract the instance value from the handle */
        drvInstance = ((handle & DRV_USART_INSTANCE_INDEX_MASK) >> 8);
        if (drvInstance >= DRV_USART_INSTANCES_NUMBER)
        {
            return (NULL);
        }
        if ((handle & DRV_USART_CLIENT_INDEX_MASK) >= gDrvUSARTObj[drvInstance].nClientsMax)
        {
            return (NULL);
        }

        clientObj = &((DRV_USART_CLIENT_OBJ *)gDrvUSARTObj[drvInstance].clientObjPool)[handle & DRV_USART_CLIENT_INDEX_MASK];

        if ((handle != clientObj->clientHandle) || (clientObj->inUse == false))
        {
            return (NULL);
        }
    }

    return(clientObj);
}

// *****************************************************************************
// *****************************************************************************
// Section: USART Driver Common Interface Implementation
// *****************************************************************************
// *****************************************************************************

// *****************************************************************************
/* Function:
    SYS_MODULE_OBJ DRV_USART_Initialize
    (
        const SYS_MODULE_INDEX drvIndex,
        const SYS_MODULE_INIT * const init
    )

  Summary:
    Initializes the USART instance for the specified driver index.

  Description:
    This routine initializes the USART driver instance for the specified driver
    index, making it ready for clients to open and use it. The initialization
    data is specified by the init parameter.

  Remarks:
    See drv_usart.h for usage information.
*/

SYS_MODULE_OBJ DRV_USART_Initialize( const SYS_MODULE_INDEX drvIndex, const SYS_MODULE_INIT * const init )
{
    DRV_USART_OBJ *dObj = NULL;
    DRV_USART_INIT *usartInit = (DRV_USART_INIT *)init ;

    /* Validate the request */
    if(drvIndex >= DRV_USART_INSTANCES_NUMBER)
    {
        //SYS_DEBUG(SYS_ERROR_ERROR, "Invalid driver instance");
        return SYS_MODULE_OBJ_INVALID;
    }

    if(gDrvUSARTObj[drvIndex].inUse != false)
    {
        //SYS_DEBUG(SYS_ERROR_ERROR, "Instance already in use");
        return SYS_MODULE_OBJ_INVALID;
    }

    /* Allocate the driver object */
    dObj                        = &gDrvUSARTObj[drvIndex];
    dObj->inUse                 = true;
    dObj->usartPlib             = usartInit->usartPlib;
    dObj->clientObjPool         = usartInit->clientObjPool;
    dObj->nClientsMax           = usartInit->numClients;
    dObj->nClients              = 0;
    dObj->currentRxClient       = (uintptr_t)NULL;
    dObj->currentTxClient       = (uintptr_t)NULL;
    dObj->isExclusive           = false;
    dObj->usartTokenCount       = 1;
<#if core.DMA_ENABLE?has_content>
    dObj->txDMAChannel          = usartInit->dmaChannelTransmit;
    dObj->rxDMAChannel          = usartInit->dmaChannelReceive;
    dObj->txAddress             = usartInit->usartTransmitAddress;
    dObj->rxAddress             = usartInit->usartReceiveAddress;
</#if>
    dObj->remapDataWidth        = usartInit->remapDataWidth;
    dObj->remapParity           = usartInit->remapParity;
    dObj->remapStopBits         = usartInit->remapStopBits;
    dObj->remapError            = usartInit->remapError;

    if (OSAL_MUTEX_Create(&dObj->clientMutex) == OSAL_RESULT_FALSE)
    {
        /*  If the mutex was not created because the memory required to
            hold the mutex could not be allocated then NULL is returned. */
        return SYS_MODULE_OBJ_INVALID;
    }

    if (OSAL_MUTEX_Create(&dObj->txTransferMutex) == OSAL_RESULT_FALSE)
    {
        /*  If the mutex was not created because the memory required to
            hold the mutex could not be allocated then NULL is returned. */
        return SYS_MODULE_OBJ_INVALID;
    }

    if (OSAL_MUTEX_Create(&dObj->rxTransferMutex) == OSAL_RESULT_FALSE)
    {
        /*  If the mutex was not created because the memory required to
            hold the mutex could not be allocated then NULL is returned. */
        return SYS_MODULE_OBJ_INVALID;
    }

    if (OSAL_SEM_Create(&dObj->txTransferDone, OSAL_SEM_TYPE_BINARY, 0, 0) == OSAL_RESULT_FALSE)
    {
        /* There was insufficient memory available for the semaphore to
        be created successfully. */
        return SYS_MODULE_OBJ_INVALID;
    }

    if (OSAL_SEM_Create(&dObj->rxTransferDone, OSAL_SEM_TYPE_BINARY, 0, 0) == OSAL_RESULT_FALSE)
    {
        /* There was insufficient memory available for the semaphore to
        be created successfully. */
        return SYS_MODULE_OBJ_INVALID;
    }

    /* Register a callback with either DMA or USART PLIB based on configuration.
     * dObj is used as a context parameter, that will be used to distinguish the
     * events for different driver instances. */
<#if core.DMA_ENABLE?has_content>
    if(dObj->txDMAChannel != SYS_DMA_CHANNEL_NONE)
    {
        SYS_DMA_ChannelCallbackRegister(dObj->txDMAChannel, _DRV_USART_TX_DMA_CallbackHandler, (uintptr_t)dObj);
    }
    else
    {
        dObj->usartPlib->writeCallbackRegister(_DRV_USART_TX_PLIB_CallbackHandler, (uintptr_t)dObj);
        (void)_DRV_USART_TX_DMA_CallbackHandler;
    }
<#else>
    dObj->usartPlib->writeCallbackRegister(_DRV_USART_TX_PLIB_CallbackHandler, (uintptr_t)dObj);
</#if>

<#if core.DMA_ENABLE?has_content>
    if(dObj->rxDMAChannel != SYS_DMA_CHANNEL_NONE)
    {
        SYS_DMA_ChannelCallbackRegister(dObj->rxDMAChannel, _DRV_USART_RX_DMA_CallbackHandler, (uintptr_t)dObj);
    }
    else
    {
        dObj->usartPlib->readCallbackRegister(_DRV_USART_RX_PLIB_CallbackHandler, (uintptr_t)dObj);
        (void)_DRV_USART_RX_DMA_CallbackHandler;
    }
<#else>
    dObj->usartPlib->readCallbackRegister(_DRV_USART_RX_PLIB_CallbackHandler, (uintptr_t)dObj);
</#if>

    /* Update the status */
    dObj->status = SYS_STATUS_READY;

    /* Return the object structure */
    return ( (SYS_MODULE_OBJ)drvIndex );
}

// *****************************************************************************
/* Function:
    SYS_STATUS DRV_USART_Status
    (
        SYS_MODULE_OBJ object
    )

  Summary:
    Gets the current status of the USART driver module.

  Description:
    This routine provides the current status of the USART driver module.

  Remarks:
    See drv_usart.h for usage information.
*/

SYS_STATUS DRV_USART_Status( SYS_MODULE_OBJ object)
{
    /* Validate the request */
    if( (object == SYS_MODULE_OBJ_INVALID) || (object >= DRV_USART_INSTANCES_NUMBER) )
    {
        //SYS_DEBUG(SYS_ERROR_ERROR, "Invalid system object handle");
        return SYS_STATUS_UNINITIALIZED;
    }

    return (gDrvUSARTObj[object].status);
}

// *****************************************************************************
/* Function:
    DRV_HANDLE DRV_USART_Open
    (
        const SYS_MODULE_INDEX index,
        const DRV_IO_INTENT ioIntent
    )

  Summary:
    Opens the specified USART driver instance and returns a handle to it.

  Description:
    This routine opens the specified USART driver instance and provides a
    handle that must be provided to all other client-level operations to
    identify the caller and the instance of the driver. The ioIntent
    parameter defines how the client interacts with this driver instance.

  Remarks:
    See drv_usart.h for usage information.
*/

DRV_HANDLE DRV_USART_Open( const SYS_MODULE_INDEX drvIndex, const DRV_IO_INTENT ioIntent )
{
    DRV_USART_OBJ *dObj = NULL;
    DRV_USART_CLIENT_OBJ *clientObj = NULL;
    uint32_t iClient;

    /* Validate the request */
    if (drvIndex >= DRV_USART_INSTANCES_NUMBER)
    {
        //SYS_DEBUG(SYS_ERROR_ERROR, "Invalid Driver Instance");
        return DRV_HANDLE_INVALID;
    }

    dObj = &gDrvUSARTObj[drvIndex];

    if(dObj->status != SYS_STATUS_READY)
    {
        //SYS_DEBUG(SYS_ERROR_ERROR, "Was the driver initialized?");
        return DRV_HANDLE_INVALID;
    }

    /* Acquire the instance specific mutex to protect the instance specific
     * client pool
     */
    if (OSAL_MUTEX_Lock(&dObj->clientMutex , OSAL_WAIT_FOREVER ) == OSAL_RESULT_FALSE)
    {
        return DRV_HANDLE_INVALID;
    }

    if(dObj->isExclusive)
    {
        /* This means the another client has opened the driver in exclusive
           mode. The driver cannot be opened again */
        OSAL_MUTEX_Unlock( &dObj->clientMutex);
        return DRV_HANDLE_INVALID;
    }

    if((dObj->nClients > 0) && (ioIntent & DRV_IO_INTENT_EXCLUSIVE))
    {
        /* This means the driver was already opened and another driver was
           trying to open it exclusively.  We cannot give exclusive access in
           this case */
        OSAL_MUTEX_Unlock( &dObj->clientMutex);
        return(DRV_HANDLE_INVALID);
    }

    /* Enter here only if the lock was obtained */
    for(iClient = 0; iClient != dObj->nClientsMax; iClient++)
    {
        if(false == ((DRV_USART_CLIENT_OBJ *)dObj->clientObjPool)[iClient].inUse)
        {
            /* This means we have a free client object to use */

            clientObj = &((DRV_USART_CLIENT_OBJ *)dObj->clientObjPool)[iClient];

            clientObj->inUse        = true;

            clientObj->hDriver      = dObj;

            clientObj->ioIntent     = ioIntent;

            clientObj->errors       = DRV_USART_ERROR_NONE;

            if(ioIntent & DRV_IO_INTENT_EXCLUSIVE)
            {
                /* Set the driver exclusive flag */
                dObj->isExclusive = true;
            }

            dObj->nClients ++;

            /* Generate and save client handle in the client object, which will
             * be then used to verify the validity of the client handle.
             */
            clientObj->clientHandle = _DRV_USART_MAKE_HANDLE(dObj->usartTokenCount, drvIndex, iClient);

            /* Increment the instance specific token counter */
            dObj->usartTokenCount = _DRV_USART_UPDATE_TOKEN(dObj->usartTokenCount);

            break;
        }
    }

    OSAL_MUTEX_Unlock(&dObj->clientMutex);

    /* Driver index is the handle */
    return clientObj ? ((DRV_HANDLE)clientObj->clientHandle) : DRV_HANDLE_INVALID;
}

// *****************************************************************************
/* Function:
    bool DRV_USART_SerialSetup
    (
        const DRV_HANDLE handle,
        DRV_USART_SERIAL_SETUP * setup
    )

  Summary:
    Sets the USART serial communication settings dynamically.

  Description:
    This function sets the USART serial communication settings dynamically.

  Remarks:
    See drv_usart.h for usage information.
*/

bool DRV_USART_SerialSetup( const DRV_HANDLE handle, DRV_USART_SERIAL_SETUP* setup )
{
    DRV_USART_OBJ* dObj;
    DRV_USART_CLIENT_OBJ* clientObj;
    DRV_USART_SERIAL_SETUP setupRemap;
    bool isSuccess = false;

    /* Validate the request */
    clientObj = _DRV_USART_DriverHandleValidate(handle);
    if ((clientObj != NULL) && (setup != NULL))
    {
        dObj = (DRV_USART_OBJ *)clientObj->hDriver;

        setupRemap.dataWidth = (DRV_USART_DATA_BIT)dObj->remapDataWidth[setup->dataWidth];
        setupRemap.parity = (DRV_USART_DATA_BIT)dObj->remapParity[setup->parity];
        setupRemap.stopBits = (DRV_USART_DATA_BIT)dObj->remapStopBits[setup->stopBits];
        setupRemap.baudRate = setup->baudRate;

        if( (setupRemap.dataWidth != DRV_USART_DATA_BIT_INVALID) && (setupRemap.parity != DRV_USART_PARITY_INVALID) && (setupRemap.stopBits != DRV_USART_STOP_BIT_INVALID))
        {
            /* Clock source cannot be modified dynamically, so passing the '0' to pick
             * the configured clock source value */
            isSuccess = dObj->usartPlib->serialSetup(&setupRemap, 0);
        }
    }
    return isSuccess;
}

// *****************************************************************************
/* Function:
    void DRV_USART_Close
    (
        DRV_Handle handle
    )

  Summary:
    Closes an opened-instance of the USART driver.

  Description:
    This routine closes an opened-instance of the USART driver, invalidating the
    handle. Any buffers in the driver queue that were submitted by this client
    will be removed. A new handle must be obtained by calling DRV_USART_Open
    before the caller may use the driver again.

  Remarks:
    See drv_usart.h for usage information.
*/

void DRV_USART_Close( DRV_HANDLE handle )
{
    DRV_USART_CLIENT_OBJ* clientObj;
    DRV_USART_OBJ* dObj;

    /* Validate the handle */
    clientObj = _DRV_USART_DriverHandleValidate(handle);

    if(clientObj != NULL)
    {
        dObj = (DRV_USART_OBJ *)clientObj->hDriver;

        /* Acquire the instance specifc mutex to protect the instance specific
         * client pool
         */
        if (OSAL_MUTEX_Lock(&dObj->clientMutex , OSAL_WAIT_FOREVER ) == OSAL_RESULT_TRUE)
        {
            /* Reduce the number of clients */
            dObj->nClients --;

            /* Reset the exclusive flag */
            dObj->isExclusive = false;

            /* De-allocate the object */
            clientObj->inUse = false;

            /* Release the instance specific mutex */
            OSAL_MUTEX_Unlock( &dObj->clientMutex );
        }
    }
}

// *****************************************************************************
/* Function:
    DRV_USART_ERROR DRV_USART_ErrorGet
    (
        const DRV_HANDLE handle
    )

  Summary:
    Gets the USART hardware errors associated with the client.

  Description:
    This function returns the errors associated with the given client.
    The call to this function also clears all the associated error flags.

  Remarks:
    See drv_usart.h for usage information.
*/

DRV_USART_ERROR DRV_USART_ErrorGet( const DRV_HANDLE handle )
{
    DRV_USART_CLIENT_OBJ* clientObj;
    DRV_USART_ERROR errors = DRV_USART_ERROR_NONE;

    /* Validate the handle */
    clientObj = _DRV_USART_DriverHandleValidate(handle);

    if(clientObj != NULL)
    {
        errors = clientObj->errors;
    }

    return errors;
}
// *****************************************************************************
/* Function:
    bool DRV_USART_WriteBuffer
    (
        const DRV_HANDLE handle,
        void * buffer,
        const size_t size
    );

  Summary:
    This is a blocking function that writes data over USART.

  Description:
    This function does a blocking write operation. The function blocks till
    the data write is complete.

  Remarks:
    See drv_usart.h for usage information.
*/

bool DRV_USART_WriteBuffer
(
    const DRV_HANDLE handle,
    void* buffer,
    const size_t numbytes
)
{
    DRV_USART_CLIENT_OBJ* clientObj = (DRV_USART_CLIENT_OBJ *)NULL;
    DRV_USART_OBJ* dObj = NULL;
    bool isSuccess = false;

    /* Validate the driver handle */
    clientObj = _DRV_USART_DriverHandleValidate(handle);

    if((clientObj != NULL) && (numbytes != 0) && (buffer != NULL))
    {
        dObj = clientObj->hDriver;

        /* Obtain transmit mutex */
        if (OSAL_MUTEX_Lock(&dObj->txTransferMutex, OSAL_WAIT_FOREVER) == OSAL_RESULT_TRUE)
        {
            /* Error is cleared for every new transfer */
            clientObj->errors = DRV_USART_ERROR_NONE;

            dObj->currentTxClient = (uintptr_t)clientObj;

<#if core.DMA_ENABLE?has_content>
            if(dObj->txDMAChannel != SYS_DMA_CHANNEL_NONE)
            {
                if (DATA_CACHE_ENABLED == true)
                {
                    /* Clean cache lines having source buffer before submitting a transfer
                     * request to DMA to load the latest data in the cache to the actual
                     * memory */
                    DCACHE_CLEAN_BY_ADDR((uint32_t *)buffer, numbytes);
                }

                SYS_DMA_ChannelTransfer(dObj->txDMAChannel, (const void *)buffer, (const void *)dObj->txAddress, numbytes);
            }
            else
            {
                dObj->usartPlib->write(buffer, numbytes);
            }
<#else>
            dObj->usartPlib->write(buffer, numbytes);
</#if>

            /* Wait for transfer to complete */
            if (OSAL_SEM_Pend(&dObj->txTransferDone, OSAL_WAIT_FOREVER) == OSAL_RESULT_TRUE)
            {
                if (dObj->txRequestStatus == DRV_USART_REQUEST_STATUS_COMPLETE)
                {
                   isSuccess = true;
                }
            }
            /* Release transmit mutex */
            OSAL_MUTEX_Unlock(&dObj->txTransferMutex);
        }
    }
    return isSuccess;
}

// *****************************************************************************
/* Function:
    bool DRV_USART_ReadBuffer
    (
        const DRV_HANDLE handle,
        void * buffer,
        const size_t size
    );

  Summary:
    This is a blocking function that reads data over USART.

  Description:
    This function does a blocking read operation. The function blocks till
    the data read is complete or error has occurred during read.

  Remarks:
    See drv_usart.h for usage information.
*/

bool DRV_USART_ReadBuffer
(
    const DRV_HANDLE handle,
    void* buffer,
    const size_t numbytes
)
{
    DRV_USART_CLIENT_OBJ* clientObj = (DRV_USART_CLIENT_OBJ *)NULL;
    DRV_USART_OBJ* dObj = NULL;
    bool isSuccess = false;

    /* Validate the driver handle */
    clientObj = _DRV_USART_DriverHandleValidate(handle);

    if((clientObj != NULL) && (numbytes != 0) && (buffer != NULL))
    {
        dObj = clientObj->hDriver;

        /* Obtain receive mutex */
        if (OSAL_MUTEX_Lock(&dObj->rxTransferMutex, OSAL_WAIT_FOREVER) == OSAL_RESULT_TRUE)
        {
            /* Error is cleared for every new transfer */
            clientObj->errors = DRV_USART_ERROR_NONE;

            dObj->currentRxClient = (uintptr_t)clientObj;

<#if core.DMA_ENABLE?has_content>
            if(dObj->rxDMAChannel != SYS_DMA_CHANNEL_NONE)
            {
                SYS_DMA_ChannelTransfer(dObj->rxDMAChannel, (const void *)dObj->rxAddress, (const void *)buffer, numbytes);
            }
            else
            {
                dObj->usartPlib->read(buffer, numbytes);
            }
<#else>
            dObj->usartPlib->read(buffer, numbytes);
</#if>
            /* Wait for transfer to complete */
            if (OSAL_SEM_Pend(&dObj->rxTransferDone, OSAL_WAIT_FOREVER) == OSAL_RESULT_TRUE)
            {
                /* Check and return status */
                if (dObj->rxRequestStatus == DRV_USART_REQUEST_STATUS_COMPLETE)
                {
<#if core.DMA_ENABLE?has_content>
                    if ((dObj->rxDMAChannel != SYS_DMA_CHANNEL_NONE) && (DATA_CACHE_ENABLED == true))
                    {
                        /* Invalidate cache lines having received buffer before using it
                         * to load the latest data in the actual memory to the cache */
                        DCACHE_INVALIDATE_BY_ADDR((uint32_t *)buffer, numbytes);
                    }
</#if>
                    isSuccess = true;
                }
            }
            /* Release receive mutex */
            OSAL_MUTEX_Unlock(&dObj->rxTransferMutex);
        }
    }
    return isSuccess;
}