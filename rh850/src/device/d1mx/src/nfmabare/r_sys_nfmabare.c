/*
****************************************************************************
PROJECT : VLIB
FILE ID : $Id: r_sys_tick.c 7402 2016-01-27 15:43:39Z florian.zimmermann $
AUTHOR  : $Author: florian.zimmermann $
============================================================================ 
DESCRIPTION
NFMA driver - device dependent part for D1x
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

#include "r_typedefs.h"
#include "r_nfma_bare_sys.h"
#include "r_dev_api.h"

#pragma ghs startnomisra
#include "v800_ghs.h"   /* __GETSR() */
#pragma ghs endnomisra

#if defined (R_DBG_PRINT_DEV_REGS) || defined (R_DBG_PRINT_MSG)
   #include <stdio.h> 
   #include "r_dbg_api.h"
#endif

/*******************************************************************************
  Section: Local Variables 
*/

/*******************************************************************************
  Section: Local Functions 
*/

/*******************************************************************************
  Title: NFMA Support Functions

  Implementation of the NFMA support functions for the D1x device 
*/

/*******************************************************************************
  Function: R_SYS_NFMA_GetBaseAddr
*/
uint32_t R_SYS_NFMA_GetBaseAddr(uint32_t instance)
{
    return 0xFFDE2000u;
}

/*******************************************************************************
  Function: R_SYS_NFMA_GetClkFreqMHz
*/
uint32_t R_SYS_NFMA_GetClkFreqMHz(uint32_t instance)
{
    /* NFMA Clock in MHz */
    return 120;     /* R_NFMA_CLKB_MHZ */
}

