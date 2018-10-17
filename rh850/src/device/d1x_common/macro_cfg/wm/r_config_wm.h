/*
****************************************************************************
PROJECT : WM
FILE    : $Id: r_config_wm.h 13421 2017-05-08 13:49:09Z matthias.nippert $
============================================================================ 
DESCRIPTION
WM configuration
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
****************************************************************************
*/

#ifndef R_CONFIG_WM_H
#define R_CONFIG_WM_H


/***********************************************************
  Title: config_wm.h
  
  D1Mx Mango board WM Configuration
*/

/***********************************************************
  Section: Global Defines
************************************************************/

/***********************************************************
  Constant: MAX_LAYER_*

  default max dimensions given by VDCE on D1Mx
*/

#define R_WM_MAX_LAYER_PITCH            8192  /* 0: no pitch supported */
#define R_WM_MAX_LAYER_WIDTH            1280// 1024
#define R_WM_MAX_LAYER_HEIGHT           1024
#define R_WM_MAX_LAYERS                 4
#define R_WM_MAX_CAPTURE_LAYERS         2
#define R_WM_MAX_SPRITES_PER_LAYER      16


/***********************************************************
  Constant: R_WM_DEV_NUM

  Number of VOUT (WM) units
*/


#define R_WM_DEV_NUM         2

#endif /* R_CONFIG_WM_H_ */
