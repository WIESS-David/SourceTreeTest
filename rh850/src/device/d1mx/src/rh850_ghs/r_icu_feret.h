/*===========================================================================*/
/* Module      = icu_feret.h                                                 */
/* Version     = V3.03                                                       */
/*               generated by DeFiXRH850 0.7.2.3                             */
/*===========================================================================*/
/*                                  COPYRIGHT                                */
/*===========================================================================*/
/* Copyright (c) 2014 by Renesas Electronics Europe GmbH,                    */
/*               a company of the Renesas Electronics Corporation            */
/*===========================================================================*/
/* Purpose:      Definition of ICU Macros                                    */
/*                                                                           */
/*===========================================================================*/
/*                                                                           */
/* Warranty Disclaimer                                                       */
/*                                                                           */
/* Because the Product(s) is licensed free of charge, there is no warranty   */
/* of any kind whatsoever and expressly disclaimed and excluded by Renesas,  */
/* either expressed or implied, including but not limited to those for       */
/* non-infringement of intellectual property, merchantability and/or         */
/* fitness for the particular purpose.                                       */
/* Renesas shall not have any obligation to maintain, service or provide bug */
/* fixes for the supplied Product(s) and/or the Application.                 */
/*                                                                           */
/* Each User is solely responsible for determining the appropriateness of    */
/* using the Product(s) and assumes all risks associated with its exercise   */
/* of rights under this Agreement, including, but not limited to the risks   */
/* and costs of program errors, compliance with applicable laws, damage to   */
/* or loss of data, programs or equipment, and unavailability or             */
/* interruption of operations.                                               */
/*                                                                           */
/* Limitation of Liability                                                   */
/*                                                                           */
/* In no event shall Renesas be liable to the User for any incidental,       */
/* consequential, indirect, or punitive damage (including but not limited    */
/* to lost profits) regardless of whether such liability is based on breach  */
/* of contract, tort, strict liability, breach of warranties, failure of     */
/* essential purpose or otherwise and even if advised of the possibility of  */
/* such damages. Renesas shall not be liable for any services or products    */
/* provided by third party vendors, developers or consultants identified or  */
/* referred to the User by Renesas in connection with the Product(s) and/or  */
/* the Application.                                                          */
/*                                                                           */
/*===========================================================================*/
/* Environment:                                                              */
/*         Device:         RH850G3M core devices (with FPU)                  */
/*                         RH850G3K core devices (__HAS_FPU__ is undefined)  */
/*         IDE:            GHS Multi for V800  V6.xx or later                */
/*===========================================================================*/
#if defined (__ghs__)
#pragma ghs startnomisra
#endif

#ifndef __ICU_FERET_H__
 #define __ICU_FERET_H__

 #define __HAS_FPU__
/* #undef __HAS_FPU__ */ 
 asm void FETRAP_ENTRY( void)
 {
   pushsp  lp-lp
   pushsp  r1-r2
   #if (__V800_registermode==32)
     pushsp  r5-r31
   #endif
   #if (__V800_registermode==26)
     pushsp  r5-r16     -- r17 to r22 are reserved for user
     pushsp  r23-r31
   #endif
   #if (__V800_registermode==22)
     pushsp  r5-r14     -- r15 to r24 are reserved for user
     pushsp  r25-r31
   #endif
   pushsp  ep-ep
   stsr  FEIC,  r6,  0      -- FEIC
   stsr  FEPSW, r7,  0      -- FEPSW
   stsr  FEPC,  r8,  0      -- FEPC
   #ifndef __V800_ignore_callt_state_in_interrupts__
     stsr  CTPSW, r9,  0      -- CTPSW
     stsr  CTPC,  r10, 0      -- CTPC
     #ifdef __HAS_FPU__
       stsr  FPSR,  r11, 0      -- FPSR
       stsr  FPEPC, r12, 0      -- FPEPC
       pushsp  r6-r12           -- FPU and CALLT to save
     #else
       pushsp  r6-r10           -- CALLT to save
     #endif    /* __HAS_FPU__ */
   #else 	/* do not save callt control registers */
     #ifdef __HAS_FPU__
       stsr	FPSR,  r9, 0     -- FPSR
       stsr	FPEPC, r10, 0    -- FPEPC
       pushsp	r6-r10           -- FPU reg to save
     #else
       pushsp	r6-r8            -- no FPU and no CALLT
     #endif    /* __HAS_FPU__ */
   #endif    /*__V800_ignore_callt_state_in_interrupts__*/
 }

 asm void FETRAP_LEAVE( void)
 { 
   #ifndef __V800_ignore_callt_state_in_interrupts__
     #ifdef __HAS_FPU__
       popsp r6-r12
       ldsr  r12, FPEPC, 0      -- FPEPC
       ldsr  r11, FPSR,  0      -- FPSR
       ldsr  r10, CTPC,  0      -- CTPC
       ldsr  r9,  CTPSW, 0      -- CTPSW
     #else
       popsp r6-r10
       ldsr  r10,  CTPC,  0     -- CTPC
       ldsr  r9,   CTPSW, 0     -- CTPSW
     #endif    /* __HAS_FPU__ */
   #else 	/* do not restore callt control registers */ 
     #ifdef __HAS_FPU__
       popsp r6-r10
       ldsr  r10, FPEPC, 0      -- FPEPC
       ldsr  r9,  FPSR,  0      -- FPSR
     #else
       popsp r6-r8
     #endif    /* __HAS_FPU__ */
   #endif   /* __V800_ignore_callt_state_in_interrupts__ */
	ldsr	r8,  FEPC,  0      -- FEPC
	ldsr	r7,  FEPSW, 0      -- FEPSW
	ldsr	r6,  FEIC, 0       -- FEIC
	popsp	ep-ep
  #if (__V800_registermode==32)
    popsp r5-r31
  #endif
  #if (__V800_registermode==26)
    popsp r23-r31       -- r17 to r22 are reserved for user
    popsp r5-r16
  #endif
  #if (__V800_registermode==22)
    popsp r25-r31       -- r15 to r24 are reserved for user
    popsp r5-r14
  #endif
    popsp r1-r2
    popsp lp-lp
    feret  
 }

 #define PRAGMA(x) _Pragma(#x)

 #define FETRAP_EXCEPTION( name, isr) \
   __attribute__((noinline)) void isr( void); \
   PRAGMA( ghs noprologue)            \
   void name( void) {                 \
     FETRAP_ENTRY();                  \
     isr();                           \
     FETRAP_LEAVE();                  \
   }

#if defined (__ghs__)
#pragma ghs endnomisra
#endif

#endif  /* __ICU_FERET_H__ */
