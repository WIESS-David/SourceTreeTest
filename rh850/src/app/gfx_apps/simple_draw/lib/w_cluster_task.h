//============================================================================================================
//
// Header file for the Cluster table rule
//
//============================================================================================================
#ifndef _WIESS_CLUSTER_TASK_H_
#define _WIESS_CLUSTER_TASK_H_
#include "r_typedefs.h"
#include "main.h"
// Nios Cluster State Machine Define
#define _STATE_CLUSTER_INIT                        0x00
#define _STATE_CLUSTER_DISPLAY                     0x01
#define _STATE_CLUSTER_ERROR                       0x02

//************************************************************************************************************
//  Function define
//************************************************************************************************************
void W_Cluster_Init(void);
void W_Cluster_Display(void);
void W_Cluster_Error(void);
void W_Cluster_Timer1(void);
void W_Cluster_Timer2(void);
void W_Cluster_AllTimer(void);

#endif

