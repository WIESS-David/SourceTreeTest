/****************************************************************************
 *  License : All rights reserved for TES Electronic Solutions GmbH
 *            See included /docs/license.txt for details
 *  Project : D/AVE HD
 *  Purpose : Compiletime settings, specifics for Linux (C5SoC platform)
 ****************************************************************************
 * Version Control Information :
 *  $Revision: 4771 $
 *  $Date: 2015-02-10 12:01:30 +0100 (Di, 10 Feb 2015) $
 *  $LastChangedBy: florian.zimmermann $
 ****************************************************************************
 * Change History (autogenerated):
 ****************************************************************************/    

#ifndef DAVEHD_SETTINGS_LINUX_H_INCLUDED
#define DAVEHD_SETTINGS_LINUX_H_INCLUDED

#include "davehd_device_data_size_default.h"

/* Our Linux port does not yet support asynchronous mode -> disable asynchronous default*/
#define DHD_KERNEL_SYNCHRONOUS_ONLY

#define DHD_DEBUG_MODE
#define DHD_KERNEL_DEBUG
/*#define DHD_KERNEL_DEBUG_ERROR*/
/*#define DHD_KERNEL_DEBUG_MEMORY*/
/*#define DHD_KERNEL_PROFILE_MEM_USAGE*/
/*#define DHD_PROFILE_MEM_USAGE*/
/*#define DHD_KERNEL_DEBUG_MEMORY_VERBOSE*/
/*#define DHD_KERNEL_DEBUG_TRACEVCD*/
#define DHD_KERNEL_TLIST_SUPPORT
/*#define DHD_KERNEL_DUMP_KERNEL_TLIST*/
#define DHD_SUPPORT_CASSERT
#define DHD_KERNEL_SOFT_IRQ
/*#define DHD_KERNEL_VALLOC_HOOK*/
#define DHD_MEMORY_MMU
#define DHD_MEMORY_NO_UMA

/* TODO: Adjust settings*/
#undef DHD_CACHELINE_SIZE
#define DHD_CACHELINE_SIZE             32
#undef DHD_VIDMEM_MINALIGN_LOG2
#define DHD_VIDMEM_MINALIGN_LOG2       12
#undef DHD_MIN_JOB_SIZE
#define DHD_MIN_JOB_SIZE                512


#endif /*DAVEHD_SETTINGS_LINUX_H_INCLUDED*/
