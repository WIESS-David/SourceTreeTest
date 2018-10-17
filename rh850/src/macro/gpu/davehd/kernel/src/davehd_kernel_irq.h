/****************************************************************************
 *  License : All rights reserved for TES Electronic Solutions GmbH
 *            See included /docs/license.txt for details
 *  Project : D/AVE HD
 *  Purpose : Interrupt service codes
 ****************************************************************************
 * Version Control Information :
 *  $Revision: 3935 $
 *  $Date: 2014-09-23 10:52:32 +0200 (Di, 23 Sep 2014) $
 *  $LastChangedBy: golczewskim $
 ****************************************************************************
 * Change History (autogenerated):
 ****************************************************************************/    

#ifndef DAVEHD_KERNEL_IRQ_H_INCLUDED
#define DAVEHD_KERNEL_IRQ_H_INCLUDED

#include "davehd_types.h"

/*----------------------------------------------------------------------------------------------------------*/

void DHD_IRQ_CALL dhd_gpu_irq_handler(dhd_gpu_device_data_t *a_device);

/*----------------------------------------------------------------------------------------------------------*/

void dhd_gpu_enable_irq(dhd_gpu_device_data_t *a_device);
void dhd_gpu_disable_irq(dhd_gpu_device_data_t *a_device);

/*----------------------------------------------------------------------------------------------------------*/

#endif /*DAVEHD_KERNEL_IRQ_H_INCLUDED*/
