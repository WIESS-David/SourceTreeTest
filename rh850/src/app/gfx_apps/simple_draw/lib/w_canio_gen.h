/* CAUTION: THIS FILE IS AUTOGEN, DON'T MODIFY 
 * DON'T MODIFY THIS FILE
 * DON'T MODIFY THIS FILE
 */

#include "r_typedefs.h"
#include <strings.h>

#define RSCFD0_BASE 0xFFD00000
#define CAN_IDS [0x287, 0x133, 0x134, 0x53e, 0x53f, 0x434, 0x340, 0x56b]

#define MAP_CAN_SIZE	8
#define MAP_GOIP_SIZE	1
#define MAP_CANIO_SIZE (MAP_CAN_SIZE + MAP_GOIP_SIZE)
#define MAP_CANIO_BANK_SIZE  MAP_CANIO_SIZE*8

typedef enum { BOOT = 1, NORMAL, AWAKED, SHUTDOWN} cluster_state;
typedef enum { BACKGROUND = 0, MAIN, OVERLAY, OVERYLAY2} draw_layer;

typedef struct
{
    uint16_t x;
    uint16_t y;
    uint16_t w;
    uint16_t h;
} rect_t;

typedef struct
{
    uint8_t ID;
    cluster_state state;
    draw_layer layer;
    rect_t draw_rect;
    uint8_t can_index;
    uint8_t can_byte;
    uint8_t can_nbytes;
    uint8_t can_bit;
    uint8_t can_nbits;
    uint8_t can_new_data;
    void * new_data_func_ptr;
    void * draw2d_func_ptr;
    void * play_auto_func_ptr;
} cluster_unit_def;

extern uint8_t u8mem_can_data[MAP_CANIO_SIZE*8*2];

//TODO: verify this
void W_Copy_CAN_to_MEM() {
    uint8_t q;
    uint8_t * SrcAddr;
    uint8_t * DestAddr;
	
	/* copy to revious(t-1) */
	DestAddr = u8mem_can_data;
	SrcAddr = u8mem_can_data + MAP_CANIO_BANK_SIZE;
	memcpy(DestAddr, SrcAddr, 8);
	
	/* write to t(0) */
	DestAddr = u8mem_can_data;
    for(q = 0; q < MAP_CAN_SIZE; q++) { //q is RX Buffer index terminology
        SrcAddr = RSCFD0_BASE + 0x0608 + (0x10 * q);
        memcpy(DestAddr, SrcAddr, 8); //DLC 8
    }
}

uint8_t W_Check_CAN_Bits(uint8_t can_idx, uint8_t byte, uint8_t nbytes, uint8_t bit, uint8_t nbits) {
    uint8_t * Addr_Ptr;
    uint16_t u16curent;
    uint16_t u16prev;
    uint8_t u8curent;
    uint8_t u8prev;
    uint32_t ret;
    uint8_t mask;

    Addr_Ptr = u8mem_can_data + can_idx * 8 + byte;
    if(nbytes > 1) //two bytes are good to verify
    {
        u16curent = *Addr_Ptr;
        u16prev = *(Addr_Ptr + MAP_CANIO_BANK_SIZE);
        ret = u16curent ^ u16prev;    
    } 
    else 
    {
        mask =  ~((~0) << nbits) << bit;
        ret = *Addr_Ptr & mask ^ *(Addr_Ptr + MAP_CANIO_BANK_SIZE) & mask;
    }
    return ret;
}

//TODO: playaduio
//TODO: new data flag ptr

	