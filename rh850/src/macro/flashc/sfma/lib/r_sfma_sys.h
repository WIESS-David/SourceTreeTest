/*
****************************************************************************
PROJECT : SFMA driver
FILE    : $Id: r_sfma_sys.h 12651 2017-02-15 08:21:44Z shinya.tomari $
============================================================================
DESCRIPTION
SFMA support functions
============================================================================
                            C O P Y R I G H T
============================================================================
                           Copyright (c) 2013 - 2014
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
  Title: SFMA Driver support functions

  SFMA (Serial Flash Memory Interface A) driver support functions

  The generic SFMA driver uses these functions. They have to be implemented 
  within the driver library for a concrete device. (e.g. D1L, D1M, D1H)

  Support functions are functions that are not part of the driver itself 
  but they must be provided to integrate the driver on a particular board.
*/

#ifndef R_SFMA_SYS_H
#define R_SFMA_SYS_H

#ifdef __cplusplus
extern "C" {
#endif


/***************************************************************************
  Section: Global Types
*/
/***********************************************************
  typedef: r_sfma_SpiConfig_t

  Description:
  This type describes the configuration of the SPI.

  Struct members:
  MemoryNum    - The parameter specifies the number of the connecting serial flash memories.
                 One serial flash memory is connected.<0uL>
                 Two serial flash memories are connected.<1uL>
  MaxClock     - The parameter specifies the maximum clock of connected the 
                 serial flash memory (Hz).
  PageSize     - The parameter specifies the page size of connected serial flash memory (Byte).
  EraseSectorSize  - The parameter specifies the erase sector size of connected
                     serial flash memory (Byte).
  FlashSize    - Size of connected the serial flash memory (byte).
  Command      - See <r_sfma_FlashCommand_t>.
  DataWidth    - The parameter specifies the data bus width.
                 1 bit data bus width.<0uL>
                 2 bits data bus width.<1uL>
                 4 bits data bus width.<2uL>

  TransferMode - The parameter specifies the transfer mode.
                 SDR transfer.<0uL>
                 DDR transfer.<1uL>
  AddressMode  - The parameter specifies the format of address output to the serial flash memory.
                 24 bit address output.<0x07uL>
                 32 bit address output.<0x0fuL>
  Cachemode    - R_SFMA_CACHE_OFF - SFMA cache off.
                 R_SFMA_CACHE_BL1 .. BL16 - SFMA cache on, with set burst length
                 (BL1 = 1 data unit of 64bit, BL16 = 16 data units of 64bit)
  PerformanceEnMode - The parameter specifies if performance enhance mode should be used
                      The setting value of performance enhance mode to be disabled.<0uL>
                      The setting value of performance enhance mode to be enabled.<1uL>
  Mode         - The parameter specifies operating mode.
                 External address space read mode.<0uL>
                 SPI operating mode.<1uL>
*/
typedef struct
{
    uint32_t              MemoryNum;
    uint32_t              MaxClock;
    uint32_t              PageSize;
    uint32_t              EraseSectorSize;
    uint64_t              FlashSize;
    /* Flash memory control command. */
    r_sfma_FlashCommand_t Command;
    uint8_t               DataWidth;
    uint8_t               TransferMode;
    uint8_t               AddressMode;
    uint32_t              Cachemode;
    uint32_t              PerformanceEnMode;
    uint32_t              Mode;
} r_sfma_SpiConfig_t;

/***************************************************************************
  typedef: r_sfma_Dev_t

  Description:
  This type describes the information of the SFMA driver.

  Struct members:
  Mode         - See <r_sfma_Mode_t>
  AccessAddr   - The parameter specifies the access address of the serial flash memory.
  AccessRange  - See <r_sfma_AccessRange_t>
  SpiConfig    - See <r_sfma_SpiConfig_t>
*/
typedef struct
{
    /* Driver information. */
    r_sfma_Mode_t        Mode;
    /* External address space mode information. */
    uint64_t             AccessAddr;
    r_sfma_AccessRange_t AccessRange;
    /* SPI configuration information. */
    r_sfma_SpiConfig_t   SpiConfig;
} r_sfma_Dev_t;

/***************************************************************************
  Section: Global Functions
*/
/***************************************************************************
  Group: SFMA driver Basic interface functions
*/

/***************************************************************************
  Function: R_SFMA_Sys_Init

  Description:
  This function is called from <R_SFMA_Init> function.

  The initialization code of environment-depend (e.g. interrupt priority, power control or
  clock control) is implemented by this function.

  Parameter:
  Unit - The parameter specifies the instance number.

  Return value:
  See <r_sfma_Error_t>.
*/
r_sfma_Error_t R_SFMA_Sys_Init(const uint32_t Unit);


/***************************************************************************
  Function: R_SFMA_Sys_DeInit

  Description:
  This function is called from <R_SFMA_DeInit> function.

  The de-initialization code of environment-depend is implemented by this function.

  Parameter:
  Unit - The parameter specifies the instance number.

  Return value:
  See <r_sfma_Error_t>.
*/
r_sfma_Error_t R_SFMA_Sys_DeInit(const uint32_t Unit);


/***************************************************************************
  Function: R_SFMA_Sys_BaseAddrGet

  Description:
  This function gives back base address of dedicated macro.

  Parameter:
  Unit    - The parameter specifies the instance number.

  Return value:
  Macro base address.
*/
uint32_t R_SFMA_Sys_BaseAddrGet(const uint32_t Unit);


/***************************************************************************
  Function: R_SFMA_Sys_ClockGet

  Description:
  This function gives back clock frequency of dedicated clock input.

  Parameter:
  Unit - The parameter specifies the instance number.

  Return value:
  The internal bus clock frequency(Hz).
*/
uint32_t R_SFMA_Sys_ClockGet(const uint32_t Unit);

/***************************************************************************
  Function: R_SFMA_Sys_CalibSetting

  Description:
  This function sets the calibration of the SFMA macro.

  Parameter:
  Unit   - The parameter specifies the instance number.
  Config - This is a pointer to the <r_sfma_Config_t> structure to the
           configuration of the unit.

  Return value:
  See <r_sfma_Error_t>.
*/
r_sfma_Error_t R_SFMA_Sys_CalibSetting(const uint32_t Unit, const r_sfma_Config_t * const Config);

/***************************************************************************
  Function:  R_SFMA_Sys_Close

  Description:
  This function closes the SFMA in external address space.

  Parameter:
  Unit   - The parameter specifies the instance number.


  Return value:
  See <r_sfma_Error_t>.
*/
r_sfma_Error_t R_SFMA_Sys_Close(const uint32_t Unit);

/***************************************************************************
  Function: R_SFMA_Sys_DDREnable

  Description:
  This function returns valid / invalid of the memory access mode (DDR or SDR).
  If you use LSI which is supported DDR mode, this function should return R_TRUE.
  (e.g. in case of D1Mx and D1L2x, this function return R_TRUE.)

  Parameter:
  None.

  Return value:
  R_TRUE or R_FALSE.
*/
uint8_t R_SFMA_Sys_DDREnable(void);

/***************************************************************************
  Function: R_SFMA_Sys_BusWidth8BitEnable

  Description:
  This function returns valid / invalid of the bus width 8bits mode (4bits or 8bits).
  If you use LSI which is supported the bus width 8bits mode, this function should return R_TRUE.
  (e.g. in case of D1Mx and D1L2H, this function return R_TRUE.)

  Parameter:
  Unit   - The parameter specifies the instance number.

  Return value:
  R_TRUE or R_FALSE.
*/
uint8_t R_SFMA_Sys_BusWidth8BitEnable(const uint32_t Unit);

/***************************************************************************
  Function: R_SFMA_Sys_SyncPeripheral

  Description:
  This function performs synchronization processing to reflect updated
  contents of the control register.

  Parameter:
  None.

  Return value:
  void.
*/
void R_SFMA_Sys_SyncPeripheral(void);

/***************************************************************************
  Function: R_SFMA_Sys_UnitWindowSelect

  Description:
  This function flips the access able space which mapped to CPU address space
  in SFMA0 and SFMA1 by specifying the unit number and the Window.

  Parameter:
  Unit   - The parameter specifies the instance number.
  Window - The parameter specifies the window for SFMA.
           See r_sfma_WindowMode_t.

  Return value:
  See <r_sfma_Error_t>.
*/
r_sfma_Error_t R_SFMA_Sys_UnitWindowSelect(const uint32_t Unit,
                                           const r_sfma_WindowMode_t Window);

/***************************************************************************
  Function: R_SFMA_Sys_UnitNumberGet

  Description:
  This function retrieves the unit number of specifying Window for SFMA.

  Parameter:
  Window   - The parameter specifies the window for SFMA.
             See r_sfma_WindowMode_t.
  Unit     - This is a pointer to the instance number.

  Return value:
  See <r_sfma_Error_t>.
*/
r_sfma_Error_t R_SFMA_Sys_UnitNumberGet(const r_sfma_WindowMode_t Window,
                                        uint32_t * const Unit);

/***************************************************************************
  Function: R_SFMA_Sys_NumberOfUnitGet

  Description:
  This function retrieves the number of unit for SFMA.

  Parameter:
  None.

  Return value:
  Number of Unit.
*/
uint32_t R_SFMA_Sys_NumberOfUnitGet(void);

/***************************************************************************
  Function: R_SFMA_Sys_ExclusiveUnitGet

  Description:
  This function retrieves the exclusive unit for specified unit.
  If exclusive unit does not exist, this function retrieves unit out of range.

  Parameter:
  Unit    - The parameter specifies the instance number.

  Return value:
  Exclusive unit.
*/
uint32_t R_SFMA_Sys_ExclusiveUnitGet(const uint32_t Unit);

/***************************************************************************
  Function: R_SFMA_Sys_SwapUnitGet

  Description:
  This function retrieves the unit to be swapped for specified unit.

  Parameter:
  Unit    - The parameter specifies the instance number.

  Return value:
  Unit to be swapped.
*/
uint32_t R_SFMA_Sys_SwapUnitGet(const uint32_t Unit);

/***************************************************************************
  Function: R_SFMA_Sys_GetDevInfo

  Description:
  This function retrieves the pointer to <r_sfma_Dev_t> information.

  Parameter:
  Unit    - The parameter specifies the instance number.

  Return value:
  Pointer to <r_sfma_Dev_t>.
*/
r_sfma_Dev_t* R_SFMA_Sys_GetDevInfo(const uint32_t Unit);

/***************************************************************************
  Group: SFMA driver port interface functions
*/

/***************************************************************************
  Function: R_SFMA_Sys_SpibscPortSet

  Description:
  This function sets the I/O signals of the SFMA.

  Parameter:
  DataBusWidth - The parameter specifies the data bus width.
  Bsz          - The parameter specifies the number of serial flash memories to be connected.

  Return value:
  See <r_sfma_Error_t>.
*/
r_sfma_Error_t R_SFMA_Sys_SpibscPortSet(const uint8_t  DataBusWidth,
                                        const uint32_t Bsz);


/***************************************************************************
  Group: SFMA driver OS interface functions
*/

/***************************************************************************
  Function: R_SFMA_Sys_Lock

  Description:
  This function locks the SFMA driver access to the specified unit for other threads.

  In the environment which is using OS, all drivers should implement the mutex
  or semaphore to this function to prevent re-entry to the driver function at
  the multi-threading.

  Parameter:
  Unit - The parameter specifies the instance number.

  Return value:
  See <r_sfma_Error_t>.
*/
r_sfma_Error_t R_SFMA_Sys_Lock(const uint32_t Unit);


/***************************************************************************
  Function: R_SFMA_Sys_Unlock

  Description:
  This function unlocks the SFMA driver access to the specified unit for other threads.

  In the environment which is using OS, all drivers should implement the mutex
  or semaphore to this function to prevent re-entry to the driver function at
  the multi-threading.

  Parameter:
  Unit - The parameter specifies the instance number.

  Return value:
  See <r_sfma_Error_t>.
*/
r_sfma_Error_t R_SFMA_Sys_Unlock(const uint32_t Unit);

/***************************************************************************
  Function: R_SFMA_Sys_AllLock

  Description:
  This function locks the SFMA driver access to all unit for other threads.

  In the environment which is using OS, all drivers should implement the mutex
  or semaphore to this function to prevent re-entry to the driver function at
  the multi-threading.

  Return value:
  See <r_sfma_Error_t>.
*/
r_sfma_Error_t R_SFMA_Sys_AllLock(void);


/***************************************************************************
  Function: R_SFMA_Sys_AllUnlock

  Description:
  This function unlocks the SFMA driver access to all unit for other threads.

  In the environment which is using OS, all drivers should implement the mutex
  or semaphore to this function to prevent re-entry to the driver function at
  the multi-threading.

  Return value:
  See <r_sfma_Error_t>.
*/
r_sfma_Error_t R_SFMA_Sys_AllUnlock(void);


/***************************************************************************
  Function: R_SFMA_Sys_Relax

  Description:
  This function enables to avoid the block of the CPU.
  This function is regularly called during execution of <R_SFMA_DataErase>, 
  <R_SFMA_DataWrite> and <R_SFMA_DataRead>.

  Parameter:
  Unit - The parameter specifies the instance number.

  Return value:
  See <r_sfma_Error_t>.
*/
r_sfma_Error_t R_SFMA_Sys_Relax(const uint32_t Unit);


/***************************************************************************
  Function: R_SFMA_Sys_GetCal

  Description:
  This function retrieves the calibration value from the CKDLY register

  Return value:
  uint32_t - Calibration value
*/
uint32_t R_SFMA_Sys_GetCal(const uint32_t Unit);

/***************************************************************************
  Function: R_SFMA_Sys_MinClockDivGet

  Description:
  This function gives back the minimum clock divider.

  Parameter:
  Unit         - The parameter specifies the instance number.
  Clock        - The parameter specifies the internal bus clock frequency(Hz).
  TransferMode - The parameter specifies the transfer mode.
                 SDR transfer.<0uL>
                 DDR transfer.<1uL>

  Return value:
  The clock division ratio.
*/
uint32_t R_SFMA_Sys_MinClockDivGet(const uint32_t Unit, const uint32_t Clock, const uint8_t TransferMode);

#ifdef __cplusplus
}
#endif

#endif /* R_SFMA_SYS_H */

