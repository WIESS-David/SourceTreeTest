                ===========================
                Read Me File for eFLASHLOAD
                ===========================

Created:    10/01/2011   [TS] First release eFLASHLOAD V1.00
Update:     04/03/2011 	 [TS] eFLASHLOAD V1.01
Update:     17/03/2011 	 [TS] eFLASHLOAD V2.00
Update:     24/09/2012 	 [TS] eFLASHLOAD V2.01
Update:     03/07/2014 	 [TS] eFLASHLOAD E3.00a
Update:     10/09/2014 	 [TS] eFLASHLOAD V3.00
Update:     04/12/2014 	 [TS] eFLASHLOAD V3.01
Update:     17/03/2015 	 [TS] eFLASHLOAD V3.02
Update:     01/08/2016 	 [MN] eFLASHLOAD V3.03
Update:     20/12/2016 	 [MN] eFLASHLOAD V3.04
Update:     14/08/2017 	 [MN] eFLASHLOAD V3.05
Update:     12/10/2017 	 [MN] eFLASHLOAD V3.06
Update:     20/11/2017 	 [MN] eFLASHLOAD V3.07

========
Contents
========

  1. General Information
  2. Manual Corrections and Updates
  3. Software Corrections and Updates
  4. Restrictions and Operating Precautions


======================
1. General Information
======================

1.1 For software updates and additional information please check
    the Renesas TOOLWEB page at:

    http://www.renesas.eu/updates

1.2 FLASH monitor samples and eFLASHLOAD configuration files for different
    V850 and RH850 devices can be found in the "\examples" directory of
    the eFLASHLOAD installation.
    Please use the corresponding eFLASHLOAD configurations files which can
    be found in the "\examples\_config" directory for starting up purpose
    and as reference.

=================================
2. Manual Corrections and Updates
=================================

2.1 User Manual R01UT0177ED0200, Rev 2.00

- Update, added support for V850ES and V850E1 based microcontrollers.

2.1 User Manual R01UT0177ED0201, Rev 2.01

- Update, added support for E1 On-Chip debug emulator.

2.2 User Manual R01UT0177ED0300, Rev 3.00

- Update, added support for RH850 based microcontrollers.

2.3 User Manual R01UT0177ED0301, Rev 3.01

- Update, aligned bookmark linkage.

===================================
3. Software Corrections and Updates
===================================

3.1. eFLASHLOAD, Version V1.01

- Correction on handling Intel-Hex record format.
- Update to support relative path information within XML based configuration file.
- Update to support different file extensions on loading Intel-Hex or Motorola-S record formats.

3.2. eFLASHLOAD, Version V2.00

- Update of eFLASHLOAD software to support V850ES and V850E1 based microcontrollers.
- Update, added FLASH monitor examples for V850ES/Fx3 and V850E2/Dx4 devices.

3.3. eFLASHLOAD, Version V2.01

- Update of eFLASHLOAD software to support E1 On-Chip debug emulator.
- Update, added FLASH monitor examples for V850E2/Fx4 and V850E2/Px4 devices.

3.4. eFLASHLOAD, Version E3.00a

- Update of eFLASHLOAD software to support RH850 based microcontrollers.
- Update, added FLASH monitor examples for RH850/F1x and RH850/D1x devices.

3.5. eFLASHLOAD, Version V3.00

- Update of eFLASHLOAD software version to release version V3.00.
- Update of RH850/D1x serial FLASH monitor example to support single and dual
  FLASH module programming.

3.6. eFLASHLOAD, Version V3.01

- Update of eFLASHLOAD software to increase communication and programming speed for RH850 based devices.
- Update of RH850/D1x serial FLASH monitor example for aligned 4KB, 32KB and 64KB block erase.

3.7. FLASH monitor updates, Version V3.02

- Update of RH850/D1M_R7F701412 Serial FLASH monitor example, added 256/512 page mode support.
- Added serial FLASH monitor example for the RH850/D1M_R7F701407 device.

3.8. FLASH monitor updates, Version V3.03

- Update of RH850/D1LM_R7F7014xx Serial FLASH monitor example.
- Added serial FLASH Monitor which supports all devices from R7F70141401 to R7F70141461. Same file base also supports single and dual Flash modules, octa flash, as well as the new SFMA1 of D1M1A.
- Please check dedicated readme file in subfolder of monitor software.

3.9. FLASH monitor updates, Version V3.04

- Update of RH850/D1LM_R7F7014xx Hyper FLASH monitor example.
- Added Hyper FLASH Monitor which supports devices R7F70141441 and R7F70141442.
- Please check dedicated readme file in subfolder of monitor software.

3.10. FLASH monitor updates, Version V3.05

- Update of RH850/D1LM_R7F7014xx Serial FLASH monitor example.
- Update of RH850/D1LM_R7F7014xx Hyper FLASH monitor example.
- Added Octa FLASH Monitor which supports devices R7F70141441 and R7F70141442.
- Please check dedicated readme file in subfolder of monitor software.

3.11. FLASH monitor updates, Version V3.06

- Update of RH850/D1LM_R7F7014xx Serial FLASH monitor example.
- Update of RH850/D1LM_R7F7014xx Octa FLASH monitor example.
- Added SFMA2 FLASH Monitor which supports the device R7F70141441.
- Please check dedicated readme file in subfolder of monitor software.

3.12. FLASH monitor updates, Version V3.07

- Update of EXEC-DLL to V4.00.00.00


=========================================
4. Restrictions and Operating Precautions
=========================================

4.1 None.

In case of any question please do not hesitate to contact:

        Technical Product Support
        Software Tools Support

        Renesas Electronics Europe GmbH
        Arcadiastrasse 10
        D-40472 Duesseldorf, Germany

        e-mail: software_support-eu@lm.renesas.com
        FAX:   +49 - (0)211 / 65 03 - 12 79

==================================
End of Read Me File for eFLASHLOAD
==================================
