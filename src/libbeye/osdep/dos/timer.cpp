#include "config.h"
#include "libbeye/libbeye.h"
using namespace	usr;
#include "libbeye/osdep/__os_dep.h"
/**
 * @namespace   libbeye
 * @file        libbeye/osdep/dos/timer.c
 * @brief       This file contains implementation of timer depended part for DOS-32.
 * @version     -
 * @remark      this source file is part of Binary EYE project (BEYE).
 *              The Binary EYE (BEYE) is copyright (C) 1995 Nickols_K.
 *              All rights reserved. This software is redistributable under the
 *              licence given in the file "Licence.en" ("Licence.ru" in russian
 *              translation) distributed in the BEYE archive.
 * @note        Requires POSIX compatible development system
 *
 * @author      Nickols_K
 * @since       1999
 * @note        Development, fixes and improvements
**/
#include <go32.h>
#include <dpmi.h>
#include <stddef.h>

static timer_callback *user_callback = NULL;

static void timer_handler()
{
  if(user_callback) (*user_callback)();
}

unsigned __FASTCALL__ __OsSetTimerCallBack(unsigned ms,timer_callback func)
{
   _go32_dpmi_seginfo si;
   si.pm_offset=(unsigned)&timer_handler;
   user_callback = func;
   _go32_dpmi_chain_protected_mode_interrupt_vector(0x1C,&si);
   return 54;
}

			     /* Restore time callback function to original
				state */
void __FASTCALL__ __OsRestoreTimer()
{
  user_callback = NULL;
}


