/*
****************************************************************************
PROJECT : OCTA driver
FILE    : $Id: r_octa_sys.h 13963 2017-06-16 06:38:44Z shinobu.uehara $
============================================================================
DESCRIPTION
OCTA support functions
============================================================================
                            C O P Y R I G H T
============================================================================
                           Copyright (c) 2017
                                  by
                       Renesas Electronics (Europe) GmbH.
                           Arcadiastrasse 10
                          D-40472 Duesseldorf
                               Germany
                          All rights reserved.
============================================================================
Purpose: only for testing, not for mass production

DISCLAIMER

LICENSEE has read, understood and accepted the terms and conditions defined in
the license agreement, especially the usage rights. In any case, it is
LICENSEE's responsibility to make sure that any user of the software complies
with the terms and conditions of the signed license agreement.

SAMPLE CODE is not part of the licensed software, as such it must not be used in
mass-production applications. It can only be used for evaluation and
demonstration purposes at customer's premises listed in the signed license
agreement.

****************************************************************************
*/

/***************************************************************************
  Title: OCTA Driver support functions

  OCTA (OctaBus Controller) driver support functions

  The generic OCTA driver uses these functions. They have to be implemented 
  within the driver library for a concrete device. (e.g. D1L, D1M, D1H)

  Support functions are functions that are not part of the driver itself 
  but they must be provided to integrate the driver on a particular board.
*/

#ifndef R_OCTA_SYS_H
#define R_OCTA_SYS_H

#ifdef __cplusplus
extern "C" {
#endif


/***************************************************************************
  Section: Global Defines
*/

/***************************************************************************
  Section: Global Functions
*/
/***************************************************************************
  Group: OCTA driver Basic interface functions
*/

/***************************************************************************
  Function: R_OCTA_Sys_Init

  Description:
  This function is called from <R_OCTA_Init>.

  The initialization code of environment-depend (e.g. interrupt priority, power control or
  clock control) is implemented by this function.

  Parameter:
  Unit - Instance number.

  Return value:
  R_OCTA_ERR_OK               - No error has occurred.
  R_OCTA_ERR_DEVICE           - OCTA driver is not applicable to target d1x device.
  
  See <r_octa_Error_t>.
*/
r_octa_Error_t R_OCTA_Sys_Init(const uint32_t Unit);


/***************************************************************************
  Function: R_OCTA_Sys_DeInit

  Description:
  This function is called from <R_OCTA_DeInit>.

  The de-initialization code of environment-depend is implemented by this function.

  Parameter:
  Unit - Instance number.

  Return value:
  R_OCTA_ERR_OK               - No error has occurred.
  
  See <r_octa_Error_t>.
*/
r_octa_Error_t R_OCTA_Sys_DeInit(const uint32_t Unit);


/***************************************************************************
  Function: R_OCTA_Sys_BaseAddrGet

  Description:
  This function gives back base address of dedicated macro.

  Parameter:
  Unit    - Instance number.

  Return value:
  Macro base address.
*/
uint32_t R_OCTA_Sys_BaseAddrGet(const uint32_t Unit);

/***************************************************************************
  Function: R_OCTA_Sys_ExternalAddrSpaceBaseAddrGet

  Description:
  This function gives back base address of external address space for dedicated macro.

  Parameter:
  Unit    - Instance number.
  Channel - Device Channel number.

  Return value:
  base address of external address space.
*/
uint32_t R_OCTA_Sys_ExternalAddrSpaceBaseAddrGet(const uint32_t Unit, const uint32_t Channel);

/***************************************************************************
  Function: R_OCTA_Sys_DefaultDummyLengthGet

  Description:
  This function gives back the default dummylength.

  Parameter:
  Unit    - Instance number.
  Channel - Device Channel number.

  Return value:
  default dummylength.
*/
uint32_t R_OCTA_Sys_DefaultDummyLengthGet(const uint32_t Unit, const uint32_t Channel,
                                          const uint8_t DeviceType);

/***************************************************************************
  Function: R_OCTA_Sys_ConfigRegSet

  Description:
  This function sets the value to set the configuration register.

  Parameter:
  Unit    - Instance number.
  Channel - Device Channel number.
  Config  - Pointer to the <r_octa_Config_t> structure.
  RegVal  - Value of the register.
  Type    - Configuration Type.

  Return value:
  Setting value to the register.
*/
uint16_t R_OCTA_Sys_ConfigRegSet(const uint32_t                  Unit,
                                 const uint32_t                  Channel,
                                 const r_octa_Config_t * const   Config,
                                 const uint16_t                  RegVal,
                                 const r_octa_StateType_t        Type);

/***************************************************************************
  Function: R_OCTA_Sys_StatusRegCheck

  Description:
  This function checks the value from the status register each Status Type,
  and it returns the result code.

  Parameter:
  Unit    - Instance number.
  Channel - Device Channel number.
  RegPos  - Bit position of the register.
  RegVal  - Value of the register.
  Type    - Status Type.
  Count   - counter.

  Return value:
  R_OCTA_ERR_OK               - No error has occurred.
  R_OCTA_ERR_TIMEOUT          - Status polling is timeout.
  R_OCTA_ERR_NOT_ACCEPTABLE   - A function was called in an incorrect state.
  
  See <r_octa_Error_t>.
*/
r_octa_Error_t R_OCTA_Sys_StatusRegCheck(const uint32_t           Unit, 
                                         const uint32_t           Channel,
                                         const uint16_t           RegPos,
                                         const uint16_t           RegVal,
                                         const r_octa_StateType_t Type,
                                         const uint32_t           Count);

/***************************************************************************
  Function: R_OCTA_Sys_StatusRegSet

  Description:
  This function sets the value to set the status register.

  Parameter:
  Unit    - Instance number.
  Channel - Device Channel number.
  RegPos  - Bit position of the register.
  RegVal  - Value of the register.
  Mode    - Block Protection mode.
            <0> protection mode.
            <1> un-protection mode.

  Return value:
  Setting value to the register.
*/
uint16_t R_OCTA_Sys_StatusRegSet(const uint32_t Unit,
                                 const uint32_t Channel,
                                 const uint16_t RegPos,
                                 const uint16_t RegVal,
                                 const uint8_t  Mode);

/***************************************************************************
  Function: R_OCTA_Sys_SyncPeripheral

  Description:
  This function performs synchronization processing to reflect updated
  contents of the control register.

  Parameter:
  None

  Return value:
  None
*/
void R_OCTA_Sys_SyncPeripheral(void);

/***************************************************************************
  Group: OCTA driver OS interface functions
*/

/***************************************************************************
  Function: R_OCTA_Sys_Lock

  Description:
  This function locks the OCTA driver access to the specified unit from other threads.

  In the environment which is using OS, all drivers should implement the mutex
  or semaphore to this function to prevent re-entry to the driver function at
  the multi-threading.

  Parameter:
  Unit - Instance number.

  Return value:
  R_OCTA_ERR_OK               - No error has occurred.
  R_OCTA_ERR_RANGE_UNIT       - Unit number is out of range.
  R_OCTA_ERR_FATAL_OS         - Fatal error has occurred at OS interface.
  
  See <r_octa_Error_t>.
*/
r_octa_Error_t R_OCTA_Sys_Lock(const uint32_t Unit);


/***************************************************************************
  Function: R_OCTA_Sys_Unlock

  Description:
  This function unlocks the OCTA driver access to the specified unit from other threads.

  In the environment which is using OS, all drivers should implement the mutex
  or semaphore to this function to prevent re-entry to the driver function at
  the multi-threading.

  Parameter:
  Unit - Instance number.

  Return value:
  R_OCTA_ERR_OK               - No error has occurred.
  R_OCTA_ERR_RANGE_UNIT       - Unit number is out of range.
  R_OCTA_ERR_FATAL_OS         - Fatal error has occurred at OS interface.
  
  See <r_octa_Error_t>.
*/
r_octa_Error_t R_OCTA_Sys_Unlock(const uint32_t Unit);


/***************************************************************************
  Function: R_OCTA_Sys_Relax

  Description:
  This function relaxes CPU to avoid blocking.
  This function is called from <R_OCTA_DataErase> and <R_OCTA_DataWrite>.

  Parameter:
  Unit - Instance number.

  Return value:
  R_OCTA_ERR_OK               - No error has occurred.
  R_OCTA_ERR_RANGE_UNIT       - Unit number is out of range.
  R_OCTA_ERR_FATAL_OS         - Fatal error has occurred at OS interface.
  
  See <r_octa_Error_t>.
*/
r_octa_Error_t R_OCTA_Sys_Relax(const uint32_t Unit);


#ifdef __cplusplus
}
#endif

#endif /* R_OCTA_SYS_H */

