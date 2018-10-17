/*
****************************************************************************
PROJECT : JCUA driver
FILE    : $Id: r_sys_jcua.c 13592 2017-05-31 08:57:47Z shinya.tomari $
============================================================================

DESCRIPTION
JCUA support functions for d1x
============================================================================
                            C O P Y R I G H T
============================================================================
                           Copyright (c) 2013 - 2015
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
demonstration purposes at customers premises listed in the signed license
agreement.

****************************************************************************
*/

/***************************************************************************
  Title: JCUA Driver support functions

  Implementation of the JCUA driver support functions for the D1x device.
*/


/***************************************************************************
  Section: Includes
*/
#include    "r_typedefs.h"
#include    "r_jcua_api.h"
#include    "r_jcua_sys.h"
#include    "r_config_jcua.h"
#include    "r_dev_api.h"
#include    "r_tick_api.h"
#include    "r_ostm_api.h"

/***************************************************************************
  Section: Local Defines
*/
/***************************************************************************
  Constant: LOC_DEV_INT_ENABLE

  The offset value of the interrupt enable value.
*/
#define LOC_DEV_INT_ENABLE       (0x01u)

/***************************************************************************
  Constant: LOC_DEV_INT_DISABLE

  The offset value of the interrupt disable value.
*/
#define LOC_DEV_INT_DISABLE      (0x00u)


/***************************************************************************
  Constant: LOC_JCSWRST

  The address of the JCSWRST register (JCUA Software reset register)
*/
#define LOC_JCSWRST     (0xFFC06010u)

/***************************************************************************
  Constant: LOC_JCUA_JCUA0RES

  A constant for bit mask value and offset value of JCUA0RES(JCSWRST.JCUA0RES)

  LOC_JCUA_JCUA0RES_MASK  - Mask value.
  LOC_JCUA_JCUA0RES_SHIFT - Offset value.
*/
#define LOC_JCUA_JCUA0RES_MASK           (0x00000100u)
#define LOC_JCUA_JCUA0RES_SHIFT          (8)

/***************************************************************************
  Constant: LOC_JCUA_RESET_WAIT_TIME_US

  Wait time before JCSWRST.
*/
#define LOC_JCUA_RESET_WAIT_TIME_US       30u


/***************************************************************************
  Constant: LOC_JCUA_BUS_RESET_TIMEOUT_MS

  The timeout [in milliseconds] for JCUA bus reset.
  (JCUA Software reset via JCSWRST register)
*/
#define LOC_JCUA_BUS_RESET_TIMEOUT_MS     2u


/***************************************************************************
  Constant: 

  LOC_USE_OSTM               - Timer select. (0: use TICK0(OSTM0)  1: use OSTM1)
  LOC_OSTM_UNIT              - Use OSTM unit number.
  LOC_JCUA_HEADER_TIMEOUT_US - Timeout value of Header part. (in usec)
                               If timeout is not needed, please define as 0.
  LOC_JCUA_DATA_TIMEOUT_US   - Timeout value of Image Data part. (in usec)
                               If timeout is not needed, please define as 0.
*/

#define LOC_USE_OSTM                     (1)
#define LOC_OSTM_UNIT                    (1)
#define LOC_JCUA_HEADER_TIMEOUT_US       (4000u)      /* 4 msec */
#define LOC_JCUA_DATA_TIMEOUT_US         (0)          /* disable */

/***************************************************************************
  Section: Local Values
*/


#if (LOC_USE_OSTM == 1)
static uint32_t loc_TimerReached    = R_FALSE;
static uint32_t loc_PauseCouter     = 0;
static uint32_t loc_TiemOutCallback = R_FALSE;
#endif

/***************************************************************************
  Section: local Functions
*/

#if (LOC_USE_OSTM == 1)
/***************************************************************************
  Function: loc_OstmStop

  Description:
  This function stops OSTM driver.

  Parameter:
  Counter - Pointer to store the remain counter to reach timeout. 

  Return value:
  See <r_jcua_Error_t>.
*/
static r_jcua_Error_t loc_OstmStop(uint32_t* const Counter);
static r_jcua_Error_t loc_OstmStop(uint32_t* const Counter)
{
    r_jcua_Error_t     error = R_JCUA_ERR_OK;
    r_ostm_Error_t     ostm_err;
    
    ostm_err = R_OSTM_Stop(LOC_OSTM_UNIT);
    if (ostm_err == R_OSTM_ERR_OK)
    {
        if (Counter != R_NULL)
        {
            *Counter = R_OSTM_GetCounterValue(LOC_OSTM_UNIT);
        }
    }
    else
    {
        error = R_JCUA_ERR_TIMER_CTRL;
    }
    return error;
}

/***************************************************************************
  Function: loc_OstmCallback

  Description:
  This function is called when the setting time has arrived.
  It will be called from ISR.

  Parameter:
  None.

  Return value:
  None.
*/
static void loc_OstmCallback(void);
static void loc_OstmCallback(void)
{
    loc_TimerReached = R_TRUE;
    loc_OstmStop(R_NULL);
    
    if (loc_TiemOutCallback == R_TRUE)
    {
        /* Emergency Stop */
        R_JCUA_WRITE32(LOC_JCSWRST, LOC_JCUA_JCUA0RES_MASK);
        
        R_JCUA_IsrTimeOut(0);
        
        loc_TiemOutCallback = R_FALSE;
    }
}

/***************************************************************************
  Function: loc_OstmInit

  Description:
  This function initializes OSTM driver.

  Parameter:
  None.

  Return value:
  See <r_jcua_Error_t>.
*/
static r_jcua_Error_t loc_OstmInit(void);
static r_jcua_Error_t loc_OstmInit(void)
{
    r_jcua_Error_t     error = R_JCUA_ERR_OK;
    r_ostm_Error_t     ostm_err;
    r_ostm_Parameter_t ostm_prm;
    
    /* Init OS timer */
    ostm_prm.Mode   = R_OSTM_INTERVAL; 
    ostm_prm.CmpVal = 0;
    ostm_prm.IntCtl = R_OSTM_START_INT_DISABLE;
    
    ostm_err = R_OSTM_Init(LOC_OSTM_UNIT, &ostm_prm);
    if (ostm_err == R_OSTM_ERR_OK)
    {
        /* Set callback */
        ostm_err = R_OSTM_SetIsrCallback(LOC_OSTM_UNIT, R_OSTM_INT_TINT, &loc_OstmCallback);
        if (ostm_err == R_OSTM_ERR_OK)
        {
            ostm_err = R_OSTM_EnableInt(LOC_OSTM_UNIT, R_OSTM_INT_TINT);
        }
    }
    if (ostm_err != R_OSTM_ERR_OK)
    {
        error = R_JCUA_ERR_TIMER_CTRL;
    }
    return error;
}

/***************************************************************************
  Function: loc_OstmDeInit

  Description:
  This function deinitializes OSTM driver.

  Parameter:
  None.

  Return value:
  See <r_jcua_Error_t>.
*/
static r_jcua_Error_t loc_OstmDeInit(void);
static r_jcua_Error_t loc_OstmDeInit(void)
{
    r_jcua_Error_t     error = R_JCUA_ERR_OK;
    r_ostm_Error_t     ostm_err;
    
    ostm_err = R_OSTM_DisableInt(LOC_OSTM_UNIT, R_OSTM_INT_TINT);
    if (ostm_err == R_OSTM_ERR_OK)
    {
        /* Deinit OS timer */
        ostm_err = R_OSTM_DeInit(LOC_OSTM_UNIT);
    }
    if (ostm_err != R_OSTM_ERR_OK)
    {
        error = R_JCUA_ERR_TIMER_CTRL;
    }
    return error;
}

/***************************************************************************
  Function: loc_OstmStart

  Description:
  This function starts OSTM driver.
  If specified time or specified count is passed, <loc_OstmCallback> will called.

  Parameter:
  Usec  - Micro second time. If Usec != 0, Usec is used.
  Count - Count by OSTM clock frequency. If Usec == 0, Count is used.

  Return value:
  See <r_jcua_Error_t>.
*/
static r_jcua_Error_t loc_OstmStart(const uint32_t Usec, const uint32_t Count);
static r_jcua_Error_t loc_OstmStart(const uint32_t Usec, const uint32_t Count)
{
    r_jcua_Error_t     error = R_JCUA_ERR_OK;
    r_ostm_Error_t     ostm_err;
    uint32_t           cmp_val;
    uint32_t           ostm_clk;
    uint32_t           ostm_mhz;
    
    /* Get clock */
    ostm_clk = R_OSTM_GetClock(LOC_OSTM_UNIT);
    ostm_mhz = ostm_clk / 1000 / 1000; /* Hz->MHz */
    if (ostm_mhz != 0)
    {
        /* Set Compare value */
        if (Usec != 0)
        {
            cmp_val = ostm_mhz * Usec;
        }
        else if (Count != 0)
        {
            cmp_val = Count;
        }
        else
        {
            cmp_val = 1;
        }
        ostm_err = R_OSTM_SetCmp(LOC_OSTM_UNIT, cmp_val);
        if (ostm_err == R_OSTM_ERR_OK)
        {
            /* Timer start */
            loc_TimerReached = R_FALSE;
            ostm_err = R_OSTM_Start(LOC_OSTM_UNIT);
        }
        if (ostm_err != R_OSTM_ERR_OK)
        {
            error = R_JCUA_ERR_TIMER_CTRL;
        }
    }
    else
    {
        error = R_JCUA_ERR_TIMER_CTRL;
    }
    return error;
}
#endif /* LOC_USE_OSTM */


/***************************************************************************
  Section: JCUA Driver Support Functions
*/


/***************************************************************************
  Group: JCUA Basic interface functions
*/

/***************************************************************************
  Function: R_JCUA_Sys_Init

  see: <r_jcua_sys.h>
*/

r_jcua_Error_t R_JCUA_Sys_Init(const uint32_t Unit)
{
    r_jcua_Error_t error = R_JCUA_ERR_OK;
    uint32_t       val;

    if (Unit == 0)
    {
        /* Initialize Unit 0 of JCUA macro. */

        /* Bus Reset for JCUA (with timeout) */
        /* D1x OPC Rev5.01 item #12 "JCUA reset operation"
           -> JCCMD.BRST must not be used. (JCU macro internal reset not sufficent)
           -> JCSWRST.JCUA0RES must be used instead. (JCU macro external reset)
              Refer to D1x UM section 44.6 "Software Reset Processing".
                Use the following software reset procedure to reset the JCUA unit:
                0. Wait for 30us.
                1. Set JCSWRST.JCUA0RES bit to 1, to generate the software reset condition.
                2. Read JCSWRST.JCUA0RES bit until bit is set to 1, to wait for software reset is active.
                3. Set JCSWRST.JCUA0RES bit to 0, to release software reset condition.
                By this sequennce all JCUA registers with exception
                of the JCSWRST register are initialized by a software reset.
        */
#if (LOC_USE_OSTM == 0)
        uint32_t Timeout;
        Timeout = (R_TICK_GetTimeUS(0) + LOC_JCUA_RESET_WAIT_TIME_US);
        do {
            ;
        } while (Timeout > R_TICK_GetTimeUS(0));

        R_JCUA_WRITE32(LOC_JCSWRST, LOC_JCUA_JCUA0RES_MASK);
        val = 0;
        Timeout = (R_TICK_GetTimeMS(0) + LOC_JCUA_BUS_RESET_TIMEOUT_MS);
        while (0 == (val & LOC_JCUA_JCUA0RES_MASK)) {
            if (Timeout <= R_TICK_GetTimeMS(0))
            {
                /* timeout occured */
                return R_JCUA_ERR_BUS_TIMEOUT;
            }

            val = R_JCUA_READ32(LOC_JCSWRST);
        }
        R_JCUA_WRITE32(LOC_JCSWRST, 0u);
        
        /* Enable interrupt. */
        R_JCUA_Sys_InterruptEnable(0);

#else
        /* Initialize OS timer unit */
        error = loc_OstmInit();
        if (error == R_JCUA_ERR_OK)
        {
            loc_TiemOutCallback = R_FALSE;
            error = loc_OstmStart(LOC_JCUA_RESET_WAIT_TIME_US, 0);
            if (error == R_JCUA_ERR_OK)
            {
                /* 0. Wait for 30us. */
                while (loc_TimerReached == R_FALSE){};
                
                /* 1. Set JCSWRST.JCUA0RES bit to 1 */
                R_JCUA_WRITE32(LOC_JCSWRST, LOC_JCUA_JCUA0RES_MASK);
                
                error = loc_OstmStart(LOC_JCUA_BUS_RESET_TIMEOUT_MS * 1000, 0);
                if (error == R_JCUA_ERR_OK)
                {
                    /* 2. Read JCSWRST.JCUA0RES bit until bit is set to 1 */
                    val = 0;
                    while (0 == (val & LOC_JCUA_JCUA0RES_MASK)) {
                        if (loc_TimerReached == R_TRUE)
                        {
                            /* timeout occured */
                            loc_OstmStop(R_NULL);
                            return R_JCUA_ERR_BUS_TIMEOUT;
                        }

                        val = R_JCUA_READ32(LOC_JCSWRST);
                    }
                    error = loc_OstmStop(R_NULL);
                    if (error == R_JCUA_ERR_OK)
                    {
                        /* 3. Set JCSWRST.JCUA0RES bit to 0 */
                        R_JCUA_WRITE32(LOC_JCSWRST, 0u);
                        
                        /* Enable interrupt. */
                        R_JCUA_Sys_InterruptEnable(0);
                    }
                }
            }
        }
#endif
    }
    else
    {
        error = R_JCUA_ERR_RANGE_UNIT;
    }
    return error;
}


/***************************************************************************
  Function: R_JCUA_Sys_DeInit

  see: <r_jcua_sys.h>
*/

r_jcua_Error_t R_JCUA_Sys_DeInit(const uint32_t Unit)
{
    r_jcua_Error_t error = R_JCUA_ERR_OK;

    if (Unit == 0)
    {
        /* De-Initialize Unit 0 of JCUA macro. */
        /* Disable interrupt. */
        R_JCUA_Sys_InterruptDisable(0);
#if (LOC_USE_OSTM == 1)
        error = loc_OstmDeInit();
#endif
    }
    else
    {
        error = R_JCUA_ERR_RANGE_UNIT;
    }
    return error;
}


/***************************************************************************
  Function: R_JCUA_Sys_InterruptEnable

  see: <r_jcua_sys.h>
*/

void R_JCUA_Sys_InterruptEnable(const uint32_t Unit)
{
    if (Unit == 0)
    {
        R_DEV_IntEnable(R_DEV_INT_JCU0EDI, LOC_DEV_INT_ENABLE);
        R_DEV_IntEnable(R_DEV_INT_JCU0DTI, LOC_DEV_INT_ENABLE);
    } /* end if */
}


/***************************************************************************
  Function: R_JCUA_Sys_InterruptDisable

  see: <r_jcua_sys.h>
*/

void R_JCUA_Sys_InterruptDisable(const uint32_t Unit)
{
    if (Unit == 0)
    {
        R_DEV_IntEnable(R_DEV_INT_JCU0DTI, LOC_DEV_INT_DISABLE);
        R_DEV_IntEnable(R_DEV_INT_JCU0EDI, LOC_DEV_INT_DISABLE);
    } /* end if */
}

/***************************************************************************
  Function: R_JCUA_Sys_BaseAddrGet

  see: <r_jcua_sys.h>
*/
uint32_t R_JCUA_Sys_BaseAddrGet(const uint32_t Unit)
{
    uint32_t base_addr = 0;

    if (Unit == 0)
    {
        base_addr = R_JCUA_BASE_ADD;
    }
    return base_addr;
}

/***************************************************************************
  Function: R_JCUA_Sys_TimerStart

  see: <r_jcua_sys.h>
*/

r_jcua_Error_t R_JCUA_Sys_TimerStart(const uint32_t Unit, const uint32_t IsHeader)
{
    r_jcua_Error_t error = R_JCUA_ERR_OK;
    
    if (Unit == 0)
    {
#if (LOC_USE_OSTM == 1)
        if (IsHeader == R_TRUE)
        {
#if (LOC_JCUA_HEADER_TIMEOUT_US != 0)
            loc_TiemOutCallback = R_TRUE;
            error = loc_OstmStart(LOC_JCUA_HEADER_TIMEOUT_US, 0);
#endif
        }
        else
        {
#if (LOC_JCUA_DATA_TIMEOUT_US != 0)
            loc_TiemOutCallback = R_TRUE;
            error = loc_OstmStart(LOC_JCUA_DATA_TIMEOUT_US, 0);
#endif
        }
#endif /* LOC_USE_OSTM */
    }
    else
    {
        error = R_JCUA_ERR_RANGE_UNIT;
    }
    
    return error;
}

/***************************************************************************
  Function: R_JCUA_Sys_TimerPause

  see: <r_jcua_sys.h>
*/
r_jcua_Error_t R_JCUA_Sys_TimerPause(const uint32_t Unit)
{
    r_jcua_Error_t error = R_JCUA_ERR_OK;
    
    if (Unit == 0)
    {
#if (LOC_USE_OSTM == 1)
        error = loc_OstmStop(&loc_PauseCouter);
#endif
    }
    else
    {
        error = R_JCUA_ERR_RANGE_UNIT;
    }
    
    return error;
}

/***************************************************************************
  Function: R_JCUA_Sys_TimerResume

  see: <r_jcua_sys.h>
*/
r_jcua_Error_t R_JCUA_Sys_TimerResume(const uint32_t Unit)
{
    r_jcua_Error_t error = R_JCUA_ERR_OK;
    
    if (Unit == 0)
    {
#if (LOC_USE_OSTM == 1)
        if (loc_TiemOutCallback == R_TRUE)
        {
            error = loc_OstmStart(0, loc_PauseCouter);
        }
#endif
    }
    else
    {
        error = R_JCUA_ERR_RANGE_UNIT;
    }
    
    return error;
}

/***************************************************************************
  Function: R_JCUA_Sys_TimerStop

  see: <r_jcua_sys.h>
*/
r_jcua_Error_t R_JCUA_Sys_TimerStop(const uint32_t Unit)
{
    r_jcua_Error_t error = R_JCUA_ERR_OK;
    
    if (Unit == 0)
    {
#if (LOC_USE_OSTM == 1)
        loc_TiemOutCallback = R_FALSE;
        error = loc_OstmStop(R_NULL);
#endif
    }
    else
    {
        error = R_JCUA_ERR_RANGE_UNIT;
    }
    
    return error;
}

/***************************************************************************
  Group: JCUA OS interface
*/


/***************************************************************************
  Function: R_JCUA_Sys_Lock

  see: <r_jcua_sys.h>
*/

r_jcua_Error_t R_JCUA_Sys_Lock(const uint32_t Unit)
{
    r_jcua_Error_t error = R_JCUA_ERR_OK;

    /* Check parameters. */
    if (Unit >= R_JCUA_MACRO_NUM)
    {
        error = R_JCUA_ERR_RANGE_UNIT;
    } /* end if */

    return error;
}


/***************************************************************************
  Function: R_JCUA_Sys_Unlock

  see: <r_jcua_sys.h>
*/

r_jcua_Error_t R_JCUA_Sys_Unlock(const uint32_t Unit)
{
    r_jcua_Error_t error = R_JCUA_ERR_OK;

    /* Check parameters. */
    if (Unit >= R_JCUA_MACRO_NUM)
    {
        error = R_JCUA_ERR_RANGE_UNIT;
    } /* end if */
    return error;
}

