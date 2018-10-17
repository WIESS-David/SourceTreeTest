
#define RSCFD0_BASE 0xFFD00000

#define CAN_IDS [0x286]

#define MAP_CAN_SIZE	8
#define MAP_GOIP_SIZE	1
#define MAP_CANIO_SIZE (MAP_CAN_SIZE + MAP_GOIP_SIZE)

//#define u8mem_can_data = []


//TODO: verify this
void W_Copy_CAN_to_MEM(uint8_t bank) {
    uint8_t i;
    uint8_t * SrcAddr;
    uint8_t * DestAddr;

    DestAddr = (bank==1)? u8mem_can_data + MAP_CAN_SIZE*8: u8mem_can_data 

    for(q= 0; q < MAP_CAN_SIZE; q++) {
        SrcAddr = RSCFD0_BASE + 0x0608 + (0x10 x q);
        
        memcpy(DestAddr, SrcAddr, 8);
    }
}

