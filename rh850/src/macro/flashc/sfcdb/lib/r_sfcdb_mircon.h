/*
****************************************************************************
PROJECT : SFMA driver
FILE    : $Id: r_sfcdb_mircon.h 13535 2017-05-23 10:30:55Z shinya.tomari $
============================================================================
DESCRIPTION
Flash driver for a mircon device
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
  Title: Serial flash command table

  Note: This file defines the macro define and the flash command table for
         Mircon MT25QL02GCBB serial flash device.
        If you use other serial flash device of Mircon, change setting
        parameters according to the model number of the target.

*/

#ifndef R_SFCDB_MIRCON_H
#define R_SFCDB_MIRCON_H

#ifdef __cplusplus
extern "C" {
#endif


/***************************************************************************
  Section: Global Defines
*/

/***************************************************************************
  Constant: R_SFC_MIRCON_MANUFACTURE_ID

  Manufacture ID.
*/
#define R_SFC_MIRCON_MANUFACTURE_ID (0x20)                  // Mircon

/***************************************************************************
  Constant: R_SFC_MIRCON_MEMORYTYPE_ID

  Memori type ID.
*/
#define R_SFC_MIRCON_MT25QL02GCBB_MEMORYTYPE_ID (0xBA)    // 3.3V 
/***************************************************************************
  Constant: R_SFC_MIRCON_CAPACITY_ID

  Capacity ID.
*/
#define R_SFC_MACRON_MT25QL02GCBB_CAPACITY_ID  (0x22)     // 2Gbits = 256MBytes
/***************************************************************************
  Constant: R_SFC_MIRCON_PAGE_SIZE

  Page size(Byte).
*/
#define R_SFC_MIRCON_PAGE_SIZE (256)                        // 256Byte

/***************************************************************************
  Constant: R_SFC_MIRCON_SECTOR_SIZE

  Erase sector size(Byte).
*/
#define R_SFC_MIRCON_SECTOR_SIZE (64 * 1024)                // 64MByte

/***************************************************************************
  Constant: R_SFC_MIRCON_FLASH_MEMORY_SIZE

  Serial flash memory size(Byte).
*/
#define R_SFC_MIRCON_FLASH_MEMORY_SIZE (256 * 1024 * 1024)  // 256MByte
/***************************************************************************
  Constant: Status Mask Define
*/
#define WRITE_ENABLE_MSK        0x80
#define PROTECT_AREA_MSK        0x7C
#define WRITE_ENABLE_LATCH_MSK  0x02
#define WRITE_IN_PROGRESS       0x01
/***************************************************************************
  Constant: Nonvolatile Configuration Mask Define
*/
#define DUMMY_CYCLE_MSK         0xF000
#define XIP_MODE_MSK            0x0E00
#define OUTPUT_DRV_MSK          0x01C0
#define DTR_MSK                 0x0020
#define RESET_HOLD_ENABLE_MSK   0x0010
#define QUAD_IO_ENABLE_MSK      0x0008
#define DUAL_IO_ENABLE_MSK      0x0004
#define SEL_128Mb_SEGMENT_MSK   0x0002
#define ADDRESS_BYTE_MSK        0x0001
/***************************************************************************
  Constant: Status Define
*/
#define WRITE_ENABLE            0x00
#define WRITE_DISABLE           0x80

#define BP_NONE                 0x20
#define BP_ALL                  0x5C

#define WRITE_LATCH_ENABLE      0x02
#define WRITE_LATCH_DISABLE     0x00

/***************************************************************************
  Constant: Nonvolatile Configuration Define
*/
#define DUMMY_01CYCLE           0x1000
#define DUMMY_02CYCLE           0x2000
#define DUMMY_03CYCLE           0x3000
#define DUMMY_04CYCLE           0x4000
#define DUMMY_05CYCLE           0x5000
#define DUMMY_06CYCLE           0x6000
#define DUMMY_07CYCLE           0x7000
#define DUMMY_08CYCLE           0x8000
#define DUMMY_09CYCLE           0x9000
#define DUMMY_10CYCLE           0xA000
#define DUMMY_11CYCLE           0xB000
#define DUMMY_12CYCLE           0xC000
#define DUMMY_13CYCLE           0xD000
#define DUMMY_14CYCLE           0xE000

#define XIP_FAST_READ           0x0000
#define XIP_DUAL_O_FAST_READ    0x0100
#define XIP_DUAL_IO_FAST_READ   0x0200
#define XIP_QUAD_O_FAST_READ    0x0300
#define XIP_QUAD_IO_FAST_READ   0x0400
#define XIP_DISABLE             0x0700

#define DRV_90_OHMS             0x0040
#define DRV_45_OHMS             0x0080
#define DRV_20_OHMS             0x0140
#define DRV_30_OHMS             0x01C0

#define DTR_ENABLE              0x0000
#define DTR_DISABLE             0x0020

#define RESET_HOLD_ENABLE       0x0010
#define RESET_HOLD_DISABLE      0x0000

#define QUAD_IO_ENABLE          0x0000
#define QUAD_IO_DISABLE         0x0008

#define DAUL_IO_ENABLE          0x0000
#define DAUL_IO_DISABLE         0x0004

#define HIGH_128Mb              0x0000
#define LOW_128Mb               0x0002

#define SEL_4BYTES_ADDRESS      0x0000
#define SEL_3BYTES_ADDRESS      0x0001
/***************************************************************************
  Section: Global Variables
*/

#ifdef __cplusplus
}
#endif

#endif /* R_SFCDB_MACRONIX_H */

