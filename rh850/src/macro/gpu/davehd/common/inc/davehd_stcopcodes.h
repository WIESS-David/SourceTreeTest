/****************************************************************************
 *  License : All rights reserved for TES Electronic Solutions GmbH
 *            See included /docs/license.txt for details
 *  Project : D/AVE HD
 *  Purpose : HW Opcode detail defines for STC 
 ****************************************************************************
 * Version Control Information :
 *  $Revision: 3935 $
 *  $Date: 2014-09-23 10:52:32 +0200 (Di, 23 Sep 2014) $
 *  $LastChangedBy: golczewskim $
 ****************************************************************************
 * Change History (autogenerated):
 ****************************************************************************/    

#ifndef DAVEHD_STCOPCODES_H_INCLUDED
#define DAVEHD_STCOPCODES_H_INCLUDED

#include "davehd_types.h"

/*----------------------------------------------------------------------------------------------------------*/
/*                                                                                                          */

#define DHD_STCOP_NOP          0x00
#define DHD_STCOP_WAIT         0x01
#define DHD_STCOP_WRITE        0x04
#define DHD_STCOP_DUMP         0x05
#define DHD_STCOP_JUMPCALL     0x08
#define DHD_STCOP_RETURN       0x09
#define DHD_STCOP_CHECKSUM     0x7e
#define DHD_STCOP_END          0x7f

#define DHD_STC_JUMP           0x00
#define DHD_STC_CALL           0x01

/*----------------------------------------------------------------------------------------------------------*/
/* Format of STC opcodes*/
/**/
typedef union tagdhd_stc_opcode {
  struct tagdhd_reg_stc_opcode_fields_t {
    dhd_uint32_t m_data_count     : 8;     /* number of datawords following*/
    dhd_uint32_t m_arguments      : 16;    /* opcode arguments*/
    dhd_uint32_t m_opcode         : 7;     
    dhd_uint32_t m_data_following : 1;     /* set if opcode has data words following                   */
  } m_fields;
  dhd_uint32_t m_value;
} dhd_stc_opcode_t;



#define DHD_TARGET_CHECKSUM 0xffffffffu
#define DHD_CHECKSUM_MAGIC 0x05d4u

#endif /*DAVEHD_STCOPCODES_H_INCLUDED*/

