#include "w_sys_reset.h"


uint32_t  W_RESET_Software(void)
{
   int8_t i8retry = 10;
   do
   {
       SYSPROTCMD0= 0xA5;
       SYSSWRESA= 1;
       SYSSWRESA= 0xFFFFFFFE;
       SYSSWRESA= 1;
   }while(SYSPROTS0== 1 || i8retry-- < 0);

   if(i8retry >= 0)
       return R_TRUE;
   else
       return R_FALSE;
}

