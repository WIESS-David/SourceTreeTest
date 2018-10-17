/*
****************************************************************************
PROJECT : WM
FILE    : $Id: r_sys_wm.c 8106 2016-03-24 15:02:00Z florian.zimmermann $
============================================================================ 
DESCRIPTION
WM system functions for D1x
============================================================================
                            C O P Y R I G H T
============================================================================
                       Copyright (c) 2013 - 2016
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
****************************************************************************
*/

/***********************************************************
  Title: D1Mx and D1Lx WM SYS API implementation  
*/

/***********************************************************
  Section: Includes
*/

#include "r_typedefs.h"
#include "r_cdi_api.h"

#include "r_ddb_api.h"      /* display database with display timings */
#include "r_vdce_api.h"
#include "r_spea_api.h"

#include "r_config_wm.h"
#include "r_wm_api.h"
#include "r_wm_sys.h"
#include "r_sys_wm.h"

#include "r_dev_pin_types.h"
#include "r_dev_api.h"

#if defined R_WM_MSQ_DEQUEUE_PROFILING || defined R_WM_MSQ_ENQUEUE_PROFILING
#include "r_profile.h"
#endif

#ifdef R_WM_SYS_ERROR_TRACKING
    #include "r_bsp_stdio_api.h"
#endif

/***********************************************************
  Section: Local Defines
*/


/***********************************************************
  Section: Global Variables
*/

/***********************************************************
  Variable : r_wm_DevSys
  
  Sys layer device object.
*/
extern r_wm_DevSys_t r_wm_DevSys[R_WM_DEV_NUM];

/***********************************************************
  Section: Local Function Declarations
*/


static float32_t loc_floorf( float32_t x );
static int32_t   loc_iroundf( float32_t f );
static float32_t loc_minf( float32_t a, float32_t b );
static float32_t loc_maxf( float32_t a, float32_t b );
static float32_t loc_log2( float32_t i );
static float32_t loc_pow2( float32_t i );
static float32_t loc_powf( float32_t a, float32_t b );


static uint32_t loc_fitCurveForVdce( uint16_t *InValue, float32_t  *OutValue, uint32_t NumEntries,
                                     r_vdce_GammaCorrect_t *FittedCurve );
static uint32_t loc_getGammaCurve( uint32_t Unit, uint8_t Gamma, r_vdce_GammaCorrect_t *Curve );



/***********************************************************
  Section: Local Variables
*/


/***********************************************************
  Section: Local Functions
*/

/*
The following function are a replacement for math.h to ensure, the drivers are stdlib-free.
You may replace these functions by a library implementation if available.

MISRA disabled due to binary fiddling with floating point variables in <loc_log2> and <loc_pow2>.
*/
#pragma ghs startnomisra

/***********************************************************
  Function: loc_floorf
  
  You may replace this by a library implementation if available.
*/
static float32_t loc_floorf( float32_t x )
{
    int64_t y = (int64_t)x;
    if (x < 0) {
        y--;
    }
    return y;
}

/***********************************************************
  Function: loc_iroundf
  
  You may replace this by a library implementation if available.
*/
static int32_t loc_iroundf( float32_t f )
{
    f += (f>0) ? 0.5f : -0.5f;
    return (int32_t)f;
}


/*******************************************************************************
  Function: minf

  Return the minimum of the two supplied integer values

  You may replace this by a library implementation if available.

  Parameters:
  a,b  - Values to be compared
  
  Returns:
  int32_t
*/
static float32_t loc_minf( float32_t a, float32_t b )
{
    return ( a < b ) ? a : b;
}

/***********************************************************
  Function: loc_maxf
  
  You may replace this by a library implementation if available.
*/
static float32_t loc_maxf( float32_t a, float32_t b )
{
    return ( a > b ) ? a : b;
}

typedef union intfloat32_tag
{
	float32_t fVal;
	uint32_t  i32Val;

} intfloat32_t;

/***********************************************************
  Function: loc_log2
  
  Needed by <loc_powf>.
*/
static float32_t loc_log2( float32_t i )
{
    const float32_t OOshift23 = 1.0f / (1uL<<23);
    const float32_t LogBodge  = 0.346607f;

    intfloat32_t _i;
    float32_t x;
    float32_t y;
    
    /* Cast types via union as we want to keep the bits as they are */
    _i.fVal = i;
    
    x  = (float32_t)_i.i32Val;
    x *=  OOshift23; /* 1/(2^23) */
    x  =  x - 127;
    
    y = x - loc_floorf(x);
    y = (y-(y*y) ) * LogBodge;
    
    return x+y;
}

/***********************************************************
  Function: loc_pow2
  
  Needed by <loc_powf>.
*/
static float32_t loc_pow2( float32_t i )
{
    const float32_t shift23  = (1uL<<23);
    const float32_t PowBodge = 0.33971f;

    intfloat32_t _x;
    float32_t x;
    float32_t y;
    
    y = i - loc_floorf(i);
    y = (y-(y*y)) * PowBodge;
    
    x  = (i + 127) - y;
    x *= shift23; /* 2^23 */

    /* Cast types via union as we want to keep the bits as they are */
    _x.i32Val = (int32_t)x;
    
	return _x.fVal;
}

/***********************************************************
  Function: loc_powf
  
  The maximum error for this implementation of powf for the range of [0.5 .. 4] is 0.7%
  This is sufficient for its intended purpose.
  You may replace this by a library implementation if available.
*/
static float32_t loc_powf( float32_t a, float32_t b )
{
    return loc_pow2( b * loc_log2(a) );
}

#pragma ghs endnomisra



/***************************************************************************
  Function: loc_fitCurveForVdce
  
  Takes two arrays <InValue>, <OutValue> of exactly 33 points, that represent 
  a colour correction curve. 
  
  <InValue> is expected to be in range of [0..256] including 256 for the last element!
  <OutValue is expected to be in range of [0..256] including 256 for the last element!
  
  Both series <InValue>, <OutValue> are required to start in the Origin (0,0), thus <InValue>[0] 
  and <OutValue>[0] need to be zero. 
  Non-matching <InValue> is an error! Non-matching <OutValue> will be fitted!
  
  Both series <InValue>, <OutValue> are required to be monotonically nondecreasing, thus 
  the next value is always bigger than the previous. Non-matching values will be fitted!

  Both series <InValue>, <OutValue> are required to have a slope not bigger than 2, thus 
  for a distance dIn: dOut < 2*dIn . Non-matching values will be fitted!
  
  Parameters:
  InValue       - 33 Reference points starting with ZERO
  OutValue      - 33 Reference points
  NumEntries    - Number of reference points
  FittedCurve   - Reference to the curve to be filled
*/
static uint32_t loc_fitCurveForVdce ( 
                                uint16_t   *InValue, 
                                float32_t  *OutValue, 
                                uint32_t    NumEntries,
                                r_vdce_GammaCorrect_t *FittedCurve )
{
    const float32_t max_gain = 2047.0f/1024.0f;
    
    float32_t fittedOutValue_last = 0.0f;
    float32_t fittedOutValue = 0.0f;

    uint32_t  i;
    uint8_t   areaSize;
    
    /* Expecting exactly 33 entries */
    if ( 33 != NumEntries )
    {
        return 0;
    }
    
    /* First entry is expected to start with zero */
    if ( InValue[0] != 0 )
    {
        return 0;
    }
    
    for (i=1; i< NumEntries; i++)
    {
        /* Restrict the area size to positive values bigger than zero */
        if ( InValue[i] <= InValue[i-1] )
        {
            areaSize = 1;
        }
        else
        {
            areaSize = InValue[i] - InValue[i-1];
        }
        
        /* Calculate a (new, if necessary) output value that lies within the HW restrictions */
        fittedOutValue = fittedOutValue_last
                       + loc_maxf( loc_minf( 
                                        OutValue[i] - fittedOutValue_last, 
                                        areaSize * max_gain ),
                                      0.0f );
        
        /* Calculate the corresponding gain and area */
        FittedCurve->Gain[i-1] = loc_iroundf( loc_minf( 2047.0f,
                                  ((fittedOutValue - fittedOutValue_last) * 1024.0f) / areaSize )  );
        FittedCurve->Area[i-1] = InValue[i-1];
        
        fittedOutValue_last = fittedOutValue;
    }
    
    return 1;
}

/***************************************************************************
  Function: loc_getGammaCurve
  
  Fill the supplied Struct with a color correction curve, that matches the given Gamma Coefficient.
  
  Parameters:
  Gamma     - Gamma coeficcient [32..256]. Internall scaled by 1/128. 
              Floating-point range is [0.25f .. 2.0f]
  Curve     - Color correction curve to be input into VDCE.
*/
static uint32_t loc_getGammaCurve( uint32_t Unit, uint8_t Gamma, r_vdce_GammaCorrect_t *Curve )
{
    /* For the gamma correction curve, we take a constant size of the segements so they are equally distributed */
    const uint8_t   seg_size = 8;
    
    float32_t f_gamma;
    uint8_t   segment;
    
    uint16_t  In[33] = {0};
    float32_t Out[33] = {0};
    
    if ( 0 == Curve )
    {
        R_WM_SYS_REPORT_ERROR(Unit, R_WM_ERR_NULL_PTR_ARGUMENT, 0);
        return 0;
    }
    
    f_gamma = Gamma / 128.0f;
    f_gamma = (0.25f > f_gamma) ? 0.25f : f_gamma;

    /* Step 0 is special. The curve starts always in the origin (0,0) */
    In[0] =   0u;
    Out[0] =  0.0f;

    for (segment = 1; segment<33; segment++)
    {
        /* Input brightness value */
        In[segment] = (segment) * seg_size;
        
        /* Output value as we want it */
        Out[segment] = 255.0f * loc_powf( In[segment]/255.0f, 1.0f/f_gamma );
    }
    
    /* Create a curve over the input/output values, staying in the capabilities of the VDCE */
    if (0 == loc_fitCurveForVdce ( In, Out, 33, Curve))
    {
        R_WM_SYS_REPORT_ERROR(Unit, R_WM_ERR_PARAM_INCORRECT, 0);
        return 0;
    }
    
    return 1;
}


/***********************************************************
  Section: WM Driver Support Functions
  
  Comments see: <WM Support Functions>
*/



/***********************************************************
  Group: WM Video Output Screen interface
*/


/***********************************************************
  Function: R_WM_Sys_ScreenTimingSetByName
*/
uint32_t R_WM_Sys_ScreenTimingSetByName     (   uint32_t        Unit,
                                                const int8_t*   Name
                                            )
{
    r_ddb_Timing_t *timing;

    if ((Name == 0) || (*Name == 0))
    {
        R_WM_SYS_REPORT_ERROR(Unit, R_WM_ERR_PARAM_INCORRECT, 0);
        return 0;
    }
    timing = R_DDB_GetDisplayTiming((const char*) Name);

    return R_WM_Sys_ScreenTimingSet(Unit, timing);
}

/***********************************************************
  Function: R_WM_Sys_ScreenTimingSet
*/
uint32_t R_WM_Sys_ScreenTimingSet(uint32_t Unit, r_ddb_Timing_t *timing)
{
    if (timing == 0)
    {
        R_WM_SYS_REPORT_ERROR(Unit, R_WM_ERR_PARAM_INCORRECT, 0);
        return 0;
    }
    
    if(R_VDCE_ERR_OK != R_VDCE_DisplayTimingSet(Unit, timing))
    {
        return 0;
    }
    /* set the interrupt service routines */
    /* SCANLINE interrupt */
    if (R_VDCE_ERR_OK != R_VDCE_IntcCallbackSet(Unit, R_VDCE_INTC_SCANLINE, loc_WM_Isr[Unit][R_WM_EVENT_SCANLINE]))
    {
        return 0;
    }

    if (timing->ScreenHeight > 240)
    {
        if (R_VDCE_ERR_OK != R_VDCE_IntcScanlineSet(Unit, timing->ScreenHeight-50))
        {
            return 0;
        }
    }
    else
    {
        if (R_VDCE_ERR_OK != R_VDCE_IntcScanlineSet(Unit, timing->ScreenHeight/10))
        {
            return 0;
        }
    }
    if (R_VDCE_ERR_OK != R_VDCE_IntcEnable(Unit, R_VDCE_INTC_SCANLINE))
    {
        return 0;
    }

    /* VBLANK interrupt */
    if (R_VDCE_ERR_OK != R_VDCE_IntcCallbackSet(Unit, R_VDCE_INTC_VBLANK, loc_WM_Isr[Unit][R_WM_EVENT_VBLANK]))
    {
        return 0;
    }
    if (R_VDCE_ERR_OK != R_VDCE_IntcEnable(Unit, R_VDCE_INTC_VBLANK))
    {
        return 0;
    }
    
    r_wm_DevSys[Unit].HorizontalMoveReserve = timing->H.BlankWidth - 16;

    return 1;
}

/***********************************************************
  Function: R_WM_Sys_ScreenEnable
*/
uint32_t R_WM_Sys_ScreenEnable(uint32_t Unit, uint32_t Enabled)
{
    r_vdce_Error_t err;
    if (Enabled == 1)
    {
        if (R_VDCE_ERR_OK != R_VDCE_IntcEnable(Unit, R_VDCE_INTC_SCANLINE))
        {
            return 0;
        }
        if (R_VDCE_ERR_OK != R_VDCE_IntcEnable(Unit, R_VDCE_INTC_VBLANK))
        {
            return 0;
        }
        err = R_VDCE_DisplayEnable(Unit);
    }
    else
    {
        if (R_VDCE_ERR_OK != R_VDCE_IntcDisable(Unit, R_VDCE_INTC_SCANLINE))
        {
            return 0;
        }
        if (R_VDCE_ERR_OK != R_VDCE_IntcDisable(Unit, R_VDCE_INTC_VBLANK))
        {
            return 0;
        }
        if (R_VDCE_ERR_OK != R_VDCE_IntcDisable(Unit, R_VDCE_INTC_ERR_CAP_WRITE_OVERFLOW))
        {
            return 0;
        }
        if (R_VDCE_ERR_OK != R_VDCE_IntcDisable(Unit, R_VDCE_INTC_ERR_LAYER0_UNDERFLOW))
        {
            return 0;
        }
        if (R_VDCE_ERR_OK != R_VDCE_IntcDisable(Unit, R_VDCE_INTC_ERR_LAYER1_UNDERFLOW))
        {
            return 0;
        }
        if (R_VDCE_ERR_OK != R_VDCE_IntcDisable(Unit, R_VDCE_INTC_ERR_LAYER2_UNDERFLOW))
        {
            return 0;
        }
        if (R_VDCE_ERR_OK != R_VDCE_IntcDisable(Unit, R_VDCE_INTC_ERR_LAYER3_UNDERFLOW))
        {
            return 0;
        }

        err = R_VDCE_DisplayDisable(Unit);
    }
    
    if (err != R_VDCE_ERR_OK)
    {
        return 0;
    }
    return 1;
}

/***********************************************************
  Function: R_WM_Sys_ScreenBgColorSet
*/
uint32_t R_WM_Sys_ScreenBgColorSet(uint32_t Unit, 
                                       uint8_t  Red, 
                                       uint8_t  Green, 
                                       uint8_t  Blue)
{
    if ( R_VDCE_ERR_OK != R_VDCE_DisplayColorSet(Unit, Red, Green, Blue) )
    {
        return 0;
    }
    return 1;
}

/***********************************************************
  Function: R_WM_Sys_ScreenColorCurveSet
*/
uint32_t R_WM_Sys_ScreenColorCurveSet(  uint32_t Unit,
                                        const r_wm_ClutEntry_t *ColorCurve,
                                        uint32_t NumEntries
                                      )
{
    r_vdce_GammaCorrect_t R = {0}, G = {0}, B = {0};
    r_vdce_Gamma_t vdceGamma = {0};
    
    uint32_t i;
    
    uint16_t  InValue[33];
    float32_t OutValueR[33], OutValueG[33], OutValueB[33];

    if ( (0 == ColorCurve) || (33 != NumEntries) )
    {
        R_WM_SYS_REPORT_ERROR(Unit, R_WM_ERR_PARAM_INCORRECT, 0);
        return 0;
    }
    
    vdceGamma.R = &R;
    vdceGamma.G = &G;
    vdceGamma.B = &B;

    for (i=0; i<33; i++)
    {
        InValue[i] =  i*8;
        OutValueR[i] = ColorCurve[i].R;
        OutValueG[i] = ColorCurve[i].G;
        OutValueB[i] = ColorCurve[i].B;
    }
    
    if (0 == loc_fitCurveForVdce ( InValue, OutValueR, 33, &R ))
    {
        R_WM_SYS_REPORT_ERROR(Unit, R_WM_ERR_PARAM_INCORRECT, 0);
        return 0;
    }
    if (0 == loc_fitCurveForVdce ( InValue, OutValueG, 33, &G ))
    {
        R_WM_SYS_REPORT_ERROR(Unit, R_WM_ERR_PARAM_INCORRECT, 0);
        return 0;
    }
    if (0 == loc_fitCurveForVdce ( InValue, OutValueB, 33, &B ))
    {
        R_WM_SYS_REPORT_ERROR(Unit, R_WM_ERR_PARAM_INCORRECT, 0);
        return 0;
    }
    
    if ( R_VDCE_ERR_OK !=  R_VDCE_DisplayGammaCorrectSet( Unit, &vdceGamma ) )
    {
        return 0;
    }
    
    return 1;
}

/***************************************************************************
  Function: R_WM_Sys_ScreenGammaSet
*/
uint32_t R_WM_Sys_ScreenGammaSet(   uint32_t Unit, 
                                    const uint8_t GammaRed, 
                                    const uint8_t GammaGreen, 
                                    const uint8_t GammaBlue 
                                )
{
    r_vdce_GammaCorrect_t vdceGammaRed =   {0};
    r_vdce_GammaCorrect_t vdceGammaGreen = {0};
    r_vdce_GammaCorrect_t vdceGammaBlue =  {0};
    r_vdce_Gamma_t vdceGamma = {0};

    if ( (128 == GammaRed) && (128 == GammaGreen) && (128 == GammaBlue) )
    {
        /* All one? Then turn off Gamma correction */
        if ( R_VDCE_ERR_OK !=  R_VDCE_DisplayGammaCorrectSet( Unit, R_NULL ) )
        {
            return 0;
        }
    }
    else
    {
        if ( 0 == loc_getGammaCurve( Unit, GammaRed, &vdceGammaRed ))
        {
            return 0;
        }
        vdceGamma.R =    &vdceGammaRed;
        
        /* Same as red gamma? Reuse red calculation */
        if ( GammaGreen == GammaRed )
        {
            vdceGamma.G =    &vdceGammaRed;
        }
        else
        {
            if ( 0 == loc_getGammaCurve( Unit, GammaGreen, &vdceGammaGreen ))
            {
                return 0;
            }
            vdceGamma.G =    &vdceGammaGreen;
        }
        
        /* Same as red gamma? Reuse red calculation */
        if ( GammaBlue == GammaRed )
        {
            vdceGamma.B =    &vdceGammaRed;
        }
        /* Same as green gamma? Reuse green calculation */
        else if ( GammaBlue == GammaGreen )
        {
            vdceGamma.B =    &vdceGammaGreen;
        }
        else
        {
            if ( 0 == loc_getGammaCurve( Unit, GammaBlue, &vdceGammaBlue ))
            {
                return 0;
            }
            vdceGamma.B =    &vdceGammaBlue;
        }
        
        if ( R_VDCE_ERR_OK !=  R_VDCE_DisplayGammaCorrectSet( Unit, &vdceGamma ) )
        {
            return 0;
        }
    }
    
    return 1;
}

/***********************************************************
  Function: R_WM_Sys_ScreenColorFormatSet
*/
uint32_t R_WM_Sys_ScreenColorFormatSet  (   uint32_t            Unit,
                                                r_wm_OutColorFmt_t  OutFmt
                                            )
{
    uint32_t  Format = ((uint32_t)OutFmt&(~(uint32_t)R_WM_OUTCOLORFMT_FLAG_MASK));
    uint32_t  Flags  = ((uint32_t)OutFmt&( (uint32_t)R_WM_OUTCOLORFMT_FLAG_MASK));

    r_vdce_OutEndian_t  VdceOutEndian = R_VDCE_OUT_ENDIAN_LAST;
    r_vdce_OutFormat_t  VdceOutFormat = R_VDCE_OUT_FORMAT_LAST;
    r_vdce_OutSwap_t    VdceOutSwap   = R_VDCE_OUT_SWAP_BR_LAST;
    r_vdce_DitherMd_t   VdceDitherMd  = R_VDCE_DTH_MD_NUM;
    r_vdce_Dither_t     VdceDither;

    /* Parse flags endian */
    if (0 == ( Flags & (uint32_t)R_WM_OUTCOLORFMT_FLAG_ENDIAN ) )
    {
        /* Default value when flag is not set. */
        VdceOutEndian = R_VDCE_OUT_ENDIAN_LITTLE;
    }
    else
    {
        VdceOutEndian = R_VDCE_OUT_ENDIAN_BIG;

        /* Remove parsed flag */
        Flags = Flags & ~(uint32_t)R_WM_OUTCOLORFMT_FLAG_ENDIAN;
    }

    /* Parse flags SWAP */
    if (0 == ( Flags & (uint32_t)R_WM_OUTCOLORFMT_FLAG_SWAP_BR ) )
    {
        /* Default value when flag is not set. */
        VdceOutSwap = R_VDCE_OUT_SWAP_BR_OFF;
    }
    else
    {
        VdceOutSwap = R_VDCE_OUT_SWAP_BR_ON;

        /* Remove parsed flag */
        Flags = Flags & ~(uint32_t)R_WM_OUTCOLORFMT_FLAG_SWAP_BR;
    }
    
    /* Parse flags Dither */
    if (0 == ( Flags & (uint32_t)R_WM_OUTCOLORFMT_FLAG_DITHER ) )
    {
        /* Default value when flag is not set. */
        VdceDitherMd = R_VDCE_DTH_MD_NUM;
    }
    else
    {
        VdceDitherMd = R_VDCE_DTH_MD_RAND;  /* Select: Random pattern dither */

        /* Remove parsed flag */
        Flags = Flags & ~(uint32_t)R_WM_OUTCOLORFMT_FLAG_DITHER;
    }

    /* Parse color formats */
    switch(Format)
    {
        case R_WM_OUTCOLORFMT_RGB888:
            VdceOutFormat = R_VDCE_OUT_FORMAT_RGB888;
            break;
        case R_WM_OUTCOLORFMT_RGB666:
            VdceOutFormat = R_VDCE_OUT_FORMAT_RGB666;
            break;
        case R_WM_OUTCOLORFMT_RGB565:
            VdceOutFormat = R_VDCE_OUT_FORMAT_RGB565;
            break;
        default:
            VdceOutFormat = R_VDCE_OUT_FORMAT_LAST;
            break;
    }
    
    /* Check if we could assign valid values */
    if ( (0 != Flags) || (R_VDCE_OUT_ENDIAN_LAST == VdceOutEndian) ||
                         (R_VDCE_OUT_SWAP_BR_LAST == VdceOutSwap))
    {
        /* Unsupported flags */
        R_WM_SYS_REPORT_ERROR(Unit, R_WM_ERR_DISPLAY_OUTPUT_FORMAT_SET, 0);
        return 0; /* R_WM_ERR_DISPLAY_OUTPUT_FORMAT_SET */
    }

    if (R_VDCE_OUT_FORMAT_LAST == VdceOutFormat)
    {
        /* Unsupported color format */
        R_WM_SYS_REPORT_ERROR(Unit, R_WM_ERR_DISPLAY_OUTPUT_FORMAT_SET, 0);
        return 0; /* R_WM_ERR_DISPLAY_OUTPUT_FORMAT_SET */
    }
    
    
    
    /* Configure VDCE */
    if(R_VDCE_ERR_OK != R_VDCE_DisplayOutEndianSet(Unit, VdceOutEndian))
    {
        return 0;
    }

    if(R_VDCE_ERR_OK != R_VDCE_DisplayOutSwapBR(Unit, VdceOutSwap))
    {
        return 0;
    }

    if(R_VDCE_ERR_OK != R_VDCE_DisplayOutFormatSet(Unit, VdceOutFormat))
    {
        return 0;
    }
    
    if (R_VDCE_DTH_MD_NUM != VdceDitherMd)
    {
        VdceDither.Sel = R_VDCE_DTH_MD_RAND;
        VdceDither.Pa  = 3; /* Dummy values for Pa-Pd. */
        VdceDither.Pb  = 0; /*  Only needed for 2x2 pattern dither (R_VDCE_DTH_MD_2X2) */
        VdceDither.Pc  = 2;
        VdceDither.Pd  = 1;
        if (R_VDCE_ERR_OK != R_VDCE_DisplayCalibrationSet(Unit, 0, 0, &VdceDither))
        {
            return 0;
        }
    }

    return 1;
}

