/*
****************************************************************************
PROJECT : VDCE driver
FILE    : $Id: r_sys_vdce.c 13578 2017-05-29 13:31:52Z matthias.nippert $
============================================================================
DESCRIPTION
VDCE system functions
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
demonstration purposes at customer's premises listed in the signed license
agreement.
****************************************************************************
*/

/******************************************************************************
  Title: Generic VDCE Support Functions

    VDCE driver Support Functions

******************************************************************************/

/*******************************************************************************
  Section: Includes
*/

#include "r_typedefs.h"   /* Renesas basic types, e.g. uint32_t */
#include "r_ddb_api.h"    /* display database with display timings */
#include "r_dev_api.h"
#include "r_config_hwversion.h"
#include "r_config_vdce.h"
#include "r_config_vdce_num.h"
#include "r_vdce_api.h"
#include "r_vdce_sys.h"

/*******************************************************************************
  Section: Local Defines
*/

/*
The following is sample code, which may not respect the correct value of <Unit>,
if both following conditions are satisfied:

  * Both video channels are active
  * One of the channel uses RSDS, OpenLDI or SerialRGB

Please make sure to remove sample code for that Unit that is not required to use 
one of the above features.
*/

/*
The following flags are used in several files.
Thus, these flags are best set globally, e.g. in a Makefile.

    USE_VDCE_RSDS       -   Configure VDCE to use RSDS on Pins "VDCE0 Out"
    USE_VDCE_SERIALRGB  -   Configure VDCE to use Serial RGB on Pins "VDCE1 Out"
    USE_VDCE_OPENLDI    -   Configure VDCE to use Open LDI on Pins "VDCE0 Out"
    USE_VDCE_VODDR      -   Configure VDCE to use VO-DDR on Pins "VDCE0 Out"
    
    TODO for system integrator:
    At the moment, for RSDS, OpenLDI and SerialRGB, divider is set for both Units 0 and 1.
    It should just be set for the Unit currently using the respective mode.
*/

/* USE_VDCE_RSDS
    0 = LVTTL, (default, if undefined)
    1 = RSDS
*/
#ifndef USE_VDCE_RSDS
    #define USE_VDCE_RSDS       (0u)
#endif

/* USE_VDCE_OPENLDI
    0 = Parallel Video Output (default, if undefined)
    1 = Open LDI
*/
#ifndef USE_VDCE_OPENLDI
    #define USE_VDCE_OPENLDI    (1u)
#endif

/* USE_VDCE_VODDR
    0 = Parallel Video Output (default, if undefined)
    1 = VO DDR Video Output
*/
#ifndef USE_VDCE_VODDR
    #define USE_VDCE_VODDR      (0u)
#endif

/* USE_VDCE_SERIALRGB
    0 = Parallel Video Output (default, if undefined)
    1 = Serial RGB
*/
#ifndef USE_VDCE_SERIALRGB
    #define USE_VDCE_SERIALRGB  (0u)
#endif

/* USE_VDCE_SERIALRGB_SPEED
    3 = Triple Speed (default, if undefined)
    4 = Quadruple Speed
*/
#ifndef USE_VDCE_SERIALRGB_SPEED
    #define USE_VDCE_SERIALRGB_SPEED  (3u)
#endif




#if (USE_VDCE_SERIALRGB | USE_VDCE_OPENLDI) & USE_VDCE_VODDR
    #error "VO-DDR must not be enabled together with SerialRGB and OpenLDI"
#endif


#define LOC_PRDNAME_THRESHOLD  (300u)
#define LOC_VDCE_REVISION_1    (1u)
#define LOC_VDCE_REVISION_2    (2u)


/* Video output pixel clocks exchange register (RH850/D1M2(H) and D1M1A only)
*/
#define VDCECTL         0xFFC0601Cu
#define VDCECTL_VOSL    (1ul<<2)

#define LOC_CKSC_IVOEXS_CTL_REG_ADDR    (0xFFF85900u)
#define LOC_PROTCMDD1     (0xfff87000u)


#define LOC_PINV45  (0xFF610B70u)
#define LOC_PINV47  (0xFF610BF0u)
#define LOC_PPCMD45 (0xFFC14B6Cu)
#define LOC_PPCMD47 (0xFFC14BECu)

/* Video Output DDR Mode clock divider register (RH850/D1M1A only)
*/

#define VODDR0_SYSCNT               (0xF2001000ul)
#define VODDR0_CLKDIV               (0xF2001004ul)

#define VODDR0_SYSCNT_CLK_START     (1ul<<0)
#define VODDR0_SYSCNT_VO1_ENABLE    (1ul<<16)

#define VODDR0_CLKDIV_CLKDIV0(div)  (((div)&0xFFul)<<0)  /* Set clock divider of 2..254 of PLLCLK */
#define VODDR0_CLKDIV_CLKDIV1(div)  (((div)&0xFFul)<<16) /* Set clock divider of 2..254 of PLLCLK */

/* The definition of the interrupt control.
*/
#define LOC_INTC_UNIT0_START   (R_DEV_INT_VDCE0ERR)
#define LOC_INTC_UNIT0_END     (R_DEV_INT_VDCE0IRVLINE)
#define LOC_INTC_UNIT1_START   (R_DEV_INT_VDCE1ERR)
#define LOC_INTC_UNIT1_END     (R_DEV_INT_VDCE1S1LOVSYNC)
#define LOC_INTC_UNIT1_OFFSET  (R_DEV_INT_VDCE1ERR - R_DEV_INT_VDCE0ERR)

#define LOC_MASK_0X00000001 (0x00000001u)
#define LOC_MASK_0X00000100 (0x00000100u)

/***************************************************************************
  Constants: LOC_CPU_FREQ_MAX
  
  The definition for maximum CPU clock frequency (Hz).
  It does not matter if it is greater than actual value.
  
*/
#define LOC_CPU_FREQ_MAX                   (240000000)

/***************************************************************************
  Constants: LOC_SYNC_FREQ_MIN
  
  The definition for minimum syncronization clock frequency (Hz).
  When frequency is unknown (i.e. external VIN clock), this definition value is used. 
  It should be enough small value.
  
*/
#define LOC_SYNC_FREQ_MIN                  (1000000)

/***************************************************************************
  Constants: LOC_1ST_WAIT_CYCLE
  
  The definition for cycle number that H/W accepts clear instruction.
  The cycle is based on synchronization clock for each interrupt. 
  
  Simulatuion result : 2 Cycle. 
  margin             : 2 Cycle. 
  It does not matter if it is greater than actual value.
*/

#define LOC_1ST_WAIT_CYCLE                  (4)

/***************************************************************************
  Constants: LOC_2ND_WAIT_CYCLE
  
  The definition for cycle number that H/W completes clear instruction.
  The cycle is based on synchronization clock for each interrupt. 
  
  Simulatuion result : 4 Cycle. 
  margin             : 4 Cycle. 
  It does not matter if it is greater than actual value.
*/
#define LOC_2ND_WAIT_CYCLE                  (8)


/* The definition of the device version.

   If this definition is not defined, VDCE works as H/W device version.
   If this definition is defined, VDCE works as specified device version for debug.
   
  Values:
    R_DEV_R7F701403 - VDCE works as RH850/D1L2x.
    R_DEV_R7F701407 - VDCE works as RH850/D1M1(H).
    R_DEV_R7F701410 - VDCE works as RH850/D1M2
    R_DEV_R7F701412 - VDCE works as RH850/D1M2H
*/
/* #define LOC_VDCE_FORCE_DEV_NUM    (R_DEV_R7F701403) */

/***************************************************************************
  Constants: LOC_UNKOWN_INT_SEL
  
  The definition for unknown interrupt. 
  
*/
#define LOC_UNKOWN_INT_SEL            ((r_dev_IntSel_t)0xFFFFFFFFu)

/***************************************************************************
  Constants: LOC_WAIT_CLEAR_COUNT
  
  The definition for maximum loop counter that H/W completes clear instruction.
  
  Simulatuion result : It took max 7 Loops in case of 1.88MHz pixel clock and 240MHz CPU clock. 
*/
#define LOC_WAIT_CLEAR_COUNT         (16)

/*******************************************************************************
  Section: Local functions
*/
/* list of static functions: */
static uint32_t loc_calcClockDiv(const uint32_t Clkin, const uint32_t Clkout) ; 
static uint32_t loc_Vdce_Pixel_SetClockM2(const uint32_t Unit, const uint32_t Freq);
static uint32_t loc_Vdce_Pixel_SetClockM1A(const uint32_t Unit, const uint32_t Freq, const uint32_t OtherUnitActive);
static uint32_t loc_Vdce_Pixel_SetClockL2M1(const uint32_t Unit, const uint32_t Freq);
static r_vdce_Error_t loc_IntcCtrl(const uint32_t       Unit,
                                   const r_dev_IntSel_t IntSel,
                                   const uint8_t        Enable);
static r_dev_Device_t loc_GetDev(void);
static r_vdce_Error_t loc_IntcCtrlAll(const uint32_t Unit, const uint8_t Enable);
static r_vdce_Error_t loc_IntcCtrl(const uint32_t Unit, const r_dev_IntSel_t IntSel, const uint8_t Enable);
static r_dev_IntSel_t loc_ConvertToIntSel(const uint32_t Unit, const r_vdce_IntType_t IntType);

/*******************************************************************************
  Function: loc_calcClockDiv
*/

static uint32_t loc_calcClockDiv(const uint32_t Clkin, const uint32_t Clkout)
{
    uint32_t divider;
    uint32_t divfrq;
    uint32_t divfrq_next;

    /* calc near divider */
    divider = Clkin / Clkout;

    /* test if next divider is better, if yes choose next divider */
    divfrq = Clkin / divider;
    divfrq_next = Clkin / (divider+1u);
    if ( (divfrq - Clkout) > (Clkout - divfrq_next))
    {
        divider += 1u;
    }

    /* divider value 0 is invalid, choose 1 instead */
    if (divider == 0u)
    {
        divider = 1;
    }

    return divider;
}

/*******************************************************************************
  Function: loc_Vdce_Pixel_SetClockM2
*/

static uint32_t loc_Vdce_Pixel_SetClockM2(const uint32_t Unit, const uint32_t Freq)
{
    uint32_t divider;
    uint32_t clkin;
    r_dev_ClkSelConfig_t ClkSelCfg[2];
    
    /* get VDCE clock */
    clkin = R_DEV_BusFrequencyHz(R_DEV_CLK_PLL2);
#if USE_VDCE_RSDS    /* 0=LVTTL, 1=RSDS (divider by 4 for pixel clock, when RSDS is used) */
    clkin /= 4;
#endif /* USE_VDCE_RSDS */
    /* dbg into: Set VDCE clock divider */
    divider = (Freq != 0) ? loc_calcClockDiv(clkin, Freq) : 0;
    if (divider == 1)
    {
        divider = 2;
    }
    if (divider > 255)
    {
        divider = 255;
    }
    
    if (Unit == 0)
    {
        ClkSelCfg[0].Cks       = R_DEV_CKS_DOTCK0;
        ClkSelCfg[0].SrcId     = R_DEV_CKS_SRC_PLL2;
        ClkSelCfg[0].Div       = (uint16_t)divider;
        ClkSelCfg[0].StpReqMsk = 0;
        
        ClkSelCfg[1].Cks       = R_DEV_CKS_VDCE0CK;
#if USE_VDCE_RSDS    /* 0=LVTTL, 1=RSDS (divider by 4 for pixel clock, when RSDS is used) */
        ClkSelCfg[1].SrcId     = R_DEV_CKS_SRC_DOTCK0_4;
#else        
        ClkSelCfg[1].SrcId     = R_DEV_CKS_SRC_DOTCK0;
#endif /* USE_VDCE_RSDS */
        ClkSelCfg[1].Div       = 0;
        ClkSelCfg[1].StpReqMsk = 0;
    }
    else
    {
        ClkSelCfg[0].Cks       = R_DEV_CKS_DOTCK1;
        ClkSelCfg[0].SrcId     = R_DEV_CKS_SRC_PLL2;
        ClkSelCfg[0].Div       = (uint16_t)divider;
        ClkSelCfg[0].StpReqMsk = 0;
        
        ClkSelCfg[1].Cks       = R_DEV_CKS_VDCE1CK;
#if USE_VDCE_RSDS    /* 0=LVTTL, 1=RSDS (divider by 4 for pixel clock, when RSDS is used) */
        ClkSelCfg[1].SrcId     = R_DEV_CKS_SRC_DOTCK1_4;
#else        
        ClkSelCfg[1].SrcId     = R_DEV_CKS_SRC_DOTCK1;
#endif /* USE_VDCE_RSDS */
        ClkSelCfg[1].Div       = 0;
        ClkSelCfg[1].StpReqMsk = 0;
    }
    (void)R_DEV_ClkIdSet(&ClkSelCfg[0]);
    (void)R_DEV_ClkIdSet(&ClkSelCfg[1]);
    
    /* If VDCE0 and VDCE1 signalling is cross-changed, also change to clock supply of the output macro */
    if ( 0 == (R_DEV_READ_REG(32, VDCECTL) & VDCECTL_VOSL) )
    {
        /* Disable exchange of VDCE0_VO_CLK and VDCE1_VO_CLK */
        R_DEV_WRITE_PROT_REG(LOC_PROTCMDD1, LOC_CKSC_IVOEXS_CTL_REG_ADDR, 0);
    }
    else
    {
        /* Enable exchange of VDCE0_VO_CLK and VDCE1_VO_CLK */
        R_DEV_WRITE_PROT_REG(LOC_PROTCMDD1, LOC_CKSC_IVOEXS_CTL_REG_ADDR, 1);
    }
    
    return (divider != 0) ? (clkin / divider):0;


}

/*******************************************************************************
  Function: loc_Vdce_Pixel_SetClockM1A
*/

static uint32_t loc_Vdce_Pixel_SetClockM1A(const uint32_t Unit, const uint32_t Freq, const uint32_t OtherUnitActive)
{
    uint32_t divider;
    uint32_t clkin;
    r_dev_ClkSelConfig_t ClkSelCfg[2];
#if USE_VDCE_VODDR    /* 0=LVTTL, 1=VODDR */
    uint32_t VoDdrClkdiv;
    uint32_t divider0, divider1;
#endif /* USE_VDCE_VODDR */
    
    /* get VDCE clock */
    clkin = R_DEV_BusFrequencyHz(R_DEV_CLK_PLL1);
#if USE_VDCE_OPENLDI    /* 0=LVTTL, 1=OpenLDI (divider by 7 for pixel clock, when Open LDI is used) */
    if (Unit == 0)
    {
        clkin /= 7;
    }

    if (Unit == 1)
    {
 #if USE_VDCE_SERIALRGB
 #else  /* When USE_VDCE_SERIALRGB is used, than OpenLDI can only use VDCE0. 
           Thus avoid OpenLDI dependend clock setting for VDCE1 here. */
        clkin /= 7;
 #endif /* USE_VDCE_SERIALRGB */
    }
#endif /* USE_VDCE_OPENLDI */

#if USE_VDCE_SERIALRGB
 #if USE_VDCE_OPENLDI
 #else /* When USE_VDCE_OPENLDI is used, only use SerialRGB on VDCE1.
          Thus avoid SerialRGB dependend clock setting for VDCE0 here. */
    if (Unit == 0)
    {
      #if USE_VDCE_SERIALRGB_SPEED == 3
        clkin /= 3;
      #elif USE_VDCE_SERIALRGB_SPEED == 4
        clkin /= 4;
      #endif /* USE_VDCE_SERIALRGB_SPEED */
    }
 #endif /* USE_VDCE_OPENLDI */
    if (Unit == 1)
    {
      #if USE_VDCE_SERIALRGB_SPEED == 3
        clkin /= 3;
      #elif USE_VDCE_SERIALRGB_SPEED == 4
        clkin /= 4;
      #endif /* USE_VDCE_SERIALRGB_SPEED */
    }
#endif /* USE_VDCE_SERIALRGB */

    /* dbg into: Set VDCE clock divider */
    divider = (Freq != 0) ? loc_calcClockDiv(clkin, Freq) : 0;
    if (divider == 1)
    {
        divider = 2;
    }
    if (divider > 255)
    {
        divider = 255;
    }

#if USE_VDCE_VODDR    /* 0=LVTTL, 1=VODDR */

    /* Test code added:
         need to assign the first unit CLK_DIVn[7:0]
    */
    if ( (Freq != 0) && (OtherUnitActive == 0) )
    {
        VoDdrClkdiv = R_DEV_READ_REG(32, VODDR0_CLKDIV);
        divider0 = VoDdrClkdiv & 0xFFu;
        divider1 = (VoDdrClkdiv & 0xFF0000u) >> 16;

        if (0 == Unit) {
            divider0 = divider;
        } else {
            divider1 = divider;
        }

        R_DEV_WRITE_REG(32, VODDR0_CLKDIV, VODDR0_CLKDIV_CLKDIV0(divider0) | VODDR0_CLKDIV_CLKDIV1(divider1) );
    }
    /* end of test code */




    /* Other Unit is already running and we are configuring this Unit: Start VO-DDR */
    if ( (Freq != 0) && (OtherUnitActive == 1) )
    {
        VoDdrClkdiv = R_DEV_READ_REG(32, VODDR0_CLKDIV);
        divider0 = VoDdrClkdiv & 0xFFu;
        divider1 = (VoDdrClkdiv & 0xFF0000u) >> 16;
        
        if (0 == Unit) {
            divider0 = divider;
        } else {
            divider1 = divider;
        }
        
        /* Check if divider for Unit0 and Unit1 have the correct proportion */
        
        if ( (divider0 < divider1) ||                           /* fVDCE0 <= fVDCE1 => div0 >= div1 */
              (((divider1*divider0)/divider0) != divider1) ||   /* divider1 must be an integer multiple of divider0 */
              ((divider0%2) == 1) || ((divider1%2) == 1) )      /* divider0 & divider1 must be even*/
        {
            /* Proportion mismatch! */
            return 0;
        }
        else
        {
            R_DEV_WRITE_REG(32, VODDR0_CLKDIV, VODDR0_CLKDIV_CLKDIV0(divider0) | VODDR0_CLKDIV_CLKDIV1(divider1) );
            R_DEV_WRITE_REG(32, VODDR0_SYSCNT, VODDR0_SYSCNT_CLK_START | VODDR0_SYSCNT_VO1_ENABLE );
        }
    }
    /* Other Unit is not running or we are deinitializing this Unit: Stop VO-DDR */
    else
    {
        R_DEV_WRITE_REG(32, VODDR0_SYSCNT, 0 );
    }
#endif /* USE_VDCE_VODDR */


    if (Unit == 0)
    {
        ClkSelCfg[0].Cks       = R_DEV_CKS_DOTCK0;
        ClkSelCfg[0].SrcId     = R_DEV_CKS_SRC_PLL1;
        ClkSelCfg[0].Div       = (uint16_t)divider;
        ClkSelCfg[0].StpReqMsk = 0;
        
        ClkSelCfg[1].Cks       = R_DEV_CKS_VDCE0CK;
#if USE_VDCE_OPENLDI    /* 0=LVTTL, 1=OpenLDI (divider by 7 for pixel clock, when Open LDI is used) */
        ClkSelCfg[1].SrcId     = R_DEV_CKS_SRC_DOTCK0_7;
#else
        ClkSelCfg[1].SrcId     = R_DEV_CKS_SRC_DOTCK0;
#endif /* USE_VDCE_RSDS */
        ClkSelCfg[1].Div       = 0;
        ClkSelCfg[1].StpReqMsk = 0;
    }
    else
    {
        ClkSelCfg[0].Cks       = R_DEV_CKS_DOTCK1;
        ClkSelCfg[0].SrcId     = R_DEV_CKS_SRC_PLLFIX;  /* actually same as R_DEV_CKS_SRC_PLL1 */
        ClkSelCfg[0].Div       = (uint16_t)divider;
        ClkSelCfg[0].StpReqMsk = 0;
        
        ClkSelCfg[1].Cks       = R_DEV_CKS_VDCE1CK;
#if USE_VDCE_OPENLDI    /* 0=LVTTL, 1=OpenLDI (divider by 7 for pixel clock, when Open LDI is used) */
 #if USE_VDCE_SERIALRGB
        /* When USE_VDCE_SERIALRGB is used, than OpenLDI can only use VDCE0. 
           Thus avoid OpenLDI dependend clock setting for VDCE1 here. */
        ClkSelCfg[1].SrcId     = R_DEV_CKS_SRC_DOTCK1;
 #else  
        ClkSelCfg[1].SrcId     = R_DEV_CKS_SRC_DOTCK1_7;
 #endif /* USE_VDCE_SERIALRGB */
#else
        ClkSelCfg[1].SrcId     = R_DEV_CKS_SRC_DOTCK1;
#endif /* USE_VDCE_RSDS */
        ClkSelCfg[1].Div       = 0;
        ClkSelCfg[1].StpReqMsk = 0;
    }

#if USE_VDCE_OPENLDI
    /* D1M1A needs to stop the source clock of both OLDICLK and IVOEXS0_OUTCLK 
       so both clocks stop at the same timing.
    */
    if(Freq == 0)
    {
        /* disable DOTCK0 or DOTCLK1 when requesting a 0 frequency (i.e de-init)*/
        ClkSelCfg[0].SrcId     = R_DEV_CKS_SRC_DISABLED;
    }
    else
    {
        /* nothing */
    }
#endif

    (void)R_DEV_ClkIdSet(&ClkSelCfg[0]);
    (void)R_DEV_ClkIdSet(&ClkSelCfg[1]);

    /* If VDCE0 and VDCE1 signalling is cross-changed, also change to clock supply of the output macro */
    if ( 0 == (R_DEV_READ_REG(32, VDCECTL) & VDCECTL_VOSL) )
    {
        /* Disable exchange of VDCE0_VO_CLK and VDCE1_VO_CLK */
        R_DEV_WRITE_PROT_REG(LOC_PROTCMDD1, LOC_CKSC_IVOEXS_CTL_REG_ADDR, 0);
    }
    else
    {
        /* Enable exchange of VDCE0_VO_CLK and VDCE1_VO_CLK */
        R_DEV_WRITE_PROT_REG(LOC_PROTCMDD1, LOC_CKSC_IVOEXS_CTL_REG_ADDR, 1);
    }
    
    return (divider != 0) ? (clkin / divider):0;
}


/*******************************************************************************
  Function: loc_Vdce_Pixel_SetClockL2M1
  
  set VDCE pixel clock (in MHz)
*/

static uint32_t loc_Vdce_Pixel_SetClockL2M1(const uint32_t Unit, const uint32_t Freq)
{
    uint32_t div, clkin;
    r_dev_ClkSelConfig_t ClkSelCfg[2];
    
    /* get VDCE clock */
    clkin = R_DEV_BusFrequencyHz(R_DEV_CLK_PLL0PIX);
    R_DBG_PRINT(R_DBG_MSG_CODE,  "Set VDCE clock divider");
    div = (Freq != 0) ? loc_calcClockDiv(clkin, Freq) : 0;
    if (div == 1)
    {
        div = 2;
    }
    if (div > 255)
    {
        div = 255;
    }
    
    ClkSelCfg[0].Cks       = R_DEV_CKS_DOTCK0;
    
    ClkSelCfg[0].SrcId     = R_DEV_CKS_SRC_PLL0PIX;
    ClkSelCfg[0].Div       = div;
    ClkSelCfg[0].StpReqMsk = 0;
    
    ClkSelCfg[1].Cks       = R_DEV_CKS_VDCE0CK;
    ClkSelCfg[1].SrcId     = R_DEV_CKS_SRC_DOTCK0;
    ClkSelCfg[1].Div       = 0;
    ClkSelCfg[1].StpReqMsk = 0;
    
    R_DEV_ClkIdSet(&ClkSelCfg[0]);
    R_DEV_ClkIdSet(&ClkSelCfg[1]);
    
    return (div != 0) ? (clkin / div):0;
}

/*******************************************************************************
  Function: loc_IntcCtrlAll
*/
static r_vdce_Error_t loc_IntcCtrlAll(const uint32_t Unit, const uint8_t Enable)
{
    r_vdce_Error_t ret;
    uint32_t	   i;
    r_dev_IntSel_t unit0_int_sel[] = { R_DEV_INT_VDCE0ERR       ,
									   R_DEV_INT_VDCE0S0VIVSYNC ,
									   R_DEV_INT_VDCE0S0LOVSYNC ,
									   R_DEV_INT_VDCE0GR3VLINE  ,
									   R_DEV_INT_VDCE0S0VFIELD  ,
									   R_DEV_INT_VDCE0S1LOVSYNC ,
									   R_DEV_INT_VDCE0OIRVIVSYNC,
									   R_DEV_INT_VDCE0OIRLOVSYNC,
									   R_DEV_INT_VDCE0IRVLINE   };
    r_dev_IntSel_t unit1_int_sel[] = { R_DEV_INT_VDCE1ERR       ,
									   R_DEV_INT_VDCE1S0VIVSYNC ,
									   R_DEV_INT_VDCE1S0LOVSYNC ,
									   R_DEV_INT_VDCE1GR3VLINE  ,
									   R_DEV_INT_VDCE1S0VFIELD  ,
									   R_DEV_INT_VDCE1S1LOVSYNC };

    ret = R_VDCE_ERR_OK;

    if (Unit < R_VDCE_MACRO_NUM)
    {
        if (Unit == R_VDCE_UNIT0)
        {
            /* Configure all interrupts on the device level */
            for (i = 0; i < (sizeof(unit0_int_sel) / sizeof(r_dev_IntSel_t)); i++)
            {
                R_DEV_IntClearFlag(unit0_int_sel[i]);
                R_DEV_IntEnable(unit0_int_sel[i], Enable);
            }
            /* GR3VBLANK is enabled independently, only disable is executed here. */
            if (Enable == 0)
            {
                R_DEV_IntClearFlag(R_DEV_INT_VDCE0GR3VBLANK);
                R_DEV_IntEnable(R_DEV_INT_VDCE0GR3VBLANK, Enable);
            }
        }
        else /* Unit == R_VDCE_UNIT1 */
        {
            /* Configure all interrupts on the device level */
            for (i = 0; i < (sizeof(unit1_int_sel) / sizeof(r_dev_IntSel_t)); i++)
            {
                R_DEV_IntClearFlag(unit1_int_sel[i]);
                R_DEV_IntEnable(unit1_int_sel[i], Enable);
            }
            /* GR3VBLANK is enabled independently, only disable is executed here. */
            if (Enable == 0)
            {
                R_DEV_IntClearFlag(R_DEV_INT_VDCE1GR3VBLANK);
                R_DEV_IntEnable(R_DEV_INT_VDCE1GR3VBLANK, Enable);
			}
        }
    }
    else
    {
        ret = R_VDCE_ERR_RANGE_UNIT;
    }
    
    return ret;
}

/*******************************************************************************
  Function: loc_IntcCtrl
*/
static r_vdce_Error_t loc_IntcCtrl(const uint32_t Unit, const r_dev_IntSel_t IntSel, const uint8_t Enable)
{
    r_vdce_Error_t ret;
    r_dev_IntSel_t int_sel;

    ret = R_VDCE_ERR_OK;

    if (Unit < R_VDCE_MACRO_NUM)
    {
        if (Unit == R_VDCE_UNIT0)
        {
            int_sel = IntSel;
            if ((int_sel >= LOC_INTC_UNIT0_START) && (int_sel <= LOC_INTC_UNIT0_END))
            {
                R_DEV_IntClearFlag(int_sel);
                R_DEV_IntEnable(int_sel, Enable);
            }
            else
            {
                ret = R_VDCE_ERR_RANGE_PARAM;
            }
        }
        else /* Unit == R_VDCE_UNIT1 */
        {
            int_sel = (r_dev_IntSel_t)(IntSel + LOC_INTC_UNIT1_OFFSET);
            if ((int_sel >= LOC_INTC_UNIT1_START) && (int_sel <= LOC_INTC_UNIT1_END))
            {
                R_DEV_IntClearFlag(int_sel);
                R_DEV_IntEnable(int_sel, Enable);
            }
            else
            {
                ret = R_VDCE_ERR_RANGE_PARAM;
            }
        }
    }
    else
    {
        ret = R_VDCE_ERR_RANGE_UNIT;
    }
    
    return ret;
}

/*******************************************************************************
  Function: loc_GetDev
*/
static r_dev_Device_t loc_GetDev(void)
{
    r_dev_Device_t  device;
    
    /* Modify device number if debug. */
#ifdef LOC_VDCE_FORCE_DEV_NUM
    device = LOC_VDCE_FORCE_DEV_NUM;
#else
    device = R_DEV_GetDev();
#endif
    return device;
}

/*******************************************************************************
  Function: loc_ConvertToIntSel
*/
static r_dev_IntSel_t loc_ConvertToIntSel(const uint32_t         Unit,
                                          const r_vdce_IntType_t IntType)
{
    r_dev_IntSel_t  int_sel;
    
    switch (IntType)
    {
        case R_VDCE_INTC_VBLANK:
            int_sel = R_DEV_INT_VDCE0S0LOVSYNC;
            break;
        case R_VDCE_INTC_VBLANK_1:
            int_sel = R_DEV_INT_VDCE0S1LOVSYNC;
            break;
        case R_VDCE_INTC_VBLANK_DELAY:
            int_sel = R_DEV_INT_VDCE0GR3VBLANK;
            break;
        case R_VDCE_INTC_SCANLINE:
            int_sel = R_DEV_INT_VDCE0GR3VLINE;
            break;
        case R_VDCE_INTC_OIR_SCANLINE:
            int_sel = R_DEV_INT_VDCE0IRVLINE;
            break;
        case R_VDCE_INTC_OIR_VBLANK:
            int_sel = R_DEV_INT_VDCE0OIRLOVSYNC;
            break;
        case R_VDCE_INTC_OIR_VSCYNC_WRITE:
            int_sel = R_DEV_INT_VDCE0OIRVIVSYNC;
            break;
        case R_VDCE_INTC_CAP_VBLANK:
            int_sel = R_DEV_INT_VDCE0S0VIVSYNC;
            break;
        case R_VDCE_INTC_CAP_END_OF_FIELD:
            int_sel = R_DEV_INT_VDCE0S0VFIELD;
            break;
        case R_VDCE_INTC_ERROR:
        case R_VDCE_INTC_NO_VI_VSYNC_SIGNAL:
        case R_VDCE_INTC_NO_VI_VSYNC_SIGNAL_1:
        case R_VDCE_INTC_ERR_OIR_UNDERFLOW:
        case R_VDCE_INTC_ERR_LAYER0_UNDERFLOW:
        case R_VDCE_INTC_ERR_LAYER1_UNDERFLOW:
        case R_VDCE_INTC_ERR_LAYER2_UNDERFLOW:
        case R_VDCE_INTC_ERR_LAYER3_UNDERFLOW:
        case R_VDCE_INTC_ERR_CAP_WRITE_OVERFLOW:
        default: 
            int_sel = LOC_UNKOWN_INT_SEL;
            break;
    }
    if ((int_sel != LOC_UNKOWN_INT_SEL) && (Unit == 1))
    {
        int_sel = (r_dev_IntSel_t)(int_sel + LOC_INTC_UNIT1_OFFSET);
    }

    return int_sel;
}

/*******************************************************************************
  Section: VDCE Macro Driver Support Functions

  Comments see: <r_vdce_sys.h>
*/

/*******************************************************************************
  Function: R_VDCE_Sys_Init
*/

r_vdce_Error_t R_VDCE_Sys_Init(const uint32_t Unit)
{
    return R_VDCE_ERR_OK;
}

/*******************************************************************************
  Function: R_VDCE_Sys_DeInit
*/

r_vdce_Error_t R_VDCE_Sys_DeInit(const uint32_t Unit)
{
    return R_VDCE_ERR_OK;
}

/*******************************************************************************
  Function: R_VDCE_Sys_BaseAddrGet
*/

uint32_t R_VDCE_Sys_BaseAddrGet(const uint32_t Unit)
{
    uint32_t ret;
    r_dev_Device_t  device  =  R_DEV_GetDev(); 

    if (Unit == R_VDCE_UNIT0)
    {
        ret = R_VDCE_BASE_ADDR0;
    }
    else if (Unit == R_VDCE_UNIT1)
    {
        if (((device >= R_DEV_R7F701408) && (device <= R_DEV_R7F701412)) || /* RH850/D1M2x */
            (device == R_DEV_R7F701441))    /* RH850/D1M1A */
        {
            ret = R_VDCE_BASE_ADDR1;
        }
        else
        {
            ret = 0;
        }
    }
    else
    {
        ret = 0;
    }

    return ret;
}

/*******************************************************************************
  Function: R_VDCE_Sys_PixelClockSet
*/

uint32_t R_VDCE_Sys_PixelClockSet(const uint32_t Unit,
                                  const uint32_t Clock,
                                  const uint32_t OtherUnitActive)
{
    uint32_t ret;
    r_dev_Device_t device;

    if (Unit >= R_VDCE_MACRO_NUM)
    {
        ret = 0;
    }
    else
    {
        /* activate VDCE pixel clock */
        /*  - pixel clock generator depends on the current device */
        device = R_DEV_GetDev();
        if (  ((device >= R_DEV_R7F701401) && (device <= R_DEV_R7F701407))
            || (device == R_DEV_R7F701442)
            || (device == R_DEV_R7F701418) )
        {
            ret = loc_Vdce_Pixel_SetClockL2M1(Unit, Clock);
        } else if (device == R_DEV_R7F701441) {
            ret = loc_Vdce_Pixel_SetClockM1A(Unit, Clock, OtherUnitActive);
        } else {
            ret = loc_Vdce_Pixel_SetClockM2(Unit, Clock);
        }
    }
    
    return ret;
}

/*******************************************************************************
  Function: R_VDCE_Sys_VdceVersionGet
*/

uint32_t R_VDCE_Sys_VdceVersionGet(void)
{
    uint16_t hw_revision;
    uint32_t vdce_revision;

    hw_revision = (uint16_t)READ_16L(PRDNAME_ADDR);

    if((uint32_t)hw_revision < LOC_PRDNAME_THRESHOLD)
    {
        vdce_revision = LOC_VDCE_REVISION_1;
    }
    else
    {
        vdce_revision = LOC_VDCE_REVISION_2;
    }

    return vdce_revision;
}

/*******************************************************************************
  Function: R_VDCE_Sys_IntcInit
*/

r_vdce_Error_t R_VDCE_Sys_IntcInit(const uint32_t Unit)
{
    return loc_IntcCtrlAll(Unit, 1);
}

/*******************************************************************************
  Function: R_VDCE_Sys_IntcDeinit
*/

r_vdce_Error_t R_VDCE_Sys_IntcDeinit(const uint32_t Unit)
{
    return loc_IntcCtrlAll(Unit, 0);
}

/***************************************************************************
  Function: R_VDCE_Sys_IntcEnable
*/
r_vdce_Error_t R_VDCE_Sys_IntcEnable(const uint32_t       Unit,
                                     const r_dev_IntSel_t IntSel)
{
    return loc_IntcCtrl(Unit, IntSel, 1);
}

/***************************************************************************
  Function: R_VDCE_Sys_IntcDisable
*/
r_vdce_Error_t R_VDCE_Sys_IntcDisable(const uint32_t       Unit,
                                      const r_dev_IntSel_t IntSel)
{
    return loc_IntcCtrl(Unit, IntSel, 0);
}

/*******************************************************************************
  Function: R_VDCE_Sys_IntcPrepareClearCheck
*/

uint32_t R_VDCE_Sys_IntcPrepareClearCheck(const uint32_t SyncFreq)
{
    uint32_t   rate;
    uint32_t   sync_freq;
    uint32_t   cpu_cycle = 0;
    uint32_t   i;
    
    /* Use default value if SyncFreq = 0. */
    sync_freq = SyncFreq;
    if (sync_freq == 0)
    {
        sync_freq = LOC_SYNC_FREQ_MIN;
    }
    
    /* Calculate rate to convert from Syncronization clock to CPU clock */
    rate = (LOC_CPU_FREQ_MAX + (sync_freq - 1)) / sync_freq;
    
    /* 1st : Wait for INT_STAx to change 0->1 by NOP instruction. */
    /*       This change will not be caught by software. */
    cpu_cycle = rate * LOC_1ST_WAIT_CYCLE;
    for (i = 0; i < cpu_cycle; i++)
    {
        R_DEV_Nop();
    }
    
    /* 2nd : Wait for INT_STAx to change 1->0 by Register Polling */
    /*       This function returns time-out count of register polling. */
    cpu_cycle = rate * LOC_2ND_WAIT_CYCLE;
    
    return cpu_cycle;
}

/***************************************************************************
  Function: R_VDCE_Sys_IntcClearCheck
*/

r_vdce_Error_t R_VDCE_Sys_IntcClearCheck(const uint32_t         Unit,
                                         const r_vdce_IntType_t IntType)
{
    r_vdce_Error_t ret = R_VDCE_ERR_FATAL_HW;
    r_dev_IntSel_t int_sel;
    uint32_t       i;
    
    int_sel = loc_ConvertToIntSel(Unit, IntType);
    if (int_sel == LOC_UNKOWN_INT_SEL)
    {
        ret = R_VDCE_ERR_RANGE_PARAM;
    }
    else
    {
        for (i = 0; i < LOC_WAIT_CLEAR_COUNT; i++)
        {
            if (R_DEV_IntGetFlag(int_sel) == 0)
            {
                ret = R_VDCE_ERR_OK;
                break;
            }
        }
    }
    return ret;
}

/*******************************************************************************
  Function: R_VDCE_Sys_PortInit
*/

r_vdce_Error_t R_VDCE_Sys_PortInit(const uint32_t Unit)
{
    return R_VDCE_ERR_OK;
}

/*******************************************************************************
  Function: R_VDCE_Sys_PortDeInit
*/

r_vdce_Error_t R_VDCE_Sys_PortDeInit(const uint32_t Unit)
{
    return R_VDCE_ERR_OK;
}

/*******************************************************************************
  Function: R_VDCE_Sys_HsyncActLevelSet
*/
uint32_t R_VDCE_Sys_HsyncActLevelSet(const uint32_t Unit, const int32_t Level)
{

    /* HS Sync inversion is handled inside VDCE macro */
    return 0;
}

/*******************************************************************************
  Function: R_VDCE_Sys_VsyncActLevelSet

*/
uint32_t R_VDCE_Sys_VsyncActLevelSet(const uint32_t Unit, const int32_t Level)
{

    /* VS Sync inversion is handled inside VDCE macro */
    return 0;
}

/*******************************************************************************
  Function: R_VDCE_Sys_ClockActEdgeSet
*/
uint32_t R_VDCE_Sys_ClockActEdgeSet(const uint32_t Unit, const int32_t Edge) 
{
    uint32_t val;

    if (Unit < R_VDCE_MACRO_NUM)
    {
        if (Unit == 0u)
        {
#if USE_VDCE_RSDS
            if(0u != (uint32_t)Edge)
            {
                /* Clock inversion is handled by phase shift in case of RSDS. */
                /* This is not implemented yet. */
                while (1) 
                {
                    /* T.B.D */
                    /* error: loop forever */
                }
            }
#else
            /* Use port logic (PINV) to invert polarity of VDCE0_VO_CLKP signal on P45_0 */
            val = R_DEV_READ_REG(16, LOC_PINV45);
            if(0u == (uint32_t)Edge)
            {
                val &= ~(uint32_t)LOC_MASK_0X00000001;
            }
            else
            {
                val |= LOC_MASK_0X00000001;
            }
            R_DEV_WRITE_PORT_PROT_REG(LOC_PPCMD45, LOC_PINV45, val);
#endif
        }
        if (Unit == 1u)
        {
            /* Use port logic (PINV) to invert polarity of VDCE1_VO_CLK signal on P47_8 */
            val = R_DEV_READ_REG(16, LOC_PINV47);
            if(0u == (uint32_t)Edge)
            {
                val &= ~(uint32_t)LOC_MASK_0X00000100;
            }
            else
            {
                val |= LOC_MASK_0X00000100;
            }
            R_DEV_WRITE_PORT_PROT_REG(LOC_PPCMD47, LOC_PINV47, val);
        }
    }

    /* Clock inversion is NOT handled inside VDCE macro */
    return 1;
}

/*******************************************************************************
  Function: R_VDCE_Sys_DesyncActLevelSet
*/
uint32_t R_VDCE_Sys_DesyncActLevelSet(const uint32_t Unit, const int32_t Level) 
{
    /* DE Sync inversion is handled inside VDCE macro */
    return 0;
}


/*******************************************************************************
  Section: VDCE Macro Driver OS interface

  Comments see: <vdce_sys.h>
*/

/*******************************************************************************
  Function: R_VDCE_Sys_Lock
*/

r_vdce_Error_t R_VDCE_Sys_Lock(const uint32_t Unit)
{
    return R_VDCE_ERR_OK;
}

/*******************************************************************************
  Function: R_VDCE_Sys_Unlock
*/

r_vdce_Error_t R_VDCE_Sys_Unlock(const uint32_t Unit)
{
    return R_VDCE_ERR_OK;
}

/*******************************************************************************
  Function: R_VDCE_Sys_AllLock
*/

r_vdce_Error_t R_VDCE_Sys_AllLock(void)
{
    return R_VDCE_ERR_OK;
}

/*******************************************************************************
  Function: R_VDCE_Sys_AllUnlock
*/

r_vdce_Error_t R_VDCE_Sys_AllUnlock(void)
{
    return R_VDCE_ERR_OK;
}

/*******************************************************************************
  Function: R_VDCE_Sys_VIChannelCheck
*/
r_vdce_Error_t R_VDCE_Sys_VIChannelCheck(const uint32_t Unit)
{
    r_vdce_Error_t  ret    = R_VDCE_ERR_OK;
    r_dev_Device_t  device = loc_GetDev();

    if (device <= R_DEV_R7F701403)       /* RH850/D1Lx */
    {
        ret = R_VDCE_ERR_NOT_ACCEPTABLE;
    }
    else if ((device <= R_DEV_R7F701407)        /* RH850/D1M1(H)  */
                || (device == R_DEV_R7F701418)
                || (device == R_DEV_R7F701442)) /* RH850/D1M1-V2 */
    {
        /* All units are available */
    }
    else if (device <= R_DEV_R7F701410)  /* RH850/D1M2 */
    {
        if (Unit == R_VDCE_UNIT0)
        {
            ret = R_VDCE_ERR_RANGE_UNIT;
        }
    }
    else if (device <= R_DEV_R7F701412)  /* RH850/D1M2H */
    {
        /* All units are available */
    }
    else if (device == R_DEV_R7F701441) /* RH850/D1M1A */
    {
        if (Unit == R_VDCE_UNIT1)
        {
            ret = R_VDCE_ERR_RANGE_UNIT;
        }
    }
    else                                 /* Unknown device */
    {
        ret = R_VDCE_ERR_NOT_ACCEPTABLE;
    }
    return ret;
}

/***********************************************************
  Function: R_VDCE_Sys_MaxResolutionGet
*/

r_vdce_Error_t R_VDCE_Sys_MaxResolutionGet ( uint32_t* ResolutionHmax,
                                             uint32_t* ResolutionVmax )
{
    r_vdce_Error_t ret = R_VDCE_ERR_OK;
    r_dev_Device_t  device  =  loc_GetDev(); 

    if ((R_NULL != ResolutionHmax) && (R_NULL != ResolutionVmax))
    {
        *ResolutionHmax = 0;
        *ResolutionVmax = 0;
        if (device <= R_DEV_R7F701403)       /* RH850/D1Lx */
        {
            *ResolutionHmax = 480;
            *ResolutionVmax = 320;
        }
        else if ((device <= R_DEV_R7F701407)        /* RH850/D1M1(H) */
                    || (device == R_DEV_R7F701418)
                    || (device == R_DEV_R7F701442)) /* RH850/D1M1-V2 */
        {
            *ResolutionHmax = 1024;
            *ResolutionVmax = 1024;
        }
        else if ((device <= R_DEV_R7F701412) || /* RH850/D1M2(H) */
                 (device == R_DEV_R7F701441))   /* RH850/D1M1A   */
        {
            *ResolutionHmax = 1280;
            *ResolutionVmax = 1024;
        }
        else                                 /* Unknown device */
        {
            /* Do nothing */
        }
    }
    else
    {
        ret = R_VDCE_ERR_PARAM_INCORRECT;
    }

    return ret;
}

/***********************************************************
  Function: R_VDCE_Sys_DeviceInfoGet
*/

r_vdce_Error_t R_VDCE_Sys_DeviceInfoGet(r_vdce_DeviceInfo_t* DevInfo)
{
    r_vdce_Error_t ret = R_VDCE_ERR_OK;
    r_dev_Device_t device = loc_GetDev(); 

    if (R_NULL != DevInfo)
    {
        /* Set default */
        DevInfo->Device         = device;
        DevInfo->VIWithDataEn   = R_FALSE;
        DevInfo->SerialRGBEn    = R_FALSE;
        DevInfo->GammaCorrectEn = R_TRUE;
        DevInfo->EnlargementEn  = R_TRUE;
        
        if (device <= R_DEV_R7F701403)       /* RH850/D1Lx */
        {
            DevInfo->GammaCorrectEn = R_FALSE;
            DevInfo->EnlargementEn  = R_FALSE;
        }
        else if (device <= R_DEV_R7F701407)        /* RH850/D1M1(H) */
        {
        }
        else if ((device <= R_DEV_R7F701412)  /* RH850/D1M2x */
              || (device == R_DEV_R7F701418))
        {
            DevInfo->VIWithDataEn   = R_TRUE;
        }
        else if ((device == R_DEV_R7F701441)  /* RH850/D1M1A */
              || (device == R_DEV_R7F701442)) /* RH850/D1M1-V2 */
        {
            DevInfo->VIWithDataEn   = R_TRUE;
            DevInfo->SerialRGBEn    = R_TRUE;
        }
        else 
        {
            /* Do nothing */
        }
    }
    else
    {
        ret = R_VDCE_ERR_PARAM_INCORRECT;
    }

    return ret;
}
