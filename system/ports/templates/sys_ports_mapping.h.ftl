/*******************************************************************************
  Ports System Service Mapping File

  Company:
    Microchip Technology Inc.

  File Name:
    sys_ports_mapping.h

  Summary:
    Ports System Service mapping file.

  Description:
    This header file contains the mapping of the APIs defined in the API header
    to either the function implementations or macro implementation or the
    specific variant implementation.
*******************************************************************************/

//DOM-IGNORE-BEGIN
/******************************************************************************
Copyright (c) 2018 released Microchip Technology Inc.  All rights reserved.

Microchip licenses to you the right to use, modify, copy and distribute
Software only when embedded on a Microchip microcontroller or digital signal
controller that is integrated into your product or third party product
(pursuant to the sublicense terms in the accompanying license agreement).

You should refer to the license agreement accompanying this Software for
additional information regarding your rights and obligations.

SOFTWARE AND DOCUMENTATION ARE PROVIDED 'AS IS' WITHOUT WARRANTY OF ANY KIND,
EITHER EXPRESS OR IMPLIED, INCLUDING WITHOUT LIMITATION, ANY WARRANTY OF
MERCHANTABILITY, TITLE, NON-INFRINGEMENT AND FITNESS FOR A PARTICULAR PURPOSE.
IN NO EVENT SHALL MICROCHIP OR ITS LICENSORS BE LIABLE OR OBLIGATED UNDER
CONTRACT, NEGLIGENCE, STRICT LIABILITY, CONTRIBUTION, BREACH OF WARRANTY, OR
OTHER LEGAL EQUITABLE THEORY ANY DIRECT OR INDIRECT DAMAGES OR EXPENSES
INCLUDING BUT NOT LIMITED TO ANY INCIDENTAL, SPECIAL, INDIRECT, PUNITIVE OR
CONSEQUENTIAL DAMAGES, LOST PROFITS OR LOST DATA, COST OF PROCUREMENT OF
SUBSTITUTE GOODS, TECHNOLOGY, SERVICES, OR ANY CLAIMS BY THIRD PARTIES
(INCLUDING BUT NOT LIMITED TO ANY DEFENSE THEREOF), OR OTHER SIMILAR COSTS.
*******************************************************************************/
//DOM-IGNORE-END

#ifndef SYS_PORTS_MAPPING_H
#define SYS_PORTS_MAPPING_H

<#if core.PORT_API_PREFIX??>
<#assign PLIB_NAME  = core.PORT_API_PREFIX?string>
<#assign PLIB_NAME_LC  = core.PORT_API_PREFIX?lower_case>
</#if>


#include "peripheral/${PLIB_NAME_LC}/plib_${PLIB_NAME_LC}.h"

// *****************************************************************************
// *****************************************************************************
// Section: PORTS System Service Mapping
// *****************************************************************************
// *****************************************************************************


static inline void SYS_PORT_PinWrite(SYS_PORT_PIN pin, bool value)
{
    ${PLIB_NAME}_PinWrite((${PLIB_NAME}_PIN)pin, value);
}

static inline bool SYS_PORT_PinRead(SYS_PORT_PIN pin)
{
    return(${PLIB_NAME}_PinRead((${PLIB_NAME}_PIN)pin));
}

static inline bool SYS_PORT_PinLatchRead(SYS_PORT_PIN pin)
{
    return(${PLIB_NAME}_PinLatchRead((${PLIB_NAME}_PIN)pin));
}

static inline void SYS_PORT_PinToggle(SYS_PORT_PIN pin)
{
    ${PLIB_NAME}_PinToggle((${PLIB_NAME}_PIN)pin);
}

static inline void SYS_PORT_PinSet(SYS_PORT_PIN pin)
{
    ${PLIB_NAME}_PinSet((${PLIB_NAME}_PIN)pin);
}

static inline void SYS_PORT_PinClear(SYS_PORT_PIN pin)
{
    ${PLIB_NAME}_PinClear((${PLIB_NAME}_PIN)pin);
}

static inline void SYS_PORT_PinInputEnable(SYS_PORT_PIN pin)
{
    ${PLIB_NAME}_PinInputEnable((${PLIB_NAME}_PIN)pin);
}

static inline void SYS_PORT_PinOutputEnable(SYS_PORT_PIN pin)
{
    ${PLIB_NAME}_PinOutputEnable((${PLIB_NAME}_PIN)pin);
}

#endif // SYS_PORTS_MAPPING_H

/*******************************************************************************
 End of File
*/