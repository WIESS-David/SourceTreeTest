/*
****************************************************************************
PROJECT : Rlin3 driver
FILE    : $Id$
============================================================================ 
DESCRIPTION
Register declaration for Rlin3 macro driver 
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

#ifndef RLIN3_REGS_H_
#define RLIN3_REGS_H_

#ifdef __cplusplus
extern "C" {
#endif

/*******************************************************************************
  Title: RLIN3 Register Offsets

  Only the driver modules shall include this header.
*/

/*******************************************************************************
  Section: Global Defines
*/

/*******************************************************************************
  Constants: Register Offsets
    RLN3nLWBR           - LIN wake-up baud rate selector register     
    RLN3nLBRP0          - LIN / UARTbaud rate prescaler 0 register    
    RLN3nLBRP1          - LIN/ UART baud rate prescaler 1 register    
    RLN3nLSTC           - LIN self-test control register              
    RLN3nLUSC           - UART standby control register               
    RLN3nLMD            - LIN mode register                           
    RLN3nLBFC           - UART configuration register                 
    RLN3nLSC            - LIN / UART space configuration register     
    RLN3nLWUP           - LIN wake-up configuration register          
    RLN3nLIE            - LIN interrupt enable register               
    RLN3nLEDE           - LIN / UARTerror detection enable register   
    RLN3nLCUC           - LIN/ UART control register                  
    RLN3nLTRC           - LIN / UARTtransmission control register     
    RLN3nLMST           - LIN/ UART mode status register              
    RLN3nLST            - LIN / UARTstatus register                   
    RLN3nLEST           - LIN/ UART error status register             
    RLN3nLDFC           - LIN data field configuration register       
    RLN3nLIDB           - LIN / UART ID buffer register               
    RLN3nLCBR           - LIN checksum buffer register                
    RLN3nLUDB0          - LIN / UART data 0 buffer register           
    RLN3nLDBR1          - LIN / UART data 1 buffer 1 register         
    RLN3nLDBR2          - LIN/ UART data 2 buffer 2 register          
    RLN3nLDBR3          - LIN/ UART data 3 buffer 3 register          
    RLN3nLDBR4          - LIN / UART data 4 buffer 4 register         
    RLN3nLDBR5          - LIN / UART data 5 buffer 5 register         
    RLN3nLDBR6          - LIN / UART data 6 buffer 6 register         
    RLN3nLDBR7          - LIN / UART data 7 buffer 7 register         
    RLN3nLDBR8          - LIN/ UART data 8 buffer 8 register          
    RLN3nLUOER          - UART operation enable register              
    RLN3nLUOR1          - UART option register 1                      
    RLN3nLUTDR          - UART transmission data register             
    RLN3nLURDR          - UART reception data register                
    RLN3nLUWTDR         - UART wait transmission data register        

*/


#define R_RLIN3_LWBR        0x01
#define R_RLIN3_LBRP0       0x02
#define R_RLIN3_LBRP1       0x03
#define R_RLIN3_LSTC        0x04
#define R_RLIN3_LMD         0x08
#define R_RLIN3_LBFC        0x09
#define R_RLIN3_LSC         0x0a
#define R_RLIN3_LWUP        0x0b
#define R_RLIN3_LIE         0x0c
#define R_RLIN3_LEDE        0x0d
#define R_RLIN3_LCUC        0x0e
#define R_RLIN3_LTRC        0x10
#define R_RLIN3_LMST        0x11
#define R_RLIN3_LST         0x12
#define R_RLIN3_LEST        0x13
#define R_RLIN3_LDFC        0x14
#define R_RLIN3_LIDB        0x15
#define R_RLIN3_LCBR        0x16
#define R_RLIN3_LUDB0       0x17
#define R_RLIN3_LDBR1       0x18
#define R_RLIN3_LDBR2       0x19
#define R_RLIN3_LDBR3       0x1a
#define R_RLIN3_LDBR4       0x1b
#define R_RLIN3_LDBR5       0x1c
#define R_RLIN3_LDBR6       0x1d
#define R_RLIN3_LDBR7       0x1e
#define R_RLIN3_LDBR8       0x1f
#define R_RLIN3_LUOER       0x20
#define R_RLIN3_LUOR1       0x21
#define R_RLIN3_LUTDR       0x24
#define R_RLIN3_LURDR       0x26
#define R_RLIN3_LUWTDR      0x28


/*******************************************************************************
  Section: Global Types
  
  RLIN3 Register Definitions
*/
  
/*******************************************************************************
  Type: r_rlin3_RegCtl0_t
  
  Content of control register CTL0
  
  Members:
  UaSldc    - Data consistency check enable 
  Reserved  - Reserved bits
  UaRxe     - Reception enable 
  UaTxe     - Transmission enable
  UaPw      - Operation control
*/

typedef struct {
    uint8_t UrteSldc    : 1;  
    uint8_t Reserved    : 4;  
    uint8_t UrteRxe     : 1;  
    uint8_t UrteTxe     : 1;  
    uint8_t UrtePw      : 1;  
} r_rlin3_RegCtl0Bit_t;


/*******************************************************************************
  Union: r_rlin3_RegCtl0_t
  
  Simplifies Bit and Byte access to register CTL0    
  
  Members:
  Bit       - Allow bit access; see: <r_rlin3_RegCtl0_t>
  Byte      - Allow access ofthe whole byte
*/

typedef union {
    r_rlin3_RegCtl0Bit_t  Bit;
    uint8_t               Byte;
} r_rlin3_RegCtl0_t;
                                  
                                  
/*******************************************************************************
  Type: r_rlin3_Reg_Ctl1_t
  
  Content of control register CTL1
  
  Members:
  UrteSlit      - Transmit interrupt request signal timing
  UrteSld       - Communication direction (MSB/LSB)
  UrteSlg       - Stop bit length
  Unused        - Not used bits
  UrteRdl       - Receive data level
  UrteTdl       - Transmit data level
  UrteSlp       - Parity configuration for transmit / receive data 
  UrteClg       - Data character length (7 or 8)
  Reserved      - Reserved bits
  UrteBlg       - Bit field (BF) length
  UrteSlbm      - BF receive mode detection
*/

typedef struct {
    uint16_t UrteSlit   : 1;  
    uint16_t UrteSld    : 1;  
    uint16_t UrteSlg    : 1; 
    uint16_t Unused     : 1; 
    uint16_t UrteRdl    : 1;  
    uint16_t UrteTdl    : 1;  
    uint16_t UrteSlp    : 2;    
    uint16_t UrteClg    : 1; 
    uint16_t Reserved   : 3;         
    uint16_t UrteBlg    : 3; 
    uint16_t UrteSlbm   : 1;                        
} r_rlin3_RegCtl1Bit_t;    
                         

/*******************************************************************************
  Union: r_rlin3_RegCtl1_t
  
  Simplifies Bit and Byte access to register CTL1    
  
  Members:
  Bit      - Allow bit access; see: <r_rlin3_RegCtl1_t>
  Byte     - Allow access ofthe whole byte
*/

typedef union {
    r_rlin3_RegCtl1Bit_t  Bit;
    uint16_t              Word;
} r_rlin3_RegCtl1_t;


/*******************************************************************************
  Type: r_rlin3_Reg_Ctl2Bit_t
  
  Content of control register CTL2
  
  Members:
  UrteBrs       - Devision value of prescaler
  Reserved      - Reserved bits
  UrtePrs       - Baud rate devision value
*/

typedef struct {
    uint16_t UrteBrs    : 12;                        
    uint16_t Reserved   : 1;                         
    uint16_t UrtePrs    : 3;                          
} r_rlin3_RegCtl2Bit_t;


/*******************************************************************************
  Union: r_rlin3_RegCtl2_t
  
  Simplifies Bit and Byte access to register CTL2    
  
  Members:
  Bit      - Allow bit access; see: <r_rlin3_RegCtl2_t>
  Byte     - Allow access ofthe whole byte
*/

typedef union {
    r_rlin3_RegCtl2Bit_t  Bit;
    uint16_t              Word;
} r_rlin3_RegCtl2_t;


/*******************************************************************************
  Type: r_rlin3_RegTrgBit_t
  
  Content of control register TRG
  
  Members:
  Reserved      - Reserved bits
  UrteBtt       - BF Transmit trigger
  UrteBrt       - BF REceive trigger  
  Unused        - Not uses bits
*/

typedef struct {
    uint8_t Reserved    : 5;  
    uint8_t UrteBtt     : 1;  
    uint8_t UrteBrt     : 1;  
    uint8_t Unused      : 1;  
} r_rlin3_RegTrgBit_t;


/*******************************************************************************
  Union: r_rlin3_RegTrg_t
  
  Simplifies Bit and Byte access to register Trg    
  
  Members:
  Bit      - Allow bit access; see: <r_rlin3_RegTrg_t>
  Byte     - Allow access ofthe whole byte
*/

typedef union {
    r_rlin3_RegTrgBit_t  Bit;
    uint8_t              Byte;
} r_rlin3_RegTrg_t;


/*******************************************************************************
  Type: r_rlin3_RegStr0Bit_t
  
  Content of control register Str0
  
  Members:
  UrteSst       - Data transmission status flag 
  UrteSsr       - Data reception status flag     
  Reserved      - Reserved bits
  UrteSsbt      - BF transmission status flag  
  UrteSsbr      - BF reception status flag    
  Unused        - Not used bits
*/

typedef struct {
    uint8_t UrteSst     : 1;  
    uint8_t UrteSsr     : 1;   
    uint8_t Reserved    : 3;  
    uint8_t UrteSsbt    : 1;  
    uint8_t UrteSsbr    : 1;  
    uint8_t Unused      : 1;  
} r_rlin3_RegStr0Bit_t;


/*******************************************************************************
  Union: r_rlin3_RegStr0_t
  
  Simplifies Bit and Byte access to register Trg    
  
  Members:
  Bit      - Allow bit access; see: <r_rlin3_RegStr0_t>
  Byte     - Allow access ofthe whole byte
*/

typedef union {
    r_rlin3_RegStr0Bit_t  Bit;
    uint8_t               Byte;
} r_rlin3_RegStr0_t;


/*******************************************************************************
  Type: r_rlin3_RegStr1Bit_t
  
  Content of control register Str1
  
  Members:
  UrteOve       - Overrun error detection flag
  UrteFe        - Framing error detection flag
  UrtePe        - Parity error detection flag
  UrteDce       - Data consistency error flag
  UrteBsf       - BF successfull transmitted flag
  Reserved      - Reserved bits  
*/

typedef struct {
    uint8_t UrteOve     : 1; 
    uint8_t UrteFe      : 1;
    uint8_t UrtePe      : 1;
    uint8_t UrteDce     : 1;
    uint8_t UrteBsf     : 1;
    uint8_t Reserved    : 3;  
} r_rlin3_RegStr1Bit_t;


/*******************************************************************************
  Union: r_rlin3_RegStr1_t
  
  Simplifies Bit and Byte access to register Trg    
  
  Members:
  Bit      - Allow bit access; see: <r_rlin3_RegStr1_t>
  Byte     - Allow access ofthe whole byte
*/

typedef union {
    r_rlin3_RegStr1Bit_t  Bit;
    uint8_t               Byte;
} r_rlin3_RegStr1_t;


/*******************************************************************************
  Type: r_rlin3_RegStcBit_t
  
  Content of control register Stc
  
  Members:
  UrteClov      - Clears overrunn error flag
  UrteClf       - Clears framing error detection flag
  UrteClp       - Clears parity error detection flag
  UrteCldc      - Clears consistency error flag
  UrteClbs      - Clears BF reception OK flag 
  Reserved      - Reserved bits  
*/

typedef struct {
    uint8_t UrteClov     : 1; 
    uint8_t UrteClf      : 1;
    uint8_t UrteClp      : 1;
    uint8_t UrteCldc     : 1;
    uint8_t UrteClbs     : 1;
    uint8_t Reserved     : 3;  
} r_rlin3_RegStcBit_t;


/*******************************************************************************
  Union: r_rlin3_RegStc_t
  
  Simplifies Bit and Byte access to register Trg    
  
  Members:
  Bit      - Allow bit access; see: <r_rlin3_RegStc_t>
  Byte     - Allow access ofthe whole byte
*/

typedef union {
    r_rlin3_RegStcBit_t  Bit;
    uint8_t              Byte;
} r_rlin3_RegStc_t;


#ifdef __cplusplus
}
#endif

#endif /* RLIN3_REGS_H_  */
