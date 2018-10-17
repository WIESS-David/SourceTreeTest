#include "r_dev_api.h"
#include "r_bsp_stdio_api.h"
#include "w_system.h"
#include "w_sys_rscan.h"

/* Rx Rule Table */
const can_cre_type RX_RULE_TABLE_LIST[RX_RULE_NUM] =
{
    //  RSCAN0GAFLIDj,RSCAN0GAFLMj,RSCAN0GAFLP0j, RSCAN0GAFLP1j
    /* CAN 0 reception rules */
    {0x00000286UL, 0xDFFFFFFFUL, 0xF0000000UL, 0x00000001UL}, /* FIFO RX 0 - for future FW update*/
    {0x00000287UL, 0xDFFFFFFFUL, 0xF1008000UL, 0x00000000UL}, /* wiess control */
    {0x00000133UL, 0xDFFFFFFFUL, 0xF2008100UL, 0x00000000UL}, /* std, data frame, check DLC(8), recv buffer enabled as number 1 */
    {0x00000134UL, 0xDFFFFFFFUL, 0xF3008200UL, 0x00000000UL}, 
    {0x0000053EUL, 0xDFFFFFFFUL, 0xF4008300UL, 0x00000000UL}, 
    {0x0000053FUL, 0xDFFFFFFFUL, 0xF5008400UL, 0x00000000UL},  
    {0x00000434UL, 0xDFFFFFFFUL, 0xF6008500UL, 0x00000000UL},
    {0x00000340UL, 0xDFFFFFFFUL, 0xF7008600UL, 0x00000000UL},
    {0x0000056BUL, 0xDFFFFFFFUL, 0xF8008700UL, 0x00000000UL},
    

/* CAN 1 reception rules */
    //{0x00000030UL, 0xDFFFFFF0UL, 0x00008800UL, 0x00000000UL}, /* NO.002 : std data frm ID = H'30~3F will be accepted, destination Rx Buffer8 */
    //{0x00000040UL, 0xDFFFFFF0UL, 0x00008900UL, 0x00000000UL}, /* NO.003 : std data frm ID = H'40~4F will be accepted, destination Rx Buffer9 */
    //{0x00000050UL, 0xDFFFFFFFUL, 0x00008a00UL, 0x00000000UL}, /* NO.004 : std data frm ID = H'50 will be accepted, destination Rx Buffer10 */
    //{0x00000060UL, 0xDFFFFFFFUL, 0x00008b00UL, 0x00000000UL}, /* NO.005 : std data frm ID = H'60 will be accepted, destination Rx Buffer11 */
};

const r_dev_PinConfig_t ccan0_pin_config[] = 
{
    /* CAN TX - CAN0 pins */
    {0, 4, 4, R_DEV_PIN_OUT   , 0u, R_DEV_PIN_PULLNO, 0u, 0u, 0u, R_DEV_PIN_CMOS1}, /* O:  tx Output */ 
    /* CAN RX - CAN0 pins */
    {0, 5, 4, R_DEV_PIN_IN   , 0u, R_DEV_PIN_PULLNO, 0u, 0u, 0u, R_DEV_PIN_CMOS1}, /* O:  rx input */ 

    /* delimiter - do not remove */
    {0u, R_DEV_PIN_LAST,0u,R_DEV_PIN_OUT,0u,R_DEV_PIN_PULLNO,   0u, 0u, 0u, R_DEV_PIN_CMOS1}
}; 

const r_dev_PinConfig_t ccan1_pin_config[] = 
{
    /* CAN TX - CAN1 pins */
    {0, 6, 4, R_DEV_PIN_OUT   , 0u, R_DEV_PIN_PULLNO, 0u, 0u, 0u, R_DEV_PIN_CMOS1}, /* O:  tx Output */ 
    /* CAN RX - CAN1 pins */
    {0, 7, 4, R_DEV_PIN_IN   , 0u, R_DEV_PIN_PULLNO, 0u, 0u, 0u, R_DEV_PIN_CMOS1}, /* O:  rx input */ 

    /* delimiter - do not remove */
    {0u, R_DEV_PIN_LAST,0u,R_DEV_PIN_OUT,0u,R_DEV_PIN_PULLNO,   0u, 0u, 0u, R_DEV_PIN_CMOS1}
};


//===================================================================================================================
// Functions
//===================================================================================================================

/*****************************************************************************
** Function:    W_CAN_GPIO_Init
** Description: Configures the CAN0/CAN1 GPIO
** Parameter:   None
** Return:      None
******************************************************************************/
void W_CAN_GPIO_Init(void)
{
    /* Set CAN0TX as P0_4 and CAN0RX as P0_5 */
    R_DEV_PinInit(ccan0_pin_config);    // Receive data from ECU
    R_DEV_PinInit(ccan1_pin_config);    // Send Data for OBDII
}

/*****************************************************************************
** Function:    W_RSCAN_Init
** Description: Configures the CAN0/CAN1 macros
** Parameter:   None
** Return:      None
******************************************************************************/
Can_RtnType W_RSCAN_Init(uint8_t u8retry_counter)
{
    uint8_t u8retry_temp;
    uint32_t u32_mem_view;
    
    W_CAN_GPIO_Init();

    u8retry_temp = u8retry_counter;
    /* Wait while CAN RAM initialization is ongoing */
    while((RSCAN0GSTS & 0x00000008) && u8retry_temp-- > 0);
    if(u8retry_counter == 0)
        return CAN_RETRY_ERROR;

    /* Switch to global/channel reset mode */
    RSCAN0GCTR &= 0xfffffffb;	//set the 3rd bit to 0 -- global stop mdoe  
    RSCAN0C0CTR &= 0xfffffffb;

    /* Configure clk_xincan as CAN-ClockSource */
    RSCAN0GCFG = 0x00000010;

    /* When fCAN is 8MHz, 
    Bitrate = fCAN/(BRP+1)/(1+TSEG1+TSEG2) = 8/1/16 = 0.5Mbps(500Kbps) */

    RSCAN0C0CFG = 0x023a0000; //SJW =3TQ, TSEG1=10TQ, TSEG2=4TQ, BRP=0

    /* ==== Rx rule setting ==== */
    W_RSCAN_SetRxRule();

    /* ==== buffer setting ==== */    
    RSCAN0RMNB = 0x08;  //Can_SetGlobalBuffer--8

    /* Set THLEIE disabled, MEIE(FIFO Message Lost Interrupt disabled)  */
    RSCAN0GCTR &= 0xfffff8ff;    


    // set receive FIFO 0	(0x7102U)										
	RSCAN0.RFCC0.UINT16[L]	= 0x7100U;						// RFIGCV[2:0] : Receive FIFO Interrupt Request Timing : When FIFO is 4/8 full.
															// RFIM : Receive FIFO Interrupt Source : An interrupt occurs each time a message has been received.
															// RFDC[2:0] : Receive FIFO Buffer Depth : 4 messages
															// RFIE : Receive FIFO interrupt is enable
															// RFE : Receive FIFO Buffer disable (only can be enabled in global operating mode or global test mode)

	// set & enable CAN receive FIFO interrupt
    //INTC1.ICRCANGRECC0.BIT.MKRCANGRECC0 = 1U;				// Disables interrupt processing
    //INTC1.ICRCANGRECC0.BIT.RFRCANGRECC0 = 0U;				// clear interrupt request flag
    //INTC1.ICRCANGRECC0.BIT.TBRCANGRECC0 = 1U;				// Table reference for interrupt vector
    //INTC1.ICRCANGRECC0.BIT.MKRCANGRECC0 = 0U;				// Enables interrupt processing
    //INTC1.ICRCANGRECC0.UINT16 |= 0x01;				        // set interrupt priority


    /* Global reset mode -> Global operating mode */
	RSCAN0.GCTR.UINT8[LL] = 0x00;							// GSLPR bit : 0 -> set as Other than global stop mode
															// GMDC[1:0] : 00 -> Global operating mode
	while((RSCAN0.GSTS.UINT8[LL] & 0x01) != 0){}			// GSLPSTS : check status Not in global reset mode
	
	/* Channel reset mode -> Channel communication mode (Channel 0) */
	RSCAN0.C0CTR.UINT8[LL] = 0x00;							// CSLPR bit : 0 -> set as Other than channel stop mode
															// CHMDC[1:0] : 01 -> Channel communication mode
                                                            // (CSLPSTS bit) check status Not in channel reset mode
    u8retry_temp = u8retry_counter;
    while ((RSCAN0.C0STS.UINT8[LL] & 0x01) == 1 && u8retry_temp-- > 0) {
    }
    if(u8retry_counter == 0)
        return CAN_RETRY_ERROR;

    /* Channel reset mode -> Channel communication mode (Channel 1) */
	RSCAN0.C1CTR.UINT8[LL] = 0x00;							// CSLPR bit : 0 -> set as Other than channel stop mode
															// CHMDC[1:0] : 00 -> Channel communication mode
                                                    		// (CSLPSTS bit) check status Not in channel reset mode
	u8retry_temp = u8retry_counter;
    while ((RSCAN0.C1STS.UINT8[LL] & 0x01) == 1 && u8retry_temp-- > 0) {
    }
    if(u8retry_counter == 0)
        return CAN_RETRY_ERROR;

	/* Channel reset mode -> Channel communication mode (Channel 2) */
	RSCAN0.C2CTR.UINT8[LL] = 0x00;							// CSLPR bit : 0 -> set as Other than channel stop mode
															// CHMDC[1:0] : 01 -> Channel communication mode
                                                            // (CSLPSTS bit) check status Not in channel reset mode
	
    u8retry_temp = u8retry_counter;
    while ((RSCAN0.C2STS.UINT8[LL] & 0x01) == 1 && u8retry_temp-- > 0) {
    }
    if(u8retry_counter == 0)
        return CAN_RETRY_ERROR;

    RSCAN0.RFCC0.UINT16[L] |= 0x0001U;
    u32_mem_view = RSCAN0.RFCC0.UINT32;
    
    return CAN_RTN_OK;
}

/******************************************************************************
** Function:    W_RSCAN_SetRxRule
** Description: Set all Rx rules
** Parameter:   None
** Return:      None
******************************************************************************/
static void W_RSCAN_SetRxRule(void)
{
    uint16_t RxRuleIdx;
    uint8_t PageRxRuleIdx;
    volatile CAN_RX_RULE_TYPE* pCRE;

    /* Set Rx rule number per channel */
    RSCAN0GAFLCFG0 |= 0x08000000;   //Channel0 rule number is 8

    /* Get access base address of Rx rule */
    pCRE = (volatile CAN_RX_RULE_TYPE*)&(RSCAN0GAFLID0);

    /* Receive Rule Table Write Enable */
    RSCAN0GAFLECTR |= 0x00000100;	//set bit8 to 1, Receive rule table write is enabled

    /* Copy Rx rule one by one */
    for (RxRuleIdx = 0U; RxRuleIdx < CAN_RX_RULE_NUM; RxRuleIdx++) //CAN_RX_RULE_NUM=12, refer to cab_table.h
    {
        PageRxRuleIdx = (uint8_t) (RxRuleIdx & CAN_PAGE_RX_RULE_IDX_MASK); //CAN_PAGE_RX_RULE_IDX_MASK= 0xF

        /* Update target Rx rule page if necessary. */
        if (PageRxRuleIdx == 0U) 
        {
            RSCAN0GAFLECTR |= RxRuleIdx >> CAN_RX_RULE_PAGE_IDX_BIT_POS; //CAN_RX_RULE_PAGE_IDX_BIT_POS= 4U
        }

        /* Set a single Rx rule.*/
        pCRE[PageRxRuleIdx] = CAN_RX_RULE_TABLE[RxRuleIdx];
    }

    /* Rx rule write disable */
    RSCAN0GAFLECTR &= 0xfffffeff;
}

/*****************************************************************************
** Function:    W_RSCAN_ReadRxBuffer
** Description: This code shows how to read message from Rx buffer
** Parameter:   pRxBufIdx - Pointer to Rx buffer that receives frame
** Return:      CAN_RTN_OK           - A frame is successfully read out
**              CAN_RTN_BUFFER_EMPTY - No frame is read out   
******************************************************************************/
Can_RtnType W_RSCAN_ReadRxBuffer(Can_FrameType* pFrame )
{
    uint8_t BufIdx;
    uint8_t CRBRCFiBufIdx;
    uint8_t OverwrittenFlag;
    uint32_t TempCRBRCF0;
    uint32_t TempCRBRCF1;
    Can_FrameType* pRxBuffer;
    volatile uint32_t* pCRBRCF;
    Can_RtnType RtnValue;

    /* Judge if new messages are available */
#ifdef RSCAN_RX_FIFO
	if (RSCAN0RFEMP == 1) { //RSCAN0RFEMP or  RSCAN0CFEMP
        RtnValue = CAN_RTN_BUFFER_EMPTY;	// buffer empty, no new data 
        return RtnValue;
    }
#endif
	
    TempCRBRCF0 = RSCAN0RMND0;	//Receive Buffer New Data Register, if it is true, new data is coming
    TempCRBRCF1 = RSCAN0RMND1;

    //if ((TempCRBRCF0 == CAN_EMPTY) && (TempCRBRCF1 == CAN_EMPTY)) //CAN_EMPTY==0
    if (TempCRBRCF0 == CAN_EMPTY) //CAN_CLR==0
    {
        RtnValue = CAN_RTN_BUFFER_EMPTY;	// buffer empty, no new data 
        return RtnValue;
    }
    
    //while(RSCAN0RMND0 || RSCAN0RMND1)
    while(RSCAN0RMND0)
    {
            /* Get Rx buffer that has new message */
            if (TempCRBRCF0 != CAN_EMPTY) 
            {
                pCRBRCF = (volatile uint32_t*)&(RSCAN0RMND0);
                for (BufIdx = 0U; BufIdx < CAN_CRBRCF0_RX_BUF_NUM; ++BufIdx) //CAN_CRBRCF0_RX_BUF_NUM=32
                {
                    if ((TempCRBRCF0 & CAN_1_BIT_MASK) == CAN_SET) //CAN_1_BIT_MASK==0x1; CAN_SET=0x1
                    {
                        break;	//if checked bit is 1, that means there is a new message in corresponding receive buffer 
                    }
                    TempCRBRCF0 = TempCRBRCF0 >> CAN_B1_BIT_POS; //CAN_B1_BIT_POS=0x1
                }
            }
            else if (TempCRBRCF1 != CAN_EMPTY)
            {
                pCRBRCF = (volatile uint32_t*)&(RSCAN0RMND1);
                for (BufIdx = 0U; BufIdx < CAN_CRBRCF1_RX_BUF_NUM; ++BufIdx) 
                {
                    if ((TempCRBRCF1 & CAN_1_BIT_MASK) == CAN_SET) 
                    {
                        break;
                    }
                    TempCRBRCF1 = TempCRBRCF1 >> CAN_B1_BIT_POS;
                }
                BufIdx += CAN_CRBRCF0_RX_BUF_NUM;	//CAN_CRBRCF0_RX_BUF_NUM ==32U
            }
            /* Calculate index value in CRBRCFi */
            CRBRCFiBufIdx = BufIdx & CAN_5_BIT_MASK;	//CAN_5_BIT_MASK  0x1fU  0B11111

            do 
            {
                /* Clear Rx complete flag of corresponding Rx buffer */
                do 
                {
                    CLR_BIT(*pCRBRCF, CRBRCFiBufIdx);	//To clear a flag to 0, the program must write 0 to the flag
                } while (GET_BIT(*pCRBRCF, CRBRCFiBufIdx) == CAN_SET);

                /* Read out message from Rx buffer */
                pRxBuffer = (Can_FrameType*) &(RSCAN0RMID0);
                *pFrame = pRxBuffer[BufIdx];

                /* Judge if current message is overwritten */
                OverwrittenFlag = GET_BIT(*pCRBRCF, CRBRCFiBufIdx);
                /* message is overwritten */
                if (OverwrittenFlag == CAN_SET) 
                {
                    /* User process for message overwritten */
                    //Usr_HandleRxBufOverwritten(BufIdx);
                }
            } while (OverwrittenFlag == CAN_SET);

            RtnValue = CAN_RTN_OK;
            return RtnValue;
    }
}


/*****************************************************************************
** Function:    W_RSCAN_SendTxBuffer
** Description: This code shows how to send message from Tx buffer
** Parameter:   pRxBufIdx - Pointer to Rx buffer that receives frame
** Return:      CAN_RTN_OK           - A frame is successfully read out
**              CAN_RTN_BUFFER_EMPTY - No frame is read out   
******************************************************************************/
Can_RtnType W_RSCAN_SendTxBuffer(uint8_t TxBufIdx, const Can_FrameType* pFrame, uint8_t u8retry_counter)
{
    volatile uint8_t* pTBSR;
    Can_FrameType* pTxBuffer;
    volatile uint8_t* pTBCR;
    uint8_t u8retry_temp;

    pTBSR = &(RSCAN0TMSTS0);
    pTBCR = &(RSCAN0TMC0);

    /* ---- Return if Tx Buffer is transmitting. ---- */    
    if( (pTBSR[TxBufIdx] & (uint8_t)0x01) == CAN_TBTST_TRANSMITTING )
    {
        return CAN_RTN_ERR;
    }

    u8retry_temp = u8retry_counter;
    /* Clear Tx buffer status */
    do 
    {
        pTBSR[TxBufIdx] = CAN_CLR;
    } while (pTBSR[TxBufIdx] != CAN_CLR && u8retry_temp-- > 0);

    if(u8retry_temp == 0)
        return CAN_RETRY_ERROR;
    
    /* Store message to tx buffer */
    pTxBuffer = (Can_FrameType*) &(RSCAN0TMID0);
    pTxBuffer[TxBufIdx] = *pFrame;

    /* Set transmission request */
    pTBCR[TxBufIdx] = CAN_TBCR_TRM;

    return CAN_RTN_OK;
}

