/*
****************************************************************************
PROJECT : RLIN3 driver
FILE    : $Id$
============================================================================ 
DESCRIPTION
Driver for RLIN3 macro

============================================================================
                            C O P Y R I G H T                                    
============================================================================
                           Copyright (c) 2012
                                  by 
                       Renesas Electronics (Europe) GmbH. 
                           Arcadiastrasse 10
                          D-40472 Duesseldorf
                               Germany
                          All rights reserved.
============================================================================
Purpose: only for testing

DISCLAIMER                                                                   
This software is supplied by Renesas Electronics Corporation and is only     
intended for use with Renesas products. No other uses are authorized. This   
software is owned by Renesas Electronics Corporation and is protected under  
all applicable laws, including copyright laws.                               
THIS SOFTWARE IS PROVIDED "AS IS" AND RENESAS MAKES NO WARRANTIES REGARDING  
THIS SOFTWARE, WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING BUT NOT      
LIMITED TO WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE   
AND NON-INFRINGEMENT. ALL SUCH WARRANTIES ARE EXPRESSLY DISCLAIMED.          
TO THE MAXIMUM EXTENT PERMITTED NOT PROHIBITED BY LAW, NEITHER RENESAS       
ELECTRONICS CORPORATION NOR ANY OF ITS AFFILIATED COMPANIES SHALL BE LIABLE  
FOR ANY DIRECT, INDIRECT, SPECIAL, INCIDENTAL OR CONSEQUENTIAL DAMAGES FOR   
ANY REASON RELATED TO THIS SOFTWARE, EVEN IF RENESAS OR ITS AFFILIATES HAVE  
BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.                             
Renesas reserves the right, without notice, to make changes to this software 
and to discontinue the availability of this software. By using this software,
you agree to the additional terms and conditions found by accessing the      
following link:                                                              
http://www.renesas.com/disclaimer *                                          
Copyright (C) 2011 Renesas Electronics Corporation. All rights reserved.     

****************************************************************************

****************************************************************************
*/

/*******************************************************************************
  Title: Main Module

  Implementation of the API and other global functions.
*/

/*******************************************************************************
  Section: Includes
*/

#include "r_typedefs.h"
#include "r_cdi_api.h"
#include "r_config_rlin3.h"
#include "r_rlin3_regs.h"
#include "r_rlin3_api.h"
#define RLIN3_MAIN_GLOBALS
#include "r_rlin3_main.h"
#include "r_rlin3_sys.h"
#include "r_dev_api.h"

#if defined (R_DBG_PRINT_DEV_REGS) || defined (R_DBG_PRINT_MSG)
   #include <stdio.h> 
   #include "r_dbg_api.h"
#endif


/*******************************************************************************
  Section: Local Defines
*/

/*******************************************************************************
  Constants: Bit flags
  
  LOC_ENABLE     - Used to set configuration bit
  LOC_DISABLE    - Used to clear configuration bit
*/

#define LOC_ENABLE   1  
#define LOC_DISABLE  0


/*******************************************************************************
  Section: Local Variables
*/

/*******************************************************************************
  Variable: loc_ErrorHandler

  A function pointer to a user callback (Applications error handler)  
  
  The value is assigned by the application using the driver 
*/

static void (*loc_ErrorHandler )(uint32_t Unit, uint32_t Error) = 0;


/*******************************************************************************
  Section: Local Functions
*/
static uint32_t loc_SecureRbWrite(
            uint32_t                 Unit,
            r_cdi_RBuf_t        *RBuf, 
            uint8_t             *Buffer, 
            uint32_t             Size,
            r_rlin3_Interrupt_t  IntType);
    
static uint32_t loc_SecureRbRead(
    uint32_t                 Unit,
    r_cdi_RBuf_t        *RBuf, 
    uint8_t             *Buffer, 
    uint32_t             Size,
    r_rlin3_Interrupt_t  IntType); 
     
/*******************************************************************************
  Function: loc_SecureRbWrite

  Writes data to the ring buffer with concurrent access of
  an ISR to the same buffer.

  The function writes to the ring buffer and makes sure, that
  no 'IntType' interrupt can occur during that operation. This is
  required to keep data in the ring buffer consistent.

  Parameters:
  Unit       - Instance number
  RBuf       - Ring buffer
  Buffer     - Data buffer
  Size       - Number of bytes in data buffer
  IntType    - Specified interrupt

  Returns:
  Number of characters, which have been copied.
*/

static uint32_t loc_SecureRbWrite
(
    uint32_t                 Unit,
    r_cdi_RBuf_t        *RBuf, 
    uint8_t             *Buffer, 
    uint32_t             Size,
    r_rlin3_Interrupt_t  IntType) 
{
    uint32_t len;
    
    R_RLIN3_DisableInt(Unit, IntType);
    len = R_CDI_RbWrite(RBuf, Buffer, Size);
    R_RLIN3_EnableInt(Unit, IntType);
    return len;
}


/*******************************************************************************
  Function: loc_SecureRbRead

  Read data from the ring buffer with concurrent access of
  an ISR to the same buffer.

  The function reads data from a ring buffer and makes sure, 
  that no 'IntType' interrupt can occur during that operation. 
  This is required to keep data in the ring buffer consistent.

  Parameters:
  Unit       - Instance number
  RBuf       - Ring buffer
  Buffer     - Data buffer
  Size       - Max number to be read from the buffer
  IntType    - Specified interrupt
  
  Returns:
  Number of characters, which have been copied.
*/

static uint32_t loc_SecureRbRead(
    uint32_t                 Unit,
    r_cdi_RBuf_t        *RBuf, 
    uint8_t             *Buffer, 
    uint32_t             Size,
    r_rlin3_Interrupt_t  IntType) 
{
    uint32_t len;
    
    R_RLIN3_DisableInt(Unit, IntType);
    len = R_CDI_RbRead(RBuf, Buffer, Size);
    R_RLIN3_EnableInt(Unit, IntType);
    return len;
}

/*******************************************************************************
  Section: Global Functions
*/

/*******************************************************************************
  Function: R_RLIN3_ErrorHandler
  
  Comment see rlin3_main.h
*/

void R_RLIN3_ErrorHandler(uint32_t Unit, r_rlin3_Error_t Error) 
{
    if (loc_ErrorHandler != 0) 
    {      /* we've got a registered error handler here ...*/
        loc_ErrorHandler(Unit, (uint32_t)Error);      /* ... and we gonna use it!*/
    } 
    else 
    {
        /* Some severe "RLIN3: Error occured" */
        R_DEV_ErrorHandler(Unit, (uint32_t)Error);
    }
}


/*******************************************************************************
  Section: Global API Functions
  
  For API documentation see rlin3_api.h
*/

/*******************************************************************************
  Function: R_RLIN3_Init
*/

r_rlin3_Error_t R_RLIN3_Init(uint32_t Unit, r_rlin3_Parameter_t *Config) 
{
    uint32_t             base;
    uint8_t              regval8;
    uint32_t             err;
    uint32_t             clk_in    = 0;       /* macro input clock    */  
    uint32_t             baudrate  = 0;       /* resulting baudrate    */  
    uint32_t             frq       = 0;       /* macro frequency (clk_in/lprs)   */
    uint16_t             brp       = 0;       /* baud rate prescaler  */
    uint16_t             bsc       = 0;       /* bit sampling count   */
    uint8_t              lprs      = 0;       /* prescaler for clk_in */ 
    uint8_t              found     = 0;       /* flag */ 
    
    base = R_SYS_RLIN3_BaseAddr(Unit);
    
    r_rlin3_Dev[Unit].DrvExtension = 0; 
    
    r_rlin3_Dev[Unit].TxOngoing = 0;
    if ((R_BAD_ADDRESS == base) || (R_RLIN3_MACRO_NUM < Unit)) 
    {
        R_RLIN3_ErrorHandler(Unit, R_RLIN3_ERR_RANGE);
        return R_RLIN3_ERR_RANGE;
    } 
    else 
    {
        r_rlin3_Dev[Unit].BaseAddr = base;
        R_RLIN3_ResetRelease(Unit); 
        R_RLIN3_Disable(Unit); 
        R_RLIN3_Reset(Unit); 
        
        err = R_CDI_RbSetup(&r_rlin3_Dev[Unit].RxBuf, r_rlin3_Dev[Unit].RxBuffer, 
                            R_RLIN3_RXBUF_SIZE, 1);
        if (0 != err) 
        {
            R_RLIN3_ErrorHandler(Unit, R_RLIN3_ERR_RBINIT); 
            return R_RLIN3_ERR_RANGE;
        }
        err = R_CDI_RbSetup(&r_rlin3_Dev[Unit].TxBuf, r_rlin3_Dev[Unit].TxBuffer, 
                            R_RLIN3_TXBUF_SIZE, 1);
        if (0 != err) 
        {
            R_RLIN3_ErrorHandler(Unit, R_RLIN3_ERR_RBINIT);
            return R_RLIN3_ERR_RANGE;
        }
        
        clk_in = 0;
        clk_in = R_SYS_RLIN3_SysGetClockHz(Unit);
        for (lprs = 0; lprs < 8; lprs++)
        {
            frq = clk_in>>lprs; 
            for (bsc = 6; bsc < 17; bsc++) 
            {
                brp = (frq/(bsc * Config->Baudrate)) - 1; 
                baudrate = frq/((brp + 1) * bsc); 
                /* ckeck accuracy - allow 2% tolerance */
                if (baudrate == Config->Baudrate)
                {
                    found = 1; 
                }
                else if (baudrate > Config->Baudrate)
                {
                    if ((Config->Baudrate/50) > (baudrate - Config->Baudrate))
                    {
                        found = 1; 
                    }
                }
                else 
                {
                    if ((Config->Baudrate/50) > (Config->Baudrate - baudrate))
                    {
                        found = 1; 
                    }
                }
                if (0 != found) 
                {
                    break; 
                }
            }
            if (0 != found) 
            {
                break; 
            }
        }    
        if (0 != found) 
        {
            /* we found the right settings, let's store the values */
            /* register LWBR*/
            /* b[0]  LWBR0 
                    Wake-up baud rate select mode 
                    0: Baud rate clock is based on System Clock configuration in 
                       Wake-up mode 
                    1: Baud rate clock source is automatically set to “fa” in Wake-up 
                       mode. 
            */
            regval8 = (uint8_t)((uint8_t)((uint8_t)(bsc - 1u) << 4u) | lprs | 0x1u);  
            R_DEV_WRITE_REG(8, base + R_RLIN3_LWBR, regval8);  
            R_DEV_WRITE_REG(8, base + R_RLIN3_LBRP0, (uint8_t)brp); 
            R_DEV_WRITE_REG(8, base + R_RLIN3_LBRP1, (uint8_t)(brp>>8)); 
            
            
            /* 
            LIN Mode Register/UART Mode Register (LMD)
            b[1:0]  LMD LIN / UART Mode 
            Select Bits 
                00: LIN Master mode 
                01: UART mode 
                10: LIN Slave mode with automatic baud rate detection 
                11: LIN Slave mode with fixed baud rate 
            b[3:2]  LCKS LIN / UART System Clock Select Bits 
                00: fa (fixed for uart mode)
                01: fb 
                10: fc 
                11: fd 
            b[4]  LIOS LIN Interrupt Output Select Bit 
                0: Module generates 1 interrupt signal - LIN mode 
                1: Module generates 3 interrupt signals - tx,rx,err  
            b[5]  LRDNFS LIN / UART Receive Data Noise Filter Select Bit 
                0: 3-bit majority voting logic for sampling RX data is enabled. 
                1: 3-bit majority voting logic for sampling RX data is disabled.
            b[7:6]  -  Reserved 
                These bits are always read as 0. The write value should 
                always be 0. 
            */     
            R_DEV_WRITE_REG(8, base + R_RLIN3_LMD, 0x01); 
            
            /* error detection */
            regval8 = (uint8_t) Config->Detection; 
            R_DEV_WRITE_REG(8, base + R_RLIN3_LEDE, regval8); 
           
            /* UART configuration */
            regval8 = (uint8_t)Config->TxDataLevel | (uint8_t)Config->RxDataLevel | (uint8_t)Config->Parity | 
                      (uint8_t)Config->Stopbit |  (uint8_t)Config->Direction | (uint8_t)Config->CharLength; 
            R_DEV_WRITE_REG(8, base + R_RLIN3_LBFC, regval8); 
        
            /*Complete the intialisation in the device level */  
            R_SYS_RLIN3_Init(Unit); 
        
            R_RLIN3_ResetRelease(Unit); 
            /* Tx rx enable */
            regval8 = (uint8_t)Config->TxEnable | (uint8_t)Config->RxEnable; 
            R_DEV_WRITE_REG(8, base + R_RLIN3_LUOER, regval8); 
            return R_RLIN3_ERR_OK;
        }
        else /* if (found) */
        {
           return R_RLIN3_ERR_RANGE;
        }
    } /* else if (R_BAD_ADDRESS == base || (R_RLIN3_MACRO_NUM > Unit))  */
}

/*******************************************************************************
  Function: R_RLIN3_Reset
*/

void R_RLIN3_Reset(uint32_t Unit) 
{
    uint32_t base = r_rlin3_Dev[Unit].BaseAddr;
    
    if ((R_RLIN3_MACRO_NUM > Unit) && (0 != base)) /* OK - initiallised */
    {
        R_DEV_WRITE_REG(8, base + R_RLIN3_LCUC, 0); 
    }

}

/*******************************************************************************
  Function: R_RLIN3_ResetRelease
*/

void R_RLIN3_ResetRelease(uint32_t Unit) 
{
    uint32_t base = r_rlin3_Dev[Unit].BaseAddr;
    
    if ((R_RLIN3_MACRO_NUM > Unit) && (0 != base)) /* OK - initiallised */
    {
        R_DEV_WRITE_REG(8, base + R_RLIN3_LCUC, 1); 
         /* wait for reset release */
        while (1 != R_DEV_READ_REG(8,base + R_RLIN3_LMST))
        {
            /**/
        }
    }

}

/*******************************************************************************
  Function: R_RLIN3_DeInit
*/

r_rlin3_Error_t R_RLIN3_DeInit(uint32_t Unit) 
{
    uint32_t base;
    
    if (Unit >= R_RLIN3_MACRO_NUM) 
    {
        R_RLIN3_ErrorHandler(Unit, R_RLIN3_ERR_RANGE);
        return R_RLIN3_ERR_RANGE;
    }
    else
    {
        base = r_rlin3_Dev[Unit].BaseAddr;
        if (0 != base) 
        {
            /* continue de-init only in case the unit was already initialised) */   
            R_RLIN3_ResetRelease(Unit); 
            R_RLIN3_Disable(Unit); 
            R_RLIN3_Reset(Unit); 
            R_DEV_WRITE_REG(8, base + R_RLIN3_LMD,   0);
            R_DEV_WRITE_REG(8, base + R_RLIN3_LBFC,  0);
            R_DEV_WRITE_REG(8, base + R_RLIN3_LEDE,  0);
            R_DEV_WRITE_REG(8, base + R_RLIN3_LWBR,  0);
            R_DEV_WRITE_REG(8, base + R_RLIN3_LBRP1, 0);
            R_DEV_WRITE_REG(8, base + R_RLIN3_LBRP0, 0);
            R_DEV_WRITE_REG(8, base + R_RLIN3_LMD,   0);

            /*Complete the de-intialisation in the device level */  
            R_SYS_RLIN3_DeInit(Unit); 
        }    
    }
    return R_RLIN3_ERR_OK;
}


/*******************************************************************************
  Function: R_RLIN3_Flush
*/

void R_RLIN3_Flush(uint32_t Unit) 
{
    uint32_t nb = 1u;
    uint8_t           readno;

    volatile uint8_t  regval;
    uint8_t           txdata;
    
    while (nb > 0) 
    {
        R_DEV_Nop();  
        nb = R_CDI_RbNum(&(r_rlin3_Dev[Unit].TxBuf));
        regval = r_rlin3_Dev[Unit].TxOngoing;
        if (0 == regval) 
        {
            readno = loc_SecureRbRead(Unit, &r_rlin3_Dev[Unit].TxBuf, &txdata, 1, R_RLIN3_INT_TX);
            if (0 != readno) 
            {
                /* send first Data manually to init Interrupt*/
                R_RLIN3_WriteChar(Unit, txdata);
            }
        }
    }
}


/*******************************************************************************
  Function: R_RLIN3_ReadChar
*/

 uint8_t R_RLIN3_ReadChar(uint32_t Unit) 
 {
    uint32_t     base;
    uint8_t      rxdata;
    
    if (Unit >= R_RLIN3_MACRO_NUM) 
    {
        R_RLIN3_ErrorHandler(Unit, R_RLIN3_ERR_RANGE);
    }
    else
    {
        base   = r_rlin3_Dev[Unit].BaseAddr;
        rxdata = R_RLIN3_READ_REG(8, base + R_RLIN3_LURDR);
    }
    return rxdata;
}


/*******************************************************************************
  Function: R_RLIN3_WriteChar
*/

r_rlin3_Error_t R_RLIN3_WriteChar(uint32_t Unit, uint8_t TxData) 
{
    uint32_t          base;
    volatile uint8_t  regval;
    
    if (Unit >= R_RLIN3_MACRO_NUM)
    {
        R_RLIN3_ErrorHandler(Unit, R_RLIN3_ERR_RANGE);
    }
    else
    {
        base = r_rlin3_Dev[Unit].BaseAddr;
        do 
        {
            regval = r_rlin3_Dev[Unit].TxOngoing;
        } while (0 != regval);
        r_rlin3_Dev[Unit].TxOngoing = 1;
        r_rlin3_Dev[Unit].TxRemain  = 1;
        /* no debug output for the tx register */
        R_RLIN3_WRITE_REG_SILENT(8, base + R_RLIN3_LUTDR, TxData);
    }
    return R_RLIN3_ERR_OK;
}


/*******************************************************************************
  Function: R_RLIN3_SetErrorCallback
*/

void R_RLIN3_SetErrorCallback(void (*ErrorCallback)(uint32_t Unit, uint32_t Error)) 
{
    loc_ErrorHandler = ErrorCallback; 
}


/*******************************************************************************
  Function: R_RLIN3_Write
*/

 uint32_t R_RLIN3_Write(uint32_t Unit, uint8_t *Buffer, uint32_t Size) 
{
    uint32_t          writeno;   /* number of written characters in rb */
    uint32_t          remaining; /* characters left to transmit to rb */
    uint8_t           readno;
    volatile uint8_t  regval;
    uint8_t           txdata;
    
    if (Unit >= R_RLIN3_MACRO_NUM) 
    {
       R_RLIN3_ErrorHandler(Unit, R_RLIN3_ERR_RANGE);
    } 
    else 
    {
        remaining = Size;
        
        do 
        {     
            writeno    = loc_SecureRbWrite(Unit, &r_rlin3_Dev[Unit].TxBuf, Buffer, remaining, R_RLIN3_INT_TX);
            remaining -= writeno;
            Buffer    += writeno;

            regval = r_rlin3_Dev[Unit].TxOngoing;
            if (0 == regval) 
            {
                readno = loc_SecureRbRead(Unit, &r_rlin3_Dev[Unit].TxBuf, &txdata, 1, R_RLIN3_INT_TX);
                if (0 != readno) 
                {
                    /* send first Data manually to init Interrupt*/
                    R_RLIN3_WriteChar(Unit, txdata);
                }
            }
        } while (0 != remaining);
         
    }  
    return Size;
}


/*******************************************************************************
  Function: R_RLIN3_Read
*/

 uint32_t R_RLIN3_Read(uint32_t Unit, uint8_t *Buffer, uint32_t MaxSize) 
{
    uint32_t len;
    
    if (Unit >= R_RLIN3_MACRO_NUM) 
    {
        R_RLIN3_ErrorHandler(Unit, R_RLIN3_ERR_RANGE);
    }
    else
    {
        len = loc_SecureRbRead(Unit, &r_rlin3_Dev[Unit].RxBuf, Buffer, MaxSize,  R_RLIN3_INT_RX); 
    }
    return len;
}


/*******************************************************************************
  Function: R_RLIN3_Disable
*/

r_rlin3_Error_t R_RLIN3_Disable(uint32_t Unit) 
{
    uint32_t    base;

    if (Unit >= R_RLIN3_MACRO_NUM)
    {
        return R_RLIN3_ERR_RANGE; 
    }
    else
    {
        base    = r_rlin3_Dev[Unit].BaseAddr;
        R_DEV_WRITE_REG(8, base + R_RLIN3_LUOER, 0);
        r_rlin3_Dev[Unit].TxOngoing = 0;
        r_rlin3_Dev[Unit].TxRemain  = 0;
        r_rlin3_Dev[Unit].RxOngoing = 0;
        r_rlin3_Dev[Unit].RxRemain  = 0;  
    }
    return R_RLIN3_ERR_OK;
}    


/*******************************************************************************
  Function: R_RLIN3_Enable
*/

r_rlin3_Error_t R_RLIN3_Enable(uint32_t Unit) 
{
    uint32_t    base;
  
    if (Unit >= R_RLIN3_MACRO_NUM)
    {
        return R_RLIN3_ERR_RANGE; 
    }
    else
    {
        R_RLIN3_ResetRelease(Unit); 
        base    = r_rlin3_Dev[Unit].BaseAddr;
        R_DEV_WRITE_REG(8, base + R_RLIN3_LUOER, 0x3);
    }
    return R_RLIN3_ERR_OK;
}


/*******************************************************************************
  Function: R_RLIN3_DisableTx
*/

r_rlin3_Error_t R_RLIN3_DisableTx(uint32_t Unit) 
{
    uint32_t    base;
    uint8_t     regval;

    if (Unit >= R_RLIN3_MACRO_NUM) 
    {
        return R_RLIN3_ERR_RANGE; 
    } 
    else 
    {
        base    = r_rlin3_Dev[Unit].BaseAddr;
        regval  = R_DEV_READ_REG(8, base + R_RLIN3_LUOER);
        regval &= (uint8_t)(~0x01u);
        R_DEV_WRITE_REG(8, base + R_RLIN3_LUOER, regval);
        r_rlin3_Dev[Unit].TxOngoing = 0;
        r_rlin3_Dev[Unit].TxRemain  = 0;  
    }
    return R_RLIN3_ERR_OK;
}


/*******************************************************************************
  Function: R_RLIN3_EnableTx
*/

r_rlin3_Error_t R_RLIN3_EnableTx(uint32_t Unit) 
{
    uint32_t    base;
    uint8_t     regval;

    if (Unit >= R_RLIN3_MACRO_NUM) 
    {
        return R_RLIN3_ERR_RANGE; 
    } 
    else 
    {
        base    = r_rlin3_Dev[Unit].BaseAddr;
        regval  = R_DEV_READ_REG(8, base + R_RLIN3_LUOER);
        regval |= 0x01u;
        R_DEV_WRITE_REG(8, base + R_RLIN3_LUOER, regval);  
    }
    return R_RLIN3_ERR_OK;
}


/*******************************************************************************
  Function: R_RLIN3_DisableRx
*/

r_rlin3_Error_t R_RLIN3_DisableRx(uint32_t Unit) 
{
    uint32_t    base;
    uint8_t     regval;

    if (Unit >= R_RLIN3_MACRO_NUM) 
    {
        return R_RLIN3_ERR_RANGE; 
    } 
    else 
    {
        base    = r_rlin3_Dev[Unit].BaseAddr;
        regval  = R_DEV_READ_REG(8, base + R_RLIN3_LUOER);
        regval &= (uint8_t)(~0x02u);
        R_DEV_WRITE_REG(8, base + R_RLIN3_LUOER, regval);
        r_rlin3_Dev[Unit].RxOngoing = 0;
        r_rlin3_Dev[Unit].RxRemain  = 0;  
    }
    return R_RLIN3_ERR_OK;
}


/*******************************************************************************
  Function: R_RLIN3_EnableRx
*/

r_rlin3_Error_t R_RLIN3_EnableRx(uint32_t Unit)
{
    uint32_t    base;
    uint8_t     regval;

    if (Unit >= R_RLIN3_MACRO_NUM)
    {
        return R_RLIN3_ERR_RANGE; 
    }
    else
    {
        base    = r_rlin3_Dev[Unit].BaseAddr;
        regval  = R_DEV_READ_REG(8, base + R_RLIN3_LUOER);
        regval |= 0x02u;
        R_DEV_WRITE_REG(8, base + R_RLIN3_LUOER, regval);  
    }
    return R_RLIN3_ERR_OK;
}                                


   
/*******************************************************************************
  Function: R_RLIN3_TriggerSbfTx
*/

r_rlin3_Error_t R_RLIN3_TriggerSbfTx(uint32_t Unit)
{
     return R_RLIN3_ERR_OK;
}


/*******************************************************************************
  Function: R_RLIN3_TriggerSbfRx
*/

r_rlin3_Error_t R_RLIN3_TriggerSbfRx(uint32_t Unit)
{
    return R_RLIN3_ERR_OK;
}


/*******************************************************************************
  Function: R_RLIN3_IsTxRemain
*/
        
r_rlin3_TxRemain_t R_RLIN3_IsTxRemain(uint32_t Unit)
{
    return (r_rlin3_TxRemain_t)r_rlin3_Dev[Unit].TxRemain;
}


/*******************************************************************************
  Function: R_RLIN3_IsrxRemain
*/

r_rlin3_RxRemain_t R_RLIN3_IsRxRemain(uint32_t Unit)
{
    return (r_rlin3_RxRemain_t)r_rlin3_Dev[Unit].RxRemain;   
}
      
      
/*******************************************************************************
  Function: R_RLIN3_IsSbfTxOngoing
*/
        
uint8_t R_RLIN3_IsSbfTxOngoing(uint32_t Unit)
{
    return 0; 
}

/*******************************************************************************
  Function: R_RLIN3_IsSbfRxOngoing
*/
        
uint8_t R_RLIN3_IsSbfRxOngoing(uint32_t Unit)
{
    return 0; 
}

/*******************************************************************************
  Function: R_RLIN3_IsDataTxOngoing
*/
        
uint8_t R_RLIN3_IsDataTxOngoing(uint32_t Unit) 
{
     if ((R_DEV_READ_REG(8, r_rlin3_Dev[Unit].BaseAddr + R_RLIN3_LST) & 0x10u) != 0x10u)
     {
         return 0;
     } 
     else 
     {
         return 1;
     }            
}

/*******************************************************************************
  Function: R_RLIN3_IsDataRxOngoing
*/
        
uint8_t R_RLIN3_IsDataRxOngoing(uint32_t Unit) 
{
     if ((R_DEV_READ_REG(8, r_rlin3_Dev[Unit].BaseAddr + R_RLIN3_LST) & 0x20u) != 0x20u)
     {
         return 0;
     } 
     else 
     {
        return 1;
     }            
}

                                 
                                

