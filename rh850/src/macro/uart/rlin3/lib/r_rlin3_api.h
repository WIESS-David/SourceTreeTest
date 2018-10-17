/*
****************************************************************************
PROJECT : Rlin3 driver
FILE    : $Id$
============================================================================ 
DESCRIPTION
Driver for RLIN3 macro
============================================================================
                            C O P Y R I G H T                            
============================================================================
                           Copyright (c) 2012
                                  by 
                       Renesas Electronics (Europe) GmbH. 
                           Arcadiastrasse 10
                          D-40472 Duesseldorf
                               Germany
                          All rights reserved.
============================================================================
Purpose: only for testing

DISCLAIMER                                                                   
This software is supplied by Renesas Electronics Corporation and is only     
intended for use with Renesas products. No other uses are authorized. This   
software is owned by Renesas Electronics Corporation and is protected under  
all applicable laws, including copyright laws.                               
THIS SOFTWARE IS PROVIDED "AS IS" AND RENESAS MAKES NO WARRANTIES REGARDING  
THIS SOFTWARE, WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING BUT NOT      
LIMITED TO WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE   
AND NON-INFRINGEMENT. ALL SUCH WARRANTIES ARE EXPRESSLY DISCLAIMED.          
TO THE MAXIMUM EXTENT PERMITTED NOT PROHIBITED BY LAW, NEITHER RENESAS       
ELECTRONICS CORPORATION NOR ANY OF ITS AFFILIATED COMPANIES SHALL BE LIABLE  
FOR ANY DIRECT, INDIRECT, SPECIAL, INCIDENTAL OR CONSEQUENTIAL DAMAGES FOR   
ANY REASON RELATED TO THIS SOFTWARE, EVEN IF RENESAS OR ITS AFFILIATES HAVE  
BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.                             
Renesas reserves the right, without notice, to make changes to this software 
and to discontinue the availability of this software. By using this software,
you agree to the additional terms and conditions found by accessing the      
following link:                                                              
http://www.renesas.com/disclaimer *                                          
Copyright (C) 2011 Renesas Electronics Corporation. All rights reserved.     

****************************************************************************
*/

#ifndef RLIN3_API_H_
#define RLIN3_API_H_

#ifdef __cplusplus
extern "C" {
#endif


/*******************************************************************************
  Title: RLIN3 API

  An application using Rlin3 should include this header file.
*/

/*******************************************************************************
  Section: Global Types
*/

/*******************************************************************************
  Enum: r_rlin3_Error_t

  RLIN3 macro driver error code.
  
  If an error occures these enums give information about the
  reason.

  Values:
  R_RLIN3_ERR_OK           - No error
  R_RLIN3_ERR_NG           - Unspecified error
  R_RLIN3_ERR_RANGE        - Unit do not exist
  R_RLIN3_ERR_PORTINIT     - Rlin3 port init failed
  R_RLIN3_ERR_BUFOVERRUN   - Buffer is already full
  R_RLIN3_ERR_RBINIT       - Ring buffer init failed
  R_RLIN3_ERR_NODATA       - No data to send
  R_RLIN3_ERR_LAST         - Delimiter 
*/

typedef enum
{
    R_RLIN3_ERR_OK         = 0x00,
    R_RLIN3_ERR_NG         = 0x01u,
    R_RLIN3_ERR_RANGE      = 0x02u,
    R_RLIN3_ERR_PORTINIT,
    R_RLIN3_ERR_BUFOVERRUN,
    R_RLIN3_ERR_RBINIT,
    R_RLIN3_ERR_NODATA,
    R_RLIN3_ERR_LAST
} r_rlin3_Error_t;


/*******************************************************************************
  Enum: r_rlin3_TxRemain_t

  TxRemain, State there is still data that has to be transmitted

  Values:
    R_RLIN3_TX_NO_REMAINING       - no transmission remaining
    R_RLIN3_TX_REMAINING          - transmission remaining
*/

typedef enum
{    
    R_RLIN3_TX_NO_REMAINING = 0,
    R_RLIN3_TX_REMAINING
} r_rlin3_TxRemain_t;


/*******************************************************************************
  Enum: r_rlin3_RxRemain_t

  RxRemain, State there is still data that has to be received

  Values:
    R_RLIN3_RX_NO_REMAINING       - no reception remaining
    R_RLIN3_RX_REMAINING          - reception remaining
*/

typedef enum
{    
    R_RLIN3_RX_NO_REMAINING = 0,
    R_RLIN3_RX_REMAINING
} r_rlin3_RxRemain_t;


/*******************************************************************************
  Enum: r_rlin3_TxE_t

  RLIN3 TX enable/disable

  These settings are used as mask - don't change the defined values 

  Values:
    R_RLIN3_TX_DISABLED       - transmission disabled
    R_RLIN3_TX_ENABLED        - transmission enabled
*/

typedef enum
{    
    R_RLIN3_TX_DISABLED = 0,
    R_RLIN3_TX_ENABLED  = 0x01u
} r_rlin3_TxE_t;


/*******************************************************************************
  Enum: r_rlin3_RxE_t

  RLIN3 RX enable/disable

  These settings are used as mask - don't change the defined values 

  Values:
    R_RLIN3_RX_DISABLED       - reception disabled
    R_RLIN3_RX_ENABLED        - reception enabled
*/

typedef enum
{    
    R_RLIN3_RX_DISABLED = 0,
    R_RLIN3_RX_ENABLED  = 0x02u
} r_rlin3_RxE_t;

    
    
/*******************************************************************************
  Enum: r_rlin3_Dir_t

  RLIN3 macro send direction.

  Configuration parameter for Uart init.
  
  These settings are used as mask - don't change the defined values 

  Values:
  R_RLIN3_MSB_FIRST          - send Most Significant Bit first
  R_RLIN3_LSB_FIRST          - send Least Significant Bit first
*/

typedef enum
{    
    R_RLIN3_LSB_FIRST = 0,
    R_RLIN3_MSB_FIRST = 0x2u
} r_rlin3_Dir_t;


/*******************************************************************************
  Enum: r_rlin3_Parity_t

  Configuration parameter for Uart init to set Parity

  Values:
  R_RLIN3_NONE_PARITY      - No Parity
  R_RLIN3_ZERO_PARITY      - Zero Parity
  R_RLIN3_ODD_PARITY       - Odd parity
  R_RLIN3_EVEN_PARITY      - Even Parity
*/

typedef enum
{
    R_RLIN3_NONE_PARITY = 0,
    R_RLIN3_EVEN_PARITY = 0x08u,
    R_RLIN3_ZERO_PARITY = 0x10u,
    R_RLIN3_ODD_PARITY  = 0x18u,
} r_rlin3_Parity_t;


/*******************************************************************************
  Enum: r_rlin3_StopBit_t

  RLIN3 macro driver stop bit configuration

  Values:
  R_RLIN3_ONE_STOPBIT      - Send one stop bit
  R_RLIN3_TWO_STOPBITS     - Send two stop bits
*/

typedef enum
{
    R_RLIN3_ONE_STOPBIT  = 0,
    R_RLIN3_TWO_STOPBITS = 0x04u 
} r_rlin3_StopBit_t;


/*******************************************************************************
  Enum: r_rlin3_CharLength_t
  
  RLIN3 macro char length configuration

  Values:
  R_RLIN3_CHARLEN_7BIT     - 7 Bit char length
  R_RLIN3_CHARLEN_8BIT     - 8 Bit char length 
*/

typedef enum
{
    R_RLIN3_CHARLEN_8BIT = 0,
    R_RLIN3_CHARLEN_7BIT = 0x01u
} r_rlin3_CharLength_t;



/*******************************************************************************
  Enum: r_rlin3_Detection_t
  
  RLIN3 mask bit settings for error detection 
  
  By combinig the values (adding) it's possible to enable multiple error detections 

  Values:
    R_RLIN3_DET_DISABLED     -  disable error detection 
    R_RLIN3_DET_BIT_ERROR    - enable bit error detection 
    R_RLIN3_DET_BIT_OVR      - enable overrrun error detection 
    R_RLIN3_DET_BIT_FRAMING  - enable framing error detection
    
     
*/

typedef enum
{
    R_RLIN3_DET_DISABLED    = 0x00u,
    R_RLIN3_DET_BIT_ERROR   = 0x01u,
    R_RLIN3_DET_BIT_OVR     = 0x04u,
    R_RLIN3_DET_BIT_FRAMING = 0x08u
    
} r_rlin3_Detection_t;


/*******************************************************************************
  Enum: r_rlin3_Tdl_t
  
  RLIN3 macro TDL configuration

  Values:
  R_RLIN3_TDL_NOT_INVERTED   - Transmit data level not inverted
  R_RLIN3_TDL_INVERTED       - Transmit data level inverted 
*/

typedef enum
{
    R_RLIN3_TDL_NOT_INVERTED = 0,
    R_RLIN3_TDL_INVERTED     = 0x40u
} r_rlin3_Tdl_t;



/*******************************************************************************
  Enum: r_rlin3_Rdl_t
  
  RLIN3 macro RDL configuration

  Values:
  R_RLIN3_RDL_NOT_INVERTED   - Receive data level not inverted
  R_RLIN3_RDL_INVERTED       - Receive data level inverted 
*/

typedef enum
{
    R_RLIN3_RDL_NOT_INVERTED = 0,
    R_RLIN3_RDL_INVERTED     = 0x20u
} r_rlin3_Rdl_t;


/*******************************************************************************
  Enum: r_rlin3_Slit_t
  
  RLIN3 macro SLIT configuration

  Values:
  R_RLIN3_TX_IRQ_START   - Tx IRQ will be generated at start of data transmission
  R_RLIN3_TX_IRQ_END     - Tx IRQ will be generated when transmission is completed 
*/

typedef enum
{
    R_RLIN3_TX_IRQ_START = 0,
    R_RLIN3_TX_IRQ_END
} r_rlin3_Slit_t;


/*******************************************************************************
  Enum: r_rlin3_SbfRx_t
  
  RLIN3 macro SBF receive mode configuration

  Values:
  R_RLIN3_SBF_RX_SINGLE   - SBF is detected once; cannot be detected during reception of data
  R_RLIN3_SBF_RX_ALWAYS   - SBF detection is always possible 
*/

typedef enum
{
    R_RLIN3_SBF_RX_SINGLE = 0,
    R_RLIN3_SBF_RX_ALWAYS
} r_rlin3_SbfRx_t;


/*******************************************************************************
  Enum: r_rlin3_SbfLength_t
  
  RLIN3 macro SBF length configuration

  Values:
  R_RLIN3_SBF_13BITS     - 13 Bits SBF length
  R_RLIN3_SBF_14BITS     - 14 Bits SBF length
  R_RLIN3_SBF_15BITS     - 15 Bits SBF length
  R_RLIN3_SBF_16BITS     - 16 Bits SBF length
  R_RLIN3_SBF_17BITS     - 17 Bits SBF length
  R_RLIN3_SBF_18BITS     - 18 Bits SBF length
  R_RLIN3_SBF_19BITS     - 19 Bits SBF length
  R_RLIN3_SBF_20BITS     - 20 Bits SBF length
*/

typedef enum
{
    R_RLIN3_SBF_13BITS = 5u,
    R_RLIN3_SBF_14BITS = 6u,
    R_RLIN3_SBF_15BITS = 7u,
    R_RLIN3_SBF_16BITS = 0,
    R_RLIN3_SBF_17BITS = 1u,
    R_RLIN3_SBF_18BITS = 2u,
    R_RLIN3_SBF_19BITS = 3u,
    R_RLIN3_SBF_20BITS = 4u,    
} r_rlin3_SbfLength_t;


/*******************************************************************************
  Enum: r_rlin3_Interrupt_t

  Rlin3 macro interrupt type. List of all available interrupts 
  for this macro

  Values:
  R_RLIN3_INT_RE         - Receive Error Interrupt
  R_RLIN3_INT_RX         - Receive  Interrupt
  R_RLIN3_INT_TX         - Transmit Interrupt
  R_RLIN3_INT_NUM        - Delimiter 
*/

typedef enum
{
    R_RLIN3_INT_RE,
    R_RLIN3_INT_RX,
    R_RLIN3_INT_TX,
    R_RLIN3_INT_NUM
} r_rlin3_Interrupt_t;

/*******************************************************************************
  Type: r_rlin3_Parameter_t

  RLIN3 macro configuration data

  Members:
  Parity         - Parity configuration see: <r_rlin3_Parity_t>
  Stopbit        - Stop bit configuration see: <r_rlin3_StopBit_t>
  Direction      - LSB / MSB configuration see: <r_rlin3_Dir_t>
  Baudrate       - Baud rate value
  CharLength     - Char length see: <r_rlin3_CharLength_t>
  Detection      - error detection setting see: <r_rlin3_Detection_t>
  TxDataLevel    - TDL configuration see: <r_rlin3_Tdl_t>
  RxDataLevel    - RDL configuration see: <r_rlin3_Rdl_t> 
  TxIrqPoint     - SLIT configuration see: <r_rlin3_Slit_t>
  SbfRxMode      - SBF Rx mode configuration see: <r_rlin3_SbfRx_t> 
  SbfLength      - SBF length see: <r_rlin3_SbfLength_t>
  TxEnable       - Tx enable see: <r_rlin3_TxE_t>
  RxEnable       - Rx enable see: <r_rlin3_RxE_t>

*/     

typedef struct {
    r_rlin3_Parity_t            Parity; 
    r_rlin3_StopBit_t           Stopbit; 
    r_rlin3_Dir_t               Direction;
    uint32_t                    Baudrate;   
    r_rlin3_CharLength_t        CharLength;
    r_rlin3_Detection_t         Detection; 
    r_rlin3_Tdl_t               TxDataLevel;
    r_rlin3_Rdl_t               RxDataLevel;
    r_rlin3_TxE_t               TxEnable;
    r_rlin3_RxE_t               RxEnable;
} r_rlin3_Parameter_t;
     


/*******************************************************************************
  Section: Global API Functions
*/

/*******************************************************************************
  Function: R_RLIN3_Init

  Driver init function. Most parameters are set automatically (for standard 
  application)

  Parameters:
  Unit          - Instance number
  Config        - see: <r_rlin3_Parameter_t>
 
  Returns:
  see: <r_rlin3_Error_t>
*/

 r_rlin3_Error_t R_RLIN3_Init(uint32_t Unit, r_rlin3_Parameter_t *Config);


/*******************************************************************************
  Function: R_RLIN3_DeInit

  Driver deinit function.

  Parameters:
  Unit       - Instance number

  Returns:
  see: <r_rlin3_Error_t>
*/

 r_rlin3_Error_t R_RLIN3_DeInit(uint32_t Unit);

/*******************************************************************************
  Function: R_RLIN3_Reset

  macro reset function.
  
  On software reset (writing 00 to LCUC[1:0]), all registers 
  will retain the existing value except the status & transmission control register
  i.e. LST, LEST, LUOE & LTRC.  

  Parameters:
  Unit       - Instance number

  Returns:
  void
  
*/

void R_RLIN3_Reset(uint32_t Unit);  

/*******************************************************************************
  Function: R_RLIN3_ResetRelease

  Release macro reset.
  

  Parameters:
  Unit       - Instance number

  Returns:
  void
  
*/

void R_RLIN3_ResetRelease(uint32_t Unit);  

/*******************************************************************************
  Function: R_RLIN3_Flush

  Waits until the TX ring buffer becomes empty.

  Parameters:
  Unit       - Instance number

  Returns:
  void
*/

void R_RLIN3_Flush(uint32_t Unit);


/*******************************************************************************
  Function: R_RLIN3_SetSpeed

  Change Rlin3 speed

  Parameters:
  Unit       - Instance number
  Baudrate   - New baudrate 

  Returns:
  see: <r_rlin3_Error_t>
*/

 r_rlin3_Error_t R_RLIN3_SetSpeed(uint32_t Unit, uint32_t Baudrate);


/*******************************************************************************
  Function: R_RLIN3_TriggerSbfTx

  Trigger LIN Sync-Break-Field transmission

  Parameters:
  Unit       - Instance number

  Returns:
  see: <r_rlin3_Error_t>
*/     

 r_rlin3_Error_t R_RLIN3_TriggerSbfTx(uint32_t Unit);


/*******************************************************************************
  Function: R_RLIN3_TriggerSbfRx

  Trigger LIN Sync-Break-Field reception

  Parameters:
  Unit       - Instance number

  Returns:
  see: <r_rlin3_Error_t>
*/     

 r_rlin3_Error_t R_RLIN3_TriggerSbfRx(uint32_t Unit);
  
  
/*******************************************************************************
  Function: R_RLIN3_Disable

  Disables RLIN3 macro

  Parameters:
  Unit       - Instance number

  Returns:
  see: <r_rlin3_Error_t>
*/ 

 r_rlin3_Error_t R_RLIN3_Disable(uint32_t Unit);       
 
  
  
/*******************************************************************************
  Function: R_RLIN3_Enable

  Enables RLIN3 macro

  Parameters:
  Unit       - Instance number

  Returns:
  see: <r_rlin3_Error_t>
*/ 
 
 r_rlin3_Error_t R_RLIN3_Enable(uint32_t Unit);        
        

/*******************************************************************************
  Function: R_RLIN3_DisableTx

  Disables RLIN3 transmission

  Parameters:
  Unit       - Instance number

  Returns:
  see: <r_rlin3_Error_t>
*/ 

 r_rlin3_Error_t R_RLIN3_DisableTx(uint32_t Unit);
        


/*******************************************************************************
  Function: R_RLIN3_EnableTx

  Enables RLIN3 transmission

  Parameters:
  Unit       - Instance number

  Returns:
  see: <r_rlin3_Error_t>
*/ 

 r_rlin3_Error_t R_RLIN3_EnableTx(uint32_t Unit);



/*******************************************************************************
  Function: R_RLIN3_DisableRx

  Disables RLIN3 reception

  Parameters:
  Unit       - Instance number

  Returns:
  see: <r_rlin3_Error_t>
*/ 

 r_rlin3_Error_t R_RLIN3_DisableRx(uint32_t Unit);
        


/*******************************************************************************
  Function: R_RLIN3_EnableRx

  Enables RLIN3 reception

  Parameters:
  Unit       - Instance number

  Returns:
  see: <r_rlin3_Error_t>
*/ 

 r_rlin3_Error_t R_RLIN3_EnableRx(uint32_t Unit);
                                         
                                         
/*******************************************************************************
  Function: R_RLIN3_ReadChar

  Read received data out of the RLIN3 RX register.
  
  This function simply returns the RX register value, it does
  not check for availability of data.

  Parameters:
  Unit       - Instance number

  Returns:
  Received data
*/

 uint8_t R_RLIN3_ReadChar(uint32_t Unit);


/*******************************************************************************
  Function: R_RLIN3_WriteChar

  Write one value to the RLIN3 TX register.

  The function waits until any ongoing transmission is completed
  and writes then the data to the TX register.

  Parameters:
  Unit         - Instance number
  TxData       - Value to be transmitted

  Returns:
  see: <r_rlin3_Error_t>
*/

 r_rlin3_Error_t R_RLIN3_WriteChar(uint32_t Unit, uint8_t TxData);   


/*******************************************************************************
  Function: R_RLIN3_Write

  Writes data from buffer to uart.

  Copy data from 'Buffer' to the driver's ring buffer and 
  start the transmission. The further data transfer will be 
  made by the TX interrupt routine. The function may return 
  before all data has been sent but not before all data has 
  been copied to the ring buffer. 

  Parameters:
  Unit       - Instance number
  Buffer     - Data buffer
  Size       - Number of bytes to transfer from the buffer

  Returns:
  Number of characters sent.
*/

 uint32_t R_RLIN3_Write(uint32_t Unit, uint8_t *Buffer, uint32_t Size);


/*******************************************************************************
  Function: R_RLIN3_Read

  Reads data from the driver's ring buffer which has been 
  received.
  
  Maximum 'MaxSize' data is read from the buffer.

  Parameters:
  Unit       - Instance number
  Buffer     - Buffer for storing the data
  MaxSize    - Size of the data buffer

  Returns:
  Number of characters read.
*/

 uint32_t R_RLIN3_Read(uint32_t Unit, uint8_t *Buffer, uint32_t MaxSize);


/*******************************************************************************
  Function: R_RLIN3_IsTxRemain

  Verify if there is still data that has to be transmitted.
  
  Parameters:
  Unit       - Instance number
  
  Returns:
  see: <r_rlin3_TxRemain_t>
*/

 r_rlin3_TxRemain_t R_RLIN3_IsTxRemain(uint32_t Unit);



/*******************************************************************************
  Function: R_RLIN3_IsrxRemain

  Verify if there is still data that has to be received.
  
  Parameters:
  Unit       - Instance number
  
  Returns:
  see: <r_rlin3_RxRemain_t>
*/

 r_rlin3_RxRemain_t R_RLIN3_IsRxRemain(uint32_t Unit); 


/*******************************************************************************
  Function: R_RLIN3_IsSbfTxOngoing

  Verify if SBF Tx is ongoing.
  
  Parameters:
  Unit       - Instance number
  
  Returns:
  
*/

 uint8_t R_RLIN3_IsSbfTxOngoing(uint32_t Unit);
                                               
                                               
/*******************************************************************************
  Function: R_RLIN3_IsSbfRxOngoing

  Verify if SBF Rx is ongoing.
  
  Parameters:
  Unit       - Instance number
  
  Returns:
  
*/                                               
                                               
 uint8_t R_RLIN3_IsSbfRxOngoing(uint32_t Unit);
     
     
/*******************************************************************************
  Function: R_RLIN3_IsDataTxOngoing

  Verify if Data Tx is ongoing
  
  Parameters:
  Unit       - Instance number
  
  Returns:
  
*/

 uint8_t R_RLIN3_IsDataTxOngoing(uint32_t Unit);
                                               
                                               
/*******************************************************************************
  Function: R_RLIN3_IsDataRxOngoing

  Verify if Data Rx is ongoing
  
  Parameters:
  Unit       - Instance number
  
  Returns:
  
*/                                               
                                               
 uint8_t R_RLIN3_IsDataRxOngoing(uint32_t Unit);                                                   
                    
                    
                    
/*******************************************************************************
  Group: RLIN3 Error and ISR Functions
*/

/*******************************************************************************
  Function: R_RLIN3_SetErrorCallback

  Set a user function as error handler.
  
  When the driver detects any error, it will call this function.

  Parameters:
  ErrorCallback - Pointer to a user function
  
  Returns:
  void
*/

void R_RLIN3_SetErrorCallback(void (*ErrorCallback )(uint32_t Unit, uint32_t Error));

/*******************************************************************************
  Function: R_RLIN3_SetIsrCallback

  Assign a user callback for the given interrupt of a unit. 
  
  The function will be called after the driver ISR has
  processed the interrupt.

  Parameters:
  Unit          - Instance number
  IntType       - What kind of interrupt; see: <r_rlin3_Interrupt_t>
  Isr           - Isr function pointer
  
  Returns:
  void
*/

 r_rlin3_Error_t R_RLIN3_SetIsrCallback(uint32_t Unit, r_rlin3_Interrupt_t IntType, 
                                 void (*Isr)(void));
                                 

/*******************************************************************************
  Function: R_RLIN3_IsrRE

  Driver RE interrupt service routine. 
  
  This function shall be called by device interrupt handler 
  (xxxxxx_isr.c) and can be found in rlin3\src\rlin3_isr.c. 

  Parameters:
  Unit       - Instance number
  
  Returns:
  void
*/

void R_RLIN3_IsrRE(uint32_t Unit);


/*******************************************************************************
  Function: R_RLIN3_IsrRx

  Driver RX interrupt service routine. 
  
  This function shall be called by sevice interrupt handler 
  (xxxxxx_isr.c) and can be found in rlin3\src\rlin3_isr.c. 

  Parameters:
  Unit       - Instance number
  
  Returns:
  void
*/

void R_RLIN3_IsrRx(uint32_t Unit);


/*******************************************************************************
  Function: R_RLIN3_IsrTx

  Driver TX interrupt service routine. 
  
  This function shall be called by device interrupt handler 
  (xxxxxx_isr.c) and can be found in rlin3\src\rlin3_isr.c.

  Parameters:
  Unit       - Instance number
  
  Returns:
  void
*/

void R_RLIN3_IsrTx(uint32_t Unit);


/*******************************************************************************
  Function: R_RLIN3_EnableInt

  Enable the UART interrupt 'Interrupt'.
  
  Parameters:
  Unit       - Instance number
  Interrupt  - Interrupt type see: <r_rlin3_Interrupt_t>
  
  Returns:
  void
*/

void R_RLIN3_EnableInt(uint32_t Unit, r_rlin3_Interrupt_t Interrupt);


/*******************************************************************************
  Function: R_RLIN3_DisableInt

  Disable the UART interrupt 'Interrupt'.
  
  Parameters:
  Unit       - Instance number
  Interrupt  - Interrupt type see: <r_rlin3_Interrupt_t>
  
  Returns:
  void
*/

void R_RLIN3_DisableInt(uint32_t Unit, r_rlin3_Interrupt_t Interrupt);


                  
#ifdef __cplusplus
}
#endif

#endif /* RLIN3_API_H_  */
