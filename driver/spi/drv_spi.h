/*******************************************************************************
  SPI Driver Interface Header File

  Company:
    Microchip Technology Inc.

  File Name:
    drv_spi.h

  Summary:
    SPI Driver Interface Header File

  Description:
    The SPI device driver provides a simple interface to manage the SPI modules
    on Microchip microcontrollers.  This file provides the interface definition
    for the SPI driver.
*******************************************************************************/

//DOM-IGNORE-BEGIN
/*******************************************************************************
Copyright (c) 2017 released Microchip Technology Inc.  All rights reserved.

Microchip licenses to you the right to use, modify, copy and distribute
Software only when embedded on a Microchip microcontroller or digital signal
controller that is integrated into your product or third party product
(pursuant to the sublicense terms in the accompanying license agreement).

You should refer to the license agreement accompanying this Software for
additional information regarding your rights and obligations.

SOFTWARE AND DOCUMENTATION ARE PROVIDED AS IS  WITHOUT  WARRANTY  OF  ANY  KIND,
EITHER EXPRESS  OR  IMPLIED,  INCLUDING  WITHOUT  LIMITATION,  ANY  WARRANTY  OF
MERCHANTABILITY, TITLE, NON-INFRINGEMENT AND FITNESS FOR A  PARTICULAR  PURPOSE.
IN NO EVENT SHALL MICROCHIP OR  ITS  LICENSORS  BE  LIABLE  OR  OBLIGATED  UNDER
CONTRACT, NEGLIGENCE, STRICT LIABILITY, CONTRIBUTION,  BREACH  OF  WARRANTY,  OR
OTHER LEGAL  EQUITABLE  THEORY  ANY  DIRECT  OR  INDIRECT  DAMAGES  OR  EXPENSES
INCLUDING BUT NOT LIMITED TO ANY  INCIDENTAL,  SPECIAL,  INDIRECT,  PUNITIVE  OR
CONSEQUENTIAL DAMAGES, LOST  PROFITS  OR  LOST  DATA,  COST  OF  PROCUREMENT  OF
SUBSTITUTE  GOODS,  TECHNOLOGY,  SERVICES,  OR  ANY  CLAIMS  BY  THIRD   PARTIES
(INCLUDING BUT NOT LIMITED TO ANY DEFENSE  THEREOF),  OR  OTHER  SIMILAR  COSTS.
*******************************************************************************/
//DOM-IGNORE-END

#ifndef DRV_SPI_H
#define DRV_SPI_H

// *****************************************************************************
// *****************************************************************************
// Section: File includes
// *****************************************************************************
// *****************************************************************************
#include "drv_spi_definitions.h"
#include "driver/driver.h"

// DOM-IGNORE-BEGIN
#ifdef __cplusplus  // Provide C++ Compatibility

    extern "C" {

#endif
// DOM-IGNORE-END

// *****************************************************************************
// *****************************************************************************
// Section: Data Types
// *****************************************************************************
// *****************************************************************************

// *****************************************************************************
/* SPI Driver Transfer Handle

  Summary:
    Handle identifying the transfer request queued.

  Description:
    A transfer handle value is returned by a call to the DRV_SPI_ReadTransferAdd
    or DRV_SPI_WriteTransferAdd or DRV_SPI_WriteReadTransferAdd functions. This
    handle is associated with the transfer request passed into the function and it
    allows the application to track the completion of the transfer request.
    The transfer handle value returned from the "transfer add"
    function is returned back to the client by the "event handler callback"
    function registered with the driver.
    
    This handle can also be used to poll the transfer completion status using
    DRV_SPI_TransferStatusGet API.
    
    The transfer handle assigned to a client request expires when a new transfer
    request is made after the completion of the current request.

  Remarks:
    None
*/

typedef uintptr_t DRV_SPI_TRANSFER_HANDLE;

// *****************************************************************************
/* SPI Driver Invalid Transfer Handle

  Summary:
    Definition of an invalid transfer handle.

  Description:
    This is the definition of an invalid transfer handle. An invalid transfer 
    handle is returned by DRV_SPI_WriteReadTransferAdd or DRV_SPI_WriteTransferAdd
    or DRV_SPI_ReadTransferAdd function if the buffer add request was not
    successful. It can happen due to invalid arguments or lack of space in the
    queue.

  Remarks:
    None
*/

#define DRV_SPI_TRANSFER_HANDLE_INVALID /*DOM-IGNORE-BEGIN*/((DRV_SPI_TRANSFER_HANDLE)(-1))/*DOM-IGNORE-END*/

// *****************************************************************************
/* SPI Driver Transfer Events

   Summary
    Identifies the possible events that can result from a transfer add request.

   Description
    This enumeration identifies the possible events that can result from a
    transfer add request caused by the client calling either
    DRV_SPI_ReadTransferAdd or DRV_SPI_WriteTransferAdd or
    DRV_SPI_WriteReadTransferAdd functions.

   Remarks:
    One of these values is passed in the "event" parameter of the event
    handling callback function that the client registered with the driver by
    calling the DRV_SPI_TransferEventHandlerSet function when a
    transfer request is completed.
    
    When status polling is used, one of these events is returned by
    DRV_SPI_TransferStatusGet function.
*/

typedef enum
{
    /* Transfer request is pending */
    DRV_SPI_TRANSFER_EVENT_PENDING,
    
    /* All data were transfered successfully. */
    DRV_SPI_TRANSFER_EVENT_COMPLETE,

    /* There was an error while processing transfer request. */
    DRV_SPI_TRANSFER_EVENT_ERROR = -1,
    
    /* Driver handle provided to get the Transfer event is wrong */
    DRV_SPI_HANDLE_INVALID = -2,

    /* Transfer Handle given is invalid or expired */
    DRV_SPI_TRANSFER_HANDLE_INVALID_OR_EXPIRED = -3
    
} DRV_SPI_TRANSFER_EVENT;

// *****************************************************************************
/* SPI Driver Transfer Event Handler Function Pointer

   Summary
    Pointer to a SPI Driver Transfer Event handler function

   Description
    This data type defines the required function signature for the SPI driver
    transfer event handling callback function. A client must register a pointer
    using the transfer event handling function whose function signature (parameter
    and return value types) match the types specified by this function pointer
    in order to receive transfer related event calls back from the driver.

    The parameters and return values are described here and a partial example
    implementation is provided.

  Parameters:
    event -             Identifies the type of event

    transferHandle -    Handle identifying the transfer to which the event relates

    context -           Value identifying the context of the application that 
                        registered the event handling function.

  Returns:
    None.

  Example:
    <code>
    void APP_MyTransferEventHandler( DRV_SPI_TRANSFER_EVENT event,
                                   DRV_SPI_TRANSFER_HANDLE transferHandle,
                                   uintptr_t context )
    {
        MY_APP_DATA_STRUCT pAppData = (MY_APP_DATA_STRUCT) context;

        switch(event)
        {
            case DRV_SPI_TRANSFER_EVENT_COMPLETE:

                // Handle the completed transfer.
                break;

            case DRV_SPI_TRANSFER_EVENT_ERROR:

                // Handle error.
                break;
        }
    }
    </code>

  Remarks:
    If the event is DRV_SPI_TRANSFER_EVENT_COMPLETE, it means that the data was
    transferred successfully.

    If the event is DRV_SPI_TRANSFER_EVENT_ERROR, it means that the data was not
    transferred successfully.

    The transferHandle parameter contains the transfer handle of the transfer
    request that is associated with the event.

    The context parameter contains the a handle to the client context,
    provided at the time the event handling function was registered using the
    DRV_SPI_TransferEventHandlerSet function.  This context handle value is
    passed back to the client as the "context" parameter.  It can be any value
    necessary to identify the client context or instance (such as a pointer to
    the client's data) of the client that made the transfer add request.

    The event handler function executes in interrupt context of the peripheral.
    Hence it is recommended of the application to not perform process
    intensive or blocking operations with in this function.

    The DRV_SPI_ReadTransferAdd, DRV_SPI_WriteTransferAdd and
    DRV_SPI_WriteReadTransferAdd functions can be called in the event handler
    to add a transfer request to the driver queue. These functions can only
    be called to add transfers to the driver instance whose event handler is
    running. For example, SPI2 driver transfer requests cannot be added in SPI1
    driver event handler. Similarly, SPIx transfer requests should not be added
    in event handler of any other peripheral.
*/

typedef void ( *DRV_SPI_TRANSFER_EVENT_HANDLER )( DRV_SPI_TRANSFER_EVENT event, DRV_SPI_TRANSFER_HANDLE transferHandle, void* context );


// *****************************************************************************
// *****************************************************************************
// Section: SPI Driver System Interface Routines
// *****************************************************************************
// *****************************************************************************

// *****************************************************************************
/* Function:
    SYS_MODULE_OBJ DRV_SPI_Initialize
    (
        const SYS_MODULE_INDEX index,
        const SYS_MODULE_INIT * const init
    )

  Summary:
    Initializes the SPI instance for the specified driver index.

  Description:
    This routine initializes the SPI driver instance for the specified driver
    index, making it ready for clients to open and use it. The initialization
    data is specified by the init parameter. The initialization may fail if the
    number of driver objects allocated are insufficient or if the specified
    driver instance is already initialized. The driver instance index is
    independent of the SPI module ID. For example, driver instance 0 can be
    assigned to SPI2. Some of the initialization parameters are overridden by 
	configuration macros in system_config.h file.

  Precondition:
    None.

  Parameters:
    index - Identifier for the instance to be initialized

    init  - Pointer to the init data structure containing any data necessary to
            initialize the driver.

  Returns:
    If successful, returns a valid handle to a driver instance object.
    Otherwise, returns SYS_MODULE_OBJ_INVALID.

  Example:
    <code>
    // The following code snippet shows an example SPI driver initialization.

    SYS_MODULE_OBJ   objectHandle;

    PLIB_SPI_API plibSpiApi[1] = {
        {
            .setup = (DRV_SETUP)SPI0_TransferSetup,
            .writeRead = (DRV_WRITEREAD)SPI0_WriteRead,
            .isBusy = (DRV_IS_BUSY)SPI0_IsBusy,
            .errorGet = (DRV_ERROR_GET)SPI0_ErrorGet,
            .callbackRegister = (DRV_CALLBACK_REGISTER)SPI0_CallbackRegister
        }
    };

    DRV_SPI_INIT drvSpi0InitData =
    {
        .spiPlib = &drvSPI0PlibAPI,
        .interruptSPI = DRV_SPI_INT_SRC_IDX0,
        
        .queueSize = DRV_SPI_QUEUE_SIZE_IDX0,
        .transferObjPool = (uintptr_t)&drvSPI0TransferObjPool[0],
        
        .numClients = DRV_SPI_CLIENTS_NUMBER_IDX0,
        .clientObjPool = (uintptr_t)&drvSPI0ClientObjPool[0], 

        .dmaChannelTransmit = DMA_CHANNEL_NONE,
        .dmaChannelReceive = DMA_CHANNEL_NONE,
        .interruptDMA = XDMAC_IRQn    
    };

    objectHandle = DRV_SPI_Initialize(DRV_SPI_INDEX_0,(SYS_MODULE_INIT*)&drvSpi0InitData);
    if (objectHandle == SYS_MODULE_OBJ_INVALID)
    {
        // Handle error
    }
    </code>

  Remarks:
    This routine must be called before any other SPI routine is called.

    This routine should only be called once during system initialization. 
    This routine will NEVER block for hardware access.
*/

SYS_MODULE_OBJ DRV_SPI_Initialize( const SYS_MODULE_INDEX index, const SYS_MODULE_INIT * const init );

// *****************************************************************************
/* Function:
    SYS_STATUS DRV_SPI_Status( SYS_MODULE_OBJ object )

  Summary:
    Gets the current status of the SPI driver module.

  Description:
    This routine provides the current status of the SPI driver module.

  Precondition:
    Function DRV_SPI_Initialize should have been called before calling this
    function.

  Parameters:
    object - Driver object handle, returned from the DRV_SPI_Initialize routine

  Returns:
    SYS_STATUS_READY -  Initialization have succeeded and the SPI is 
                        ready for additional operations 

    SYS_STATUS_DEINITIALIZED -  Indicates that the driver has been
                                deinitialized

  Example:
    <code>
    SYS_MODULE_OBJ      object;     // Returned from DRV_SPI_Initialize
    SYS_STATUS          spiStatus;

    spiStatus = DRV_SPI_Status(object);
    if (spiStatus == SYS_STATUS_READY)
    {
        // This means now the driver can be opened using the
        // DRV_SPI_Open() function.
    }
    </code>

  Remarks:
    A driver can be opened only when its status is SYS_STATUS_READY.
*/

SYS_STATUS DRV_SPI_Status( SYS_MODULE_OBJ object);

// *****************************************************************************
// *****************************************************************************
// Section: SPI Driver Common Client Interface Routines
// *****************************************************************************
// *****************************************************************************

// *****************************************************************************
/* Function:
    DRV_HANDLE DRV_SPI_Open
    (
        const SYS_MODULE_INDEX index,
        const DRV_IO_INTENT ioIntent
    )

  Summary:
    Opens the specified SPI driver instance and returns a handle to it.

  Description:
    This routine opens the specified SPI driver instance and provides a
    handle that must be provided to all other client-level operations to
    identify the caller and the instance of the driver. The ioIntent
    parameter defines how the client interacts with this driver instance.

    Specifying a DRV_IO_INTENT_EXCLUSIVE will cause the driver to provide
    exclusive access to this client. The driver cannot be opened by any
    other client.

  Precondition:
    Function DRV_SPI_Initialize must have been called before calling this
    function.

  Parameters:
    index  -    Identifier for the object instance to be opened

    intent -    Zero or more of the values from the enumeration DRV_IO_INTENT 
                "ORed" together to indicate the intended use of the driver. 
                See function description for details.

  Returns:
    If successful, the routine returns a valid open-instance handle (a number
    identifying both the caller and the module instance).

    If an error occurs, the return value is DRV_HANDLE_INVALID. Error can occur
    - if the number of client objects allocated via DRV_SPI_CLIENTS_NUMBER is
      insufficient.
    - if the client is trying to open the driver but driver has been opened
      exclusively by another client.
    - if the driver peripheral instance being opened is not initialized or is
      invalid.
    - if the client is trying to open the driver exclusively, but has already
      been opened in a non exclusive mode by another client.
    - if the driver is not ready to be opened, typically when the initialize
      routine has not completed execution.

  Example:
    <code>
    DRV_HANDLE handle;

    handle = DRV_SPI_Open(DRV_SPI_INDEX_0, DRV_IO_INTENT_EXCLUSIVE);
    if (handle == DRV_HANDLE_INVALID)
    {
        // Unable to open the driver
        // May be the driver is not initialized or the initialization
        // is not complete.
    }
    </code>

  Remarks:
    The handle returned is valid until the DRV_SPI_Close routine is called.
    This routine will NEVER block waiting for hardware. If the requested intent
    flags are not supported, the routine will return DRV_HANDLE_INVALID.
*/

DRV_HANDLE DRV_SPI_Open(const SYS_MODULE_INDEX index, const DRV_IO_INTENT ioIntent);

// *****************************************************************************
/* Function:
    void DRV_SPI_Close( DRV_Handle handle )

  Summary:
    Closes an opened-instance of the SPI driver.

  Description:
    This routine closes an opened-instance of the SPI driver, invalidating the
    handle. Any transfer request in the driver queue that were submitted by this
    client will be removed. A new handle must be obtained by calling DRV_SPI_Open 
    before the caller may use the driver again.

  Precondition:
    DRV_SPI_Open must have been called to obtain a valid opened device handle.

  Parameters:
    handle -    A valid open-instance handle, returned from the driver's
                open routine

  Returns:
    None.

  Example:
    <code>
    // 'handle', returned from the DRV_SPI_Open

    DRV_SPI_Close(handle);

    </code>

  Remarks:
    None.
*/

void DRV_SPI_Close( const DRV_HANDLE handle);

// *****************************************************************************
/*
  Function:
    bool DRV_SPI_TransferSetup ( DRV_HANDLE handle, DRV_SPI_TRANSFER_SETUP * setup )

  Summary:
    Sets the dynamic configuration of the driver.
    
  Description:
    This function should be used to update any of the DRV_SPI_TRANSFER_SETUP
    parameters for the selected client of the driver dynamically. It is mainly
    helpful for multi client scenario where different clients need different
    setup like baud rate, clock settings, chip select etc.   

  Preconditions:
    DRV_SPI_Open must have been called to obtain a valid opened device handle.

  Parameters:
    handle      - A valid open-instance handle, returned from the driver's
                   open routine
    *setup       - A structure containing the new configuration settings

  Returns:
    None.

  Example:
    <code>
        // mySPIHandle is the handle returned by the DRV_SPI_Open function.
        DRV_SPI_TRANSFER_SETUP setup;
        
        setup.baudRateInHz = 10000000;
        setup.clockPhase = DRV_SPI_CLOCK_PHASE_TRAILING_EDGE;
        setup.clockPolarity = DRV_SPI_CLOCK_POLARITY_IDLE_LOW;
        setup.dataBits = DRV_SPI_DATA_BITS_16;
        setup.chipSelect = PIO_PIN_RC5;
        setup.csPolarity = DRV_SPI_CS_POLARITY_ACTIVE_LOW;
        
        DRV_SPI_TransferSetup ( mySPIHandle, &setup );
    </code>

  Remarks:
    None.
    
*/
bool DRV_SPI_TransferSetup ( DRV_HANDLE handle, DRV_SPI_TRANSFER_SETUP * setup );

// *****************************************************************************
// *****************************************************************************
// Section: SPI Driver Transfer Queuing Model Interface Routines
// *****************************************************************************
// *****************************************************************************

// *****************************************************************************
/* Function:
    void DRV_SPI_WriteReadTransferAdd
    (
        const DRV_HANDLE handle,
        void*       pTransmitData,
        size_t      txSize,    
        void*       pReceiveData, 
        size_t      rxSize,
        DRV_SPI_TRANSFER_HANDLE * const transferHandle
    );

  Summary:
    Queues a write-read transfer operation.

  Description:
    This function schedules a non-blocking write-read operation. The function
    returns with a valid transfer handle in the transferHandle argument if
    the request was scheduled successfully. The function adds the request to
    the instance specific software queue and returns immediately. While the
    request is in the queue, the application buffer is owned by the driver
    and should not be modified. 
    This API will write txSize and at the same time counting of rxSize to be
    read will start. If user wants 'n' bytes to be read after txSize has been
    written, then he should keep rxSize value as 'txSize + n'.
    
    The function returns DRV_SPI_TRANSFER_HANDLE_INVALID in the
    transferHandle argument:
    - if neither of the transmit or receive arguments are valid.
    - if the transfer handle is NULL.
    - if the queue size is full or queue depth is insufficient.
    - if the driver handle is invalid.

    If the requesting client registered an event callback with the driver, the
    driver will issue a DRV_SPI_TRANSFER_EVENT_COMPLETE event if the transfer
    was processed successfully or DRV_SPI_TRANSFER_EVENT_ERROR event if the
    transfer was not processed successfully.

  Precondition:
    DRV_SPI_Open must have been called to obtain a valid opened device handle.

  Parameters:
    handle -    Handle of the communication channel as returned by the
                DRV_SPI_Open function.
                   
    *pTransmitData  Pointer to the data which has to be transmitted. if it is
                    NULL, that means only data receiving is expected. For 9
                    to 15bit mode, data should be right aligned in the 16 bit
                    memory location.
    txSize          Number of bytes to be transmitted. For 9 to 15bit mode, size
                    should be given as if it is 16bit mode.
    *pReceiveData   Pointer to the location where received data has to be stored.
                    It is user's responsibility to ensure pointed location has
                    sufficient memory to store the read data.
                    if it is NULL, that means only data transmission is expected.
                    For 9 to 15bit mode, received data will be right aligned in
                    the 16 bit memory location.
    rxSize          Number of bytes to be received. For 9 to 15bit mode, size
                    should be given as if it is 16bit mode.
                    if "n" number of bytes has to be received AFTER transmitting
                    "m" number of bytes, then "txSize" should be set as "m" and
                    "rxSize" should be set as "m+n".
    
    transferHandle - Handle which is returned by transfer add function. 
    
  Returns:
    None.

  Example:
    <code>

    MY_APP_OBJ myAppObj;
    uint8_t myTxbuffer[MY_TX_BUFFER_SIZE];
    uint8_t myRxbuffer[MY_RX_BUFFER_SIZE];
    DRV_SPI_TRANSFER_HANDLE transferHandle;

    // mySPIHandle is the handle returned by the DRV_SPI_Open function.

    DRV_SPI_WriteReadTransferAdd(mySPIhandle, myTxBuffer, MY_TX_BUFFER_SIZE,
                                    myRxBuffer, MY_RX_BUFFER_SIZE, &transferHandle);

    if(transferHandle == DRV_SPI_TRANSFER_HANDLE_INVALID)
    {
        // Error handling here
    }

    // Event is received when the transfer is processed.
    </code>

  Remarks:
    This function can be called from within the SPI Driver Transfer Event
    Handler that is registered by the client. It should not be called in the
    event handler associated with another SPI driver instance or event handler
    of any other peripheral. It should not be called directly in any ISR.
*/

void DRV_SPI_WriteReadTransferAdd(
    const   DRV_HANDLE  handle,
    void*   pTransmitData,
    size_t  txSize,    
    void*   pReceiveData, 
    size_t  rxSize,
    DRV_SPI_TRANSFER_HANDLE * const transferHandle);

// *****************************************************************************
/* Function:
    void DRV_SPI_TransferEventHandlerSet
    (
        const DRV_HANDLE handle,
        const DRV_SPI_TRANSFER_EVENT_HANDLER eventHandler,
        const void* context
    )

  Summary:
    Allows a client to set a transfer event handling function for the driver
    to call back when queued transfer has finished.

  Description:
    This function allows a client to register a transfer event handling function
    with the driver to call back when queued transfer has finished.
    When a client calls either the DRV_SPI_ReadTransferAdd or
    DRV_SPI_WriteTransferAdd or DRV_SPI_WriteReadTransferAdd function, it is
    provided with a handle identifying the transfer request that was added to the
    driver's queue.  The driver will pass this handle back to the
    client by calling "eventHandler" function when the transfer has
    completed.

    The event handler should be set before the client performs any "transfer add"
    operations that could generate events. The event handler once set, persists
    until the client closes the driver or sets another event handler (which
    could be a "NULL" pointer to indicate no callback).

  Precondition:
    DRV_SPI_Open must have been called to obtain a valid open instance handle.

  Parameters:
    handle -    A valid open-instance handle, returned from the driver's open 
                routine.

    eventHandler - Pointer to the event handler function.

    context -   The value of parameter will be passed back to the client
                unchanged, when the eventHandler function is called. It can be
                used to identify any client specific data object that
                identifies the instance of the client module (for example, it
                may be a pointer to the client module's state structure).

  Returns:
    None.

  Example:
    <code>
    // myAppObj is an application specific state data object.
    MY_APP_OBJ myAppObj;

    uint8_t myTxBuffer[MY_TX_BUFFER_SIZE];
    uint8_t myRxBuffer[MY_RX_BUFFER_SIZE];
    DRV_SPI_TRANSFER_HANDLE transferHandle;

    // mySPIHandle is the handle returned by the DRV_SPI_Open function.

    // Client registers an event handler with driver. This is done once

    DRV_SPI_TransferEventHandlerSet( mySPIHandle, APP_SPITransferEventHandler,
                                     (void*)&myAppObj );

    DRV_SPI_WriteReadTransferAdd(mySPIhandle, myTxBuffer,
                                MY_TX_BUFFER_SIZE, myRxBuffer,
                                MY_RX_BUFFER_SIZE, &transferHandle);

    if(transferHandle == DRV_SPI_TRANSFER_HANDLE_INVALID)
    {
        // Error handling here
    }

    // Event is received when the transfer is completed.

    void APP_SPITransferEventHandler(DRV_SPI_TRANSFER_EVENT event,
            DRV_SPI_TRANSFER_HANDLE handle, void* context)
    {
        // The context handle was set to an application specific
        // object. It is now retrievable easily in the event handler.
        MY_APP_OBJ myAppObj = (MY_APP_OBJ *) context;

        switch(event)
        {
            case DRV_SPI_TRANSFER_EVENT_COMPLETE:
                // This means the data was transferred.
                break;

            case DRV_SPI_TRANSFER_EVENT_ERROR:
                // Error handling here.
                break;

            default:
                break;
        }
    }
    </code>

  Remarks:
    If the client does not want to be notified when the queued transfer request
    has completed, it does not need to register a callback.
*/

void DRV_SPI_TransferEventHandlerSet( const DRV_HANDLE handle, const DRV_SPI_TRANSFER_EVENT_HANDLER eventHandler, void* context );

// *****************************************************************************
/* Function:
    DRV_SPI_TRANSFER_EVENT DRV_SPI_TransferStatusGet
    ( 
        const DRV_HANDLE handle,
        const DRV_SPI_TRANSFER_HANDLE transferHandle 
    )

  Summary:
    Returns transfer add request status.

  Description:
    This function can be used to poll the status of the queued transfer request
    if the application doesn't prefer to use the event handler (callback)
    function to get notified.

  Precondition:
    Either DRV_SPI_ReadTransferAdd or DRV_SPI_WriteTransferAdd or
    DRV_SPI_WriteReadTransferAdd function must have been called and a valid
    transfer handle must have been returned.
    
  Parameters:
    handle         - A valid open-instance handle, returned from the driver's open 
                     routine.
    transferHandle - Handle of the transfer request of which status has to be
                     obtained.
	
  Returns:
    One of the elements of the enum "DRV_SPI_TRANSFER_EVENT".

  Example:
  <code>
    // myAppObj is an application specific object.
    MY_APP_OBJ myAppObj;

    uint8_t mybuffer[MY_BUFFER_SIZE];
    DRV_SPI_TRANSFER_HANDLE transferHandle;
    DRV_SPI_TRANSFER_EVENT event;

    // mySPIHandle is the handle returned by the DRV_SPI_Open function.

    DRV_SPI_ReadTransferAdd( mySPIhandle, myBuffer, MY_RECEIVE_SIZE, &transferHandle);

    if(transferHandle == DRV_SPI_TRANSFER_HANDLE_INVALID)
    {
        // Error handling here
    }
    
    //Check the status of the transfer request
    //This call can be used to wait until the transfer is completed.
    event  = DRV_SPI_TransferStatusGet(mySPIhandle, transferHandle);
  </code>

  Remarks:
    None.   
*/

DRV_SPI_TRANSFER_EVENT DRV_SPI_TransferStatusGet( const DRV_HANDLE handle, const DRV_SPI_TRANSFER_HANDLE transferHandle );


//DOM-IGNORE-BEGIN
#ifdef __cplusplus
}
#endif
//DOM-IGNORE-END

#include "driver/spi/src/drv_spi_local.h"
    
#endif // #ifndef DRV_SPI_H