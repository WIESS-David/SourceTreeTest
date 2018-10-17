//============================================================================================================
//
// Header file for the CAN table rule
//
//============================================================================================================


#ifndef _WIESS_RSCAN_TABLE_H_
#define _WIESS_RSCAN_TABLE_H_

//============================================================================================================
// Defines
//============================================================================================================

#define RX_RULE_NUM_MAX              (192U)       /* Max Rx Rule number */
#define RX_RULE_PAGE_NUM             (1U)         /* Rx Rule Table page number */

#define RX_RULE_NUM                  (9U)         /* Rx Rule number */
#define RX_RULE_NUM_CH0              (9U)         /* Channel 0 Rx rule number */
#define RX_RULE_NUM_CH1              (0U)         /* Channel 1 Rx rule number */
#define RX_RULE_NUM_CH2              (0U)         /* Channel 2 Rx rule number */
#define RX_RULE_NUM_CH3              (0U)         /* Channel 3 Rx rule number, not used */
#define RX_RULE_NUM_CH4              (0U)         /* Channel 4 Rx rule number, not used */

#define PID_COOLANT_TMP               0x05
#define PID_RPM                       0x0C
#define PID_SPEED                     0x0D
#define PID_FUEL_LEVEL                0x2F

/* structure */
typedef struct {
  unsigned long lword[4];
}can_cre_type;


#endif
