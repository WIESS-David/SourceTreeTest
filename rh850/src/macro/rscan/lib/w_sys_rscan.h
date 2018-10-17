//============================================================================================================
//
// Header file for the RS CAN
//
//============================================================================================================
#ifndef _RSCAN_H_
#define _RSCAN_H_

#include "dr7f701461.dvf.h"
#include "r_typedefs.h"
#include "w_rscan_table.h"

//************************************************************************************************************
// Register bit define
//************************************************************************************************************
/* ---- CAN frame ----- */
typedef struct
{
    uint32_t ID    :29;
    uint32_t THLEN :1;
    uint32_t RTR   :1;
    uint32_t IDE   :1;
    
    uint32_t TS    :16;
    uint32_t LBL   :12;
    uint32_t DLC   :4;
    uint8_t  DB[8];
} Can_FrameType;

/*---- structure for Tx Frame information --------------------*/
/* ---- CAN function return ---- */
typedef uint8_t Can_RtnType;

/* ---- function return value ---- */
#define CAN_RTN_OK                          0
#define CAN_RTN_FIFO_FULL                   1
#define CAN_RTN_BUFFER_EMPTY                2
#define CAN_RETRY_ERROR                     254
#define CAN_RTN_ERR                         255

//************************************************************************************************************
//  Macro define
//************************************************************************************************************
#ifndef CAN_ENABLE
#define CAN_ENABLE                          1U
#endif
#ifndef CAN_DISABLE
#define CAN_DISABLE                         0U
#endif

/* ---- bit value ---- */
#define CAN_SET                             1U
#define CAN_CLR                             0U

/* ---- bit mask ---- */
#define CAN_1_BIT_MASK                      1U
#define CAN_2_BIT_MASK                      3U
#define CAN_3_BIT_MASK                      7U
#define CAN_4_BIT_MASK                      0xfU
#define CAN_5_BIT_MASK                      0x1fU

/* ---- bit position ---- */
#define CAN_B0_BIT_POS                      0U
#define CAN_B1_BIT_POS                      1U
#define CAN_B2_BIT_POS                      2U
#define CAN_B3_BIT_POS                      3U
#define CAN_B4_BIT_POS                      4U
#define CAN_B5_BIT_POS                      5U
#define CAN_B6_BIT_POS                      6U
#define CAN_B7_BIT_POS                      7U
#define CAN_B8_BIT_POS                      8U
#define CAN_B9_BIT_POS                      9U
#define CAN_B10_BIT_POS                     10U
#define CAN_B11_BIT_POS                     11U
#define CAN_B12_BIT_POS                     12U
#define CAN_B13_BIT_POS                     13U
#define CAN_B14_BIT_POS                     14U
#define CAN_B15_BIT_POS                     15U

/* **** CAN channel usage **** */
#define CAN_CRBRCF0_RX_BUF_NUM              32U
#define CAN_CRBRCF1_RX_BUF_NUM              32U
#define CAN_CRBRCF2_RX_BUF_NUM              32U
#define CAN_PAGE_RX_RULE_IDX_MASK           0xfU
#define CAN_RX_RULE_PAGE_IDX_BIT_POS        4U
#define CAN_RAM_LW_SIZE                     (0x1C20U >> 2U)
#define CAN_RAM_PAGE_IDX_BIT_POS            6U
#define CAN_RAM_DATA_IDX_MASK               0x3fU

#define RX_RULE_NUM_MAX                     (192U)       /* Max Rx Rule number */
#define RX_RULE_PAGE_NUM                    (1U)         /* Rx Rule Table page number */

/* ==== Rx rule table (Refer to can_table.h) ==== */
#define CAN_C0RN                            RX_RULE_NUM_CH0
#define CAN_C1RN                            RX_RULE_NUM_CH1
#define CAN_C2RN                            RX_RULE_NUM_CH2
#define CAN_C3RN                            RX_RULE_NUM_CH3
#define CAN_C4RN                            RX_RULE_NUM_CH4
#define CAN_RX_RULE_NUM                     (CAN_C0RN + CAN_C1RN + CAN_C2RN + CAN_C3RN + CAN_C4RN)
#define CAN_RX_RULE_TABLE                   RX_RULE_TABLE_LIST
#define CAN_RX_RULE_TYPE                    can_cre_type

/* ---- TX buffer, TX status flag ---- */
#define CAN_TBTST_NOT_TRANSMITTING          0U
#define CAN_TBTST_TRANSMITTING              1U

/* ---- bit operations ---- */
#define GET_BIT(reg, pos)                   (((reg) >> (pos)) & 1U)
#define SET_BIT(reg, pos)                   ((reg) |= 1U << (pos))
#define CLR_BIT(reg, pos)                   ((reg) &= ~(1UL << (pos)))

/* ---- CiTBCRj ---- */
#define CAN_TBTR_BIT_POS                    0U
#define CAN_TBAR_BIT_POS                    1U
#define CAN_TBOE_BIT_POS                    2U

/* transmission command */
#define CAN_TBCR_TRM                        (CAN_ENABLE << CAN_TBTR_BIT_POS)

// RSCAN Retry Counter Define
#define _RSCAN_RETRY_COUNTER                16


#define CAN_EMPTY                             0U

//************************************************************************************************************
//  Function define
//************************************************************************************************************
uint8_t W_RSCAN_Init(uint8_t u8retry_counter);
Can_RtnType W_RSCAN_ReadRxBuffer(Can_FrameType* pFrame);
Can_RtnType W_RSCAN_SendTxBuffer(uint8_t TxBufIdx, const Can_FrameType* pFrame, uint8_t u8retry_counter);
static void W_RSCAN_SetRxRule(void);
extern const Can_FrameType CANTraStandData;
extern Can_FrameType CANRecData;

#endif


