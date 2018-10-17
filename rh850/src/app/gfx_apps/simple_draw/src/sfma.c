/************************************************************************************************
PROJECT : simple_draw
FILE    : $Id: sfma.c 13335 2017-04-24 07:17:12Z shinya.tomari $
============================================================================ 
DESCRIPTION
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

/*******************************************************************************
  Section: Includes
*/

#include "r_typedefs.h"        /* Renesas basic types, e.g. uint32_t */
#include "r_bsp_api.h"         /* Board support package */
#include "r_sfma_api.h"
#include "r_sfcdb_macronix.h"

#include "config.h"
#include "main.h"
#include "r_gpio_api.h"
/***********************************************************
  Section: Local Variables
*/
static uint32_t loc_srcData   [4096];
static uint32_t loc_verifyData[4096];
static uint32_t u32temp[8432];

/***********************************************************
  Section: Local Defines
*/
#define LOC_SFMA_SECTOR_SIZE  (65536u)
#if (LOC_SFMA_UNIT == 0)
#define LOC_SF_ADDRESS_EX     (0x10000000)
#else
#define LOC_SF_ADDRESS_EX     (0xC0000000)
#endif

/***************************************************************************
  Constant: SF_DEFAULT_CAL

  Default calibration for Serial flash memory (Macronix device only).

    SPBCLK clock phase shift        3 (range 0 - 7, written to the upper 16bits)
    Sampling point phase shift      1 (range 0 - 7, written to the Lower 16 bits)
*/
#define SF_SPBCLK_CLOCK_PHASE_SHIFT    (3u)
#define SF_SAMPLING_POINT_PHASE_SHIFT  (1u)
#define LOC_SFMA_DEFAULT_CAL    ((SF_SPBCLK_CLOCK_PHASE_SHIFT << 16) | SF_SAMPLING_POINT_PHASE_SHIFT)


/***********************************************************
  Section: Local Functions
*/
static void loc_SFMAConfig(r_sfma_Config_t *Config, r_sfma_Mode_t Mode, r_sfma_DataTransferMode_t Transfer);
/*******************************************************************************
    Function: SetSFMAConfig

    Fills the flash device config and command structure according to the target
    device.
 */
static void loc_SFMAConfig(r_sfma_Config_t *Config, r_sfma_Mode_t Mode, r_sfma_DataTransferMode_t Transfer)
{
    if(Config != R_NULL)
    {
        Config->Mode                        = Mode;
        Config->MemoryNum                   = R_SFMA_MEMORY_DUAL;
        Config->DataTransferMode            = Transfer;
        Config->AddressMode                 = R_SFMA_ADDRESS_32BIT;
        Config->SerialFlashMemoryPageSize   = 0x100uL; /* 256Byte */
        Config->SerialFlashMemoryMaxClock   = (80*1000*1000);
        Config->CacheMode                   = R_SFMA_CACHE_OFF;
        Config->PerformanceEnMode           = R_SFMA_PER_EN_MODE_DISABLE;
        Config->Calibration                 = LOC_SFMA_DEFAULT_CAL;

        if(R_SFMA_MEMORY_DUAL == Config->MemoryNum)
        {
            Config->SerialFlashMemorySectorSize = LOC_SFMA_SECTOR_SIZE * 2;
            Config->SerialFlashMemorySize       = 128*1024*1024;
        }
        else
        {
            Config->SerialFlashMemorySectorSize = LOC_SFMA_SECTOR_SIZE;
            Config->SerialFlashMemorySize       = 64*1024*1024;
        }

        if (R_SFMA_ADDRESS_32BIT == Config->AddressMode)
        {
            if ((40*1000*1000) < Config->SerialFlashMemoryMaxClock)
            {
                if ((Config->DataTransferMode == R_SFMA_SDR_QUAD_IO) ||
                    (Config->DataTransferMode == R_SFMA_DDR_QUAD_IO))
                {
                    /* at 80MHZ with external address space use performance enhance mode */
                    if(R_SFMA_MODE_EX_SPACE == Mode)
                    {
                        Config->Command = &r_sfc_Macronix_32bitAddrCmdPerformanceEnhance_80MHz;
                        Config->PerformanceEnMode = R_SFMA_PER_EN_MODE_ENABLE;
                    }
                    else
                    {
                        Config->Command = &r_sfc_Macronix_32bitAddrQuadCmd_80MHz;
                    }
                }
                else
                {
                    Config->Command = &r_sfc_Macronix_32bitAddrCmd_80MHz;
                }
            }
            else
            {
                if ((Config->DataTransferMode == R_SFMA_SDR_QUAD_IO) ||
                    (Config->DataTransferMode == R_SFMA_DDR_QUAD_IO))
                {
                    Config->Command = &r_sfc_Macronix_32bitAddrQuadCmd_40MHz;
                }
                else
                {
                    Config->Command = &r_sfc_Macronix_32bitAddrCmd_40MHz;
                }
            }
        }
        else
        {
            if ((40*1000*1000) < Config->SerialFlashMemoryMaxClock)
            {
                if ((Config->DataTransferMode == R_SFMA_SDR_QUAD_IO) ||
                    (Config->DataTransferMode == R_SFMA_DDR_QUAD_IO))
                {
                    Config->Command = &r_sfc_Macronix_24bitAddrQuadCmd_80MHz;
                }
                else
                {
                    Config->Command = &r_sfc_Macronix_24bitAddrCmd_80MHz;
                }
            }
            else
            {
                if ((Config->DataTransferMode == R_SFMA_SDR_QUAD_IO) ||
                    (Config->DataTransferMode == R_SFMA_DDR_QUAD_IO))
                {
                    Config->Command = &r_sfc_Macronix_24bitAddrQuadCmd_40MHz;
                }
                else
                {
                    Config->Command = &r_sfc_Macronix_24bitAddrCmd_40MHz;
                }
            }
        }
    }
}


static void loc_SFMAConfig_Test(r_sfma_Config_t *Config, r_sfma_Mode_t Mode, r_sfma_DataTransferMode_t Transfer)
{
    if(Config != R_NULL)
    {
        Config->Mode                        = Mode;
        Config->MemoryNum                   = R_SFMA_MEMORY_SINGLE;
        Config->DataTransferMode            = Transfer;
        Config->AddressMode                 = R_SFMA_ADDRESS_24BIT;
        Config->SerialFlashMemoryPageSize   = 0x100uL; /* 256Byte */
        Config->SerialFlashMemoryMaxClock   = (80*1000*1000);
        Config->CacheMode                   = R_SFMA_CACHE_OFF;
        Config->PerformanceEnMode           = R_SFMA_PER_EN_MODE_DISABLE;
        Config->Calibration                 = LOC_SFMA_DEFAULT_CAL;


        Config->SerialFlashMemorySectorSize = LOC_SFMA_SECTOR_SIZE;
        Config->SerialFlashMemorySize       = 128*1024*1024;
        

        Config->Command = &r_sfc_Micron_24bitAddrStrCmd_80MHz;
 
    }
}
/*******************************************************************************
    Function: loc_CompareData32

    Compare two 32-bit arrays.
    Returns index of first mismatch or length if arrays are equal.
 */
uint32_t loc_CompareData32(uint32_t a[], uint32_t b[], uint32_t length32)
{
    uint32_t i;

    for(i = 0; i < length32; i++)
    {
        if ( a[i] != b[i] )
        {
            return i;
        }
    }
    return length32;
}
 
/*******************************************************************************
    Function: loc_StartSFMA

    Opens SFMA for writing and writes a pattern to it.
    Afterwards SFMA is closed but opened for reading again, so that
    the application can use it.
 */

void* loc_StartSFMA(void)
{
    r_sfma_Error_t err = R_SFMA_ERR_OK;
    uint32_t sf_adr    = 0x00000000;
    r_sfma_Config_t cfg;
    uint32_t i;

    /* fill color: olive */
    for(i = 0; i < 4096; i++)
    {
        loc_srcData[i] = 0xff808000;
    }

    err |= R_SFMA_Init(LOC_SFMA_UNIT);

    /* open for writing */
    loc_SFMAConfig(&cfg, R_SFMA_MODE_SPI_OPERATING, R_SFMA_DDR_SINGLE_IO);
    err |= R_SFMA_Open(LOC_SFMA_UNIT, &cfg);
    err |= R_SFMA_ProtectionModeSet(LOC_SFMA_UNIT, R_SFMA_MODE_UNPROTECT);
    
    /* check if data is already there & program only if data is missing*/
/*    err |= R_SFMA_DataRead(LOC_SFMA_UNIT, sf_adr, (uint8_t*)loc_verifyData, sizeof(loc_srcData));
    if (4096 != loc_CompareData32(loc_srcData, loc_verifyData, 4096))
    {
        // write data to serial flash 
        err |= R_SFMA_DataErase(LOC_SFMA_UNIT, sf_adr, sizeof(loc_srcData));
        err |= R_SFMA_DataWrite(LOC_SFMA_UNIT, sf_adr, (uint8_t*)loc_srcData, sizeof(loc_srcData));
    
        // verify the data is correct 
        err |= R_SFMA_DataRead(LOC_SFMA_UNIT, sf_adr, (uint8_t*)loc_verifyData, sizeof(loc_srcData));
        if (4096 != loc_CompareData32(loc_srcData, loc_verifyData, 4096))
        {
            err |= R_SFMA_ERR_NG;
        }
    }
*/    
    err |= R_SFMA_Close(LOC_SFMA_UNIT);

    /* open again for reading */
    loc_SFMAConfig(&cfg, R_SFMA_MODE_EX_SPACE, R_SFMA_DDR_QUAD_IO);
    err |= R_SFMA_Open(LOC_SFMA_UNIT, &cfg);
    err |= R_SFMA_AccessAddressSet(LOC_SFMA_UNIT, sf_adr, R_SFMA_ACCESS_RANGE_128MB);

    if(err != R_SFMA_ERR_OK)
    {
        loc_Error((int32_t)err);
    }
    return (void *)(LOC_SF_ADDRESS_EX + sf_adr);
}


/*******************************************************************************
    Function: loc_StopSFMA

    Closes and stops SFMA
 */
void loc_StopSFMA(void)
{
    r_sfma_Error_t err = R_SFMA_ERR_OK;

    err |= R_SFMA_Close(LOC_SFMA_UNIT);
    err |= R_SFMA_DeInit(LOC_SFMA_UNIT);

    if(err != R_SFMA_ERR_OK)
    {
        loc_Error((int32_t)err);
    }
}

void get_flash_2(uint32_t *pdate_address , uint32_t u32size)
{
    //uint32_t u32delay;
    uint32_t u32i;
    
    for(u32i = 0; u32i < u32size;)
    {
        u32temp[u32i] = *(pdate_address + u32i);
        u32i += 1;
    }
}

void* loc_StartSFMA2(void)
{
    r_sfma_Error_t err = R_SFMA_ERR_OK;
    uint32_t sf_adr    = 0x00000000;
    r_sfma_Config_t cfg;
    //uint32_t i;
    //uint8_t u8FlashID;
    //uint8_t u8FlashType;
    //uint8_t u8FlashCapacity;
    
    /* Initial SFMA2 function */
    err |= R_SFMA_Init(LOC_SFMA_UNIT);

#if 0
//Change_ImgDataHiLo();
    // For erasing, reading and writing date by using SPI API.
    /* fill color: olive */
 /*   for(i = 0; i < 8192; i++)
    {
        loc_srcData[i] = 0xff555555;//i;
    }
*/
    /* Loading SDR standrad table */
    loc_SFMAConfig_Test(&cfg, R_SFMA_MODE_SPI_OPERATING, R_SFMA_SDR_SINGLE_IO);
    err |= R_SFMA_Open(LOC_SFMA_UNIT, &cfg);
    R_SFMA_JEDECRead(LOC_SFMA_UNIT,&u8FlashID,&u8FlashType,&u8FlashCapacity);
    
    if ( R_SFC_MIRCON_MANUFACTURE_ID != u8FlashID                ||
         R_SFC_MACRONIX_MT25QL02GCBB_MEMORYTYPE_ID != u8FlashType||
         R_SFC_MACRONIX_MT25QL02GCBB_CAPACITY_ID != u8FlashCapacity)
    {
        err |= R_SFMA_ERR_NG;
    }
    err |= R_SFMA_Close(LOC_SFMA_UNIT);

    
    /* Loading SDR Quad table */  
    loc_SFMAConfig_Test(&cfg, R_SFMA_MODE_SPI_OPERATING, R_SFMA_SDR_QUAD_IO);
    err |= R_SFMA_Open(LOC_SFMA_UNIT, &cfg);
    err |= R_SFMA_ProtectionModeSet(LOC_SFMA_UNIT, R_SFMA_MODE_UNPROTECT);

    err |= R_SFMA_DataErase(LOC_SFMA_UNIT, sf_adr, sizeof(Icon1_ImgData));
    err |= R_SFMA_DataWrite(LOC_SFMA_UNIT, sf_adr, (uint8_t*)Icon1_ImgData, sizeof(Icon1_ImgData));

    /* verify the data is correct */
    err |= R_SFMA_DataRead(LOC_SFMA_UNIT, sf_adr, (uint8_t*)loc_verifyData, sizeof(loc_verifyData));

    err |= R_SFMA_Close(LOC_SFMA_UNIT);

#else

    // For access data by using point
    loc_SFMAConfig_Test(&cfg, R_SFMA_MODE_EX_SPACE, R_SFMA_SDR_QUAD_IO);
    err |= R_SFMA_Open(LOC_SFMA_UNIT, &cfg);
    err |= R_SFMA_AccessAddressSet(LOC_SFMA_UNIT, sf_adr, R_SFMA_ACCESS_RANGE_256MB);
    get_flash_2((uint32_t *)0xC00AB530,32);
return (void *)(LOC_SF_ADDRESS_EX + sf_adr);

#endif
}
