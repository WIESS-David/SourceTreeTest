/*
****************************************************************************
PROJECT :ADC driver
FILE    : $Id$
============================================================================
DESCRIPTION
Driver for ADC macro
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

#ifndef  ADCA_REGS_H_
#define  ADCA_REGS_H_

#ifdef __cplusplus
extern "C" {
#endif

/*******************************************************************************
  Title: ADC7010 Register Offsets

  Only the ADC driver shall include this header.
*/

/*******************************************************************************
  Section: Global Defines
*/

/*******************************************************************************
  Constants: Register Offsets

  The Register Offsets of ADC7010
  
   **  ADC Specific  Registers (Virtual  Channel) **
   
 R_ADC_VCR(m)       -  Virtual Channel Register  m                 000H  + m * 4H        
 R_ADC_PWDVCR       -  PWM-Diag  Virtual Channel Register          0f4H            
 R_ADC_DR(m)        -  Data  Register  m                           100H  + m * 2H      
 R_ADC_DIR(m)       -  Data  Supplementary Information Register  n 200H  + m * 4H  
 R_ADC_PWDTSNDR     -  PWM-Diag  data  register                    178H              
 R_ADC_PWDDIR       -  PWM-Diag  Data  Supplementary Information   2f4H          
 R_ADC_TSNVCR       -  Temperature sensor Virtual Channel Register 0F0H                         

   **  ADC Specific  Registers (Control) **
          
 R_ADC_ADHALTR      -  A/D Force Halt  Register                    300H            
 R_ADC_ADCR         -  A/D Control Register                        304H              
 R_ADC_MPXCURR      -  MPX Current Register                        30CH              
 R_ADC_THSMPSTCR    -  T&H Sampling  Start Control Register        314H          
 R_ADC_THCR         -  T&H Control Register                        318H              
 R_ADC_THAHLDSTCR   -  T&H Group A Hold  Start Control Register    31CH      
 R_ADC_THBHLDSTCR   -  T&H Group B Hold  Start Control Register    320H      
 R_ADC_THACR        -  T&H Group A Control Register                324H          
 R_ADC_THBCR        -  T&H Group B Control Register                328H          
 R_ADC_THER         -  T&H Enable  Register                        32CH              
 R_ADC_THGSR        -  T&H Group Select  Register                  330H            
 R_ADC_SMPCR        -  Sampling  Control Register                  380H              

   **  ADC Specific  Registers (Safety-related) **
   
 R_ADC_SFTCR        -  Safety  Control Register                    334H              
 R_ADC_ULLMTBR0     -  Upper Limit/Lower Limit Table Register  0   338H        
 R_ADC_ULLMTBR1     -  Upper Limit/Lower Limit Table Register  1   33CH        
 R_ADC_ULLMTBR2     -  Upper Limit/Lower Limit Table Register  2   340H        
 R_ADC_ECR          -  Error Clear Register                        344H              
 R_ADC_ULER         -  Upper Limit/Lower Limit Error Register      348H          
 R_ADC_OWER         -  Overwrite Error Register                    34CH              

   ** Scan  Group Unique  Registers ** 
              
 R_ADC_SGSTCR(x)    -  Scan  Group x Start Control Register        400H  + x * 40H   
 R_ADC_PWDSGCR      -  PWM-Diag  Scan  Group Control Register      508H        
 R_ADC_SGCR(x)      -  Scan  Group x Control Register              408H  + x * 40H    
 R_ADC_SGVCSP(x)    -  Scan  Group x Start Virtual Channel Pointer 40CH  + x * 40H
 R_ADC_SGVCEP(x)    -  Scan  Group x End Virtual Channel Pointer   410H  + x * 40H
 R_ADC_SGMCYCR(x)   -  Scan  Group x Multicycle  Register          414H  + x * 40H    
 R_ADC_PWDSGSEFCR   -  PWM-Diag  Scan  End Flag  Clear Register    518H        
 R_ADC_SGSEFCR(x)   -  Scan  Group x End Flag  Clear Register      418H  + x * 40H
 R_ADC_SGSTR        -  Scan  Group x Status  Register              308H       
 R_ADC_SGTSEL(x)    -  A/D Conversion  Trigger Select  Control   41CH  + x * 40H
 R_ADC_SGPRCR       -  Proirity Control register                   38CH

   ** Self-Diagnostic Specific  Registers **     

 R_ADC_DGCTL0       -  Self-Diagnostic Voltage Level Control       350H          
 R_ADC_DGCTL1       -  Self-Diagnostic Control Register            354H              
 R_ADC_PDCTL2       -  Pull  Down  Control Register                35CH            

   ** Emulation Specific  Register **

 EMUCR              -  Emulation Control Register                  388H              
 
 R_ADC_SG_REG_OFFSET - register offset for scan group regs         040H 
*/
  
#define R_ADC_VCR(m)            (0x000 + (##m## * 4))
/* for F1A
#define R_ADC_PWDVCR            (0x094) */
#define R_ADC_PWDVCR            (0x0f4)
#define R_ADC_DR(m)             (0x100 + (##m## * 2))
#define R_ADC_DIR(m)            (0x200 + (##m## * 4))
/* for F1A
#define R_ADC_PWDTSNDR          (0x148)
#define R_ADC_PWDDIR            (0x294) */

#define R_ADC_TSNVCR            (0x0F0)
#define R_ADC_PWDTSNDR          (0x178)
#define R_ADC_PWDDIR            (0x2f4)
#define R_ADC_ADHALTR           (0x300)
#define R_ADC_ADCR              (0x304)
#define R_ADC_MPXCURR           (0x30C)
#define R_ADC_THSMPSTCR         (0x314)
#define R_ADC_THCR              (0x318)
#define R_ADC_THAHLDSTCR        (0x31C)
#define R_ADC_THBHLDSTCR        (0x320)
#define R_ADC_THACR             (0x324)
#define R_ADC_THBCR             (0x328)
#define R_ADC_THER              (0x32C)
#define R_ADC_THGSR             (0x330)
#define R_ADC_SMPCR             (0x380)
#define R_ADC_SFTCR             (0x334)
#define R_ADC_ULLMTBR0          (0x338)
#define R_ADC_ULLMTBR1          (0x33C)
#define R_ADC_ULLMTBR2          (0x340)
#define R_ADC_ECR               (0x344)
#define R_ADC_ULER              (0x348)
#define R_ADC_OWER              (0x34C)

#define R_ADC_SG_REG_OFFSET     (0x040)

#define R_ADC_TSNSGCR           (0x408)
#define R_ADC_SGSTCR(x)         (0x400 + (##x## * R_ADC_SG_REG_OFFSET))
#define R_ADC_PWDSGCR           (0x508)
#define R_ADC_SGCR(x)           (0x408 + (##x## * R_ADC_SG_REG_OFFSET))
#define R_ADC_SGVCSP(x)         (0x40C + (##x## * R_ADC_SG_REG_OFFSET))
#define R_ADC_SGVCEP(x)         (0x410 + (##x## * R_ADC_SG_REG_OFFSET))
#define R_ADC_SGMCYCR(x)        (0x414 + (##x## * R_ADC_SG_REG_OFFSET))
#define R_ADC_PWDSGSEFCR        (0x518)
#define R_ADC_SGSEFCR(x)        (0x418 + (##x## * R_ADC_SG_REG_OFFSET))
#define R_ADC_SGSTR             (0x308)
#define R_ADC_SGTSEL(x)         (0x41C + (##x## * R_ADC_SG_REG_OFFSET))
#define R_ADC_DGCTL0            (0x350)
#define R_ADC_DGCTL1            (0x354)
#define R_ADC_PDCTL2            (0x35C)
#define R_ADC_EMUCR             (0x388)

#define R_ADC_SGPRCR            (0x38C)


#ifdef __cplusplus
}
#endif

#endif /* _ADCA_REGS_H_  */
