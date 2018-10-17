/****************************************************************************
 *  License : All rights reserved for TES Electronic Solutions GmbH
 *            See included /docs/license.txt for details
 *  Project : D/AVE HD
 *  Purpose : Kernel Video Memory Manager
 ****************************************************************************
 * Version Control Information :
 *  $Revision: 9286 $
 *  $Date: 2016-07-07 12:54:47 +0200 (Do, 07 Jul 2016) $
 *  $LastChangedBy: michael.golczewski $
 ****************************************************************************
 * Change History (autogenerated):
 ****************************************************************************/    

#ifndef DAVEHD_VIDMEM_ALLOCATOR_H_INCLUDED
#define DAVEHD_VIDMEM_ALLOCATOR_H_INCLUDED

#include "davehd_types.h"


/*----------------------------------------------------------------------------------------------------------*/
/* heap structure configuration*/

#define DHD_VIDMEM_CHUNKSIZE      64                       /* number of block header structures allocated in one chunk*/
#define DHD_VIDMEM_FLINDEX_MAX    30ul                       /* entries in first level table (maximum allocation size is 1<<FLINDEX_MAX)*/
#define DHD_VIDMEM_SLCOUNT_LOG2   5ul                        /* log2 of second level linear subdivisions for each block size */

/*----------------------------------------------------------------------------------------------------------*/
/* derived constants*/

#define DHD_VIDMEM_MINALIGN       (1ul << DHD_VIDMEM_MINALIGN_LOG2)
#define DHD_VIDMEM_SLCOUNT        ((dhd_uint32_t)(1ul << DHD_VIDMEM_SLCOUNT_LOG2))
#define DHD_VIDMEM_FLSHIFT        ((dhd_uint32_t)(DHD_VIDMEM_SLCOUNT_LOG2 + DHD_VIDMEM_MINALIGN_LOG2))
#define DHD_VIDMEM_FLCOUNT        ((DHD_VIDMEM_FLINDEX_MAX - DHD_VIDMEM_FLSHIFT) + 1)
#define DHD_VIDMEM_SMALLBLOCK     (1ul << DHD_VIDMEM_FLSHIFT)
#define DHD_VIDMEM_MINALIGN_MASK  (DHD_VIDMEM_MINALIGN - 1)

/*----------------------------------------------------------------------------------------------------------*/
/**/
typedef struct tagdhd_gpu_vidmem_block {
  struct tagdhd_gpu_vidmem_block  *m_nextadr;                 /* links to adjacent blocks in memory*/
  struct tagdhd_gpu_vidmem_block  *m_prevadr;
  struct tagdhd_gpu_vidmem_block  *m_nextfree;                /* links to other blocks in the same freelist */
  struct tagdhd_gpu_vidmem_block  *m_prevfree;  
  dhd_gpu_ptr_t                    m_address;                 /* start address in gpu ram*/
  dhd_uint32_t                     m_size;                    /* size in bytes*/
  dhd_bool_t                       m_free : 1;                /* set if the block is not currently allocated*/
  dhd_bool_t                       m_head : 1;                /* set if this block is the head of a chunk of blocks*/

} dhd_gpu_vidmem_block_t;

/*----------------------------------------------------------------------------------------------------------*/
/**/
typedef struct tagdhd_gpu_vidmem_heap {
  dhd_uint32_t            m_sizeof;                           /* size of this structure (for validation)*/
  dhd_gpu_ptr_t           m_base_address;                     /* general heap gpu start address*/
  dhd_uint32_t            m_size;                             /* overall heap size*/
  dhd_gpu_vidmem_block_t  m_memory_list;                      /* circular list of all blocks and sentinel node*/
  dhd_gpu_vidmem_block_t *m_free_block;                       /* pointer to next unused block structure*/

  dhd_uint32_t            m_fl_bitmap;                        /* first level freelist mask bitmap*/
  dhd_uint32_t            m_sl_bitmap[DHD_VIDMEM_FLCOUNT];    /* second level freelist mask bitmaps   */
  dhd_gpu_vidmem_block_t *m_blocks[DHD_VIDMEM_FLCOUNT][DHD_VIDMEM_SLCOUNT]; /* free block lists grouped by blocksize*/

} dhd_gpu_vidmem_heap_t;

/*----------------------------------------------------------------------------------------------------------*/

dhd_gpu_vidmem_heap_t *dhd_internal_create_heap(const dhd_gpu_config_t *a_config);
dhd_enum_t dhd_internal_destroy_heap(dhd_gpu_vidmem_heap_t *a_heap);
dhd_enum_t dhd_internal_alloc_vidmem(dhd_gpu_vidmem_heap_t *a_heap, dhd_gpu_connection_t *a_client, dhd_uint32_t a_size, dhd_uint32_t a_alignment, dhd_uint32_t a_tag, dhd_uint32_t a_usage, dhd_vidmem_t *a_result);
dhd_enum_t dhd_internal_free_vidmem(dhd_gpu_vidmem_heap_t *a_heap, dhd_gpu_connection_t *a_client, dhd_vidmem_t *a_memory);
dhd_bool_t dhd_internal_is_on_heap(const dhd_gpu_vidmem_heap_t *a_heap, dhd_gpu_ptr_t a_address);

/*----------------------------------------------------------------------------------------------------------*/

void dhd_internal_debug_heap(dhd_gpu_device_data_t *a_device, dhd_gpu_vidmem_heap_t *a_heap);

/*----------------------------------------------------------------------------------------------------------*/

#endif /*DAVEHD_VIDMEM_ALLOCATOR_H_INCLUDED*/
