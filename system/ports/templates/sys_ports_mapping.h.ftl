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
